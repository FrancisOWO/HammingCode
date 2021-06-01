#include <sstream>
#include <string>
#include <time.h>
#include "HammingBack.h"
using namespace std;

//����һ��10�������������߱�����ǰ��string,string����ΪVALUE_SIZE
string HammingBack::int2bstring(int num)
{
    //ֻȡ���ݵĵ�8λ������8λ����װ��res
    string res;
    for (int i = 0; i < VALUE_SIZE; i++)
		res.push_back(!!(num & (0x1 << (VALUE_SIZE - 1 - i))) + '0');

	return res;
}

//����һ���߱�����ǰ�Ķ�����string������10������
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

//��������10�����������㺺�����룬���ֳ�����VALUE_SIZE����
int HammingBack::calHammingDistance(int num1, int num2, int size)
{
    if (size > int(sizeof(int)*8))
		return -1;

	int res = 0;

	//���ʽ���㣬�ڵ�iλ��ͬ��res +1
	for (int i = 0; i < size; i++)
		res += !!((num1 & (0x1 << i)) ^ (num2 & (0x1 << i)));

	return res;
}

//����2��string�����01���������������룬�Գ���Ϊ��׼�����32λ
int HammingBack::calHammingDistance(std::string num1, std::string num2)
{
	unsigned int a, b;

	a = bstring2int(num1);
	b = bstring2int(num2);

	int biggerSize = num1.length() > num2.length() ? num1.length() : num2.length();
	return calHammingDistance(a, b, biggerSize);
}

//��������λ��������У��λ��
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

//��������λ����У��λ��ţ����������У��λ���õ�����λ����
int HammingBack::calCheckDatalen(int dataLen, int checkNo)
{
    int checkLen = calCheckLen(dataLen);
    //��ų���Χ���˳�
    if(checkNo > checkLen || checkNo < 1)
        return 0;
     //����У��λ���������λ��
    int fullLen = dataLen + checkLen;   //�ܳ���
    int divider = (1 << checkNo);
    int base = (divider >> 1);
    int dividend = fullLen + 1 - base;
    int mod = dividend % divider;
    if(mod > base)
        mod = base;
    //��һ������λΪ2^n������ȡ2^n��������2^n��
    int dlen = ((dividend/divider)<<(checkNo-1)) + mod;
    return dlen;
}

//��������λ����У��λ��ţ��������У��λ���õ�����λ�������
void HammingBack::calCheckDnoList(int dataLen, int checkNo, int *dnoList, int dlen)
{
    int checkLen = calCheckLen(dataLen);
    //��ų���Χ���˳�
    if(checkNo > checkLen || checkNo < 1)
        return;
    int base = (1 << (checkNo-1));
    int delta = base;
    //��һ������λΪ2^n������ȡ2^n��������2^n��
    for(int i = 0; i < dlen; i++){
        dnoList[i] = base;
        base++;
        if((i+1) % delta == 0)
            base += delta;
    }
}

//����һ��string�����01��������������HammingResult
//HammingResult : {full, data, check}
HammingResult HammingBack::calHammingResult(string data)
{
	string resFull;		//װ�ܵ�����
	string resData;		//װ����
	string resCheck;	//װУ��λ����λ��ǰ

	int dataLen = data.length();
    int checkLen = calCheckLen(dataLen);
	int fullLen = dataLen + checkLen;
    int base = 1;
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

//תΪfull data check���ַ�����ʽ
string HammingBack::hr2string(HammingResult hr)
{
	string res = hr.fullRes + ' ' + hr.data + ' ' + hr.check;
    return res;
}

//�������һ��nbit��string����λ��ǰ
string HammingBack::randomBstring()
{
    srand(time(0));
    return int2bstring(rand());
}
