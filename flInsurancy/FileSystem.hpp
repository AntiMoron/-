#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include"config.hpp"
#include<windows.h>
#include<cstdio>
#include<io.h>
namespace flins
{
	enum OPEN_MODE
	{
		READ_MODE,
		READ_WRITE_MODE
	};
//	#ifdef WIN_OS
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
//	#endif // WIN_OS
}

#endif // FILESYSTEM_HPP
