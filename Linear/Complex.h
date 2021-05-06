#pragma once

class Complex
{
public:

	double x, y;

	Complex();
	Complex(double _x, double _y);

	Complex operator +(Complex b);
	Complex operator -(Complex b);
	Complex operator *(Complex b);
	Complex conj();
};