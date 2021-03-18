#pragma once

#include "Spells.h"
#include "StatusEffect.h"
#include "Parchment.h"
#include "Resources.h"

struct BoardStatus {
	std::vector<Parchment*> hand;
	std::vector<StatusEffect*> effects;
	HealthStatus enemyHealth;
	HealthStatus playerHealth;

	BoardStatus(std::vector<Parchment*> InHand, std::vector<StatusEffect*> InEffects, HealthStatus InEnemyHealth = HealthStatus::Unknown, HealthStatus InPlayerHealth = HealthStatus::Unknown) 
	: hand(InHand), effects(InEffects), enemyHealth(InEnemyHealth), playerHealth(InPlayerHealth)
	{

	}
};

struct ConsoleBoardStatus {	//Used by the ConsoleFormatter to remember information about the last frame of the Board.
	std::vector<std::string> parchmentScreenText;
	std::vector<std::string> effectsScreenText;
};