#include <sstream>
#include <string>
#include "HammingBack.h"
using namespace std;

string HammingBack::int2bstring(int num)
{
	//只取低8位
	
	string res;

	//表达式功能将低8位逆序装入res
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

	//表达式计算，第低i位不同则res +1
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
	string resFull;		//装总的码字
	string resData;		//装数据
	string resCheck;	//装校验位，低位在前

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
		if (i + 1 == base) {	//需要添加新的校验位
			resCheck.push_back('0');
			resFull.push_back('0');
			base <<= 1;
		}
		else
			resFull.push_back(data.at(dataPos++));
	}

	//维护校验位
	for (int i = 0; i < checkLen; i++) {
		int count = 0, checkPos = 1 << i;
		//对于每个校验位，要找到校验位要校验的那些位
		//第2位校验位满足xxx1x，每2个跳过2个……
		//第3个校验位满足xx1xx，每4个跳过4个……
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
