#pragma once
#include <string>

//��߲�����32
#define VALUE_SIZE 8

struct HammingResult {
	std::string fullRes;
	std::string data;
	std::string check;
};

class HammingBack {

public:
	//����һ��10�������������߱�����ǰ��string,string����ΪVALUE_SIZE
	std::string int2bstring(int num);

	//����һ���߱�����ǰ�Ķ�����string������10������
	int bstring2int(std::string num);

	//��������10�����������㺺�����룬���ֳ�����VALUE_SIZE����
	int calHammingDistance(int num1, int num2, int size = VALUE_SIZE);

	//����2��string�����01���������������룬�Գ���Ϊ��׼�����32λ
	int calHammingDistance(std::string num1, std::string num2);

	//����һ��string�����01��������������HammingResult
	//HammingResult : {full, data, check}
	HammingResult calHammingResult(std::string data);

	//תΪfull data check���ַ�����ʽ
	std::string hr2string(HammingResult hr);
};