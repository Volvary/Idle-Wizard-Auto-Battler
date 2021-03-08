#include "pch.h"
#include "Spells.h"

std::vector<Spell*> SpellList = std::vector<Spell*>();
void PrepareSpellList();


Spell* Spell::GetSpellByType(SpellTypes Type)
{
	Spell* Out = nullptr;

	const std::vector<Spell*> Spells = GetSpellList();

	for (Spell* spell : Spells) {
		if (spell->type == Type) {
			Out = spell;
		}
	}
	return Out;
}

void PrepareSpellList()
{
	if (SpellList.size() != 0) {
		return;
	}
	SpellList.push_back(new Spell(SpellTypes::Unknown,					std::string("Unk"), Pixel(0, 0, 0)));
	SpellList.push_back(new Spell(SpellTypes::MagicMissile,				std::string("MM"), Pixel(78, 71, 68), Pixel(53, 51, 53)));
	SpellList.push_back(new Spell(SpellTypes::SpellFocus,				std::string("SF"), Pixel(94, 133, 162), Pixel(81, 123, 154)));
	SpellList.push_back(new Spell(SpellTypes::Debilitate,				std::string("De"), Pixel(64, 51, 85), Pixel(64, 51, 85)));
	SpellList.push_back(new Spell(SpellTypes::FireBall,					std::string("FB"), Pixel(219, 153, 97), Pixel(219, 153, 97)));
	SpellList.push_back(new Spell(SpellTypes::LightningBolt,			std::string("LB"), Pixel(173, 170, 112), Pixel(162, 162, 105)));
	SpellList.push_back(new Spell(SpellTypes::MagicWeapon,				std::string("MW"), Pixel(75, 76, 78), Pixel(54, 59, 66)));
	SpellList.push_back(new Spell(SpellTypes::ForceOfWill,				std::string("FoW"), Pixel(193, 114, 123), Pixel(193, 114, 123)));
	SpellList.push_back(new Spell(SpellTypes::VoidSyphon,				std::string("VS"), Pixel(156, 137, 121), Pixel(144, 128, 116)));
	SpellList.push_back(new Spell(SpellTypes::VoidBolt,					std::string("VB"), Pixel(119, 74, 138), Pixel(103, 61, 129)));
	SpellList.push_back(new Spell(SpellTypes::Hellstorm,				std::string("He"), Pixel(211, 127, 68), Pixel(211, 127, 68)));
	SpellList.push_back(new Spell(SpellTypes::KelphiorBlackBeam,		std::string("KBB")	,Pixel(43, 15,	22), Pixel(42, 13, 21))	);
	SpellList.push_back(new Spell(SpellTypes::Counterspell,				std::string("Co")	,Pixel(58, 40,	127), Pixel(58, 40, 127))	);
}

const std::vector<Spell*> GetSpellList()
{
	PrepareSpellList();

	return SpellList;
}

void DeleteSpellList()
{
	while (SpellList.size() > 0) {
		delete SpellList.back();
		SpellList.pop_back();
	}
}
