#include "windows.h"
#include "stdafx.h"
#include "XFunc.h"
//������ strPath: Ŀ¼������·����ע�ⲻҪ��'/'��β

//����ֵ�� ���ΪĿ¼�������棬���򷵻ؼ�
//�ж�һ��Ŀ¼�Ƿ����
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
�ж��ļ���Ŀ¼�Ƿ����

�������ļ���Ŀ¼����������(��·��)���������ļ�����Ҳ������Ŀ¼��

����ֵ: ������ڣ������棬���򷵻ؼ١�
*/
BOOL  FileExist(CString strFileName)
{
	CFileFind fFind;
	return fFind.FindFile(strFileName);
}


/*
����һ��Ŀ¼
*/
BOOL CreateFolder(CString strPath)
{
	SECURITY_ATTRIBUTES attrib;
	/*
	����Ŀ¼�ĳ�������

	*/
	attrib.bInheritHandle = FALSE;
	attrib.lpSecurityDescriptor = NULL;
	attrib.nLength = sizeof(SECURITY_ATTRIBUTES);
	return ::CreateDirectory(strPath, &attrib);
}

/*
�ļ���С��

������ �ļ�����, ע�⣬���������Ŀ¼(�ļ���)���ú�������ֵ����ݹ����Ŀ¼�������ļ���С�����Ըú���ֻ�������ļ���С��ͳ�ơ�

����ֵ�� �ļ���С����λΪByte
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
�����ļ��еĴ�С

�������ļ��е�����·�����ú�����ʹ�����ļ�

����ֵ�� �ļ��еĴ�С����λΪbyte
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