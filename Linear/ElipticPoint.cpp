#include "EllipticPoint.h"

CrazyAlgorithms::EllipticPoint::EllipticPoint()
{
	isZero = false;

	x = 0;
	y = 0;
}

CrazyAlgorithms::EllipticPoint::EllipticPoint(bool _isZero)
{
	isZero = isZero;

	x = 0;
	y = 0;
}

CrazyAlgorithms::EllipticPoint::EllipticPoint(BigInt _x, BigInt _y)
{
	isZero = false;

	x = _x;
	y = _y;
}

CrazyAlgorithms::EllipticPoint CrazyAlgorithms::EllipticPoint::minus(EllipticPoint p)
{
	if (p.isZero == true)
	{
		return p;
	}
	else
	{
		return EllipticPoint(p.x, -p.y);
	}
}

bool CrazyAlgorithms::EllipticPoint::operator == (const EllipticPoint& other)
{
	if (isZero == other.isZero)
	{
		if (x == other.x && y == other.y)
		{
			return true;
		}

		return false;
	}

	return false;
}

bool CrazyAlgorithms::EllipticPoint::operator != (const EllipticPoint& other)
{
	return !((*this) == other);
}