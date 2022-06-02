#include <bitset>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <cmath>
#include <iomanip>

typedef unsigned short BASE;
typedef unsigned int DBASE;
typedef unsigned long long DDBASE;
#define BASE_SIZE (sizeof (BASE)*8)
using namespace std;

unsigned long long fi(unsigned long long n)
{
	unsigned long long f = n;
	if (n % 2 == 0)
	{
		while (n % 2 == 0)
		{
			n /= 2;
		}
		f /= 2;
	}
	for (unsigned long long i = 3; i * i <= n; i += 2)
	{
		if (n % i == 0)
		{
			while (n % i == 0)
			{
				n /= i;
			}
			f /= i;
			f *= (i - 1);
		}
	}
	if (n > 1)
	{
		f /= n;
		f *= (n - 1);
	}

	return f;
}

int jacobi(int a, int n)
{
	if (a == 0)
	{
		return 0;
	}
	if (a == 1)
	{
		return 1;
	}

	int k = 0;
	while (a % 2 == 0)
	{
		k++;
		a = a / 2;
	}

	int s;
	if (k % 2 == 0)
	{
		s = 1;
	}
	else
	{
		if (n % 8 == 1 || n % 8 == 7)
		{
			s = 1;
		}
		else
		{
			s = -1;
		}
	}

	if (n % 4 == 3 && a % 4 == 3)
	{
		s = -s;
	}

	if (a == 1)
	{
		return s;
	}
	else
	{
		return s * jacobi(n % a, a);
	}
}

//0  1
//b6 21
//21b6

class bnum {
	int len = 0;          //используемая память
	int max_len = 0;      //выделеная память
	BASE* coef = nullptr; //массив коэфициентов

	void del_zeros()
	{
		int i = max_len - 1;
		while (coef[i] == 0 && i >= 0)
		{
			i--;
		}

		len = i + 1;
		if (i == -1)
		{
			len++;
		}
	}
public:
	bnum(string s)
	{
		size_t len_s = s.length();

		if (coef != nullptr)
		{
			delete[] coef;
			coef = nullptr;
		}
		max_len = len = 1;
		coef = new BASE[max_len];
		coef[0] = 0;

		size_t j = 0;
		BASE basis = 10;

		while (j < len_s)
		{
			*this = *this * basis + BASE(s[j] - '0');
			j++;
		}
	}

	/* конструктор
	// maxLen - длина нулевого числа
	// random - случайные значения или нулевое число
	*/
	bnum(int maxLen = 1, bool random = false)
	{

		coef = new BASE[maxLen];
		max_len = len = maxLen;

		int eigths = BASE_SIZE / 8;

		if (random)
		{
			unsigned char num;
			for (int i = 0; i < max_len; i++)
			{
				coef[i] = 0;
				for (int j = 0; j < eigths; j++)
				{
					coef[i] <<= 8;
					num = rand() % 256;
					coef[i] |= num;
				}
			}
			del_zeros();
		}
		else
		{
			len = 1;
			for (int i = 0; i < max_len; i++)
			{
				coef[i] = 0;
			}
		}
	}

	~bnum()
	{
		if (coef != nullptr)
		{
			delete[] coef;
			coef = nullptr;
		}
	}

	bnum(const bnum& other)
	{
		len = other.len;
		max_len = other.len;
		coef = new BASE[max_len];

		for (int i = 0; i < len; i++)
		{
			coef[i] = other.coef[i];
		}
	}

	bnum& operator = (const bnum& other)
	{
		//самоприсваивание
		if (this == &other)
		{
			return *this;
		}

		//чистим мусор
		if (coef != nullptr)
		{
			delete[] coef;
		}

		len = other.len;
		max_len = other.max_len;
		coef = new BASE[max_len];

		for (int i = 0; i < len; i++)
		{
			coef[i] = other.coef[i];
		}

		return *this;
	}

	//16-иричный

	friend istream& operator >>(istream& in, bnum& cur)
	{
		string temp, str;
		in >> temp;
		int temp_len = int(temp.length());
		int count = 0; bool f = true;
		for (int i = 0; i < temp_len; i++)
		{
			if (f && temp[i] == '0')
			{
				count++;
				continue;
			}
			f = false;
			if (temp[i] >= '0' && temp[i] <= '9')
			{
				continue;
			}

			if (temp[i] >= 'a' && temp[i] <= 'f')
			{
				continue;
			}

			cout << "String contain wrong symbols, try again" << endl;
			exit(1);
		}
		temp.erase(0, count);
		temp_len = int(temp.length());
		if (temp_len == 0)
		{
			if (cur.coef != nullptr)
			{
				delete[] cur.coef;
				cur.coef = nullptr;
			}

			cur.max_len = cur.len = 1;
			cur.coef = new BASE[cur.max_len];
			cur.coef[0] = 0;
			return in;
		}

		int fours = BASE_SIZE / 4;

		if (temp_len % fours != 0)
		{
			for (int i = temp_len % fours; i < fours; i++)
			{
				str += "0";
			}
		}

		str += temp;
		int i = int(str.length());
		cur.max_len = i / fours;
		cur.len = cur.max_len;

		if (cur.coef != nullptr)
		{
			delete[] cur.coef;
			cur.coef = nullptr;
		}

		cur.coef = new BASE[cur.max_len];

		for (int j = 0; j < cur.max_len; j++)
		{
			i -= fours;
			cur.coef[j] = 0;
			for (int k = 0; k < fours; k++)
			{
				cur.coef[j] <<= 4;
				int num;

				if (str[i + k] >= '0' && str[i + k] <= '9')
				{
					num = str[i + k] - '0';
				}
				else
				{
					num = str[i + k] - 'a' + 10;
				}

				cur.coef[j] |= BASE(num);
			}
		}

		return in;
	}

	friend ostream& operator <<(ostream& out, bnum& cur)
	{
		if (cur.coef[0] == 0 && cur.len == 1)
		{
			out << "0";
			return out;
		}

		out << hex;
		int i = cur.len - 1;
		BASE temp = 15, g;
		bool f = true;
		for (int j = BASE_SIZE - 4; j >= 0; j -= 4)
		{
			g = temp & (cur.coef[i] >> j);
			if (g == 0 && f)
			{
				continue;
			}
			else
			{
				f = false;
				out << int(g);
			}
		}

		for (i = cur.len - 2; i >= 0; i--)
		{
			for (int j = BASE_SIZE - 4; j >= 0; j -= 4)
			{
				g = temp & (cur.coef[i] >> j);
				out << int(g);
			}
		}
		out << dec;

		return out;
	}

	//десятичный
	/*
	friend istream& operator >>(istream& in, bnum& cur)
	{
		string s;
		size_t len_s;
		bool flag = true;
		while (flag)
		{
			try
			{
				in >> s;
				len_s = s.length();
				for (size_t i = 0; i < len_s; i++)
				{
					if (s[i] >= '0' && s[i] <= '9')
					{
						continue;
					}

					throw exception("Check if the string can only contain numbers from 0 to 9 and try again");
				}
				flag = false;
			}
			catch (const exception& ex)
			{
				cout << ex.what() << endl;
			}
		}

		if (cur.coef != nullptr)
		{
			delete[] cur.coef;
			cur.coef = nullptr;
		}
		cur.max_len = cur.len = 1;
		cur.coef = new BASE[cur.max_len];
		cur.coef[0] = 0;

		size_t j = 0;
		BASE basis = 10;

		while (j < len_s)
		{
			cur = cur * basis + BASE(s[j] - '0');
			j++;
		}

		return in;
	}

	friend ostream& operator <<(ostream& out, bnum& other)
	{
		bnum cur = other;
		if (cur.coef[0] == 0 && cur.len == 1)
		{
			out << "0";
			return out;
		}

		int j = 0;
		BASE basis = 10;
		string res;

		while (!(cur.coef[0] == 0 && cur.len == 1))
		{
			bnum temp = cur % basis;
			res = to_string(temp.coef[0]) + res;
			cur = cur / basis;
		}
		out << res;

		return out;
	}
	*/

	bool operator == (const bnum other)
	{
		if (len != other.len)
		{
			return false;
		}

		for (int i = 0; i < len; i++)
		{
			if (other.coef[i] != coef[i])
			{
				return false;
			}
		}

		return true;
	}

	bool operator != (const bnum other)
	{
		return !(*this == other);
	}

	bool operator > (const bnum other)
	{
		if (len > other.len)
		{
			return true;
		}

		if (len < other.len)
		{
			return false;
		}

		for (int i = len - 1; i >= 0; i--)
		{
			if (coef[i] > other.coef[i])
			{
				return true;
			}
			if (coef[i] < other.coef[i])
			{
				return false;
			}
		}

		return false;
	}

	bool operator <= (const bnum other)
	{
		return !(*this > other);
	}

	bool operator < (const bnum other)
	{
		if (len < other.len)
		{
			return true;
		}

		if (len > other.len)
		{
			return false;
		}

		for (int i = len - 1; i >= 0; i--)
		{
			if (coef[i] < other.coef[i])
			{
				return true;
			}
			if (coef[i] > other.coef[i])
			{
				return false;
			}
		}

		return false;
	}

	bool operator >= (const bnum other)
	{
		return !(*this < other);
	}

	bnum operator + (const bnum& other)
	{
		int l = max(len, other.len) + 1,
			t = min(len, other.len),
			j = 0;
		BASE k = 0;
		DBASE tmp = 0;

		bnum res;
		if (res.coef != nullptr)
		{
			delete[] res.coef;
			res.coef = nullptr;
		}
		res.coef = new BASE[l];

		//пока оба числа не закончились
		while (j < t)
		{
			tmp = DBASE(coef[j]) + DBASE(other.coef[j]) + DBASE(k);
			res.coef[j] = tmp;
			k = tmp >> BASE_SIZE;
			j++;
		}

		//закончилось левое число
		if (j >= len)
		{
			while (j < other.len)
			{
				tmp = DBASE(other.coef[j]) + DBASE(k);
				res.coef[j] = tmp;
				k = tmp >> BASE_SIZE;
				j++;
			}
		}
		//закончилось правое число
		else
		{
			while (j < len)
			{
				tmp = DBASE(coef[j]) + DBASE(k);
				res.coef[j] = tmp;
				k = tmp >> BASE_SIZE;
				j++;
			}
		}

		if (k == 0)
		{
			res.max_len = res.len = j;
		}
		else
		{
			res.coef[j] = k;
			res.max_len = res.len = j + 1;
		}

		return res;
	}

	bnum& operator += (bnum& other)
	{
		int l = max(len, other.len) + 1,
			t = min(len, other.len),
			j = 0;
		BASE k = 0;
		DBASE tmp = 0;

		BASE* newcoef = new BASE[l];

		//пока оба числа не закончились
		while (j < t)
		{
			tmp = DBASE(coef[j]) + DBASE(other.coef[j]) + DBASE(k);
			newcoef[j] = tmp;
			k = tmp >> BASE_SIZE;
			j++;
		}

		//закончилось левое число
		if (j >= len)
		{
			while (j < other.len)
			{
				tmp = DBASE(other.coef[j]) + DBASE(k);
				newcoef[j] = tmp;
				k = tmp >> BASE_SIZE;
				j++;
			}
		}
		//закончилось правое число
		else
		{
			while (j < len)
			{
				tmp = DBASE(coef[j]) + DBASE(k);
				newcoef[j] = tmp;
				k = tmp >> BASE_SIZE;
				j++;
			}
		}

		if (coef != nullptr)
		{
			delete[] coef;
			coef = nullptr;
		}
		coef = newcoef;

		if (k == 0)
		{
			max_len = len = j;
		}
		else
		{
			newcoef[j] = k;
			max_len = len = j + 1;
		}

		return *this;
	}

	bnum operator + (const BASE other)
	{
		bnum new_other;
		new_other.coef[0] = other;
		new_other += *this;
		new_other.del_zeros();

		return new_other;
	}

	bnum operator - (const bnum& other)
	{
		if (*this < other)
		{
			cout << "First number less than second try again" << endl;
			return 0;
		}
		int j = 0;
		BASE k = 0;
		DBASE tmp;
		bnum res(len);

		while (j < other.len)
		{
			tmp = (DBASE(1) << BASE_SIZE) + DBASE(coef[j]);
			tmp = tmp - DBASE(other.coef[j]) - DBASE(k);
			res.coef[j] = tmp;
			k = !(tmp >> BASE_SIZE);
			j++;
		}

		while (j < len)
		{
			tmp = (DBASE(1) << BASE_SIZE) + DBASE(coef[j]);
			tmp -= DBASE(k);
			res.coef[j] = tmp;
			k = !(tmp >> BASE_SIZE);
			j++;
		}

		res.del_zeros();

		return res;
	}

	bnum& operator -= (bnum& other)
	{
		if (*this < other)
		{
			cout << "First number less than second try again" << endl;
			return *this;
		}

		BASE* newcoef = new BASE[len];
		int j = 0;
		BASE k = 0;
		DBASE tmp;

		while (j < other.len)
		{
			tmp = (DBASE(1) << BASE_SIZE) + DBASE(coef[j]);
			tmp = tmp - DBASE(other.coef[j]) - DBASE(k);
			newcoef[j] = tmp;
			k = !(tmp >> BASE_SIZE);
			j++;
		}

		while (j < len)
		{
			tmp = (DBASE(1) << BASE_SIZE) + DBASE(coef[j]);
			tmp -= k;
			newcoef[j] = tmp;
			k = !(tmp >> BASE_SIZE);
			j++;
		}

		if (coef != nullptr)
		{
			delete[] coef;
			coef = nullptr;
		}
		coef = newcoef;

		del_zeros();

		return *this;
	}

	bnum operator * (const BASE other)
	{
		int j = 0;
		BASE k = 0;
		DBASE tmp;
		bnum res(len + 1);

		while (j < len)
		{
			tmp = DBASE(coef[j]) * DBASE(other) + DBASE(k);
			res.coef[j] = tmp;
			k = tmp >> BASE_SIZE;
			j++;
		}
		//перенос
		res.coef[j] = k;

		res.del_zeros();

		return res;
	}

	bnum operator * (const bnum& other)
	{
		int j = 0;
		DBASE tmp;
		bnum res(len + other.len);
		while (j < other.len)
		{
			//если коэффициент 2-ого множителя равен нулю то пропуск
			if (other.coef[j] == 0)
			{
				j++;
				continue;
			}
			int i = 0;
			BASE k = 0;

			//умножение
			while (i < len)
			{
				tmp = DBASE(coef[i]) * DBASE(other.coef[j]) + DBASE(res.coef[i + j]) + DBASE(k);
				res.coef[i + j] = tmp;
				k = tmp >> BASE_SIZE;
				i++;
			}
			//перенос записать
			//длина 1 числа плюс сдвиг
			res.coef[j + len] = k;
			j++;
		}

		res.del_zeros();

		return res;
	}

	bnum& operator *= (bnum& other)
	{
		bnum res = (*this) * other;
		return res;
	}

	bnum operator / (const BASE other)
	{
		int j = 0;
		DBASE tmp, r = 0;
		bnum res(len);
		while (j < len)
		{
			tmp = ((DBASE(r)) << BASE_SIZE) + DBASE(coef[len - 1 - j]);

			//находим разряд частного
			res.coef[len - 1 - j] = tmp / DBASE(other);
			r = tmp % DBASE(other);
			j++;
		}

		res.del_zeros();

		return res;
	}

	bnum operator / (const bnum& other)
	{
		//частные случаи
		if (other.coef[0] == 0 && other.len == 1)
		{
			bnum res;
			cout << "The divisor cannot be zero" << endl;
			return res;
		}
		if (*this < other)
		{
			bnum res;
			return res;
		}
		if (*this == other)
		{
			bnum res;
			res.coef[0] = 1;
			return res;
		}
		if (other.len == 1)
		{
			BASE num = other.coef[0];
			bnum res;
			res = *this / num;
			return res;
		}

		//делимое
		bnum u = *this;

		//делитель
		bnum v = other;

		//длинна делителя
		int n = v.len;

		//длина делимого
		int m = u.len - n;

		//нормализация
		DBASE d;
		d = (DDBASE(1) << BASE_SIZE) / (DDBASE(v.coef[n - 1]) + DDBASE(1));

		u = u * BASE(d);
		v = v * BASE(d);

		//дозапись в начало делимого 0
		if (u.len - n == m)
		{
			BASE* newcoef = new BASE[long long(u.len) + 1];
			for (int i = 0; i < u.len; i++)
			{
				newcoef[i] = u.coef[i];
			}
			newcoef[u.len] = 0;
			delete[] u.coef;
			u.coef = newcoef;
			u.max_len++;
			u.len++;
		}

		//начальная установка
		int j = m;
		bool k = false;
		BASE* q = new BASE[long long(m) + 1];
		DBASE _q, _r;

		//цикл по j, дает один разряд частного по j
		while (j >= 0)
		{
			//вычисляем приблеженный остаток и частное
			_q = ((DDBASE(u.coef[j + n]) << BASE_SIZE) + DDBASE(u.coef[j + n - 1])) / DDBASE(v.coef[n - 1]);
			_r = ((DDBASE(u.coef[j + n]) << BASE_SIZE) + DDBASE(u.coef[j + n - 1])) % DDBASE(v.coef[n - 1]);

			if (_q == (DBASE(1) << BASE_SIZE) || _q * v.coef[n - 2] > (DBASE(1) << BASE_SIZE) * _r + u.coef[j + n - 2])
			{
				_q--;
				_r = _r + v.coef[n - 1];
			}

			if (_r < (DBASE(1) << BASE_SIZE))
			{
				if (_q == (DBASE(1) << BASE_SIZE) || _q * v.coef[n - 2] > (DBASE(1) << BASE_SIZE) * _r + u.coef[j + n - 2])
				{
					_q--;
				}
			}

			//из соответствующей части делимого вычитаем делитель умноженный на _q
			bnum u_temp(n + 1);
			for (int i = j; i <= j + n; i++)
			{
				u_temp.coef[i - j] = u.coef[i];
			}

			u_temp.len = n + 1;
			bnum v_temp;
			v_temp = v * BASE(_q);

			//проверяем левая часть меньше правой если да
			//то сделаем займ (добавим слева 1)
			if (u_temp < v_temp)
			{
				BASE* newcoef = new BASE[long long(u_temp.len) + 1];
				for (int i = 0; i < u_temp.len; i++)
				{
					newcoef[i] = u_temp.coef[i];
				}
				newcoef[u_temp.len] = 1;
				delete[] u_temp.coef;
				u_temp.coef = newcoef;
				u_temp.max_len++;
				u_temp.len++;
				k = true;
			}
			else
			{
				k = false;
			}

			u_temp = u_temp - v_temp;
			int pred_len = u_temp.len;
			q[j] = _q;

			if (k)
			{
				//компенсация сложения
				q[j]--;
				u_temp += v;
			}

			for (int i = j; i <= j + n; i++)
			{
				if (i - j > pred_len - 1)
				{
					u.coef[i] = 0;
					continue;
				}
				u.coef[i] = u_temp.coef[i - j];
			}
			u.del_zeros();
			j--;
		}

		bnum res;
		delete[]res.coef;
		res.coef = q;
		res.max_len = m + 1;
		res.len = m + 1;

		return res;
	}

	bnum operator % (const BASE other)
	{
		int j = 0;
		DBASE tmp, r = 0;
		bnum res;

		while (j < len)
		{
			tmp = ((DBASE(r)) << BASE_SIZE) + DBASE(coef[len - 1 - j]);
			r = tmp % DBASE(other);
			j++;
		}

		res.coef[0] = r;

		return res;
	}

	bnum operator % (const bnum& other)
	{
		//частные случаи
		if (other.coef[0] == 0 && other.len == 1)
		{
			bnum res;
			cout << "The divisor cannot be zero" << endl;
			return res;
		}
		if (*this < other)
		{
			bnum res = *this;
			return res;
		}
		if (*this == other)
		{
			bnum res;
			return res;
		}
		if (other.len == 1)
		{
			BASE num = other.coef[0];
			bnum res;
			res = *this % num;
			return res;
		}
		//делимое
		bnum u = *this;

		//делитель
		bnum v = other;

		//длинна делителя
		int n = v.len;

		//длина делимого
		int m = u.len - n;

		//шаг D1 нормализация
		DBASE d;
		d = (DDBASE(1) << BASE_SIZE) / (DDBASE(v.coef[n - 1]) + DDBASE(1));

		u = u * BASE(d);
		v = v * BASE(d);

		//дозапись в начало делимого 0
		if (u.len - n == m)
		{
			BASE* newcoef = new BASE[long long(u.len) + 1];
			for (int i = 0; i < u.len; i++)
			{
				newcoef[i] = u.coef[i];
			}
			newcoef[u.len] = 0;
			delete[] u.coef;
			u.coef = newcoef;
			u.max_len++;
			u.len++;
		}

		//начальная установка
		int j = m;
		bool k = false;
		BASE* q = new BASE[long long(m) + 1];
		DBASE _q, _r;

		//цикл по j, дает один разряд частного по j
		while (j >= 0)
		{
			//вычисляем приблеженный остаток и частное
			_q = ((DDBASE(u.coef[j + n]) << BASE_SIZE) + DDBASE(u.coef[j + n - 1])) / DDBASE(v.coef[n - 1]);
			_r = ((DDBASE(u.coef[j + n]) << BASE_SIZE) + DDBASE(u.coef[j + n - 1])) % DDBASE(v.coef[n - 1]);

			if (_q == (DBASE(1) << BASE_SIZE) || _q * v.coef[n - 2] > (DBASE(1) << BASE_SIZE) * _r + u.coef[j + n - 2])
			{
				_q--;
				_r = _r + v.coef[n - 1];
			}

			if (_r < (DBASE(1) << BASE_SIZE))
			{
				if (_q == (DBASE(1) << BASE_SIZE) || _q * v.coef[n - 2] > (DBASE(1) << BASE_SIZE) * _r + u.coef[j + n - 2])
				{
					_q--;
				}
			}

			//из соответствующей части делимого вычитаем делитель умноженный на _q
			bnum u_temp(n + 1);
			for (int i = j; i <= j + n; i++)
			{
				u_temp.coef[i - j] = u.coef[i];
			}

			u_temp.len = n + 1;
			bnum v_temp;
			v_temp = v * BASE(_q);

			//проверяем левая часть меньше правой если да
			//то сделаем займ (добавим слева 1)
			if (u_temp < v_temp)
			{
				BASE* newcoef = new BASE[long long(u_temp.len) + 1];
				for (int i = 0; i < u_temp.len; i++)
				{
					newcoef[i] = u_temp.coef[i];
				}
				newcoef[u_temp.len] = 1;
				delete[] u_temp.coef;
				u_temp.coef = newcoef;
				u_temp.max_len++;
				u_temp.len++;
				k = true;
			}
			else
			{
				k = false;
			}

			u_temp = u_temp - v_temp;
			int pred_len = u_temp.len;
			q[j] = _q;

			if (k)
			{
				//компенсация сложения
				q[j]--;
				u_temp += v;
			}

			for (int i = j; i <= j + n; i++)
			{
				if (i - j > pred_len - 1)
				{
					u.coef[i] = 0;
					continue;
				}
				u.coef[i] = u_temp.coef[i - j];
			}
			u.del_zeros();
			j--;
		}

		u = u / BASE(d);

		return u;
	}

	bnum fast_sqr()
	{
		bnum res(2 * len + 1);
		DDBASE tmp = 0;
		for (size_t i = 0; i < len; i++)
		{
			tmp = DDBASE(res.coef[2 * i]) + DDBASE(coef[i]) * DDBASE(coef[i]);
			//(uv)
			res.coef[2 * i] = tmp; //v

			for (size_t j = i + 1; j < len; j++)
			{
				unsigned int cu = tmp >> BASE_SIZE;
				tmp = DDBASE(res.coef[i + j]) + DDBASE(2) * DDBASE(coef[i]) * DDBASE(coef[j]) + DDBASE(cu);
				res.coef[i + j] = tmp;
			}
			tmp >>= BASE_SIZE;
			tmp += DDBASE(res.coef[i + len]);
			tmp += DDBASE(res.coef[i + len + 1]) << BASE_SIZE;

			res.coef[i + len] = tmp;
			res.coef[i + len + 1] = tmp >> BASE_SIZE;
		}
		res.del_zeros();
		return res;
	}

	bnum sqr_in(const bnum y)
	{
		int n = (y.len - 1) * BASE_SIZE;
		int idx = BASE_SIZE - 1;
		while (true)
		{
			if (y.coef[y.len - 1] & (1 << idx))
			{
				n += idx + 1;
				break;
			}
			idx--;
		}

		bnum z(1);
		z = *this;
		for (int i = n - 2; i >= 0; i--)
		{
			z = z.fast_sqr();
			if (y.coef[i / BASE_SIZE] & (1 << (i % BASE_SIZE)))
			{
				z = z * (*this);
			}
		}
		return z;
	}

	//-right
	//+left
	bnum shift(int shift)
	{
		if (shift == 0)
		{
			bnum res = *this;
			return res;
		}
		if (shift < 0)
		{
			if ((-shift) >= len)
			{
				bnum res(1);
				return res;
			}

			bnum res(len + shift);
			for (int i = (-shift); i < len; i++)
			{
				res.coef[i + shift] = coef[i];
			}
			res.del_zeros();
			return res;
		}
		else
		{
			bnum res(len + shift);

			for (int i = 0; i < len; i++)
			{
				res.coef[i + shift] |= coef[i];
			}
			res.del_zeros();
			return res;
		}
	}

	bnum barret(bnum m, bnum z)
	{
		if (!(len <= 2 * m.len))
		{
			cout << "This algrithm can not work with this numbers" << endl;
			bnum res;
			return res;
		}
		bnum q_ = ((*this).shift(-(m.len - 1)) * z).shift(-(m.len + 1));

		bnum r1 = *this, r2 = q_ * m, r_;

		if (r1 >= r2)
		{
			r_ = r1 - r2;
		}
		else
		{
			r_.coef[0] = 1;
			r_ = r_.shift(m.len + 1) + r1 - r2;
		}

		while (r_ >= m)
		{
			r_ = r_ - m;
		}

		return r_;
	}

	bnum barret_z(bnum m)
	{
		bnum z;
		z.coef[0] = 1;
		z = z.shift(2 * m.len) / m;
		return z;
	}

	bnum sqr_in_mod(bnum a, bnum n)
	{
		bnum res;
		bnum n_1(1);
		n_1.coef[0] = 1;
		n_1 = n - n_1;

		res = a.sqr_in(n_1);
		res = res % n;
		return res;
	}

	bnum sqr_in_mod_solov_shtras(bnum a, bnum n)
	{
		bnum res;
		bnum n_1(1);
		n_1.coef[0] = 1;
		n_1 = n - n_1;
		bnum n_2;
		n_2.coef[0] = 2;
		n_1 = n_1 / n_2;

		res = a.sqr_in(n_1);
		res = res % n;
		return res;
	}

	bool test_ferma(int t)
	{
		while (t--)
		{
			bnum a;
			while (true)
			{
				bnum a_tmp(1, true);
				if (a_tmp.coef[0] >= 2 && a_tmp.coef[0] <= coef[0] - 2)
				{
					a = a_tmp;
					break;
				}
			}

			bnum res = a.sqr_in_mod(a, *this);
			if (res.coef[0] != 1)
			{
				return false;
			}
		}

		return true;
	}

	float eps_Ferma(bnum n, int t)
	{
		return pow((static_cast<float>(fi(n.coef[0])) / n.coef[0]), t);
	}

	float eps_solov_shtras(bnum n, int t)
	{
		return pow((static_cast<float>(fi(n.coef[0])) / (n.coef[0] * 2)), t);
	}

	bool solov_shtras(int t)
	{
		while (t--)
		{
			bnum a;
			while (true)
			{
				bnum a_tmp(1, true);
				if (a_tmp.coef[0] >= 2 && a_tmp.coef[0] <= coef[0] - 2)
				{
					a = a_tmp;
					break;
				}
			}

			bnum res = a.sqr_in_mod_solov_shtras(a, *this);
			if (res.coef[0] != 1 && res.coef[0] != (coef[0] - 1))
			{
				return false;
			}

			int s = jacobi(a.coef[0], coef[0]);
			s = (s + coef[0]) % coef[0];
			if (res.coef[0] != s)
			{

				return false;
			}
		}

		return true;
	}

	bool miller_rabin(int t)
	{
		int tmp = coef[0], s = 0;
		tmp--;
		while (tmp % 2 == 0)
		{
			tmp /= 2;
			s++;
		}
		bnum r;
		r.coef[0] = tmp;
		while (t--)
		{
			bnum b;
			while (true)
			{
				bnum b_tmp(1, true);
				if (b_tmp.coef[0] >= 2 && b_tmp.coef[0] <= coef[0] - 2)
				{
					b = b_tmp;
					break;
				}
			}

			bnum y;
			y = b.sqr_in(r);
			y = y % (*this);

			if (y.coef[0] != coef[0] - 1 && y.coef[0] != 1)
			{
				int j = 1;
				while (j < s && y.coef[0] != coef[0] - 1)
				{
					y = y.fast_sqr();
					y = y % (*this);
					if (y.coef[0] == 1)
					{
						return false;
					}
					j++;
				}
				if (y.coef[0] != coef[0] - 1)
				{
					return false;
				}
			}

		}

		return true;
	}
};


int main()
{
	srand(unsigned int(time(NULL)));
	int times = 13;
	for (int i = 0; i < 6; i++)
	{
		bnum rnd(1, true);
		cout << rnd;
		if (rnd.test_ferma(times))
		{
			cout << "\tTest Ferma: prime number\t\t\t";
		}
		else
		{
			cout << "\tTest Ferma: composite number\t\t\t";
		}

		cout << "\teps error in %: ";
		printf("%.5f", rnd.eps_Ferma(rnd, times) * 100);
		cout << "\n" << endl;


		if (rnd.solov_shtras(times))
		{
			cout << "\tTest Solovey-Strassen: prime number\t\t";
		}
		else
		{
			cout << "\tTest Solovey-Strassen: composite number\t\t";
		}

		cout << "\teps error in %: ";
		printf("%.5f", rnd.eps_solov_shtras(rnd, times) * 100);
		cout << "\n" << endl;

		if (rnd.miller_rabin(times))
		{
			cout << "\tTest Miller - Rabin: prime number\t\t";
		}
		else
		{
			cout << "\tTest Miller - Rabin: composite number\t";
		}


		cout << "\n" << endl;
	}

	while (true)
	{
		bnum rnd(1, true);

		if (rnd.test_ferma(1) && !rnd.solov_shtras(times))
		{
			cout << "Test ferma lies:" << endl;
			cout << rnd << endl;
			cout << "\tTest Ferma: prime number" << endl;
			cout << "\tTest Solovey-Strassen: composite number" << endl;
			if (rnd.miller_rabin(times))
			{
				cout << "\tTest Miller - Rabin: prime number" << endl;
			}
			else
			{
				cout << "\tTest Miller - Rabin: composite number" << endl;
			}

			break;
		}
	}

	//test 3
	//int N = 1000, M = 2000;
	//bnum m(M / 2 + M / 4, true);
	//bnum z = m.barret_z(m);
	//for (size_t i = 0; i < N; i++)
	//{
	//	bnum x(M, true);
	//	bnum x_mod = x % m;
	//	bnum x_barret = x.barret(m, z);
	//	if (x_barret != x_mod)
	//	{
	//		cout << "Fail on test: " << i << endl;
	//	}
	//	if (i % 100 == 0 && i > 0)
	//	{
	//		cout << "Done " << i << " tests\n";
	//	}
	//}
	//cout << "\tDone cmp test\n" << endl;
	//int NN = 1000, MM = 2000;
	//bnum mm(MM / 2 + MM / 4, true);
	//bnum zz = mm.barret_z(mm);
	//auto begin = chrono::steady_clock::now();
	//for (size_t i = 0; i < NN; i++)
	//{
	//	bnum xx(M, true);
	//	bnum x_barret = xx.barret(mm, zz);
	//}
	//auto end = chrono::steady_clock::now();
	//auto ms = chrono::duration_cast<chrono::milliseconds>(end - begin);
	//cout << "Time barret: " << ms.count() << endl;
	//begin = chrono::steady_clock::now();
	//for (size_t i = 0; i < NN; i++)
	//{
	//	bnum xx(M, true);
	//	bnum x_mod = xx % mm;
	//}
	//end = chrono::steady_clock::now();
	//ms = chrono::duration_cast<chrono::milliseconds>(end - begin);
	//cout << "Time mod: " << ms.count() << endl;
	//while (true)
	//{
	//	bnum x;
	//	cin >> x;
	//	bnum m;
	//	cin >> m;
	//	bnum z = m.barret_z(m);
	//	bnum res = x.barret(m, z);
	//	cout << res << endl;
	//}

	//test 1
	//int N = 20, M = 6000;
	//for (size_t i = 0; i < N; i++)
	//{
	//	bnum g(M, true);
	//	bnum g_mul = g * g;
	//	bnum g_fast = g.fast_sqr();
	//	if (g_mul!=g_fast)
	//	{
	//		cout << "Incorrect" << endl;
	//	}
	//}
	//cout << "Done cmp test\n\tIf no \"Incorrect\" that's correct\n" << endl;
	//auto begin = chrono::steady_clock::now();
	//for (size_t i = 0; i < N; i++)
	//{
	//	bnum g(M, true);
	//	bnum h = g.fast_sqr();
	//}
	//auto end = chrono::steady_clock::now();
	//auto ms = chrono::duration_cast<chrono::milliseconds>(end - begin);
	//cout << "Time fast_sqr: " << ms.count() << endl;
	//begin = chrono::steady_clock::now();
	//for (size_t i = 0; i < N; i++)
	//{
	//	bnum g(M, true);
	//	bnum h = g * g;
	//}
	//end = chrono::steady_clock::now();
	//ms = chrono::duration_cast<chrono::milliseconds>(end - begin);
	//cout << "Time multiply: " << ms.count() << endl;
	//cout << "Done speed test\n" << endl;
	//cout << "Interactive:" << endl;
	//while (true)
	//{
	//	bnum num;
	//	cin >> num;
	//	bnum num_fast = num.fast_sqr();
	//	cout << num_fast << endl;
	//}

	//test 2
	//bnum x, y;
	//cin >> x;
	//cin >> y;
	//bnum xx = x.sqr_in(y);
	//cout << xx << endl;

	//test ZERO
	//int M = 1000,
	//	T = 1000,
	//	good_count = 0;
	//bnum A,
	//	B,
	//	C,
	//	D;
	//srand(time(NULL));
	//while (T--)
	//{
	//	int n = rand() % M + 1,
	//		m = rand() % M + 1;
	//	bnum AA(n, true),
	//		BB(m, true);
	//	A = AA;
	//	B = BB;
	//	C = A / B;
	//	D = A % B;
	//	if (n >= m)
	//	{
	//		if (A == C * B + D && A - D == C * B && D < B)
	//		{
	//			good_count++;
	//		}
	//	}
	//	else
	//	{
	//		if (A == C * B + D && D < B)
	//		{
	//			good_count++;
	//		}
	//	}
	//	//cout << "T " << T << endl;
	//	//cout << "n " << n << endl;
	//	//cout << "m " << m << endl;
	//}
	//if (good_count == 1000)
	//{
	//	cout << "Good" << endl;
	//}
	//else
	//{
	//	cout << "BAD" << endl;
	//}
	system("pause");
	return 0;
}