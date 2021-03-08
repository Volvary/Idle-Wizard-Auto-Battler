#pragma once

#include "Resources.h"

struct Parchment;
class StatusEffect;
struct BoardStatus;

class ConsoleFormatter
{
	int numIterationInSec = 0;
	float iterationLastSec = 0.0f;
	time_t currentSecond;
	time_t secondStart;


public:
	ConsoleFormatter();
	~ConsoleFormatter();

	void WriteStatusToScreen(BoardStatus* boardState);

	void Seek(int x, int y);
	void PrintAtCoord(int x, int y, std::string text, int lengthToSquash);

};

