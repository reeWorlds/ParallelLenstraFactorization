#pragma once

#include "CrazyAlgorithms.h"

class CrazyAlgorithms::EllipticPoint
{
public:

	bool isZero;
	BigInt x;
	BigInt y;

	EllipticPoint();
	EllipticPoint(bool isZero);
	EllipticPoint(BigInt _x, BigInt _y);
	
	static EllipticPoint minus(EllipticPoint p);

	bool operator == (const EllipticPoint& other);
	bool operator != (const EllipticPoint& other);

};