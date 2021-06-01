#include <sstream>
#include <string>
#include <time.h>
#include "HammingBack.h"
using namespace std;

//传入一个10进制数，传出高比特在前的string,string长度为VALUE_SIZE
string HammingBack::int2bstring(int num)
{
    //只取数据的低8位，将低8位逆序装入res
    string res;
    for (int i = 0; i < VALUE_SIZE; i++)
		res.push_back(!!(num & (0x1 << (VALUE_SIZE - 1 - i))) + '0');

	return res;
}

//传入一个高比特在前的二进制string，传出10进制数
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

//传入两个10进制数，计算汉明距离，数字长度由VALUE_SIZE限制
int HammingBack::calHammingDistance(int num1, int num2, int size)
{
    if (size > int(sizeof(int)*8))
		return -1;

	int res = 0;

	//表达式计算，第低i位不同则res +1
	for (int i = 0; i < size; i++)
		res += !!((num1 & (0x1 << i)) ^ (num2 & (0x1 << i)));

	return res;
}

//传入2个string代表的01串，传出汉明距离，以长串为标准，最高32位
int HammingBack::calHammingDistance(std::string num1, std::string num2)
{
	unsigned int a, b;

	a = bstring2int(num1);
	b = bstring2int(num2);

	int biggerSize = num1.length() > num2.length() ? num1.length() : num2.length();
	return calHammingDistance(a, b, biggerSize);
}

//传入数据位数，传出校验位数
int HammingBack::calCheckLen(int dataLen)
{
    int base = 1;
    int checkLen = 0;
    while (base - 1 < dataLen + checkLen) {
        base = base << 1;
        checkLen++;
    }
    return checkLen;
}

//传入数据位数和校验位标号，传出计算该校验位所用的数据位个数
int HammingBack::calCheckDatalen(int dataLen, int checkNo)
{
    int checkLen = calCheckLen(dataLen);
    //标号超范围，退出
    if(checkNo > checkLen || checkNo < 1)
        return 0;
     //计算校验位所需的数据位数
    int fullLen = dataLen + checkLen;   //总长度
    int divider = (1 << checkNo);
    int base = (divider >> 1);
    int dividend = fullLen + 1 - base;
    int mod = dividend % divider;
    if(mod > base)
        mod = base;
    //第一个数据位为2^n，连着取2^n个，跳过2^n个
    int dlen = ((dividend/divider)<<(checkNo-1)) + mod;
    return dlen;
}

//传入数据位数和校验位标号，填充计算该校验位所用的数据位标号序列
void HammingBack::calCheckDnoList(int dataLen, int checkNo, int *dnoList, int dlen)
{
    int checkLen = calCheckLen(dataLen);
    //标号超范围，退出
    if(checkNo > checkLen || checkNo < 1)
        return;
    int base = (1 << (checkNo-1));
    int delta = base;
    //第一个数据位为2^n，连着取2^n个，跳过2^n个
    for(int i = 0; i < dlen; i++){
        dnoList[i] = base;
        base++;
        if((i+1) % delta == 0)
            base += delta;
    }
}

//传入一个string代表的01串，传出编码后的HammingResult
//HammingResult : {full, data, check}
HammingResult HammingBack::calHammingResult(string data)
{
	string resFull;		//装总的码字
	string resData;		//装数据
	string resCheck;	//装校验位，低位在前

	int dataLen = data.length();
    int checkLen = calCheckLen(dataLen);
	int fullLen = dataLen + checkLen;
    int base = 1;
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

//转为full data check的字符串形式
string HammingBack::hr2string(HammingResult hr)
{
	string res = hr.fullRes + ' ' + hr.data + ' ' + hr.check;
    return res;
}

//随机生成一个nbit的string，高位在前
string HammingBack::randomBstring()
{
    srand(time(0));
    return int2bstring(rand());
}
