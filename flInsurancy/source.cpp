#include<memory>
#include<memory.h>
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
		memcpy(dest[1], src[0],fileSize);
		for(file_size i = 0;i<10;i++)
		{
			printf("%X ",*src[i]);
		}
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
		for(file_size i = 0;i<10;i++)
		{
			printf("%X ",*src[i]);
		}
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
			std::cout << "File Mapped!\nFile Size: " << s.getFileSize()
				<< "Byte(s)" << std::endl;
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
			std::cout << "File Mapped!\nFile Size: " << s.getFileSize()
				<< "Byte(s)" << std::endl;
			auto sampleRule = std::make_shared<SampleDecodeRule>();
			RuleExecutor::execute(sampleRule.get(),
				s, argv[3]);
		}
		catch(const char* e)
		{
			std::cout << "Error:" << e << std::endl;
		}
		catch (unsigned long e)
		{
			std::cout << "Error: File not found" << e;
		}
	}
	return 0;
}
