#include "pch.h"
#include "LogicalBrain.h"
#include <windows.h>
#include "Parchment.h"
#include "StatusEffect.h"
#include "BoardStatus.h"
#include "ScreenReader.h"
#include <typeinfo>
#include "ConsoleFormatter.h"

bool LogicalBrain::IsSleeping()
{
	if (interactionSleep > 0) {
		console->PrintAtCoord(26, 0, "Interaction Sleep: " + std::to_string(interactionSleep), 24);
		return true;
	}

	return false;
}

void LogicalBrain::QueueMovementEvent(Pixel pixelToClick) {
	QueueMovementEvent(pixelToClick.X, pixelToClick.Y);
}

void LogicalBrain::QueueMovementEvent(int posX, int posY, bool bShouldClick)
{
	INPUT Inputs[3] = { 0 };

	double fScreenWidth = ::GetSystemMetrics(SM_CXSCREEN);
	double fScreenHeight = ::GetSystemMetrics(SM_CYSCREEN) - 50;
	double fx = posX * (65535.0f / fScreenWidth);
	double fy = posY * (65535.0f / fScreenHeight);

	Inputs[0].type = INPUT_MOUSE;
	Inputs[0].mi.dx = (LONG)fx;
	Inputs[0].mi.dy = (LONG)fy;
	Inputs[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;

	if (bShouldClick) {
		Inputs[1].type = INPUT_MOUSE;
		Inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

		Inputs[2].type = INPUT_MOUSE;
		Inputs[2].mi.dwFlags = MOUSEEVENTF_LEFTUP;
	}

	SendInput(3, Inputs, sizeof(INPUT));

	interactionSleep = 10;
}

void LogicalBrain::DecideInteractionForFrame()
{
	if (IsSleeping()) {
		return;
	}

	if (boardState && boardState->playerHealth == HealthStatus::Panic) {
		interactionSleep = HEAL_SLEEP;
		QueueMovementEvent(ScreenReader::GetHealthPotionPosition());
		console->AddEventToLog("Player Health critical, using a potion.");
	}
	
	if (nextCardToPlay != nullptr) {
		bIsBurning = false;
		if (!nextCardToPlay->bReady || !nextCardToPlay->bConfirmed) {
			nextCardToPlay = nullptr;
			QueueMovementEvent(100, 600, false);
		}
		else {
			for (Parchment* parchment : boardState->hand) {
				if (nextCardToPlay == parchment && *nextCardToPlay == *parchment) {
					QueueMovementEvent(ScreenReader::GetClickLocationPerSpell(parchment));
					StatusEffect* effect = parchment->PostCastEffects();
					if (effect) {
						if (!ScreenReader::RegisterStatusEffect(effect)) {	//If the effect was not registered, delete it.
							delete effect;

							if (DEBUG_MEMORY) {
								DebugMemory::IncrementEffectsDestroyed();
							}
						}
					}
					return;
				}
			}
		}

		nextCardToPlay = nullptr;
	}
	else
	{
		//Purge parchments that could be invalid at this point.
		for (int i = 0; i < cardsToPlay.size(); i++) {
			if (!cardsToPlay[i]) {
				cardsToPlay.erase(cardsToPlay.begin() + i);
				i--;
			}
			else if (!cardsToPlay[i]->bReady || !cardsToPlay[i]->bConfirmed) {
				cardsToPlay.erase(cardsToPlay.begin() + i);
				i--;
			}
		}

		if (cardsToPlay.size() > 0) {
			nextCardToPlay = cardsToPlay[0];
			cardsToPlay.erase(cardsToPlay.begin());
		}
	}
}

void LogicalBrain::AnalyzeHand(BoardStatus* boardStatus)
{
	//If we were thinking the expeditions were done but we restarted a fight, reset the Expeditions.
	if (completedExpedition >= totalExpeditions) {
		completedExpedition = 0;
	}
	boardState = boardStatus;

	if (IsSleeping() || bIsBurning) {	//Skip this Analyze if we are sleeping or waiting for a parchment to be burned.
		return;
	}


	if (nextCardToPlay == nullptr) {

		std::vector<Parchment*> spellsToPlay;
		cardsToPlay.clear();

		for (Parchment* parchment : boardState->hand) {
			if (parchment && parchment->bReady && parchment->bConfirmed && parchment->spell->type != SpellTypes::Unknown) {
				if (parchment->ShouldCastNow(boardState)) {
					spellsToPlay.push_back(parchment);
				}
			}
			else if (parchment && !parchment->bReady) {
				parchment->status = PlayStatus::Charging;
			}
		}

		if (spellsToPlay.size() > 0) {
			for (Parchment* parchment : spellsToPlay) {
				//Push spells to the front of the queue if they are buffs.
				if (parchment->spell->type == SpellTypes::VoidSyphon || parchment->spell->type == SpellTypes::MagicWeapon) {
					cardsToPlay.insert(cardsToPlay.begin(), parchment);
				}
				else {
					cardsToPlay.push_back(parchment);
				}
				console->AddEventToLog("Playing spell " + SpellTypeToText(parchment->spell->type) + " with reason " + PlayStatusToText(parchment->status));
			}
		}
		else {
			if (boardState->hand.size() > 5) {
				Parchment* spellToWaste = nullptr;
				int spellPriority = INT_MAX;
				for (Parchment* spell : boardState->hand) {
					if (spell->position != -1 && spell->bReady && spell->bConfirmed) {
						auto it = spellWastePriority.find(spell->spell->type);
						if (it != spellWastePriority.end()) {
							int priority = it->second;
							if (priority && priority < spellPriority) {
								spellToWaste = spell;
								spellPriority = priority;
							}
						}
						else {
							//std::cout << "Error occurred with spell of type " << spell->spell->name;
						}
						
					}
				}

				if (spellToWaste != nullptr) {
					spellToWaste->status = PlayStatus::HandFull;
					cardsToPlay.push_back(spellToWaste);
					console->AddEventToLog("Hand is full, burning #" + std::to_string(spellToWaste->position));
					bIsBurning = true;
				}
			}
		}
	}
}

void LogicalBrain::AddExpedition(int expeditionToAdd)
{
	totalExpeditions += expeditionToAdd;
	PrintCurrentExpeditions();
}

LogicalBrain::LogicalBrain()
{
	spellWastePriority = {
		{SpellTypes::Counterspell, 2},
		{SpellTypes::Debilitate, 1},
		{SpellTypes::FireBall, 3},
		{SpellTypes::ForceOfWill, 1},
		{SpellTypes::Hellstorm, 2},
		{SpellTypes::KelphiorBlackBeam, 4},
		{SpellTypes::LightningBolt, 2},
		{SpellTypes::MagicMissile, 3},
		{SpellTypes::MagicWeapon, 5},
		{SpellTypes::SpellFocus, 3},
		{SpellTypes::VoidBolt, 4},
		{SpellTypes::VoidSyphon, 5}
	};
}


LogicalBrain::~LogicalBrain()
{
}

bool LogicalBrain::RoundFinished(bool bStageCompleted)
{
	if (IsSleeping()) {
		return false;
	}

	cardsToPlay.clear();
	boardState = nullptr;
	nextCardToPlay = nullptr;

	console->RoundFinished(bStageCompleted);

	if (!bStageCompleted) {
		interactionSleep = 50;
		QueueMovementEvent(ContinuePress);
	}
	else {

		completedExpedition++;
		PrintCurrentExpeditions();


		interactionSleep = 500;
		QueueMovementEvent(ContinuePress);

		bNeedToRestart = completedExpedition < totalExpeditions;

		return bNeedToRestart;
	}

	return false;
}

void LogicalBrain::PrintCurrentExpeditions()
{
	console->PrintAtCoord(52, 0, "Expeditions: " + std::to_string(completedExpedition + 1) + "/" + std::to_string(totalExpeditions), 20);
}

bool LogicalBrain::IterateLogic()
{
	if (IsSleeping()) {
		return true;
	}

	if (bNeedToRestart) {
		interactionSleep = 50;
		QueueMovementEvent(StartPress);
	}
	return false;
}

void LogicalBrain::SetConsoleFormatter(ConsoleFormatter* consoleFormatter)
{
	console = consoleFormatter;
	console->PrintAtCoord(52, 0, "Expeditions: 1/" + std::to_string(totalExpeditions), 20);

	console->PrintAtCoord(80, 10, PlayStatusToText(PlayStatus::AlwaysPlay) + " = " + "Always Play" , 0);
	console->PrintAtCoord(80, 11, PlayStatusToText(PlayStatus::BuffsReady) + " = " + "Buffs Ready" , 0);
	console->PrintAtCoord(80, 12, PlayStatusToText(PlayStatus::CantCantrip) + " = " + "Can't Cantrip" , 0);
	console->PrintAtCoord(80, 13, PlayStatusToText(PlayStatus::Cantripping) + " = " + "Cantripping" , 0);
	console->PrintAtCoord(80, 14, PlayStatusToText(PlayStatus::ChargeBigSpell) + " = " + "Charge Big Spell" , 0);
	console->PrintAtCoord(80, 15, PlayStatusToText(PlayStatus::Charging) + " = " + "Charging" , 0);
	console->PrintAtCoord(80, 16, PlayStatusToText(PlayStatus::Frenzy) + " = " + "Frenzy" , 0);
	console->PrintAtCoord(80, 17, PlayStatusToText(PlayStatus::HandFull) + " = " + "Hand Full" , 0);
	console->PrintAtCoord(80, 18, PlayStatusToText(PlayStatus::NotEnoughBuff) + " = " + "Not Enough Buff" , 0);
	console->PrintAtCoord(80, 19, PlayStatusToText(PlayStatus::NotEnoughDamage) + " = " + "Not Enough Damage" , 0);
	console->PrintAtCoord(80, 20, PlayStatusToText(PlayStatus::OtherBuffCharging) + " = " + "Other Buff Charging" , 0);
}

void LogicalBrain::TickSleep()
{
	if (interactionSleep) {
		interactionSleep--;
	}
}
