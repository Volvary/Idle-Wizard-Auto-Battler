#pragma once

#include "Resources.h"
#include <map>

#define HEAL_SLEEP 15

struct Parchment;
class StatusEffect;
struct BoardStatus;
class ConsoleFormatter;

class LogicalBrain
{
	std::vector<Parchment*> cardsToPlay;
	Parchment* nextCardToPlay = nullptr;
	BoardStatus* boardState = nullptr;
	ConsoleFormatter* console = nullptr;

	int interactionSleep = 0;

	std::map<SpellTypes, int> spellWastePriority;

	Pixel ContinuePress = Pixel(1075, 925);
	Pixel StartPress = Pixel(1180, 950);

	bool bNeedToRestart = false;
	bool bIsBurning = false;

	int totalExpeditions = 2;
	int completedExpedition = 0;

	bool IsSleeping();
public:
	void QueueMovementEvent(int posX, int posY, bool bShouldClick = true);
	void QueueMovementEvent(Pixel pixelToClick);

	void DecideInteractionForFrame();
	void AnalyzeHand(BoardStatus* boardStatus);

	void AddExpedition(int expeditionToAdd);

	LogicalBrain();
	~LogicalBrain();
	bool RoundFinished(bool bStageCompleted);

	void PrintCurrentExpeditions();

	bool IterateLogic();

	void SetConsoleFormatter(ConsoleFormatter* consoleFormatter);

	void TickSleep();
};