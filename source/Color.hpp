#pragma once

#include <3ds.h>

struct FColor
{
	FColor (u8 r, u8 g, u8 b, u8 a = 255) : r(r), g(g), b(b), a(a) {};
	u8 r;
	u8 g;
	u8 b;
	u8 a;
};
