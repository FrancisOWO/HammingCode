#include <sstream>
#include <string>
#include "HammingBack.h"
using namespace std;

string HammingBack::int2bstring(int num)
{
	//ֻȡ��8λ
	
	string res;

	//���ʽ���ܽ���8λ����װ��res
	for (int i = 0; i < VALUE_SIZE; i++)
		res.push_back(!!(num & (0x1 << (VALUE_SIZE - 1 - i))) + '0');

	return res;
}

int HammingBack::bstring2int(string num)
{
	int res = 0;

	int pos = num.length() - 1, base = 1;
	for (unsigned int i = 0; i < num.length(); i++) {
		res += (num.at(pos - i) - '0') ? base : 0;
		base <<= 1;
	}
	return res;
}

int HammingBack::calHammingDistance(int num1, int num2, int size)
{
	if (size > sizeof(int) * 8)
		return -1;

	int res = 0;

	//���ʽ���㣬�ڵ�iλ��ͬ��res +1
	for (int i = 0; i < size; i++)
		res += !!((num1 & (0x1 << i)) ^ (num2 & (0x1 << i)));

	return res;
}

int HammingBack::calHammingDistance(std::string num1, std::string num2)
{
	unsigned int a, b;

	a = bstring2int(num1);
	b = bstring2int(num2);

	int biggerSize = num1.length() > num2.length() ? num1.length() : num2.length();
	return calHammingDistance(a, b, biggerSize);
}

HammingResult HammingBack::calHammingResult(string data)
{
	string resFull;		//װ�ܵ�����
	string resData;		//װ����
	string resCheck;	//װУ��λ����λ��ǰ

	int dataLen = data.length();
	int base = 1;

	int checkLen = 0;
	while (base - 1 < dataLen + checkLen) {
		base = base << 1;
		checkLen++;
	}

	int fullLen = dataLen + checkLen;
	base = 1;
	int dataPos = 0;
	for (int i = 0; i < fullLen; i++) {
		if (i + 1 == base) {	//��Ҫ����µ�У��λ
			resCheck.push_back('0');
			resFull.push_back('0');
			base <<= 1;
		}
		else
			resFull.push_back(data.at(dataPos++));
	}

	//ά��У��λ
	for (int i = 0; i < checkLen; i++) {
		int count = 0, checkPos = 1 << i;
		//����ÿ��У��λ��Ҫ�ҵ�У��λҪУ�����Щλ
		//��2λУ��λ����xxx1x��ÿ2������2������
		//��3��У��λ����xx1xx��ÿ4������4������
		for (int j = checkPos - 1; j < fullLen; j += checkPos)
			for (int k = 0; k != checkPos && j < fullLen; k++, j++)
				count ^= resFull.at(j) - '0';
		resCheck.at(i) = '0' + count;
		resFull.at(checkPos - 1) = '0' + count;
	}
	resData = data;

	return HammingResult({ resFull, resData, resCheck });
}

string HammingBack::hr2string(HammingResult hr)
{
	string res = hr.fullRes + ' ' + hr.data + ' ' + hr.check;
	return res;
}
