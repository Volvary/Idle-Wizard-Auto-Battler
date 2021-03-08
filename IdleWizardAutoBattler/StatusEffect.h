#pragma once
#include <time.h>
#include "Spells.h"
#include <map>

class StatusEffect
{
public:

	time_t timeStored;
	time_t timeEnding;

	SpellTypes effectType = SpellTypes::Unknown;

	StatusEffect(SpellTypes InEffectType);

	bool HasEnded();

	StatusEffect();
	~StatusEffect();
};

const std::map<SpellTypes, float> StatusLength = {
	{SpellTypes::ForceOfWill, 10.0f},
	{SpellTypes::MagicWeapon, 8.0f},
	{SpellTypes::FireBall, 4.0f},
	{SpellTypes::Hellstorm, 6.0f}
};