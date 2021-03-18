#include "pch.h"
#include "AutoBattler.h"
#include "Resources.h"
#include "ScreenReader.h"
#include "ConsoleFormatter.h"
#include "Parchment.h"
#include "StatusEffect.h"
#include "LogicalBrain.h"
#include "BoardStatus.h"
#include "ObjectPooler.h"
#include <time.h>


AutoBattler::AutoBattler()
{
	reader = new ScreenReader();
	formatter = new ConsoleFormatter();
	brain = new LogicalBrain();
	brain->SetConsoleFormatter(formatter);
}


AutoBattler::~AutoBattler()
{
	delete reader;
	delete formatter;
	delete brain;

	ObjectPooler::DestroySingleton();
}

void AutoBattler::Init()
{
	bool bDone = false;
	bool bAlerted = false;
	bool bLogicRequired = false;
	bool bLastFramePlusState = false;
	bool bLastFrameMinusState = false;
	
	while (!bDone) {

		if ((GetKeyState(VK_CAPITAL) & 0x0001) != 0) {
			if (!bAlerted) {
				formatter->PrintAtCoord(0,7, "Caps Lock active, interrupting execution.", 43);
				bAlerted = true;
			}
			Sleep(100);
			continue;
		}

		if (bAlerted) {
			formatter->PrintAtCoord(0,7,"Caps Lock disabled, resuming execution.", 43);
			bAlerted = false;
		}

		reader->LoadScreenToBuffer();

		if (reader->IsInCombat()) {
			reader->TickStatus();
			BoardStatus boardState = BoardStatus(reader->GetParchmentsInHand(), reader->GetActiveStatus(), reader->GetEnemyHealth(), reader->GetPlayerHealth());
			brain->AnalyzeHand(&boardState);
			brain->DecideInteractionForFrame();

			formatter->WriteStatusToScreen(&boardState);
		}
		else if (bLogicRequired) {
			bLogicRequired = brain->IterateLogic();
		}
		else{
			bool bStageCompleted = false;
			if (reader->IsWaitingOnVictoryScreen(bStageCompleted)) {
				if (brain->RoundFinished(bStageCompleted)) {
					bLogicRequired = true;
				}
			}
			else {
				Sleep(1000);
			}
		}
		brain->TickSleep();

		bool bAdd = GetKeyState(VK_ADD);		//Num +
		bool bSub = GetKeyState(VK_SUBTRACT);	//Num -
		if (bAdd != bLastFramePlusState) {
			bLastFramePlusState = bAdd;
			if (bAdd) {
				brain->AddExpedition(1);
			}
		}

		if (bSub != bLastFrameMinusState) {
			bLastFrameMinusState = bSub;
			if (bSub) {
				brain->AddExpedition(-1);
			}
		}

		bDone = GetKeyState(VK_END);
	}

	formatter->Seek(0, 7);
}