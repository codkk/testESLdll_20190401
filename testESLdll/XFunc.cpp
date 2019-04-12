#include "windows.h"
#include "stdafx.h"
#include "XFunc.h"
//参数： strPath: 目录的完整路径，注意不要以'/'结尾

//返回值： 如果为目录，返回真，否则返回假
//判断一个目录是否存在
BOOL FolderExist(CString strPath)
{
	WIN32_FIND_DATA wfd;
	BOOL rValue = FALSE;
	HANDLE hFind = FindFirstFile(strPath, &wfd);
	if ((hFind != INVALID_HANDLE_VALUE) &&
		(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
	{
		rValue = TRUE;
	}
	FindClose(hFind);
	return rValue;
}
/*
判断文件或目录是否存在

参数：文件或目录的完整名字(带路径)，可以是文件名，也可以是目录名

返回值: 如果存在，返回真，否则返回假。
*/
BOOL  FileExist(CString strFileName)
{
	CFileFind fFind;
	return fFind.FindFile(strFileName);
}


/*
创建一个目录
*/
BOOL CreateFolder(CString strPath)
{
	SECURITY_ATTRIBUTES attrib;
	/*
	设置目录的常见属性

	*/
	attrib.bInheritHandle = FALSE;
	attrib.lpSecurityDescriptor = NULL;
	attrib.nLength = sizeof(SECURITY_ATTRIBUTES);
	return ::CreateDirectory(strPath, &attrib);
}

/*
文件大小：

参数： 文件名字, 注意，如果给的是目录(文件夹)，该函数返回值不会递归计算目录下所有文件大小。所以该函数只适用于文件大小的统计。

返回值： 文件大小。单位为Byte
*/
DWORD GetFileSize(CString filepath)
{
	WIN32_FIND_DATA fileInfo;
	HANDLE hFind;
	DWORD fileSize;
	CString filename;
	filename = filepath;
	hFind = FindFirstFile(filename, &fileInfo);
	if (hFind != INVALID_HANDLE_VALUE)
		fileSize = fileInfo.nFileSizeLow;

	FindClose(hFind);
	return fileSize;
}

/*
计算文件夹的大小

参数：文件夹的完整路径。该函数不使用与文件

返回值： 文件夹的大小，单位为byte
*/
long GetFolderSize(CString strDirPath)
{
	CString strFilePath;

	long    dwDirSize = 0;
	strFilePath += strDirPath;
	strFilePath += "//*.*";
	CFileFind finder;
	BOOL bFind = finder.FindFile(strFilePath);
	while (bFind)
	{
		bFind = finder.FindNextFile();
		if (!finder.IsDots())
		{
			CString strTempPath = finder.GetFilePath();
			if (!finder.IsDirectory())
			{
				dwDirSize += finder.GetLength();
			}
			else
			{
				dwDirSize += GetFolderSize(strTempPath);
			}
		}
	}
	finder.Close();
	return dwDirSize;
}