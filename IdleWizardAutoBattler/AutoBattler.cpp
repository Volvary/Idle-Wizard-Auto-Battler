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


AutoBattler::AutoBattler()
{
	reader = new ScreenReader();
	formatter = new ConsoleFormatter();
	brain = new LogicalBrain();
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
	
	while (!bDone) {

		if ((GetKeyState(VK_CAPITAL) & 0x0001) != 0) {
			if (!bAlerted) {
				formatter->PrintAtCoord(0,5, "Caps Lock active, interrupting execution.", 43);
				bAlerted = true;
			}
			continue;
		}

		if (bAlerted) {
			formatter->PrintAtCoord(0,5,"Caps Lock disabled, resuming execution.", 43);
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
			bLogicRequired = false;
			brain->IterateLogic();
		}
		else{
			bool bStageCompleted = false;
			if (reader->IsWaitingOnVictoryScreen(bStageCompleted)) {
				if (brain->RoundFinished(bStageCompleted)) {
					bLogicRequired = true;
				}
			}
		}

		/* Spell Threshold Range Analyzer */
		/*auto SpellList = GetSpellList();
		int line = 9;
		for(Spell* spell: SpellList) {
			for (Spell * otherSpell : SpellList) {
				if (spell != otherSpell) {
					int TotalRGBDifference = abs((int)(spell->averageIconColor.R - otherSpell->averageIconColor.R)) +
						abs((int)(spell->averageIconColor.G - otherSpell->averageIconColor.G)) +
						abs((int)(spell->averageIconColor.B - otherSpell->averageIconColor.B));

					if (TotalRGBDifference < SPELLDETECTION_THRESHOLD){
						formatter->PrintAtCoord(0, line, spell->name + " and " + otherSpell->name + " are in range of each other.", 0);
						line++;
					}
					else if	(TotalRGBDifference < SPELLDETECTION_THRESHOLD * 2) {
						formatter->PrintAtCoord(0, line, spell->name + " and " + otherSpell->name + " are in extended range of each other.", 0);
						line++;
					}
				}
			}
		}*/

		bDone = GetKeyState(VK_END);
	}

	formatter->Seek(0, 7);
}