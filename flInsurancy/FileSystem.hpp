#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP
#include"config.hpp"

namespace flins
{
	enum OPEN_MODE
	{
		READ_MODE,
		READ_WRITE_MODE
	};
	#ifdef WIN_OS
	#include<windows.h>
	#include<cstdio>
	#include<io.h>
	class FileMappingSystem
	{
	public:
		typedef unsigned long long file_size;
		typedef unsigned char byte;

		static void generateFile(const char* fileName,file_size s)
		{
			FILE* fp = NULL;
			fopen_s(&fp, fileName, "wb");
			if(s > 1024)
			{
				file_size bufSize = 1024 * 1024;
				file_size times = (s / bufSize);
				file_size tail = (s % bufSize);
				while(times > 30)
				{
					bufSize *= 2;
					times = (s / bufSize);
					tail = (s % bufSize);
				}
				byte* chBuf = new byte[bufSize];
				while(times--)
				{
					fwrite(chBuf,bufSize,1,fp);
				}
				byte ch;
				while(tail--)
					fwrite(&ch,sizeof(ch),1,fp);
				delete [] chBuf;
			}
			else
			{
				byte ch;
				while(s--)
					fwrite(&ch,sizeof(ch),1,fp);
			}
			fclose(fp);
		}

		FileMappingSystem(const char* fileName,
					enum OPEN_MODE mode) throw(DWORD)
		{
			DWORD dwFlags = 0;
			switch(mode)
			{
			case READ_MODE:
				dwFlags |= GENERIC_READ;
				break;
			case READ_WRITE_MODE:
				dwFlags |= (GENERIC_READ | GENERIC_WRITE);
				break;
			}
			hFile = CreateFile(fileName,dwFlags,0,
								NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
								 if (hFile == INVALID_HANDLE_VALUE)
			if (hFile == INVALID_HANDLE_VALUE)
			{
				throw GetLastError();
			}
			hFileMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, 0, NULL);
			if (hFileMap == NULL)
			{
				throw GetLastError();
			}

			SYSTEM_INFO SysInfo;
			GetSystemInfo(&SysInfo);
			DWORD dwGran = SysInfo.dwAllocationGranularity;
			DWORD dwFileSizeHigh;
			fileSize = GetFileSize(hFile, &dwFileSizeHigh);
			fileSize |= (file_size(dwFileSizeHigh) << 32);
			CloseHandle(hFile);


			qwFileOffset = 0;

			file_size fileSizeMode = fileSize - (fileSize % dwGran);
			if (fileSizeMode < fileSize)
				fileSizeMode += dwGran;
			DWORD dwBlockBytes = fileSizeMode;
			if (fileSize < dwBlockBytes)
				dwBlockBytes = DWORD(fileSize);
			if (qwFileOffset >= 0)
			{
				// 映射视图
				lpbMapAddress = (byte*)(MapViewOfFile(hFileMap,FILE_MAP_ALL_ACCESS,
					0, 0,
					dwBlockBytes));
				if (lpbMapAddress == NULL)
				{
					throw GetLastError();
				}
			}
		}

		~FileMappingSystem()
		{
			CloseHandle(hFileMap);
		}
        byte* operator [] (file_size pos) const
        {
			return lpbMapAddress + pos;
        }

		file_size getFileOffset()const
		{
			return qwFileOffset;
		}
		file_size getFileSize() const
		{
			return fileSize;
		}
	private:
		HANDLE hFile,hFileMap;
		byte* lpbMapAddress;
		file_size qwFileOffset;
		file_size fileSize;
	};
	#endif // WIN_OS

//	#ifdef LINUX_OS
	#include <sys/mman.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <fcntl.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <unistd.h>
    class FileMappingSystem
    {
    private:
		struct MappedByte
		{
			unsigned char data;
			MappedByte(){}
			MappedByte(unsigned char d)
			{
				data = d;
			}
			MappedByte& operator = (unsigned char d)
			{
				data = d;
				return *this;
			}
		};
    public:
		typedef off_t file_size;
		typedef MappedByte byte;
		static void generateFile(const char* fileName,file_size s)
		{
			FILE* fp = fopen(fileName,"w");
			fclose(fp);
			truncate(fileName, s);
		}

		FileMappingSystem(const char* fileName,
					enum OPEN_MODE mode) throw(const char*)
		{
			int fd;
			struct stat sb;
			off_t offset = 0, pa_offset;
			size_t length;
			ssize_t s;
			switch(mode)
			{
			case READ_MODE:
				fd = open(fileName, O_RDONLY);
				break;
			case READ_WRITE_MODE:
				fd = open(fileName, O_RDWR);
				break;
			}
			if (fd == -1)
				throw ("open");
			if (fstat(fd, &sb) == -1)           /* To obtain file size */
				throw ("fstat");
			pa_offset = offset & ~(sysconf(_SC_PAGE_SIZE) - 1);
			/* offset for mmap() must be page aligned */
			if (offset >= sb.st_size)
			{
				throw (stderr, "offset is past end of file\n");
			}
			fileSize = sb.st_size;
			length = sb.st_size - offset;
			int flag = PROT_READ;
			if(READ_WRITE_MODE == mode)
			{
				flag |= PROT_WRITE;
			}
			addr = reinterpret_cast<byte*>(mmap(NULL, length + offset - pa_offset, flag,
						MAP_PRIVATE, fd, pa_offset));
			if (addr == MAP_FAILED)
				throw ("mmap");
		}

        byte* operator [] (file_size pos) const
        {
			return addr + pos;
        }

		file_size getFileOffset()const
		{
			return fileOffset;
		}
		file_size getFileSize() const
		{
			return fileSize;
		}
	private:
		byte *addr;
		file_size fileOffset;
		file_size fileSize;
    };

//	#endif // LINUX_OS
}

#endif // FILESYSTEM_HPP
