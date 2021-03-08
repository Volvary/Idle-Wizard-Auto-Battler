// IdleWizardAutoBattler.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include "AutoBattler.h"

int main()
{
	AutoBattler* autoBattler = new AutoBattler();

	autoBattler->Init();

	delete autoBattler;
}