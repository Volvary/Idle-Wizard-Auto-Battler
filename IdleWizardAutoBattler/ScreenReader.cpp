#include "pch.h"
#include "ScreenReader.h"

#include <d3d9helper.h>
#include <cstdio>
#include <windows.h>
#include <tlhelp32.h>
#include "Resources.h"
#include "Parchment.h"
#include "StatusEffect.h"

static ScreenReader* Instance = nullptr;

ScreenReader::ScreenReader()
{
	idleWizardHWND = FindWindow(NULL, L"IdleWizard");

	Instance = this;
}


ScreenReader::~ScreenReader()
{
	DeleteDC(capturedDC);

	EmptyLists();

	DeleteSpellList();
}

void GetImageDataFromHBITMAP(HDC hdc, HBITMAP bitmap, BYTE* pImageData)
{
	BITMAPINFO bmpInfo;
	memset(&bmpInfo, 0, sizeof(BITMAPINFOHEADER));
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	GetDIBits(hdc, bitmap, 0, 0, NULL, &bmpInfo, DIB_RGB_COLORS);
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biCompression = BI_RGB;
	GetDIBits(hdc, bitmap, 0, bmpInfo.bmiHeader.biHeight, pImageData, &bmpInfo, DIB_RGB_COLORS);
}

void ScreenReader::LoadScreenToBuffer()
{
	DeleteDC(capturedDC);

	static const int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	static const int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	BringWindowToTop(idleWizardHWND);

	HDC hDesktopDC = GetDC(idleWizardHWND);
	HDC hCaptureDC = CreateCompatibleDC(hDesktopDC);
	BitBlt(hCaptureDC, 0, 0, nScreenWidth, nScreenHeight,
		hDesktopDC, 0, 0, SRCCOPY | CAPTUREBLT);

	capturedDC = hCaptureDC;
	
	{
		HBITMAP hCaptureBitmap = CreateCompatibleBitmap(hDesktopDC, nScreenWidth, nScreenHeight);
		SelectObject(hCaptureDC, hCaptureBitmap);
		BitBlt(hCaptureDC, 0, 0, nScreenWidth, nScreenHeight,
			hDesktopDC, 0, 0, SRCCOPY | CAPTUREBLT);
		//bmpPrinter.CreateBMPFile(LPTSTR(L"LastFrame.bmp"), hCaptureBitmap);

		DeleteObject(hCaptureBitmap);
	}

	ReleaseDC(idleWizardHWND, hDesktopDC);
}

Pixel ScreenReader::GetPixelAtCoord(int x, int y)
{
	COLORREF Ref = GetPixel(capturedDC, x, y);
	return Pixel(x, y, GetRValue(Ref), GetGValue(Ref), GetBValue(Ref), 1);
}

bool ScreenReader::GetAveragePixelAtCoords(int x1, int y1, int x2, int y2, Pixel &Out)
{
	if (x1 > x2 || y1 > y2) {
		return false;
	}

	Out = Pixel();

	for (int i = x1; i < x2; i++) {
		for (int k = y1; k < y2; k++) {
			Out += GetPixelAtCoord(i, k);
		}
	}

	Out.Average();

	return true;
}

HealthStatus ScreenReader::GetEnemyHealth()
{
	HealthStatus Out = HealthStatus::Unknown;

	if (GetPixelAtCoord(EnemyHealthBarLeft + (int)(EnemyHealthBarLength * 0.75), EnemyHealthBarReadLine).R >= HEALTHBAR_REDTHRESHOLD) {
		Out = HealthStatus::NearFull;
	} 
	else if (GetPixelAtCoord(EnemyHealthBarLeft + (int)(EnemyHealthBarLength * 0.5), EnemyHealthBarReadLine).R >= HEALTHBAR_REDTHRESHOLD) {
		Out = HealthStatus::ThreeQuarter;
	}
	else if (GetPixelAtCoord(EnemyHealthBarLeft + (int)(EnemyHealthBarLength * 0.25), EnemyHealthBarReadLine).R >= HEALTHBAR_REDTHRESHOLD) {
		Out = HealthStatus::Half;
	}
	else if (GetPixelAtCoord(EnemyHealthBarLeft + (int)(EnemyHealthBarLength * FrenzyPercentage), EnemyHealthBarReadLine).R >= HEALTHBAR_REDTHRESHOLD) {
		Out = HealthStatus::Quarter;
	}
	else if (GetPixelAtCoord(EnemyHealthBarLeft + (int)(EnemyHealthBarLength * FrenzyPercentage), EnemyHealthBarReadLine).R < HEALTHBAR_REDTHRESHOLD) {
		Out = HealthStatus::Frenzy;
	}

	return Out;
}

HealthStatus ScreenReader::GetPlayerHealth()
{
	HealthStatus Out = HealthStatus::Unknown;

	if (GetPixelAtCoord(PlayerHealthBarLeft + (int)(PlayerHealthBarLength * 0.75), PlayerHealthBarReadLine).R >= HEALTHBAR_REDTHRESHOLD) {
		Out = HealthStatus::NearFull;
	}
	else if (GetPixelAtCoord(PlayerHealthBarLeft + (int)(PlayerHealthBarLength * 0.5), PlayerHealthBarReadLine).R >= HEALTHBAR_REDTHRESHOLD) {
		Out = HealthStatus::ThreeQuarter;
	}
	else if (GetPixelAtCoord(PlayerHealthBarLeft + (int)(PlayerHealthBarLength * 0.25), PlayerHealthBarReadLine).R >= HEALTHBAR_REDTHRESHOLD) {
		Out = HealthStatus::Half;
	}
	else if (GetPixelAtCoord(PlayerHealthBarLeft + (int)(PlayerHealthBarLength * PlayerPanicPercentage), PlayerHealthBarReadLine).R >= HEALTHBAR_REDTHRESHOLD) {
		Out = HealthStatus::Quarter;
	}
	else if (GetPixelAtCoord(PlayerHealthBarLeft + (int)(PlayerHealthBarLength * PlayerPanicPercentage), PlayerHealthBarReadLine).R < HEALTHBAR_REDTHRESHOLD) {
		Out = HealthStatus::Panic;
	}

	return Out;
}

const std::vector<Parchment*> ScreenReader::GetParchmentsInHand()
{	
	for (int i = 0; i < 6; i++) {
		Parchment* current = nullptr;
		for (Parchment* parchment : parchmentsInHand) {
			if (parchment->position == i) {
				current = parchment;
			}
		}
		
		Pixel averagePixel;
		bool bSuccess = GetAveragePixelAtCoords(FirstSpellParchmentPoint + (int)(ParchmentPointJumps * i), ParchmentReadyLine, 
													(FirstSpellParchmentPoint + (int)(ParchmentPointJumps * i)) + 2, ParchmentReadyLine + 2, averagePixel);
		
		if (averagePixel.B == 0) {
			if (current && current->position != -1) {
				parchmentsInHand.erase(std::find(parchmentsInHand.begin(), parchmentsInHand.end(), current));
				delete current;

				if (DEBUG_MEMORY) {
					DebugMemory::IncrementParchmentDestroyed();
				}
			}
		}
		else {
			if (!current) {
				current = new Parchment();
				
				if (DEBUG_MEMORY) {
					DebugMemory::IncrementParchmentCreated();
				}

				current->position = i;
				parchmentsInHand.push_back(current);
			}

			{
				Pixel averageIconPixel = Pixel();
				GetAveragePixelAtCoords(SpellIconCenter[i] - (int)floor(ParchmentIconAverageSize * 0.5f), ParchmentIconLine - (int)floor(ParchmentIconAverageSize * 0.5f),
					SpellIconCenter[i] + (int)ceil(ParchmentIconAverageSize * 0.5f), ParchmentIconLine + (int)ceil(ParchmentIconAverageSize * 0.5f),
					averageIconPixel);
				current->averageIconColor = averageIconPixel;

				//If we just confirmed, check the spell type
				if (current->SetSpell(GetSpellByClosestIconColor(averageIconPixel, averagePixel.B > 100))) {
					Parchment* replacement = ConfirmParchmentSubtype(current);
					if (replacement) {
						parchmentsInHand.erase(std::find(parchmentsInHand.begin(), parchmentsInHand.end(), current));
						parchmentsInHand.push_back(replacement);
						delete current;

						if (DEBUG_MEMORY) {
							DebugMemory::IncrementParchmentDestroyed();
						}

						i--;
						continue;
					}
				}
			}

			current->readyPixelColor = averagePixel.B;

			current->bReady = averagePixel.B > 100;
		}
	}

	return (const std::vector<Parchment*>) parchmentsInHand;
}

Spell* ScreenReader::GetSpellByClosestIconColor(Pixel averageIconPixel, bool bReady)
{
	std::vector<Spell*> closest = std::vector<Spell*>();
	int closestTotalRGBDifference = INT_MAX;

	const std::vector<Spell*> Spells = GetSpellList();

	
	for (Spell* spell : Spells) {
		Pixel averageToCompare = bReady ? spell->averageIconColor : spell->averageChargingIconColor;

		int TotalRGBDifference = abs((int)(averageIconPixel.R - averageToCompare.R)) +
										abs((int)(averageIconPixel.G - averageToCompare.G)) +
										abs((int)(averageIconPixel.B - averageToCompare.B));
		if (closestTotalRGBDifference > TotalRGBDifference) {
			if (closestTotalRGBDifference > SPELLDETECTION_THRESHOLD) {
				closest.clear();
			}
			closest.push_back(spell);
			closestTotalRGBDifference = TotalRGBDifference;
		}
	}

	return closest.size() == 1 ? closest[0] : Spells[0];
}

void ScreenReader::TickStatus()
{
	for (unsigned int i = 0; i < activeBuffs.size(); i++) {
		if (activeBuffs[i]->HasEnded()) {
			delete activeBuffs[i];
			activeBuffs.erase(activeBuffs.begin()+i);
			i--;

			if (DEBUG_MEMORY) {
				DebugMemory::IncrementEffectsDestroyed();
			}

			continue;
		}
	}
}

Pixel ScreenReader::GetHealthPotionPosition()
{
	return Pixel(Instance->HealthPotionSelectionPoint, Instance->HealthPotionSelectionLine);
}

Parchment* ScreenReader::ConfirmParchmentSubtype(Parchment* current)
{
	Parchment* replacement = nullptr;

	switch (current->spell->type)
	{
	case SpellTypes::Counterspell:
	case SpellTypes::Debilitate:
	case SpellTypes::FireBall:
	case SpellTypes::Hellstorm:
	case SpellTypes::KelphiorBlackBeam:
	case SpellTypes::MagicMissile:
	case SpellTypes::VoidBolt:
		replacement = new BurnSpell(current);
		break;
	case SpellTypes::LightningBolt:
		replacement = new LightningBolt(current);
		break;
	case SpellTypes::SpellFocus:
		replacement = new Cantrip(current);
		break;
	case SpellTypes::MagicWeapon:
	case SpellTypes::VoidSyphon:
		replacement = new DamageBuff(current);
		break;
	default:
		break;
	}

	if (replacement && DEBUG_MEMORY) {
		DebugMemory::IncrementParchmentCreated();
	}

	return replacement;
}

bool ScreenReader::IsInCombat()
{
	Pixel GolemFace;
	bool bSuccess = GetAveragePixelAtCoords(520, 0, 545, 10, GolemFace);

	//std::cout << GolemFace.R << " " << GolemFace.G << " " << GolemFace.B << "    " << GolemFaceAverage.R << " " << GolemFaceAverage.G << " " << GolemFaceAverage.B;
	
	return (abs((int)(GolemFace.R - GolemFaceAverage.R)) + abs((int)(GolemFace.G - GolemFaceAverage.G)) + abs((int)(GolemFace.B - GolemFaceAverage.B)) < COMBAT_TEST_THRESHOLD);
}

bool ScreenReader::IsWaitingOnVictoryScreen(bool& OutStageCompleted)
{
	Pixel ContinueBox;
	bool bSuccess = GetAveragePixelAtCoords(VictoryCornerPoint - VictorySamplingSize, VictoryCornerLine - VictorySamplingSize,
		VictoryCornerPoint + VictorySamplingSize, VictoryCornerLine + VictorySamplingSize, ContinueBox);

	bool bOut = false;

	if (bSuccess) {
		int continueAverage = abs((int)(ContinueBox.R - ContinueSampleAverage.R)) + abs((int)(ContinueBox.G - ContinueSampleAverage.G)) + abs((int)(ContinueBox.B - ContinueSampleAverage.B));
		if(continueAverage < VICTORY_TEST_THRESHOLD){
			OutStageCompleted = false;
			bOut = true;
		}
		int backToLobby = abs((int)(ContinueBox.R - BackSampleAverage.R)) + abs((int)(ContinueBox.G - BackSampleAverage.G)) + abs((int)(ContinueBox.B - BackSampleAverage.B));

		if (backToLobby < LOBBY_TEST_THRESHOLD) {
			OutStageCompleted = true;
			bOut = true;
		}
	}

	if (bOut == true) {
		EmptyLists();
		activeBuffs.clear();
		parchmentsInHand.clear();
	}

	return bOut;
}

void ScreenReader::EmptyLists()
{
	while (parchmentsInHand.size() > 0) {
		delete parchmentsInHand.back();
		parchmentsInHand.pop_back();

		if (DEBUG_MEMORY) {
			DebugMemory::IncrementParchmentDestroyed();
		}
	}

	while (activeBuffs.size() > 0) {
		delete activeBuffs.back();
		activeBuffs.pop_back();

		if (DEBUG_MEMORY) {
			DebugMemory::IncrementEffectsDestroyed();
		}
	}
}

Pixel ScreenReader::GetClickLocationPerSpell(Parchment* parchment)
{
	return Pixel(Instance->SpellIconCenter[parchment->position], (int)Instance->ParchmentReadyLine);
}

bool ScreenReader::RegisterStatusEffect(StatusEffect* effect)
{
	if (Instance) {
		Instance->activeBuffs.push_back(effect);
		return true;
	}
	return false;
}
