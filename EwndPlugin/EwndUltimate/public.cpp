#include "public.h"

void WriteUInt(unsigned char* pBuf, unsigned int data)
{
	*(unsigned int*)pBuf = data;
}


std::string �ı�_����д(std::string& str)
{
	std::string ret;
	int nStrLen = str.size();
	if (!nStrLen) {
		return ret;
	}
	ret.resize(nStrLen);
	for (unsigned int n = 0; n < nStrLen; ++n) {
		ret[n] = std::toupper(str[n]);
	}
	return ret;
}