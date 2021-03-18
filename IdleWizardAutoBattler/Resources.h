#pragma once

#include "Pixel.h"
#include <iostream>
#include <string>
#include <vector>
#include "Spells.h"
#include "PlayStatus.h"

#define DEBUG_MEMORY 1

enum class HealthStatus {
	Unknown,
	NearFull,
	ThreeQuarter,
	Half,
	Quarter,
	Frenzy,			//Defines the point at which the Auto Battler fires everything instantly to finish off the monster.
	Panic			//Similar status to alert that the player is super low in health.
};

std::string HealthStatusToText(HealthStatus Status);

std::string SpellTypeToText(SpellTypes Status);

std::string PlayStatusToText(PlayStatus Status);

namespace DebugMemory {

	void IncrementParchmentCreated();
	void IncrementParchmentDestroyed();
	void IncrementEffectsCreated();
	void IncrementEffectsDestroyed();
	
	int GetParchmentCreated();
	int GetParchmentDestroyed();
	int GetEffectsCreated();
	int GetEffectsDestroyed();
}

