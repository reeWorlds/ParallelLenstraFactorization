#include "EllipticCurve.h"

CrazyAlgorithms::EllipticCurve::EllipticCurve()
{
	p = (BigInt::pow(2, 128) - 3) / 76439;
	a = BigInt("4451685225093714772084598273548424");
	b = BigInt("2061118396808653202902996166388514");
	point = EllipticPoint(BigInt("188281465057972534892223778713752"), BigInt("3419875491033170827167861896082688"));
}

CrazyAlgorithms::EllipticCurve::EllipticCurve(BigInt _p, mt19937& rng)
{
	p = _p;

	a = BigInt::getRandomInteger(1, p - 1, rng);
	point.x = BigInt::getRandomInteger(1, p - 1, rng);
	point.y = BigInt::getRandomInteger(1, p - 1, rng);

	b = (point.y*point.y - (point.x*point.x*point.x + a*point.x)) % p;
}

CrazyAlgorithms::EllipticCurve::EllipticCurve(BigInt _p, BigInt _a, BigInt _b, EllipticPoint _point)
{
	p = _p;
	a = _a;
	b = _b;
	point = _point;
}

CrazyAlgorithms::EllipticPoint CrazyAlgorithms::EllipticCurve::add(EllipticPoint p1, EllipticPoint p2)
{
	if (p1.isZero)
	{
		return p2;
	}

	if (p2.isZero)
	{
		return p1;
	}

	if (p1 != p2 && p1.x == p2.x) {
		return EllipticPoint(true);
	}

	if (p1 == p2 && p1.y == 0)
	{
		return EllipticPoint(true);
	}

	BigInt lambda;
	if (p1 != p2 && p1.x != p2.x)
	{
		BigInt x2x1 = BigInt::minusMod(p2.x, p1.x, p);
		lambda = BigInt::divideMod(BigInt::minusMod(p2.y, p1.y, p), x2x1, p);
	}
	else
	{
		lambda = BigInt::divideMod(((p1.x * 3) % p*p1.x) % p + a, p1.y * 2, p);
	}

	BigInt xNew = (lambda*lambda - (p1.x + p2.x)) % p;
	BigInt yNew = (lambda*(p1.x - xNew) - p1.y) % p;

	return EllipticPoint(xNew, yNew);
}

pair<pair<BigInt, BigInt>, BigInt> CrazyAlgorithms::EllipticCurve::gcdExt(BigInt& a, BigInt& b)
{
	if (b == 0)
	{
		//cout << a.toString() << "\n";

		return{ {BigInt(1), BigInt(0)}, a };
	}

	BigInt q = a / b;
	BigInt r = a - b*q;

	pair<pair<BigInt, BigInt>, BigInt> subResult = gcdExt(b, r); // x, y, gcd

	return{ {subResult.first.second, subResult.first.first - q*subResult.first.second}, subResult.second };
}

pair<CrazyAlgorithms::EllipticPoint, int> CrazyAlgorithms::EllipticCurve::addIfPossible(EllipticPoint& p1, EllipticPoint& p2)
{
	if (p1.isZero)
	{
		return{ p2, 1 };
	}

	if (p2.isZero)
	{
		return{ p1, 1 };
	}

	if (p1 != p2 && p1.x == p2.x) {
		return{ EllipticPoint(true), 1 };
	}

	if (p1 == p2 && p1.y == 0)
	{
		return{ EllipticPoint(true), 1 };
	}

	BigInt nominator, denominator;
	if (p1 != p2 && p1.x != p2.x)
	{
		nominator = BigInt::minusMod(p2.y, p1.y, p);
		denominator = BigInt::minusMod(p2.x, p1.x, p);
	}
	else
	{
		nominator = (p1.x * 3 * p1.x + a) % p;
		denominator = p1.y * 2 % p;
	}

	pair<pair<BigInt, BigInt>, BigInt> inverse_gcd = gcdExt(denominator, p);

	if (inverse_gcd.second != 1)
	{
		return{ EllipticPoint(inverse_gcd.second , -1), 0 };
	}

	inverse_gcd.first.first %= p;

	BigInt xNew = (BigInt::sqr(nominator * inverse_gcd.first.first) - (p1.x + p2.x)) % p;
	BigInt yNew = (nominator * inverse_gcd.first.first * ((p1.x - xNew) % p) - p1.y) % p;

	return{ EllipticPoint(xNew, yNew), 1 };
}

pair<CrazyAlgorithms::EllipticPoint, int> CrazyAlgorithms::EllipticCurve::multIfPossible(EllipticPoint& p1, BigInt k)
{
	if (p1.isZero || k == 0)
	{
		return{ EllipticPoint(true), 1 };
	}

	if (k == 1)
	{
		return{ p1, 1 };
	}

	BigInt k2 = k;
	k2.divideBy2();

	pair<EllipticPoint, int> tmp = multIfPossible(p1, k2);
	if (tmp.second == 0)
	{
		return tmp;
	}
	pair<EllipticPoint, int> res = addIfPossible(tmp.first, tmp.first);
	if (res.second == 0)
	{
		return res;
	}

	if (k.m[0] % 2 == 1)
	{
		res = addIfPossible(p1, res.first);
	}

	return res;
}

CrazyAlgorithms::EllipticPoint CrazyAlgorithms::EllipticCurve::mult(EllipticPoint p1, BigInt k)
{
	if (p1.isZero || k == 0)
	{
		return EllipticPoint(true);
	}

	if (k == 1)
	{
		return p1;
	}

	BigInt k2 = k;
	k2.divideBy2();

	EllipticPoint tmp = mult(p1, k2);
	EllipticPoint res = add(tmp, tmp);

	if (k.m[0] % 2 == 1)
	{
		res = add(p1, res);
	}

	return res;
}

pair <CrazyAlgorithms::EllipticPoint, CrazyAlgorithms::EllipticPoint> CrazyAlgorithms::EllipticCurve::encrypt(EllipticPoint message, EllipticPoint publicKey)
{
	pair <EllipticPoint, EllipticPoint> result;

	BigInt randomValue = BigInt::getRandomInteger(0, p - 1, CrazyAlgorithms::rng);

	result.first = mult(point, randomValue);
	result.second = add(mult(publicKey, randomValue), message);

	return result;
}

CrazyAlgorithms::EllipticPoint CrazyAlgorithms::EllipticCurve::decrypt(pair<EllipticPoint, EllipticPoint> text, BigInt privateKey)
{
	EllipticPoint S = mult(text.first, privateKey);
	EllipticPoint message = add(text.second, EllipticPoint::minus(S));

	return message;
}

CrazyAlgorithms::EllipticPoint CrazyAlgorithms::EllipticCurve::getRandomPointOnCurve(int countRetry)
{
	if (countRetry == 0)
	{
		return EllipticPoint(-1, -1);
	}

	BigInt x = BigInt::getRandomInteger(0, p - 1, CrazyAlgorithms::rng);

	BigInt v = (BigInt::powMod(x, 3, p) + a* x % p + b) % p;

	auto y2 = cipollaAlgrithm(v, p);
	
	if (y2 != -1)
	{
		return EllipticPoint(x, y2);
	}
	else
	{
		return getRandomPointOnCurve(countRetry - 1);
	}
}