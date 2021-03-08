#include "pch.h"
#include "ConsoleFormatter.h"
#include "Resources.h"
#include <conio.h>
#include <stdio.h>
#include <windows.h>
#include "Parchment.h"
#include <string>
#include "StatusEffect.h"
#include "BoardStatus.h"

void ConsoleFormatter::PrintAtCoord(int x, int y, std::string text, int lengthToSquash)
{
	Seek(x, y);

	for (int i = 0; i < lengthToSquash; i++) {
		std::cout << " ";
	}

	if (text != "") {
		Seek(x, y);
		std::cout << text;
	}

	Seek(0, 8);		//Always Seek back to the same location to not have a moving cursor on the screen and to have a specific location to print error messages
}

ConsoleFormatter::ConsoleFormatter()
{
	time(&secondStart);
}


ConsoleFormatter::~ConsoleFormatter()
{
}

void ConsoleFormatter::WriteStatusToScreen(BoardStatus* boardState)
{
	numIterationInSec++;
	time(&currentSecond);
	if (secondStart + 1 <= currentSecond) {
		PrintAtCoord(90, 0, std::to_string(numIterationInSec) + " FPS", 10);
		numIterationInSec = 0;
		time(&secondStart);
	}

	if (DEBUG_MEMORY) {
		PrintAtCoord(90, 2, "P. Created: " + std::to_string(DebugMemory::GetParchmentCreated()), 20);
		PrintAtCoord(90, 3, "P. Deleted: " + std::to_string(DebugMemory::GetParchmentDestroyed()), 20);
		PrintAtCoord(90, 4, "E. Created: " + std::to_string(DebugMemory::GetEffectsCreated()), 20);
		PrintAtCoord(90, 5, "E. Deleted: " + std::to_string(DebugMemory::GetEffectsDestroyed()), 20);
	}

	PrintAtCoord(0, 0, "Enemy: " + HealthStatusToText(boardState->enemyHealth), 23);
	PrintAtCoord(0, 1, "Player: " + HealthStatusToText(boardState->playerHealth), 23);

	PrintAtCoord(0, 2, "", 90);	//Crush any previously printed values for the hand.
	PrintAtCoord(0, 3, "", 90);

	for (unsigned int i = 0; i < boardState->hand.size(); i++) {
		if (!boardState->hand[i]->bConfirmed) {
			continue;
		}

		PrintAtCoord(0 + 25 * (boardState->hand[i]->position % 3),
			2 + (int)(boardState->hand[i]->position * 0.35),				//Rounding error here is better than waiting for division to happen since we know max hand is 6.
			//hand[i]->bReady ? "Ready" : "Charging(" + std::to_string((unsigned int)hand[i]->readyPixelColor) + ")" ,
			(boardState->hand[i]->bReady ? "R " : "C ") + boardState->hand[i]->spell->name + " (" + boardState->hand[i]->averageIconColor.ToString(true) + ")",
			24);
	}

	PrintAtCoord(0, 5, "", 60);	//Crush any previously printed values for the buffs.
	PrintAtCoord(0, 6, "", 60);

	for (unsigned int i = 0; i < boardState->effects.size(); i++) {
		PrintAtCoord(0 + 25 * (i % 3),
			5 + (int)(i * 0.35),
			//hand[i]->bReady ? "Ready" : "Charging(" + std::to_string((unsigned int)hand[i]->readyPixelColor) + ")" ,
			SpellTypeToText(boardState->effects[i]->effectType) + " " + std::to_string((int)(boardState->effects[i]->timeEnding - boardState->effects[i]->timeStored)),
			24);
	}
}

void ConsoleFormatter::Seek(int x, int y)
{
	COORD coord;
	coord.X = x;
	coord.Y = y;

	static const HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleCursorPosition(consoleHandle, coord);
}
