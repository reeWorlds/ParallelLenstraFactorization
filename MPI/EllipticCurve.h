#pragma once

#include "CrazyAlgorithms.h"
#include "EllipticPoint.h"

class CrazyAlgorithms::EllipticCurve
{
public:

	BigInt p;
	BigInt a, b;
	EllipticPoint point;



	EllipticCurve();
	EllipticCurve(BigInt _p, mt19937& rng);
	EllipticCurve(BigInt _p, BigInt _a, BigInt _b, EllipticPoint _point);

	EllipticPoint add(EllipticPoint p1, EllipticPoint p2);
	EllipticPoint mult(EllipticPoint p1, BigInt k);

	pair<pair<BigInt, BigInt>, BigInt> gcdExt(BigInt& a, BigInt& b); // x, y, gcd
	pair<EllipticPoint, int> addIfPossible(EllipticPoint& p1, EllipticPoint& p2);
	pair<EllipticPoint, int> multIfPossible(EllipticPoint& p1, BigInt k);

	pair<EllipticPoint, EllipticPoint> encrypt(EllipticPoint message, EllipticPoint publicKey);
	EllipticPoint decrypt(pair<EllipticPoint, EllipticPoint> text, BigInt privateKey);

	EllipticPoint getRandomPointOnCurve(int countRetry = 20);
};