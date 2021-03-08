#pragma once

#include "Pixel.h"
#include <vector>

enum class SpellTypes {
	Unknown,
	MagicMissile,
	SpellFocus,
	Debilitate,
	FireBall,
	LightningBolt,
	MagicWeapon,
	ForceOfWill,
	VoidSyphon,
	VoidBolt,
	Hellstorm,
	KelphiorBlackBeam,
	Counterspell
};

class Spell {

public:
	SpellTypes type;
	std::string name;
	Pixel averageIconColor;
	Pixel averageChargingIconColor;

	Spell() {

	}

	Spell(SpellTypes Type, std::string Name, Pixel AverageColor, Pixel AverageChargeColor = Pixel()) :type(Type), name(Name), averageIconColor(AverageColor) {
		if (AverageChargeColor == Pixel()) {
			averageChargingIconColor = averageIconColor * 0.8f;
		}
		else {
			averageChargingIconColor = AverageChargeColor;
		}
	}

	static Spell* GetSpellByType(SpellTypes Type);
};

const std::vector<Spell*> GetSpellList();
void DeleteSpellList();