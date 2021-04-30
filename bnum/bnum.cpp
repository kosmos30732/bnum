#include <iostream>
#include <cstddef>
#include <ctime>
#include <cstdint>
#include <string>
#include <fstream>

typedef unsigned int BASE;
typedef unsigned long long DBASE;
#define BASE_SIZE (sizeof (BASE)*8)
using namespace std;

class bnum {
    int len = 0;          //используемая память
    int max_len = 0;      //выделеная память
    BASE* coef = nullptr; //массив коэфициентов
    
    void del_zeros()
    {
        int i = max_len - 1;
        while (coef[i]==0 && i>=0)
        {
            i--;
        }
       
        len = i + 1;
        if (i==-1)
        {
            len++;
        }
    }

public:
    /* конструктор
    // maxLen - длина нулевого числа
    // random - случайные значения или нулевое число
    */
    bnum(int maxLen = 1, bool random=false)
    {
        srand(time(NULL));

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
        if (coef!=nullptr)
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
        if (this==&other)
        {
            return *this;
        }

        //чистим мусор
        if (coef!=nullptr)
        {
            delete[]coef;
        }

        len = other.len;
        max_len = other.len;
        coef = new BASE[max_len];

        for (int i = 0; i < len; i++)
        {
            coef[i] = other.coef[i];
        }

        return *this;
    }
    
    //16-иричный
    /*
    friend istream& operator >>(istream& in, bnum& cur)
    {
        string temp, str;
        in >> temp;
        int temp_len = temp.length();
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
        temp_len = temp.length();
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
        int i = str.length();
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
        if (cur.coef[0]==0 && cur.len==1)
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
            if (g ==0 &&f)
            {
                continue;
            }
            else
            {
                f = false;
                out << int(g);
            }
        }

        for (i = cur.len-2; i>=0; i--)
        {
            for (int j = BASE_SIZE-4; j>=0; j-=4)
            {
                g = temp & (cur.coef[i] >> j);
                out <<int(g);
            }            
        }
        out << dec;

        return out;
    }
    */
    
    //десятичный
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

        while(j<len_s)
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
    
    bool operator == (const bnum other)
    {
        if (len!=other.len)
        {
            return false;
        }

        for (int i = 0; i < len; i++)
        {
            if (other.coef[i]!=coef[i])
            {
                return false;
            }
        }

        return true;
    }

    bool operator != (const bnum other)
    {
        return !(*this==other);
    }

    bool operator > (const bnum other)
    {
        if (len>other.len)
        {
            return true;
        }

        if (len<other.len)
        {
            return false;
        }

        for (int i = len-1; i>=0; i--)
        {
            if (coef[i]>other.coef[i])
            {
                return true;
            }
            if (coef[i]<other.coef[i])
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
            j = 0, k = 0;
        DBASE tmp=0;

        bnum res;
        if (res.coef != nullptr)
        {
            delete[] res.coef;
            res.coef = nullptr;
        }
        res.coef = new BASE[l];

        //пока оба числа не закончились
        while (j<t)
        {
            tmp = DBASE(coef[j]) + DBASE(other.coef[j]) + DBASE(k);
            res.coef[j] = tmp;
            k = tmp >> BASE_SIZE;
            j++;
        }
        
        //закончилось левое число
        if (j >= len)
        {
            while (j<other.len)
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
            while (j<len)
            {
                tmp = DBASE(coef[j]) + DBASE(k);
                res.coef[j] = tmp;
                k = tmp >> BASE_SIZE;
                j++;
            }
        }

        if (k==0)
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
            j = 0, k = 0;
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
        if (*this<other)
        {
            cout << "First number less than second try again" << endl;
            return 0;
        }
        int j = 0, k = 0;
        DBASE tmp;
        bnum res(len);        

        while (j<other.len)
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
        int j = 0, k = 0;
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
        int j = 0, k = 0;
        DBASE tmp;
        bnum res(len + 1);
        
        while (j<len)
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
        bnum res(len+other.len);
        while (j<other.len)
        {
            //если коэффициент 2-ого множителя равен нулю то пропуск
            if (other.coef[j]==0)
            {
                j++;
                continue;
            }
            int k = 0, i = 0;

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
            int k = 0, i = 0;

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

        if (coef != nullptr)
        {
            delete[] coef;
            coef = nullptr;
        }

        coef = res.coef;
        res.coef = nullptr;

        max_len = res.max_len;
        len = res.len;

        return *this;
    }

    bnum operator / (const BASE other)
    {
        int j = 0;
        DBASE tmp, r = 0;
        bnum res(len);
        while (j<len)
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
        if (other.coef[0]==0 && other.len==1)
        {
            bnum res;
            cout << "The divisor cannot be zero" << endl;
            return res;
        }
        if (*this<other)
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
        //t = len
        //n = other.len
        bnum res(len);

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
            bnum res=other;
            return res;
        }
        if (*this == other)
        {
            bnum res;
            return res;
        }

        bnum res(other.len);

        return res;
    }
};

int main()
{
    bnum divisible, remnant, quotient, num_res;
    BASE divisor;
    cin >> divisible;
    cin>> divisor;

    cout << divisible << endl;
    cout << divisor << endl;
    cout << divisible << endl;
    cout << divisor << endl;

    
    quotient = divisible / divisor;
    cout << "quotient: "<< divisible<<" / "<< divisor<<" = " << quotient << endl;

    remnant = divisible % divisor;
    cout << "remnant: " << divisible << " % " << divisor << " = " << remnant << endl;

    num_res = quotient * divisor;
    num_res += remnant;
    if (num_res == divisible)
    {
        cout << endl;
        cout << quotient << " * " <<  divisor  << " + " << remnant << " = " << divisible << endl;
        cout << "Verification was successful" << endl;
    }
    else
    {
        cout << "Something wrong with operations" << endl;
    }
    
    return 0;
}