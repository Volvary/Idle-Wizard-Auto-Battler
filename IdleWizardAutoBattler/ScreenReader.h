#pragma once

#include <vector>
#include <windows.h>
#include "BmpPrinter.h"
#include "Resources.h"

struct Parchment;
class StatusEffect;

#define HEALTHBAR_REDTHRESHOLD 50
#define PARCHMENT_READYTHRESHOLD 100
#define SPELLDETECTION_THRESHOLD 15
#define COMBAT_TEST_THRESHOLD 5
#define VICTORY_TEST_THRESHOLD 5
#define LOBBY_TEST_THRESHOLD 5

//** This will be 
class ScreenReader
{
	BmpPrinter bmpPrinter;

	HWND idleWizardHWND;

	//Last snapshot data.
	HDC capturedDC;

	std::vector<Parchment*> parchmentsInHand;
	std::vector<StatusEffect*> activeBuffs;

public:
	ScreenReader();
	~ScreenReader();

	void LoadScreenToBuffer();

	Pixel GetPixelAtCoord(int x, int y);
	bool GetAveragePixelAtCoords(int x1, int y1, int x2, int y2, Pixel &Out);

	HealthStatus GetEnemyHealth();
	HealthStatus GetPlayerHealth();

	const std::vector<Parchment*> GetParchmentsInHand();
	const std::vector<StatusEffect*> GetActiveStatus() { return activeBuffs; }
	Spell* GetSpellByClosestIconColor(Pixel averageIconPixel, bool bReady);

	void TickStatus();

	/** Screen Values*/
	const short EnemyHealthBarLeft = 765;
	const short EnemyHealthBarRight = 1215;
	const short EnemyHealthBarReadLine = 16;	//The pixel line read to see the health.
	const short EnemyHealthBarLength = (EnemyHealthBarRight - EnemyHealthBarLeft);
	const float FrenzyPercentage = 0.20f;		//The percentage at which we start offloading on the enemy to finish the fight.

	const short PlayerHealthBarLeft = 725;
	const short PlayerHealthBarRight = 1215;
	const short PlayerHealthBarLength = (PlayerHealthBarRight - PlayerHealthBarLeft);
	const short PlayerHealthBarReadLine = 989;	//The pixel line read to see the health.
	const float PlayerPanicPercentage = 0.15f;	//Similar to Frenzy, the percentage at which to start panicking and protecting our health.

	const short FirstSpellParchmentPoint = 730;
	const float ParchmentPointJumps = 77.6f;
	const short ParchmentReadyLine = 873;
	const std::vector<short> SpellIconCenter = { 763 , 839, 913, 992, 1069, 1146 };
	const short ParchmentIconLine = 909;
	const short ParchmentIconAverageSize = 9;

	const std::vector<short> BuffImageCorner = { 767, 795, 824, 852, 881 };
	const short BuffDetectionOffset = 10;
	const short BuffDetectionLine = 832;
	const short BuffNearlyDoneOffset = 14;
	const short BuffNearlyDoneSize = 2;

	const short HealthPotionSelectionPoint = 1450;
	const short HealthPotionSelectionLine = 710;

	const short VictoryCornerPoint = 1075;
	const short VictoryCornerLine = 925;
	const short VictorySamplingSize = 5;
	const Pixel ContinueSampleAverage = Pixel(177, 177, 177);
	const Pixel BackSampleAverage = Pixel(158, 158, 158);

	const Pixel GolemFaceAverage = Pixel(195, 145, 144);
	static Pixel GetHealthPotionPosition();
private:
	Parchment* ConfirmParchmentSubtype(Parchment* current);
public:
	bool IsInCombat();
	bool IsWaitingOnVictoryScreen(bool& OutStageCompleted);

	void EmptyLists();

	static Pixel GetClickLocationPerSpell(Parchment* parchment);
	static bool RegisterStatusEffect(StatusEffect* effect);
};

