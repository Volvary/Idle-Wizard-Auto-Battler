#pragma once
#include <stdint.h>
#include <string>

struct Pixel {
	uint32_t X;
	uint32_t Y;
	uint32_t R;
	uint32_t G;
	uint32_t B;
	uint32_t Count;

	Pixel() {
		X = Y = 0;
		R = G = B = 0;
		Count = 0;
	}

	Pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, int count){
		X = x;
		Y = y;
		R = r;
		G = g;
		B = b;
		Count = count;
	}

	Pixel(int r, int g, int b) {
		R = r;
		G = g;
		B = b;
		X = Y = Count = 0;
	}

	Pixel(int x, int y) {
		X = x;
		Y = y;
		R = G = B = 0;
		Count = 0;
	}

	std::string ToString(bool bSimplify = false)
	{
		char buffer[50];
		if (!bSimplify) {
			sprintf_s(buffer, "%d,%d : %u, %u, %u, %u", X, Y, R, G, B, Count);
		}
		else {
			sprintf_s(buffer, "%u, %u, %u",R, G, B);
		}

		return std::string(buffer);
	}

	void Average() {
		X /= Count;
		Y /= Count;
		R /= Count;
		G /= Count;
		B /= Count;
		Count = 1;
	}

	Pixel operator+=(Pixel other) {
		X += other.X;
		Y += other.Y;
		R += other.R;
		G += other.G;
		B += other.B;
		Count += other.Count;
		return *this;
	}

	bool operator==(Pixel other) {
		return (R == other.R && G == other.G && B == other.G);
	}

#pragma warning(push)
#pragma warning(disable : 4244)
	Pixel operator *(float f) {
		return Pixel(R * f, G * f, B * f);
	}
#pragma warning(pop)
};