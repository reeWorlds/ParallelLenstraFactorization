#include "CrazyAlgorithms.h"
#include "EllipticCurve.h"

time_t getRandomTime()
{
	auto now = chrono::high_resolution_clock::now();

	auto duration = now.time_since_epoch();
	auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

	return time_t(millis);
}

mt19937 CrazyAlgorithms::rng = mt19937(getRandomTime());

BigInt CrazyAlgorithms::pollardRhoG(BigInt x, BigInt mod)
{
	return (x*x + 1) % mod;
}

pair<BigInt, BigInt> CrazyAlgorithms::mult(pair<BigInt, BigInt> a, pair<BigInt, BigInt> b, BigInt& c, BigInt& modulo)
{
	pair<BigInt, BigInt> res;

	res.first = a.first * b.first + c * a.second * b.second;
	res.second = a.first * b.second + a.second * b.first;

	res.first %= modulo;
	res.second %= modulo;

	return res;
}

pair<BigInt, BigInt> CrazyAlgorithms::sqr(pair<BigInt, BigInt> val, BigInt& c, BigInt& modulo)
{
	return mult(val, val, c, modulo);
}

pair<BigInt, BigInt> CrazyAlgorithms::powModPair(pair<BigInt, BigInt> val, BigInt &c, BigInt power, BigInt& modulo)
{
	if (power == 1)
	{
		return val;
	}

	if (power.m[0] % 2 == 0)
	{
		power.divideBy2();
		return sqr(powModPair(val, c, power, modulo), c, modulo);
	}
	else
	{
		power.divideBy2();
		return mult(sqr(powModPair(val, c, power, modulo), c, modulo), val, c, modulo);
	}
}

int CrazyAlgorithms::isPrimeSmall(int a)
{
	for (int i = 2; i * i <= a; i++)
	{
		if (a % 2 == 0)
		{
			return 0;
		}
	}

	return 1;
}

vector <BigInt> CrazyAlgorithms::getSmallPrimes(BigInt limit)
{
	vector <BigInt> primes;

	primes.push_back(2);

	for (int i = 3; BigInt(i) < limit; i += 2)
	{
		if (isPrimeSmall(i))
		{
			primes.push_back(i);
		}
	}

	return primes;
}

BigInt CrazyAlgorithms::pollardRhoAlgorithm(BigInt n)
{
	BigInt x = 2;
	BigInt y = 2;
	BigInt d = 1;

	while (d == 1)
	{
		x = pollardRhoG(x, n);
		y = pollardRhoG(pollardRhoG(y, n), n);

		d = BigInt::gcd((x - y).makeAbs(), n);
	}

	return d;
}

BigInt CrazyAlgorithms::giantStepBabyStep(BigInt a, BigInt b, BigInt p)
{
	BigInt blockSize = BigInt::sqrt(p);

	map<BigInt, BigInt> blockAns;
	
	BigInt subSuffix = BigInt::powMod(a, p - blockSize, p);
	if (subSuffix == b)
	{
		return p - blockSize;
	}
	blockAns[subSuffix] = p - blockSize;
	for (BigInt i = p - blockSize + 1; i < p; i += 1)
	{
		subSuffix *= a;
		subSuffix %= p;

		if (subSuffix == b)
		{
			return i;
		}

		blockAns[subSuffix] = i;
	}

	BigInt blockMultiplier = BigInt::powMod(a, blockSize, p);

	BigInt val = b;
	for (BigInt i = p - blockSize - blockSize; i >= 0; i -= blockSize)
	{
		val *= blockMultiplier;
		val %= p;

		if (blockAns.count(val) != 0)
		{
			return i + blockAns[val] - p + blockSize;
		}
	}

	return -1;
}

BigInt CrazyAlgorithms::eulerFunction(BigInt n)
{
	BigInt res = n;

	auto factors = factorize(n);

	for (auto factor : factors)
	{
		res = res / factor.first * (factor.first - 1);
	}

	return res;
}

BigInt CrazyAlgorithms::mobiousFunction(BigInt n)
{
	if (n == 1)
	{
		return 1;
	}

	auto factors = factorize(n);

	for (auto factor : factors)
	{
		if (factor.second > 1)
		{
			return 0;
		}
	}

	if (factors.size() % 2 == 0)
	{
		return 1;
	}
	else
	{
		return -1;
	}
}

BigInt CrazyAlgorithms::legendreSymbol(BigInt a, BigInt p)
{
	BigInt val = BigInt::powMod(a, (p - 1) / 2, p);

	if (val == (p - 1))
	{
		val = BigInt("-1");
	}

	return val;
}

BigInt CrazyAlgorithms::jacobiSymbol(BigInt a, BigInt n)
{
	auto factors = factorize(n);

	BigInt res = 1;

	for (auto factor : factors)
	{
		res *= BigInt::pow(legendreSymbol(a, factor.first), factor.second);
	}

	return res;
}

BigInt CrazyAlgorithms::cipollaAlgrithm(BigInt n, BigInt p)
{
	/*
	if (millerRabinTest(p) == false)
	{
		return BigInt("-1");
	}
	*/

	/*
	if (legendreSymbol(n, p) == -1)
	{
		return BigInt("-1");
	}
	*/

	if (n % p == 0)
	{
		return BigInt(0);
	}

	// step1
	BigInt a = 1;

	while (true)
	{
		BigInt y = (a * a - n) % p;

		if (legendreSymbol(y, p) == -1)
		{
			break;
		}

		a += 1;
	}

	// step2
	BigInt c = (a*a - n) % p;

	pair<BigInt, BigInt> exponent = powModPair({ a, 1 }, c, (p + 1) / 2, p);

	return exponent.first;
}

bool CrazyAlgorithms::millerRabinTest(BigInt n, BigInt k)
{
	if (n == 1)
	{
		return false;
	}

	if (n == 2)
	{
		return true;
	}

	if (n.m[0] % 2 == 0)
	{
		return false;
	}

	if (n <= 3)
	{
		return true;
	}

	BigInt d = n, r = 0;
	d -= 1;
	while (d.m[0] % 2 == 0)
	{
		d.divideBy2();
		r += 1;
	}

	while (k > 0)
	{
		k -= 1;

		BigInt a = BigInt::getRandomInteger(2, n - 2, rng);
		BigInt x = BigInt::powMod(a, d, n);

		if (x == 1 || x == (n - 1))
		{
			continue;
		}

		int continueLoop = 0;
		for (BigInt i = 0; i < r - 1; i += 1)
		{
			x = x*x % n;

			if (x == (n - 1))
			{
				continueLoop = 1;

				break;
			}
		}

		if (continueLoop == 1)
		{
			continue;
		}

		return false;
	}

	return true;
}

BigInt CrazyAlgorithms::lenstraAlgorithm(BigInt n, BigInt e)
{
	BigInt maxIter = 1;

	vector <BigInt> primes = getSmallPrimes(e);

	while (maxIter > 0)
	{
		maxIter -= 1;

		EllipticCurve curve(n, rng);

		EllipticPoint sumP(curve.point);

		for (BigInt prime : primes)
		{
			BigInt p = prime;

			while (p <= e)
			{
				pair<EllipticPoint, int> newPoint = curve.multIfPossible(sumP, prime);

				if (newPoint.second == 0)
				{
					return newPoint.first.x;
				}
				else
				{
					sumP = newPoint.first;
				}

				p *= prime;
			}
		}
	}

	return BigInt("-1");
}

vector<pair<BigInt, BigInt> > CrazyAlgorithms::factorize(BigInt n)
{
	if (n == 1)
	{
		return vector <pair<BigInt, BigInt> >();
	}

	vector <pair<BigInt, BigInt> > res;

	if (millerRabinTest(n) == true)
	{
		res.push_back({ n, BigInt(1) });

		return res;
	}

	vector <BigInt> primes; // {divisor, power}
	vector <BigInt> inf;

	inf.push_back(n);

	while (!inf.empty())
	{
		BigInt x = inf.back();
		inf.pop_back();

		BigInt a = pollardRhoAlgorithm(x), b = x / a;

		if (millerRabinTest(a) == true)
		{
			primes.push_back(a);
		}
		else
		{
			inf.push_back(a);
		}

		if (millerRabinTest(b) == true)
		{
			primes.push_back(b);
		}
		else
		{
			inf.push_back(b);
		}
	}

	sort(primes.begin(), primes.end());

	res.push_back({ primes[0], BigInt(1) });
	for (int i = 1; i < primes.size(); i++)
	{
		if (primes[i] == res.back().first)
		{
			res.back().second += 1;
		}
		else
		{
			res.push_back({ primes[i], BigInt(1) });
		}
	}

	return res;
}