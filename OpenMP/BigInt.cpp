#include "BigInt.h"

void BigInt::normalize()
{
	while (m.size() > 1 && m.back() == 0)
	{
		m.pop_back();
	}

	if (m.size() == 1 && m[0] == 0 && p == 0)
	{
		p = 1;
	}
}

int BigInt::getBaseBySize(int sz)
{
	int res = 1;
	for (int i = 1; i <= sz; i++)
	{
		res *= 10;
	}

	return res;
}

void BigInt::setBaseSize1()
{
	vector <int> newM;

	for (auto it : m)
	{
		for (int i = 0; i < baseSize; i++)
		{
			newM.push_back(it % 10);
			it /= 10;
		}
	}

	m = newM;

	normalize();

	baseSize = 1;
	base = 10;
}

void BigInt::setBaseSize(int newBaseSize)
{
	if (baseSize != 1)
	{
		setBaseSize1();
	}

	int newBase = getBaseBySize(newBaseSize);
	vector<int> newM;

	for (int i = 0, j = 1; i < m.size(); i++)
	{
		if (j == 1)
		{
			newM.push_back(0);
		}

		newM.back() += j * m[i];
		j *= 10;
		if (j == newBase)
		{
			j = 1;
		}
	}

	m = newM;
	base = newBase;
	baseSize = newBaseSize;

	normalize();
}

int BigInt::compareAbs(const BigInt& a, const BigInt& b)
{
	// < -1, == 0, > 1

	_ASSERT(a.baseSize == b.baseSize);

	if (a.m.size() < b.m.size())
	{
		return -1;
	}
	else if (a.m.size() > b.m.size())
	{
		return 1;
	}
	else
	{
		for (int i = int(a.m.size()) - 1; i >= 0; i--)
		{
			if (a.m[i] < b.m[i])
			{
				return -1;
			}
			else if (a.m[i] > b.m[i])
			{
				return 1;
			}
		}

		return 0;
	}
}


int BigInt::FFTBase = 1;
vector<Complex> BigInt::roots = { { 0, 0 }, { 1, 0 } };
vector<int> BigInt::rev = { 0, 1 };

void BigInt::ensure_FFTBase(int nFFTBase) const
{
	if (nFFTBase <= FFTBase)
		return;

	rev.resize(1 << nFFTBase);
	for (int i = 0; i < (1 << nFFTBase); i++)
	{
		rev[i] = (rev[i >> 1] >> 1) + ((i & 1) << (nFFTBase - 1));
	}

	roots.resize(1 << nFFTBase);
	while (FFTBase < nFFTBase)
	{
		double angle = 2.0 * acos(-1.0) / (1 << (FFTBase + 1));
		for (int i = (1 << (FFTBase - 1)); i < 1 << FFTBase; i++)
		{
			roots[i << 1] = roots[i];
			double angle_i = angle * (2 * i + 1 - (1 << FFTBase));
			roots[(i << 1) + 1] = Complex(cos(angle_i), sin(angle_i));
		}

		FFTBase++;
	}
}

void BigInt::fft(vector<Complex>& a, int n) const
{
	if (n == -1)
		n = a.size();

	unsigned long zeros; // = __builtin_ctz(n);
	_BitScanReverse(&zeros, n);
	ensure_FFTBase(zeros);

	int shift = FFTBase - zeros;
	for (int i = 0; i < n; i++)
	{
		if (i < (rev[i] >> shift))
			swap(a[i], a[rev[i] >> shift]);
	}

	for (int k = 1; k < n; k <<= 1)
	{
		for (int i = 0; i < n; i += 2 * k)
		{
			for (int j = 0; j < k; j++)
			{
				Complex z = a[i + j + k] * roots[j + k];
				a[i + j + k] = a[i + j] - z;
				a[i + j] = a[i + j] + z;
			}
		}
	}
}

void BigInt::multiplyFFT(vector <int> &a, vector <int> &b, vector <int> &res) const
{
	int need = a.size() + b.size() - 1;
	int nFFTBase = 0;

	while ((1 << nFFTBase) < need)
		nFFTBase++;

	ensure_FFTBase(nFFTBase);
	int sz = 1 << nFFTBase;

	vector<Complex> fa;
	if (sz > fa.size())
		fa.resize(sz);

	for (int i = 0; i < sz; i++)
	{
		int x = (i < a.size() ? a[i] : 0);
		int y = (i < b.size() ? b[i] : 0);
		fa[i] = Complex(x, y);
	}

	fft(fa, sz);
	Complex r(0, -0.25 / sz);
	for (int i = 0; i < ((sz >> 1) + 1); i++)
	{
		int j = (sz - i) & (sz - 1);
		Complex z = (fa[j] * fa[j] - (fa[i] * fa[i]).conj()) * r;
		if (i != j)
		{
			fa[j] = (fa[i] * fa[i] - (fa[j] * fa[j]).conj()) * r;
		}

		fa[i] = z;
	}

	fft(fa, sz);
	res.resize(need);
	for (int i = 0; i< need; i++)
	{
		if (fa[i].x > 0)
		{
			res[i] = fa[i].x + 0.5;
		}
		else
		{
			res[i] = fa[i].x - 0.5;
		}
	}
}

void BigInt::multiplyBy2()
{
	m.push_back(0);

	for (int i = int(m.size()) - 2; i >= 0; i--)
	{
		m[i] *= 2;
		if (m[i] >= base)
		{
			m[i] -= base;
			m[i + 1]++;
		}
	}

	normalize();
}

void BigInt::divideBy2()
{
	for (int i = int(m.size()) - 1; i > 0; i--)
	{
		if (m[i] % 2 == 1)
		{
			m[i - 1] += base;
		}
		m[i] /= 2;
	}
	m[0] /= 2;

	normalize();
}

BigInt BigInt::sqr(const BigInt& val)
{
	return val*val;
}

BigInt BigInt::powModPrivate(const BigInt& val, const BigInt& power, const BigInt& mod)
{
	if (power == 0)
	{
		return 1;
	}

	if (power.m[0] % 2 == 0)
	{
		BigInt newPower = power;
		newPower.divideBy2();

		return sqr(powModPrivate(val, newPower, mod)) % mod;
	}
	else
	{
		BigInt newPower = power;
		newPower.divideBy2();

		return sqr(powModPrivate(val, newPower, mod)) * val % mod;
	}
}

BigInt BigInt::sqrtBin(const BigInt& l, const BigInt& r, const BigInt& val)
{
	if (l == r)
	{
		return l;
	}

	BigInt mid = (l + r) + 1;
	mid.divideBy2();

	if (mid*mid > val)
	{
		return sqrtBin(l, mid - 1, val);
	}
	else
	{
		return sqrtBin(mid, r, val);
	}
}


BigInt::BigInt()
{
	base = standartBase;
	baseSize = standartBaseSize;

	p = 1;
	m.push_back(0);
}

BigInt::BigInt(long long a)
{
	base = standartBase;
	baseSize = standartBaseSize;

	if (a == 0)
	{
		p = 1;
		m.push_back(0);

		return;
	}

	if (a > 0)
	{
		p = 1;
	}
	else
	{
		p = 0;
		a = -a;
	}

	while (a > 0)
	{
		m.push_back(a % base);
		a /= base;
	}	
}

BigInt::BigInt(const string& a)
{
	base = standartBase;
	baseSize = standartBaseSize;

	if (a == "-0" || a == "0")
	{
		p = 1;
		m.push_back(0);

		return;
	}

	int i, j, last = 0;

	if (a[0] == '-')
	{
		p = 0;
		last = 1;
	}
	else
	{
		p = 1;
	}

	for (i = int(a.size()) - 1, j = 1; i >= last; i--)
	{
		if (j == 1)
		{
			m.push_back(0);
		}

		m.back() += (a[i] - '0')*j;

		j *= 10;
		if (j == base)
		{
			j = 1;
		}
	}

	normalize();
}

BigInt BigInt::makeAbs()
{
	p = 1;

	return (*this);
}

bool BigInt::operator ==(const BigInt& other) const
{
	_ASSERT(baseSize == other.baseSize);

	if (p != other.p)
	{
		return false;
	}

	int comp = compareAbs(*this, other);

	return comp == 0;
}

bool BigInt::operator !=(const BigInt& other) const
{
	_ASSERT(baseSize == other.baseSize);

	return !(operator ==(other));
}

bool BigInt::operator <(const BigInt& other) const
{
	_ASSERT(baseSize == other.baseSize);

	if (p == 0 && other.p == 1)
	{
		return true;
	}

	if (p == 1 && other.p == 0)
	{
		return false;
	}

	int comp = compareAbs(*this, other);

	if ((p == 1 && comp == -1) || (p == 0 && comp == 1))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool BigInt::operator >(const BigInt& other) const
{
	_ASSERT(baseSize == other.baseSize);

	if (p == 0 && other.p == 1)
	{
		return false;
	}

	if (p == 1 && other.p == 0)
	{
		return true;
	}

	int comp = compareAbs(*this, other);

	if ((p == 1 && comp == 1) || (p == 0 && comp == -1))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool BigInt::operator <=(const BigInt& other) const
{
	_ASSERT(baseSize == other.baseSize);

	return !(operator >(other));
}

bool BigInt::operator >=(const BigInt& other) const
{
	_ASSERT(baseSize == other.baseSize);

	return !(operator <(other));
}


BigInt BigInt::operator -()const
{
	BigInt res = (*this);
	res.p ^= 1;

	return res;
}

BigInt BigInt::operator +(const BigInt& other) const
{
	_ASSERT(baseSize == other.baseSize);

	int comp = compareAbs((*this), other);
	
	if (comp == -1)
	{
		return (other + (*this));
	}

	BigInt res = (*this);
	res.m.push_back(0);

	if (res.p == other.p)
	{
		int i;
		for (i = 0; i < other.m.size(); i++)
		{
			res.m[i] += other.m[i];

			if (res.m[i] >= res.base)
			{
				res.m[i + 1]++;
				res.m[i] -= res.base;
			}
		}

		while (res.m[i] >= base)
		{
			res.m[i + 1]++;
			res.m[i] -= res.base;

			i++;
		}
	}
	else
	{
		int i;
		for (i = 0; i < other.m.size(); i++)
		{
			res.m[i] -= other.m[i];

			if (res.m[i] < 0)
			{
				res.m[i + 1]--;
				res.m[i] += res.base;
			}
		}

		while (res.m[i] < 0)
		{
			res.m[i + 1]--;
			res.m[i] += res.base;

			i++;
		}
	}

	res.normalize();

	return res;
}

BigInt BigInt::operator -(const BigInt& other) const
{
	_ASSERT(baseSize == other.baseSize);

	return (*this) + (-other);
}

BigInt& BigInt::operator +=(const BigInt& other)
{
	_ASSERT(baseSize == other.baseSize);

	int comp = compareAbs(*this, other);

	if (comp == 1)
	{
		m.push_back(0);

		if (p == other.p)
		{
			int i;
			for (i = 0; i < other.m.size(); i++)
			{
				m[i] += other.m[i];

				if (m[i] >= base)
				{
					m[i + 1]++;
					m[i] -= base;
				}
			}

			while (m[i] >= base)
			{
				m[i + 1]++;
				m[i] -= base;

				i++;
			}
		}
		else
		{
			int i;
			for (i = 0; i < other.m.size(); i++)
			{
				m[i] -= other.m[i];

				if (m[i] < 0)
				{
					m[i + 1]--;
					m[i] += base;
				}
			}

			while (m[i] < 0)
			{
				m[i + 1]--;
				m[i] += base;

				i++;
			}
		}

		normalize();
	}
	else
	{
		(*this) = (*this) + other;
	}

	return (*this);
}

BigInt& BigInt::operator -=(const BigInt& other)
{
	_ASSERT(baseSize == other.baseSize);

	int comp = compareAbs(*this, other);

	if (comp == 1)
	{
		m.push_back(0);

		if (p != other.p)
		{
			int i;
			for (i = 0; i < other.m.size(); i++)
			{
				m[i] += other.m[i];

				if (m[i] >= base)
				{
					m[i + 1]++;
					m[i] -= base;
				}
			}

			while (m[i] >= base)
			{
				m[i + 1]++;
				m[i] -= base;

				i++;
			}
		}
		else
		{
			int i;
			for (i = 0; i < other.m.size(); i++)
			{
				m[i] -= other.m[i];

				if (m[i] < 0)
				{
					m[i + 1]--;
					m[i] += base;
				}
			}

			while (m[i] < 0)
			{
				m[i + 1]--;
				m[i] += base;

				i++;
			}
		}

		normalize();
	}
	else
	{
		(*this) = (*this) - other;
	}

	return (*this);
}


BigInt BigInt::multQuadtaric(const BigInt& other) const
{
	long long tempBase = 100000, tempBaseSize = 5;

	BigInt a = (*this), b = other;
	a.setBaseSize(tempBaseSize);
	b.setBaseSize(tempBaseSize);

	BigInt res = 0;
	res.setBaseSize(tempBaseSize);
	res.m.clear();
	vector <long long> resM;
	resM.resize(a.m.size() + b.m.size() - 1, 0);

	for (int i = 0; i < a.m.size(); i++)
	{
		for (int j = 0; j < b.m.size(); j++)
		{
			resM[i + j] += ((long long) a.m[i]) * b.m[j];
		}
	}
	for (int i = 0; i + 1 < resM.size(); i++)
	{
		if (resM[i] >= tempBase)
		{
			resM[i + 1] += resM[i] / 100000;
			resM[i] %= 100000;
		}
	}
	while (resM.back() >= 100000)
	{
		int ost = resM.back() / 100000;
		resM.back() %= 100000;
		resM.push_back(ost);
	}

	for (auto digit : resM)
	{
		res.m.push_back(digit);
	}
	res.setBaseSize(standartBaseSize);

	res.normalize();
	
	res.p = 1 ^ (a.p ^ b.p);

	return res;
}

BigInt BigInt::operator *(const BigInt& other) const
{
	if (m.size()*baseSize <= 20 || other.m.size()*other.baseSize <= 20)
	{
		return multQuadtaric(other);
	}

	long long tempBase = 1000, tempBaseSize = 3;

	BigInt a = (*this);
	BigInt b = other;

	BigInt res;
	res.m.clear();
	res.setBaseSize(tempBaseSize);

	a.setBaseSize(tempBaseSize);
	b.setBaseSize(tempBaseSize);

	multiplyFFT(a.m, b.m, res.m);

	for (int i = 0; i + 1 < res.m.size(); i++)
	{
		if (res.m[i] >= 1000)
		{
			res.m[i + 1] += res.m[i] / 1000;
			res.m[i] %= 1000;
		}
	}
	while (res.m.back() >= 1000)
	{
		int ost = res.m.back() / 1000;
		res.m.back() %= 1000;
		res.m.push_back(ost);
	}

	res.setBaseSize(standartBaseSize);

	res.p = 1 ^ (a.p ^ b.p);

	return res;
}

BigInt BigInt::operator /(const BigInt& other) const
{
	if (compareAbs(*this, other) == -1)
	{
		return 0;
	}

	BigInt res = 0, st2 = 1, a = (*this), b = other;

	a.makeAbs();
	b.makeAbs();

	while (compareAbs(a, b) != -1)
	{
		st2.multiplyBy2();
		b.multiplyBy2();
	}

	st2.divideBy2();
	b.divideBy2();

	while (st2 > 0)
	{
		if (compareAbs(a, b) != -1)
		{
			res += st2;
			a -= b;
		}

		st2.divideBy2();
		b.divideBy2();
	}

	res.p = 1 ^ (p^other.p);

	res.setBaseSize(standartBaseSize);

	return res;
}

BigInt BigInt::operator %(const BigInt& other) const
{
	BigInt div = (*this) / other;

	BigInt res = (*this) - div * BigInt::abs(other);

	if (res < 0)
	{
		res += other;
	}

	return res;
}

BigInt& BigInt::operator *=(const BigInt& other)
{
	(*this) = (*this) * other;

	return (*this);
}

BigInt& BigInt::operator /=(const BigInt& other)
{
	(*this) = (*this) / other;

	return (*this);
}

BigInt& BigInt::operator %=(const BigInt& other)
{
	(*this) = (*this) % other;

	return (*this);
}


BigInt BigInt::addMod(const BigInt& a, const BigInt& b, const BigInt& mod)
{
	return (a + b) % mod;
}

BigInt BigInt::minusMod(const BigInt& a, const BigInt& b, const BigInt& mod)
{
	return (a - b) % mod;
}

BigInt BigInt::multiplyMod(const BigInt& a, const BigInt& b, const BigInt& mod)
{
	return (a * b) % mod;
}

BigInt BigInt::getInverseMod(const BigInt& a, const BigInt& mod)
{
	return powMod(a, mod - 2, mod);
}

BigInt BigInt::divideMod(const BigInt& a, const BigInt& b, const BigInt& mod)
{
	return (a * getInverseMod(b, mod)) % mod;
}


BigInt BigInt::pow(const BigInt& val, const BigInt& power)
{
	if (power == 0)
	{
		return 1;
	}
	if (power.m[0] % 2 == 0)
	{
		return sqr(pow(val, power / 2));
	}
	else
	{
		return sqr(pow(val, power / 2)) * val;
	}
}

BigInt BigInt::powMod(const BigInt& val, const BigInt& power, const BigInt& mod)
{
	BigInt newVal = val % mod;
	BigInt newPower = power % mod;

	return powModPrivate(newVal, newPower, mod);
}


BigInt BigInt::sqrt(const BigInt& val)
{
	int valLen = val.toString().size();
	BigInt r = pow(10, valLen / 2 + 1);

	if (r < val)
	{
		return sqrtBin(0, r, val);
	}
	else
	{
		return sqrtBin(0, val, val);
	}
}

BigInt BigInt::abs(const BigInt& val)
{
	BigInt newBig = val;
	newBig.makeAbs();

	return newBig;
}

BigInt BigInt::gcd(BigInt a, BigInt b)
{
	BigInt t;

	while (b != 0)
	{
		t = b;
		b = a % b;
		a = t;
	}

	return a;
}

BigInt BigInt::getRandomInteger(BigInt l, BigInt r, mt19937& rng)
{
	BigInt bound = r - l + 1;

	BigInt res;

	res.m.resize(bound.m.size());
	for (int i = 0; i < res.m.size(); i++)
	{
		res.m[i] = rng() % res.base;
	}
	res %= bound;

	return l + res;
}

BigInt BigInt::getLog2()
{
	BigInt st2 = 1, res = 0;

	while (st2 < (*this))
	{
		st2.multiplyBy2();
		res += 1;
	}

	return res;
}

string BigInt::toString() const
{
	if (m.size() == 1 && m[0] == 0)
	{
		return "0";
	}

	string res;

	for (auto it : m)
	{
		for (int i = 0; i < baseSize; i++)
		{
			res+= ('0' + it % 10);
			it /= 10;
		}
	}

	while (res.back() == '0')
	{
		res.pop_back();
	}
	reverse(res.begin(), res.end());

	if (p == 0)
	{
		res = "-" + res;
	}

	return res;
}