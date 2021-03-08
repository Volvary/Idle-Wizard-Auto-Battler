#pragma once
#include "Resources.h"

class ScreenReader;
class ConsoleFormatter;
struct Parchment;
class StatusEffect;
class LogicalBrain;


//** The main logic behind the Auto Battler. This will contain all the various components required to run the Auto Battler.
class AutoBattler
{
private:

	ScreenReader* reader;
	ConsoleFormatter* formatter;
	LogicalBrain* brain;

public:
	AutoBattler();
	~AutoBattler();

	void Init();

	std::vector<Parchment> spellsInHand;
};