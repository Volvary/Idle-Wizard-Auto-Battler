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
#include <algorithm>

void ConsoleFormatter::PrintAtCoord(int x, int y, std::string text, int previousTextLength)
{
	Seek(x, y);

	if (text != "") {
		std::cout << text;
	}
	
	int squashLength = previousTextLength + 1 - text.size();
	std::cout << std::string(squashLength > 0 ? squashLength : 0, ' ');

	//Seek(0, 8);		//Always Seek back to the same location to not have a moving cursor on the screen and to have a specific location to print error messages
}

void ConsoleFormatter::AddEventToLog(std::string text)
{
	for (int i = eventLog.size() -1; i >= 0; i--) {
		if (!eventLog.size()) {
			break;
		}
		else if (i + 1 >= maxEventLogLength) {
			continue;
		}

		PrintAtCoord(0, 10 + i + 1, eventLog[i], eventLog.size() > i+1 ? eventLog[i + 1].size() : 0);
		if (eventLog.size() >= maxEventLogLength && i+1 < eventLog.size()) {
			eventLog[i+1] = eventLog[i];
		}
	}
	PrintAtCoord(0, 10, text, eventLog.size() ? eventLog[0].size() : 0);
	if (eventLog.size() < maxEventLogLength) {
		eventLog.insert(eventLog.begin(), text);
	}
	else {
		eventLog[0] = text;
	}

}

void ConsoleFormatter::RoundFinished(bool bExpeditionDone)
{
	for (int i = 0; i < 6; i++) {
		previousState->parchmentScreenText[i] = "";
	}

	for (int i = 0; i < previousState->effectsScreenText.size(); i++) {
		PrintAtCoord(73, 2 + i, "", previousState->effectsScreenText[i].size());
	}
	previousState->effectsScreenText.clear();

	std::string type = bExpeditionDone ? "Expedition" : "Round";
	AddEventToLog(type + " completed.");

	PrintAtCoord(0, 2, "", 70);
	PrintAtCoord(0, 3, "", 70);

}

ConsoleFormatter::ConsoleFormatter()
{
	time(&secondStart);
	previousState = new ConsoleBoardStatus();
	
	for (int i = 0; i < 6; i++) {
		previousState->parchmentScreenText.push_back("");
	}
}


ConsoleFormatter::~ConsoleFormatter()
{
	delete previousState;
	previousState = nullptr;
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

	std::vector<int> foundScrolls;

	for (unsigned int i = 0; i < boardState->hand.size(); i++) {
		if (!boardState->hand[i]->bConfirmed) {
			continue;
		}

		foundScrolls.push_back(boardState->hand[i]->position);

		std::string textToType = (boardState->hand[i]->bReady ? "R " : "C ") + boardState->hand[i]->spell->name + " (" + PlayStatusToText(boardState->hand[i]->status) + ")";

		if (textToType != previousState->parchmentScreenText[i]) {
			PrintAtCoord(0 + 25 * (boardState->hand[i]->position % 3),
				2 + (int)(boardState->hand[i]->position * 0.35),				//Rounding error here is better than waiting for division to happen since we know max hand is 6.
				textToType,	previousState->parchmentScreenText[boardState->hand[i]->position].size());

			previousState->parchmentScreenText[i] = textToType;
		}
		
	}

	std::sort(foundScrolls.begin(), foundScrolls.end());

	int arrayIterator = 0;
	for (unsigned int i = 0; i < 6; i++) {
		if (!foundScrolls.size()) {
			break;
		}
		
		if (foundScrolls.size() > arrayIterator && foundScrolls[arrayIterator] == i) {
			arrayIterator++;
			continue;
		}

		if (previousState->parchmentScreenText[i] != "") {
			PrintAtCoord(0 + 25 * (i % 3),
				2 + (int)(i * 0.35),
				"", previousState->parchmentScreenText[i].size() + 1);

			previousState->parchmentScreenText[i] = "";
		}
		
	}

	for (unsigned int i = 0; i < boardState->effects.size(); i++) {
		
		std::string textToPrint = SpellTypeToText(boardState->effects[i]->effectType) + " " + std::to_string((int)(boardState->effects[i]->timeEnding - boardState->effects[i]->timeStored));
		
		if (textToPrint != (previousState->effectsScreenText.size() < i ? previousState->effectsScreenText[i] : "")) {

			PrintAtCoord(73, 2 + i, textToPrint, 16);

			if (previousState->effectsScreenText.size() < i) {
				previousState->effectsScreenText[i] = textToPrint;
			}
			else {
				previousState->effectsScreenText.push_back(textToPrint);
			}
		}
	}


	for (int i = previousState->effectsScreenText.size() - 1; i > boardState->effects.size() - 1; i--) {
		PrintAtCoord(73, 2 + i, "", previousState->effectsScreenText[i].size());

		previousState->effectsScreenText.pop_back();
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
