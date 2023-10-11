#include "tommath.h"

struct EccCryptoInfo {
	std::string finiteField;
	std::string paramA;
	std::string privateKey;
	std::pair<std::string, std::string> publicKey;
	std::pair<std::string, std::string> basePoint;
};

#define PRIVATE_KEY_LEN 512
#define FINITE_FIELD_LEN 512

// ENCODE_BYTE_LEN must be EVEN
#define ENCODE_BYTE_LEN 72

int GetPrime(mp_int* m, int lon) {
	mp_prime_rand(m, 10, lon, MP_PRIME_2MSB_ON);
	return MP_OKAY;
}

void Get_G_X_Y(mp_int* x1, mp_int* y1, mp_int* b, mp_int* a)
{
	mp_int tempx;
	mp_int temp1;
	mp_int temp2;
	mp_int temp3;

	mp_init(&tempx);
	mp_init(&temp1);
	mp_init(&temp2);
	mp_init(&temp3);

	//y2=x3+ax+b,随机产生X坐标,根据X坐标计算Y坐标
	GetPrime(x1, 30);
	mp_expt_n(x1, 3, &temp1);
	mp_mul(a, x1, &temp2);
	mp_add(&temp1, &temp2, &temp3);
	mp_add(&temp3, b, &tempx);
	mp_sqrt(&tempx, y1);

	mp_clear(&tempx);
	mp_clear(&temp1);
	mp_clear(&temp2);
	mp_clear(&temp3);
}

void Get_G_Y(mp_int* x1, mp_int* y1, mp_int* b, mp_int* a)
{
	mp_int tempx;
	mp_int temp1;
	mp_int temp2;
	mp_int temp3;

	mp_init(&tempx);
	mp_init(&temp1);
	mp_init(&temp2);
	mp_init(&temp3);

	//y2=x3+ax+b,随机产生X坐标,根据X坐标计算Y坐标
	mp_expt_n(x1, 3, &temp1);
	mp_mul(a, x1, &temp2);
	mp_add(&temp1, &temp2, &temp3);
	mp_add(&temp3, b, &tempx);
	mp_sqrt(&tempx, y1);

	mp_clear(&tempx);
	mp_clear(&temp1);
	mp_clear(&temp2);
	mp_clear(&temp3);
}

void Get_B(mp_int* b, mp_int* a, mp_int* p)
{
	mp_int temp;
	mp_int compare;
	mp_int temp1;
	mp_int temp2;
	mp_int temp3;
	mp_int temp4;
	mp_int temp5;

	mp_init(&compare);
	mp_zero(&compare);
	mp_init(&temp);
	mp_init(&temp1);
	mp_init(&temp2);
	mp_init(&temp3);
	mp_init(&temp4);
	mp_init(&temp5);

	while (1)
	{
		//4a3+27b2≠0 (mod p)
		GetPrime(b, 40);
		mp_expt_n(a, 3, &temp1);
		mp_sqr(b, &temp2);
		mp_mul_d(&temp1, 4, &temp3);
		mp_mul_d(&temp2, 27, &temp4);
		mp_add(&temp3, &temp4, &temp5);
		mp_mod(&temp5, p, &temp);

		if (mp_cmp(&temp, &compare) != 0)
		{
			break;
		}
	}

	mp_clear(&temp);
	mp_clear(&temp1);
	mp_clear(&temp2);
	mp_clear(&temp3);
	mp_clear(&temp4);
	mp_clear(&temp5);
}

int Two_points_add(mp_int* x1, mp_int* y1, mp_int* x2, mp_int* y2, mp_int* x3, mp_int* y3, mp_int* a, bool zero, mp_int* p)
{
	mp_int x2x1;
	mp_int y2y1;
	mp_int tempk;
	mp_int tempy;
	mp_int tempzero;
	mp_int k;
	mp_int temp;
	mp_int temp2;

	mp_init(&x2x1);
	mp_init(&y2y1);
	mp_init(&tempk);
	mp_init(&tempy);
	mp_init(&tempzero);
	mp_init(&k);

	if (zero)
	{
		mp_copy(x1, x3);
		mp_copy(y1, y3);
		zero = false;
		goto L;
	}
	mp_zero(&tempzero);
	mp_sub(x2, x1, &x2x1);
	if (mp_cmp(&x2x1, &tempzero) == -1)
	{
		mp_init(&temp);
		mp_add(&x2x1, p, &temp);
		mp_zero(&x2x1);
		mp_copy(&temp, &x2x1);
		mp_clear(&temp);
	}
	mp_sub(y2, y1, &y2y1);
	if (mp_cmp(&y2y1, &tempzero) == -1)
	{
		mp_init(&temp);
		mp_add(&y2y1, p, &temp);
		mp_zero(&y2y1);
		mp_copy(&temp, &y2y1);
		mp_clear(&temp);
	}
	if (mp_cmp(&x2x1, &tempzero) != 0)
	{
		mp_invmod(&x2x1, p, &tempk);
		mp_mulmod(&y2y1, &tempk, p, &k);
	}
	else
	{
		if (mp_cmp(&y2y1, &tempzero) == 0)
		{
			mp_init_set(&temp, 2);
			mp_mulmod(&temp, y1, p, &tempy);
			mp_clear(&temp);
			mp_invmod(&tempy, p, &tempk);
			mp_init(&temp);
			mp_sqr(x1, &temp);
			mp_init(&temp2);
			mp_mul_d(&temp, 3, &temp2);
			mp_clear(&temp);
			mp_init(&temp);
			mp_add(&temp2, a, &temp);
			mp_clear(&temp2);
			mp_mulmod(&temp, &tempk, p, &k);
			mp_clear(&temp);
		}
		else
		{
			zero = true;
			goto L;
		}
	}

	mp_init(&temp);
	mp_sqr(&k, &temp);
	mp_init(&temp2);
	mp_sub(&temp, x1, &temp2);
	mp_clear(&temp);
	mp_submod(&temp2, x2, p, x3);
	mp_clear(&temp2);

	mp_init(&temp);
	mp_sub(x1, x3, &temp);
	mp_init(&temp2);
	mp_mul(&temp, &k, &temp2);
	mp_clear(&temp);
	mp_submod(&temp2, y1, p, y3);
	mp_clear(&temp2);


L:

	mp_clear(&x2x1);
	mp_clear(&y2y1);
	mp_clear(&tempk);
	mp_clear(&tempy);
	mp_clear(&tempzero);
	mp_clear(&k);

	return 1;
}

bool Ecc_points_mul(mp_int* qx, mp_int* qy, mp_int* px, mp_int* py, mp_int* d, mp_int* a, mp_int* p)
{
	mp_int X1, Y1;
	mp_int X2, Y2;
	mp_int X3, Y3;
	mp_int XX1, YY1;
	mp_int A, P;

	int i;
	bool zero = false;
	char Bt_array[800] = { 0 };
	char cm = '1';

	mp_to_radix(d, Bt_array, 800, NULL, 2);

	mp_init(&X3);
	mp_zero(&X3);
	mp_init(&Y3);
	mp_zero(&X3);
	mp_init_copy(&X1, px);
	mp_init_copy(&X2, px);
	mp_init_copy(&XX1, px);
	mp_init_copy(&Y1, py);
	mp_init_copy(&Y2, py);
	mp_init_copy(&YY1, py);

	mp_init_copy(&A, a);
	mp_init_copy(&P, p);

	for (i = 1; i <= PRIVATE_KEY_LEN - 1; i++)
	{
		mp_copy(&X2, &X1);
		mp_copy(&Y2, &Y1);
		Two_points_add(&X1, &Y1, &X2, &Y2, &X3, &Y3, &A, zero, &P);
		mp_copy(&X3, &X2);
		mp_copy(&Y3, &Y2);
		if (Bt_array[i] == cm)
		{

			mp_copy(&XX1, &X1);
			mp_copy(&YY1, &Y1);
			Two_points_add(&X1, &Y1, &X2, &Y2, &X3, &Y3, &A, zero, &P);
			mp_copy(&X3, &X2);
			mp_copy(&Y3, &Y2);

		}

	}

	if (zero)
	{
		//cout << "It is Zero_Unit!";
		return false;//如果Q为零重新产生D
	}

	mp_copy(&X3, qx);
	mp_copy(&Y3, qy);

	mp_clear(&X1);
	mp_clear(&Y1);
	mp_clear(&X2);
	mp_clear(&Y2);
	mp_clear(&X3);
	mp_clear(&Y3);
	mp_clear(&XX1);
	mp_clear(&YY1);
	mp_clear(&A);
	mp_clear(&P);

	return true;
}

// mp_int -> buffer
int CipherWrite(mp_int* a, std::vector<uint8_t>& output)
{
	int i, j;

	for (i = 0; i < a->used; i++)
	{
		for (j = 0; j < 4; j++)
		{
			output.push_back(uint8_t(a->dp[i] >> mp_digit(8 * (3 - j))));
		}
	}

	output.push_back(uint8_t(255));
	return MP_OKAY;
}

// char -> mp_int
int ch2mp_int(mp_int* a, char* ch, int chLength)
{
	mp_digit* temp;
	int loop = chLength / 3;
	int size = ceil((float)chLength / 3);
	int i, j, res;
	if (a->alloc < size)
	{
		res = mp_grow(a, size);
		if (res != MP_OKAY)
			return res;
	}

	a->sign = MP_ZPOS;
	mp_zero(a);
	temp = a->dp;

	//以4个字符为单元循环，把字符放入的mp_int的每个单元中
	for (j = 0; j < loop; j++)
	{
		for (i = 0; i < 3; i++)
		{
			*temp <<= (mp_digit)CHAR_BIT;
			*temp |= (mp_digit)(ch[j * 3 + i]) & 255;
		}
		*temp |= 0x3000000;
		temp++;
	}

	//剩余字符少余4个时，只需一个mp_digit单元存放
	if (chLength % 3 > 0)
	{
		for (i = 0; i < chLength % 3; i++)
		{
			*temp <<= (mp_digit)CHAR_BIT;
			*temp |= (mp_digit)(ch[loop * 3 + i]) & 255;
		}
		*temp |= ((chLength % 3) << 24);
	}
	a->used = size;
	return MP_OKAY;
}

std::vector<uint8_t> Ecc_encipher(std::vector<uint8_t> rawData, mp_int* qX, mp_int* qY, mp_int* pX, mp_int* pY, mp_int* a, mp_int* p)
{
	std::pair<std::vector<uint8_t>, size_t> data;
	data.first = rawData;
	data.second = 0;
	std::vector<uint8_t> result;

	mp_int mX, mY;
	mp_int c1X, c1Y;
	mp_int c2X, c2Y;
	mp_int r;
	mp_int tempX, tempY;
	bool zero = false;
	char cipherX[280] = { 0 };
	char cipherY[280] = { 0 };

	mp_init(&mX);
	mp_init(&mY);
	mp_init(&c1X);
	mp_init(&c1Y);
	mp_init(&c2X);
	mp_init(&c2Y);
	mp_init(&r);
	mp_init(&tempX);
	mp_init(&tempY);

	unsigned int dataLength = data.first.size();
	int frequency = 0;
	int residue = 0;

	frequency = dataLength / ENCODE_BYTE_LEN;
	residue = dataLength % ENCODE_BYTE_LEN;

	const short enLengthTemp = ENCODE_BYTE_LEN / 2;

	for (size_t i = 0; i < frequency; i++)
	{
		GetPrime(&r, 100);

		for (size_t j = 0; j < enLengthTemp; j++)
		{
			cipherX[j] = data.first[data.second++];
		}

		for (size_t j = 0; j < enLengthTemp; j++)
		{
			cipherY[j] = data.first[data.second++];
		}

		ch2mp_int(&mX, cipherX, enLengthTemp);//文件存入 		
		ch2mp_int(&mY, cipherY, enLengthTemp);//文件存入

		Ecc_points_mul(&c2X, &c2Y, pX, pY, &r, a, p);//加密
		Ecc_points_mul(&tempX, &tempY, qX, qY, &r, a, p);
		Two_points_add(&mX, &mY, &tempX, &tempY, &c1X, &c1Y, a, zero, p);

		CipherWrite(&c1X, result);
		CipherWrite(&c1Y, result);
		CipherWrite(&c2X, result);
		CipherWrite(&c2Y, result);
	}
	if (residue > 0)
	{
		GetPrime(&r, 100);

		if (residue <= enLengthTemp)
		{
			for (size_t j = 0; j < residue; j++)
			{
				cipherX[j] = data.first[data.second++];
			}

			ch2mp_int(&mX, cipherX, residue);//文件存入 
			mp_zero(&mY);
		}
		else
		{
			for (size_t j = 0; j < enLengthTemp; j++)
			{
				cipherX[j] = data.first[data.second++];
			}

			for (size_t j = 0; j < residue - enLengthTemp; j++)
			{
				cipherY[j] = data.first[data.second++];
			}

			ch2mp_int(&mX, cipherX, enLengthTemp);//文件存入 
			ch2mp_int(&mY, cipherY, residue - enLengthTemp);//文件存入 
		}

		Ecc_points_mul(&c2X, &c2Y, pX, pY, &r, a, p);//加密
		Ecc_points_mul(&tempX, &tempY, qX, qY, &r, a, p);
		Two_points_add(&mX, &mY, &tempX, &tempY, &c1X, &c1Y, a, zero, p);

		CipherWrite(&c1X, result);
		CipherWrite(&c1Y, result);
		CipherWrite(&c2X, result);
		CipherWrite(&c2Y, result);
	}

	mp_clear(&mX);
	mp_clear(&mY);
	mp_clear(&c1X);
	mp_clear(&c1Y);
	mp_clear(&c2X);
	mp_clear(&c2Y);
	mp_clear(&r);
	mp_clear(&tempX);
	mp_clear(&tempY);

	return result;
}

// buffer -> mp_int
int CipherRead(mp_int* a, std::pair<std::vector<uint8_t>, size_t>& input)
{
	mp_digit* temp;
	int i, j, res;
	char ch[700] = { 0 };
	int chLength = 0;

	while (1)
	{
		ch[chLength] = input.second < input.first.size() ? input.first[input.second++] : 255;
		if (chLength % 4 == 0)
		{
			if (ch[chLength] == char(255)) goto L1;
		}
		chLength++;
	}
L1:

	if (a->alloc < chLength / 4)
	{
		res = mp_grow(a, chLength / 4);
		if (res != MP_OKAY)
			return res;
	}

	a->sign = MP_ZPOS;
	mp_zero(a);
	temp = a->dp;
	a->used = chLength / 4;

	for (i = 0; i < a->used; i++)
	{
		for (j = 0; j < 4; j++)
		{
			if (j != 0)
			{
				*temp <<= (mp_digit)CHAR_BIT;
			}
			*temp |= (mp_digit)(ch[i * 4 + j]) & 255;
		}
		temp++;
	}
	return MP_OKAY;
}

// mp_int -> char
int mp_int2ch(mp_int* a, char* ch)
{
	int i, j;
	short c = 0;
	mp_digit* temp;

	temp = a->dp;

	for (j = 0; j < a->used; j++)//以1个单元为循环，把1个单元的比特串赋给6个字符
	{
		if (j != 0)
		{
			temp++;
		}
		c = 0;
		for (i = (*temp >> 24) - 1; i >= 0; --i)
		{
			ch[j * 3 + c] = char(*temp >> mp_digit(CHAR_BIT * i));
			c++;
		}
	}
	//ch[(a->used - 1) * 3 + c] = '\0';
	return (a->used - 1) * 3 + c;
}

std::vector<uint8_t> Ecc_decipher(std::vector<uint8_t> rawData, mp_int* k, mp_int* a, mp_int* p)
{
	std::pair<std::vector<uint8_t>, size_t> data;
	data.first = rawData;
	data.second = 0;
	std::vector<uint8_t> result;

	mp_int c1X, c1Y;
	mp_int c2X, c2Y;
	mp_int tempX, tempY;
	mp_int mX, mY;
	mp_int temp;

	mp_init(&temp);
	mp_init(&c1X);
	mp_init(&c1Y);
	mp_init(&c2X);
	mp_init(&c2Y);
	mp_init(&tempX);
	mp_init(&tempY);
	mp_init(&mX);
	mp_init(&mY);

	mp_int tempZero;
	mp_init(&tempZero);

	char buffer[700] = { 0 };
	bool zero = false;

	while (data.second < data.first.size())
	{
		CipherRead(&c1X, data);
		CipherRead(&c1Y, data);
		CipherRead(&c2X, data);
		CipherRead(&c2Y, data);

		mp_zero(&tempZero);
		if (mp_cmp(&c1X, &tempZero) == 0) break;

		Ecc_points_mul(&tempX, &tempY, &c2X, &c2Y, k, a, p);

		mp_neg(&tempY, &temp);
		Two_points_add(&c1X, &c1Y, &tempX, &temp, &mX, &mY, a, zero, p);

		// **IMPORTANT**: unsigned types CANNOT be used here
		// since resSize CAN be assigned with NEGATIVE values
		// and i must also be signed to compare with resSize
		int resSize;
		resSize = mp_int2ch(&mX, buffer);
		for (int i = 0; i < resSize; ++i)
		{
			result.push_back(buffer[i]);
		}
		resSize = mp_int2ch(&mY, buffer);
		for (int i = 0; i < resSize; ++i)
		{
			result.push_back(buffer[i]);
		}
	}

	mp_clear(&c1X);
	mp_clear(&c1Y);
	mp_clear(&c2X);
	mp_clear(&c2Y);
	mp_clear(&tempX);
	mp_clear(&tempY);
	mp_clear(&mX);
	mp_clear(&mY);
	mp_clear(&temp);

	return result;
}

void EccKeyGen(std::string& p, std::string& a, std::string& gx, std::string& gy, std::string& qx, std::string& qy, std::string& k)
{
#ifdef DEBUG
	time_t st = time(0);
#endif // DEBUG

	mp_int GX;
	mp_int GY;
	mp_int K;
	mp_int A;
	mp_int B;
	mp_int QX;
	mp_int QY;
	mp_int P;

	mp_init(&GX);
	mp_init(&GY);
	mp_init(&K);
	mp_init(&A);
	mp_init(&B);
	mp_init(&QX);
	mp_init(&QY);
	mp_init(&P);

	char temp[800] = { 0 };

	srand((unsigned)time(0));

	GetPrime(&P, FINITE_FIELD_LEN);
	mp_to_radix(&P, temp, 800, NULL, 10);
	p = temp;
#ifdef DEBUG
	cout << "finite field:" << endl;
	mp_to_radix(&P, temp, 800, NULL, 10);
	cout << temp << endl;
#endif // DEBUG

	GetPrime(&A, 30);
	mp_to_radix(&A, temp, 800, NULL, 10);
	a = temp;
#ifdef DEBUG
	cout << "parameter a:" << endl;
	mp_to_radix(&A, temp, 800, NULL, 10);
	cout << temp << endl;
#endif // DEBUG

	Get_B(&B, &A, &P);
	Get_G_X_Y(&GX, &GY, &B, &A);
#ifdef DEBUG
	cout << "parameter b:" << endl;
	mp_to_radix(&B, temp, 800, NULL, 10);
	cout << temp << endl;
	cout << "x coordinate of base point:" << endl;
	mp_to_radix(&GX, temp, 800, NULL, 10);
	cout << temp << endl;
	cout << "y coordinate of base point:" << endl;
	mp_to_radix(&GY, temp, 800, NULL, 10);
	cout << temp << endl;
#endif // DEBUG

	do
	{
		GetPrime(&K, PRIVATE_KEY_LEN);
	} while (!Ecc_points_mul(&QX, &QY, &GX, &GY, &K, &A, &P));
	mp_to_radix(&K, temp, 800, NULL, 10);
	k = temp;
	mp_to_radix(&GX, temp, 800, NULL, 10);
	gx = temp;
	mp_to_radix(&GY, temp, 800, NULL, 10);
	gy = temp;
	mp_to_radix(&QX, temp, 800, NULL, 10);
	qx = temp;
	mp_to_radix(&QY, temp, 800, NULL, 10);
	qy = temp;
#ifdef DEBUG
	cout << "private key:" << endl;
	cout << temp << endl;
	cout << "x coordinate of public key:" << endl;
	mp_to_radix(&QX, temp, 800, NULL, 10);
	cout << temp << endl;
	cout << "y coordinate of public key:" << endl;
	mp_to_radix(&QY, temp, 800, NULL, 10);
	cout << temp << endl;
#endif // DEBUG

	mp_clear(&GX);
	mp_clear(&GY);
	mp_clear(&K);//私有密钥
	mp_clear(&A);
	mp_clear(&B);
	mp_clear(&QX);
	mp_clear(&QY);
	mp_clear(&P);//Fp中的p(有限域P)
#ifdef DEBUG
	cout << "Encipher used " << time(0) - st << " s." << endl;
#endif // DEBUG

	return;
}

std::vector<uint8_t> EccEnc(std::vector<uint8_t> data, std::string& p, std::string& a, std::string& gx, std::string& gy, std::string& qx, std::string& qy)
{
#ifdef DEBUG
	time_t st = time(0);
#endif // DEBUG

	mp_int GX;
	mp_int GY;
	mp_int A;
	mp_int QX;
	mp_int QY;
	mp_int P;

	mp_init(&GX);
	mp_init(&GY);
	mp_init(&A);
	mp_init(&QX);
	mp_init(&QY);
	mp_init(&P);

	char temp[800] = { 0 };
	std::vector<uint8_t> result;

	if (p != "" && a != "" && gx != "" && gy != "" && qx != "" && qy != "")
	{
		strcpy(temp, p.c_str());
		mp_read_radix(&P, temp, 10);
		strcpy(temp, a.c_str());
		mp_read_radix(&A, temp, 10);
		strcpy(temp, gx.c_str());
		mp_read_radix(&GX, temp, 10);
		strcpy(temp, gy.c_str());
		mp_read_radix(&GY, temp, 10);
		strcpy(temp, qx.c_str());
		mp_read_radix(&QX, temp, 10);
		strcpy(temp, qy.c_str());
		mp_read_radix(&QY, temp, 10);
		result = Ecc_encipher(data, &QX, &QY, &GX, &GY, &A, &P);
	}

	mp_clear(&GX);
	mp_clear(&GY);
	mp_clear(&A);
	mp_clear(&QX);
	mp_clear(&QY);
	mp_clear(&P);

#ifdef DEBUG
	cout << "Encipher used " << time(0) - st << " s." << endl;
#endif // DEBUG

	return result;
}

std::vector<uint8_t> EccDec(std::vector<uint8_t> data, std::string p, std::string a, std::string k)
{
#ifdef DEBUG
	time_t st = time(0);
#endif // DEBUG

	mp_int K;
	mp_int A;
	mp_int P;

	mp_init(&K);
	mp_init(&A);
	mp_init(&P);

	char temp[800] = { 0 };
	std::vector<uint8_t> result;

	if (k != "" && a != "" && p != "")
	{
		strcpy(temp, k.c_str());
		mp_read_radix(&K, temp, 10);
		strcpy(temp, a.c_str());
		mp_read_radix(&A, temp, 10);
		strcpy(temp, p.c_str());
		mp_read_radix(&P, temp, 10);
		result = Ecc_decipher(data, &K, &A, &P);
	}

	mp_clear(&K);
	mp_clear(&A);
	mp_clear(&P);

#ifdef DEBUG
	cout << "Decipher used " << time(0) - st << " s." << endl;
#endif // DEBUG

	return result;
}
