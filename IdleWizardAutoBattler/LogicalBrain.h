#pragma once

#include "Resources.h"
#include <map>

#define HEAL_SLEEP 5

struct Parchment;
class StatusEffect;
struct BoardStatus;

class LogicalBrain
{
	std::vector<Parchment*> cardsToPlay;
	Parchment* nextCardToPlay = nullptr;
	BoardStatus* boardState = nullptr;

	int interactionSleep = 0;

	std::map<SpellTypes, int> spellWastePriority;

	Pixel ContinuePress = Pixel(1075, 925);
	Pixel StartPress = Pixel(1180, 950);

	bool bNeedToRestart = false;

	const int totalExpeditions = 2;
	int completedExpedition = 0;
public:
	void QueueMovementEvent(int posX, int posY, bool bShouldClick = true);
	void QueueMovementEvent(Pixel pixelToClick);

	void DecideInteractionForFrame();
	void AnalyzeHand(BoardStatus* boardStatus);

	LogicalBrain();
	~LogicalBrain();
	bool RoundFinished(bool bStageCompleted);
	void IterateLogic();
};