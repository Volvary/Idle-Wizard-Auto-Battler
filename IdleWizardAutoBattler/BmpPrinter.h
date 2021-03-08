#pragma once

#include <Windows.h>
#include <stdio.h>
#include <assert.h>

class BmpPrinter
{
	PBITMAPINFO CreateBitmapInfoStruct(HBITMAP);

public:
	BmpPrinter();
	~BmpPrinter();

	void CreateBMPFile(LPTSTR pszFile, HBITMAP hBMP);
};

