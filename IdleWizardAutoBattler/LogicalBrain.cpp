#include "pch.h"
#include "LogicalBrain.h"
#include <windows.h>
#include "Parchment.h"
#include "StatusEffect.h"
#include "BoardStatus.h"
#include "ScreenReader.h"
#include <typeinfo>

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
}

void LogicalBrain::DecideInteractionForFrame()
{
	if (interactionSleep > 0) {
		interactionSleep--;
		return;
	}

	if (boardState->playerHealth == HealthStatus::Panic) {
		interactionSleep = HEAL_SLEEP;
		QueueMovementEvent(ScreenReader::GetHealthPotionPosition());
	}
	
	if (nextCardToPlay != nullptr) {
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

	if (nextCardToPlay == nullptr) {

		std::vector<Parchment*> spellsToPlay;

		for (Parchment* parchment : boardState->hand) {
			if (parchment && parchment->bReady && parchment->bConfirmed && parchment->spell->type != SpellTypes::Unknown) {
				if (parchment->ShouldCastNow(boardState)) {
					spellsToPlay.push_back(parchment);
				}
			}
		}

		if (spellsToPlay.size() > 0) {
			cardsToPlay.clear();
			for (Parchment* parchment : spellsToPlay) {
				//Push spells to the front of the queue if they are buffs.
				if (parchment->spell->type == SpellTypes::VoidSyphon || parchment->spell->type == SpellTypes::MagicWeapon) {
					cardsToPlay.insert(cardsToPlay.begin(), parchment);
				}
				else {
					cardsToPlay.push_back(parchment);
				}
			}
		}
		else {
			if (boardState->hand.size() >= 5) {
				Parchment* spellToWaste = nullptr;
				int spellPriority = INT_MAX;
				for (Parchment* spell : boardState->hand) {
					if (spell->position != -1 && spell->bReady) {
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
					cardsToPlay.push_back(spellToWaste);
				}
			}
		}
	}
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
	completedExpedition++;

	if (interactionSleep > 0) {
		interactionSleep--;
		return false;
	}

	cardsToPlay.clear();
	boardState = nullptr;
	nextCardToPlay = nullptr;

	if (!bStageCompleted) {
		interactionSleep = 50;
		QueueMovementEvent(ContinuePress);
	}
	else {
		interactionSleep = 500;
		QueueMovementEvent(ContinuePress);

		bNeedToRestart = completedExpedition <= totalExpeditions;

		return bNeedToRestart;
	}

	return false;
}

void LogicalBrain::IterateLogic()
{
	if (bNeedToRestart) {
		interactionSleep = 50;
		QueueMovementEvent(StartPress);
	}
}
