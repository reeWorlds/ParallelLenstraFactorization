#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>
#include <algorithm>
#include <random>

#include "Complex.h"

using namespace std;

#define standartBase 100000000
#define standartBaseSize 8

class BigInt
{
	int base;
	int baseSize;

	int p;
	vector <int> m;

	void normalize();

	
	static int getBaseBySize(int sz);

	void setBaseSize1();
	void setBaseSize(int newBaseSize);

	static int compareAbs(const BigInt& a, const BigInt& b); // < -1, = 0, > 1


	static int FFTBase;
	static vector<Complex> roots;
	static vector<int> rev;
	void ensure_FFTBase(int nFFTBase) const;
	void fft(vector<Complex>& a, int n = -1) const;
	void multiplyFFT(vector <int> &a, vector <int> &b, vector <int> &res) const;

	void divideBy2();
	void multiplyBy2();

	static BigInt sqr(const BigInt& val);

	static BigInt powModPrivate(const BigInt& val, const BigInt& power, const BigInt& mod);

	static BigInt sqrtBin(const BigInt& l, const BigInt& r, const BigInt& val);

public:

	BigInt();
	BigInt(long long a);
	BigInt(const string& a);


	BigInt makeAbs();

	
	bool operator ==(const BigInt& other) const;
	bool operator !=(const BigInt& other) const;
	bool operator <(const BigInt& other) const;
	bool operator >(const BigInt& other) const;
	bool operator <=(const BigInt& other) const;
	bool operator >=(const BigInt& other) const;

	BigInt operator -()const;
	BigInt operator +(const BigInt& other) const;
	BigInt operator -(const BigInt& other) const;
	
	BigInt& operator +=(const BigInt& other);
	BigInt& operator -=(const BigInt& other);


	BigInt multQuadtaric(const BigInt& other) const;
	BigInt operator*(const BigInt& other) const;
	BigInt operator/(const BigInt& other) const;
	BigInt operator%(const BigInt& other) const;

	BigInt& operator*=(const BigInt& other);
	BigInt& operator/=(const BigInt& other);
	BigInt& operator%=(const BigInt& other);


	static BigInt addMod(const BigInt& a, const BigInt& b, const BigInt& mod);
	static BigInt minusMod(const BigInt& a, const BigInt& b, const BigInt& mod);
	static BigInt multiplyMod(const BigInt& a, const BigInt& b, const BigInt& mod);
	static BigInt getInverseMod(const BigInt& a, const BigInt& mod);
	static BigInt divideMod(const BigInt& a, const BigInt& b, const BigInt& mod);


	static BigInt pow(const BigInt& val, const BigInt& power);
	static BigInt powMod(const BigInt& val, const BigInt& power, const BigInt& mod);

	static BigInt sqrt(const BigInt& val);
	static BigInt abs(const BigInt& val);

	static BigInt gcd(BigInt a, BigInt b);

	static BigInt getRandomInteger(BigInt l, BigInt r, mt19937& rng);

	BigInt getLog2();

	string toString() const;

	friend class CrazyAlgorithms;
};