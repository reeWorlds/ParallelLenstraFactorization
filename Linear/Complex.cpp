#include "Complex.h"

Complex::Complex()
{
	x = y = 0;
}

Complex::Complex(double _x, double _y) : x(_x), y(_y)
{

}

Complex Complex::operator +(Complex b)
{
	return Complex(x + b.x, y + b.y);
}

Complex Complex::operator -(Complex b)
{
	return Complex(x - b.x, y - b.y);
}

Complex Complex::operator *(Complex b)
{
	return Complex(x * b.x - y * b.y, x * b.y + y * b.x);
}

Complex Complex::conj()
{
	return Complex(x, -y);
}