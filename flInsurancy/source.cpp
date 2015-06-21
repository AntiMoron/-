#include<memory>
#include<memory.h>
#include<iostream>
#include"rule.hpp"

using namespace flins;


class SampleRule : public FileRule
{
	file_size getOutputFileSize(file_size origin) const throw()
	{
		return origin + 1;
	}
	void operateFiles(const FileMappingSystem& src,
					const FileMappingSystem& dest)
	{
		file_size fileSize = src.getFileSize();
		file_size blockSize = 1024 * 1024;
		file_size times = fileSize / blockSize;
		file_size tail = fileSize % blockSize;
		for(file_size t = 0;t < times; ++t)
		{
			byte* sp = src[blockSize * t];
			byte* op = dest[blockSize * t + 1];
			memcpy(op,sp,blockSize);
		}
		memcpy(src[blockSize * times],
				dest[blockSize * times + 1],tail);
	}
};

int main(int cmdCount,char* argv[])
{
	try
	{
		typedef FileMappingSystem::byte byte;
		FileMappingSystem s("123.wmv",READ_WRITE_MODE);
		auto sampleRule = std::make_shared<SampleRule>();
		RuleExecutor::execute(sampleRule.get(),
					s,"output.wmv");
	}
	catch(unsigned long e)
	{
		std::cout << "Error: File not found" << e;
	}
//	if(cmdCount != 3)
//	{
//		std::cout << "Usage:\n"
//		"executable srcPath destPath\n" << std::endl;
//		return -1;
//	}
	return 0;
}

