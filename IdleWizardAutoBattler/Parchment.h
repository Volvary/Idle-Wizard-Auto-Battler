#pragma once
#include "Resources.h"
#include "StatusEffect.h"
#include "BoardStatus.h"

#define HYSTERESIS_ITERATIONS 10

struct Parchment {
	bool bReady = false;
	Spell* spell = nullptr;
	int position = -1;
	short hysteresisCounter = 0;
	bool bConfirmed = false;

	///Debug
	uint16_t readyPixelColor;
	Pixel averageIconColor;

	Parchment() {
		position = -1;		//Initialize the position to -1 so we know it's not an appropriately setup Parchment.
		spell = nullptr;
	}

	Parchment(Parchment* InParchment) {
		bReady = InParchment->bReady;
		spell = InParchment->spell;
		position = InParchment->position;
		bConfirmed = InParchment->bConfirmed;
		hysteresisCounter = InParchment->hysteresisCounter;

		readyPixelColor = InParchment->readyPixelColor;
		averageIconColor = InParchment->averageIconColor;
	}

	Parchment(bool IsReady, SpellTypes SpellType, int Position) : bReady(IsReady), position(Position) {
		spell = Spell::GetSpellByType(SpellType);
	}

	virtual bool IsBurnSpell() {
		return false;
	}

	virtual StatusEffect* PostCastEffects() {

		StatusEffect* Out = nullptr;

		switch (spell->type){
		case SpellTypes::ForceOfWill:
			Out = new StatusEffect(SpellTypes::ForceOfWill);
			break;
		case SpellTypes::Hellstorm:
			Out = new StatusEffect(SpellTypes::Hellstorm);
			break;
		case SpellTypes::MagicWeapon:
			Out = new StatusEffect(SpellTypes::MagicWeapon);
			break;
		case SpellTypes::FireBall:
			Out = new StatusEffect(SpellTypes::FireBall);
			break;
		default:
			break;
		}

		if (Out && DEBUG_MEMORY) {
			DebugMemory::IncrementEffectsCreated();
		}

		return Out;
	}

	virtual bool ShouldCastNow(BoardStatus* boardState) {
		return true;
	}

	bool SetSpell(Spell* inSpell) {
		if (inSpell != spell) {
			spell = inSpell;
			hysteresisCounter = 0;
			bConfirmed = false;
		}
		else {
			if (hysteresisCounter >= HYSTERESIS_ITERATIONS && !bConfirmed) {
				bConfirmed = true;
				return true;
			}
			else if(hysteresisCounter < HYSTERESIS_ITERATIONS) {
				hysteresisCounter++;
			}
		}

		return false;
	}

	bool operator== (Parchment other) {
		return (position == other.position && spell == other.spell);
	}
};

struct DamageBuff : public Parchment {


	DamageBuff(Parchment* InParchment) {
		bReady = InParchment->bReady;
		spell = InParchment->spell;
		position = InParchment->position;
		bConfirmed = InParchment->bConfirmed;
		hysteresisCounter = InParchment->hysteresisCounter;

		readyPixelColor = InParchment->readyPixelColor;
		averageIconColor = InParchment->averageIconColor;
	}

	virtual bool ShouldCastNow(BoardStatus* boardState) override
	{
		int readySpells = 0;
		bool overrideMW = false;
		bool shouldCast = true;

		for (Parchment* parchment : boardState->hand) {
			
			if(parchment->spell->type == SpellTypes::MagicWeapon){
				if (!parchment->bReady) {
					shouldCast = false;
				}
				else {
					overrideMW = true;
				}
			}
			else {
				if (parchment->bConfirmed && parchment->IsBurnSpell()) {
					readySpells++;
				}
			}
		}

		if (!shouldCast && !overrideMW) {
			return false;
		}
		
		if (readySpells >= 3) {
			return true;
		}
		return false;
	}

};

struct BurnSpell : public Parchment {
	bool ShouldCastNow(BoardStatus* boardState) override {
		
		if (boardState->enemyHealth == HealthStatus::Frenzy) {
			return true;
		}

		int buffsInHand = 0;
		int activeBuffs = 0;

		//Count buffs in hand
		for (Parchment* parchment : boardState->hand) {
			if (parchment->spell->type == SpellTypes::VoidSyphon || parchment->spell->type == SpellTypes::MagicWeapon) {
				buffsInHand++;
			}
		}

		//Count buffs active
		for (StatusEffect* status : boardState->effects) {
			if (status->effectType == SpellTypes::MagicMissile /*|| voidMana > 20%*/) {
				activeBuffs++;
			}
		}

		//If we have less buffs in hand, cast it.
		if (activeBuffs < buffsInHand) {
			return false;
		}

		return activeBuffs > 0;
	}

	BurnSpell() {
		Parchment::Parchment();
	}

	BurnSpell(Parchment* InParchment) {
		bReady = InParchment->bReady;
		spell = InParchment->spell;
		position = InParchment->position;
		bConfirmed = InParchment->bConfirmed;
		hysteresisCounter = InParchment->hysteresisCounter;

		readyPixelColor = InParchment->readyPixelColor;
		averageIconColor = InParchment->averageIconColor;
	}

	virtual bool IsBurnSpell() override
	{
		return true;
	}

};

struct LightningBolt : public BurnSpell {
	bool ShouldCastNow(BoardStatus* boardState) override {

		bool Out = BurnSpell::ShouldCastNow(boardState);

		for (Parchment* parchment : boardState->hand) {
			if (parchment->spell->type == SpellTypes::VoidSyphon || parchment->spell->type == SpellTypes::KelphiorBlackBeam || parchment->spell->type == SpellTypes::Hellstorm) {
				if (!parchment->bReady) {
					return true;
				}
			}
		}

		return Out;
	}

	LightningBolt(Parchment* InParchment) {
		bReady = InParchment->bReady;
		spell = InParchment->spell;
		position = InParchment->position;
		bConfirmed = InParchment->bConfirmed;
		hysteresisCounter = InParchment->hysteresisCounter;

		readyPixelColor = InParchment->readyPixelColor;
		averageIconColor = InParchment->averageIconColor;
	}
};

struct Cantrip : public BurnSpell {
	bool ShouldCastNow(BoardStatus* boardState) override {
		
		return boardState->hand.size() < 5 || boardState->enemyHealth == HealthStatus::Frenzy;

	}

	Cantrip(Parchment* InParchment) {
		bReady = InParchment->bReady;
		spell = InParchment->spell;
		position = InParchment->position;
		bConfirmed = InParchment->bConfirmed;
		hysteresisCounter = InParchment->hysteresisCounter;

		readyPixelColor = InParchment->readyPixelColor;
		averageIconColor = InParchment->averageIconColor;
	}
};