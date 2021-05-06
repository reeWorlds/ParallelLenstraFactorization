#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <set>
#include <map>
#include <unordered_map>
#include <queue>
#include <deque>
#include <cmath>
#include <math.h>
#include <string>
#include <random>
#include <memory.h>
#include <bitset>
#include <chrono>
#include <time.h>
#include <mutex>

#include "BigInt.h"

using namespace std;


class CrazyAlgorithms
{	
	static BigInt pollardRhoG(BigInt x, BigInt mod);

	static pair<BigInt, BigInt> mult(pair<BigInt, BigInt> a, pair<BigInt, BigInt> b, BigInt& c, BigInt& modulo);
	static pair<BigInt, BigInt> sqr(pair<BigInt, BigInt> val, BigInt& c, BigInt& modulo);
	static pair<BigInt, BigInt> powModPair(pair<BigInt, BigInt> val, BigInt &c, BigInt power, BigInt& modulo);

	static int isPrimeSmall(int a);
	static vector <BigInt> getSmallPrimes(BigInt limit);

public:

	static mt19937 rng;
	static mutex mux;

	class EllipticPoint;
	class EllipticCurve;

	static BigInt pollardRhoAlgorithm(BigInt n); // return 1 factor of number

	static BigInt giantStepBabyStep(BigInt a, BigInt b, BigInt p);

	static BigInt eulerFunction(BigInt n);
	static BigInt mobiousFunction(BigInt n);
	
	static BigInt legendreSymbol(BigInt a, BigInt p);
	static BigInt jacobiSymbol(BigInt a, BigInt n);

	static BigInt cipollaAlgrithm(BigInt n, BigInt p); // -1 if n is not square of some number or p is not prime

	static bool millerRabinTest(BigInt n, BigInt k = 40); // true if prime


	static BigInt lenstraAlgorithm(BigInt n, BigInt e = 700);


	static vector<pair<BigInt, BigInt> > factorize(BigInt n);

};