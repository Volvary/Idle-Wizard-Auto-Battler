#include "pch.h"
#include "Resources.h"

std::string HealthStatusToText(HealthStatus Status) {

	std::string Out;

	switch (Status) {
	case HealthStatus::NearFull:
		Out = "Near Full";
		break;
	case HealthStatus::ThreeQuarter:
		Out = "Three Quarter";
		break;
	case HealthStatus::Half:
		Out = "Half";
		break;
	case HealthStatus::Quarter:
		Out = "Quarter";
		break;
	case HealthStatus::Frenzy:
		Out = "FRENZY";
		break;
	case HealthStatus::Panic:
		Out = "PANIC";
		break;
	default:
		Out = "Unknown";
		break;
	}

	return Out;
}

std::string SpellTypeToText(SpellTypes Status)
{
	std::string Out = "";

	switch (Status)
	{
	case SpellTypes::Unknown:
		Out = "Unknown";
		break;
	case SpellTypes::MagicMissile:
		Out = "Magic Missile";
		break;
	case SpellTypes::SpellFocus:
		Out = "Spell Focus";
		break;
	case SpellTypes::Debilitate:
		Out = "Debilitate";
		break;
	case SpellTypes::FireBall:
		Out = "Fire Ball";
		break;
	case SpellTypes::LightningBolt:
		Out = "Lightning Bolt";
		break;
	case SpellTypes::MagicWeapon:
		Out = "Magic Weapon";
		break;
	case SpellTypes::ForceOfWill:
		Out = "Force of Will";
		break;
	case SpellTypes::VoidSyphon:
		Out = "Void Syphon";
		break;
	case SpellTypes::VoidBolt:
		Out = "Void Bolt";
		break;
	case SpellTypes::Hellstorm:
		Out = "Hellstorm";
		break;
	case SpellTypes::KelphiorBlackBeam:
		Out = "Kelphior Black Beam";
		break;
	case SpellTypes::Counterspell:
		Out = "Counterspell";
		break;
	default:
		break;
	}

	return Out;
}

static int ParchmentCreated = 0;
static int ParchmentDestroyed = 0;

static int EffectsCreated = 0;
static int EffectsDestroyed = 0;

void DebugMemory::IncrementParchmentCreated()
{
	ParchmentCreated++;
}

void DebugMemory::IncrementParchmentDestroyed()
{
	ParchmentDestroyed++;
}

void DebugMemory::IncrementEffectsCreated()
{
	EffectsCreated++;
}

void DebugMemory::IncrementEffectsDestroyed()
{
	EffectsDestroyed++;
}

int DebugMemory::GetParchmentCreated()
{
	return ParchmentCreated;
}

int DebugMemory::GetParchmentDestroyed()
{
	return ParchmentDestroyed;
}

int DebugMemory::GetEffectsCreated()
{
	return EffectsCreated;
}

int DebugMemory::GetEffectsDestroyed()
{
	return EffectsDestroyed;
}
