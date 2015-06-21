#include<memory>
#include<memory.h>
#include<iostream>
#include"rule.hpp"

using namespace flins;


class SampleEncodeRule : public FileRule
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

class SampleDecodeRule : public FileRule
{
	file_size getOutputFileSize(file_size origin) const throw()
	{
		return origin - 1;
	}
	void operateFiles(const FileMappingSystem& src,
		const FileMappingSystem& dest)
	{
		file_size fileSize = src.getFileSize();
		byte* sp = src[1];
		byte* op = dest[0];
		memcpy(op, sp, fileSize - 1);
	}
};



int main(int cmdCount,char* argv[])
{
	if(cmdCount != 4)
	{
		std::cout << "Usage:\n"
		"executable encode srcPath destPath\n"
		"executable decode srcPath destPath\n"<< std::endl;
		return -1;
	}
	if (strcmp(argv[1],"encode") == 0)
	{
		try
		{
			typedef FileMappingSystem::byte byte;
			FileMappingSystem s(argv[2], READ_WRITE_MODE);
			std::cout << "File Mapped." << std::endl;
			auto sampleRule = std::make_shared<SampleEncodeRule>();
			RuleExecutor::execute(sampleRule.get(),
				s, argv[3]);
		}
		catch(const char* e)
		{
			std::cout << "Error:" << e << std::endl;
		}
		catch (unsigned long e)
		{
			std::cout << "Error: File not found" << e << std::endl;
		}
	}
	else if (strcmp(argv[1], "decode") == 0)
	{
		try
		{
			typedef FileMappingSystem::byte byte;
			FileMappingSystem s(argv[2], READ_WRITE_MODE);
			std::cout << "File Mapped." << std::endl;
			auto sampleRule = std::make_shared<SampleDecodeRule>();
			RuleExecutor::execute(sampleRule.get(),
				s, argv[3]);
		}
		catch (unsigned long e)
		{
			std::cout << "Error: File not found" << e;
		}
	}
	return 0;
}
