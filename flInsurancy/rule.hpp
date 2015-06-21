#ifndef RULE_HPP
#define RULE_HPP
#include"FileSystem.hpp"
namespace flins
{
	class FileRule
	{
	public:
		typedef FileMappingSystem::byte byte;
		typedef FileMappingSystem::file_size file_size;
		virtual file_size getOutputFileSize(file_size origin) const throw() = 0;
		virtual void operateFiles(const FileMappingSystem& src,
								const FileMappingSystem& dest) = 0;
	};

	class RuleExecutor
	{
		RuleExecutor() = delete;
	public:
		static void execute(FileRule* rule,
						const FileMappingSystem& src,
						const char* dest)
		{
			FileMappingSystem::generateFile(dest,rule->getOutputFileSize(src.getFileSize()));
			FileMappingSystem o(dest,READ_WRITE_MODE);
			rule->operateFiles(src,o);
		}
	};
}

#endif // RULE_HPP
