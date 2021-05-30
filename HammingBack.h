#pragma once
#include <string>

//最高不超过32
#define VALUE_SIZE 8

struct HammingResult {
	std::string fullRes;
	std::string data;
	std::string check;
};

class HammingBack {

public:
	//传入一个10进制数，传出高比特在前的string,string长度为VALUE_SIZE
	std::string int2bstring(int num);

	//传入一个高比特在前的二进制string，传出10进制数
	int bstring2int(std::string num);

	//传入两个10进制数，计算汉明距离，数字长度由VALUE_SIZE限制
	int calHammingDistance(int num1, int num2, int size = VALUE_SIZE);

	//传入2个string代表的01串，传出汉明距离，以长串为标准，最高32位
	int calHammingDistance(std::string num1, std::string num2);

	//传入一个string代表的01串，传出编码后的HammingResult
	//HammingResult : {full, data, check}
	HammingResult calHammingResult(std::string data);

	//转为full data check的字符串形式
	std::string hr2string(HammingResult hr);
};