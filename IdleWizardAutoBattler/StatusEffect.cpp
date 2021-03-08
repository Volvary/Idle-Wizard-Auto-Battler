#include "pch.h"
#include "StatusEffect.h"


StatusEffect::StatusEffect()
{
}


StatusEffect::StatusEffect(SpellTypes InEffectType): effectType(InEffectType)
{
	time(&timeStored);

	auto it = StatusLength.find(InEffectType);
	if (it != StatusLength.end()) {
		float endTime = StatusLength.at(InEffectType);
		timeEnding = timeStored + (time_t)endTime;
	}
}

bool StatusEffect::HasEnded()
{
	time(&timeStored);

	if (timeEnding < timeStored) {
		return true;
	}

	return false;
}

StatusEffect::~StatusEffect()
{
}
