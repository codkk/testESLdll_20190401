#pragma once
#include "afxwin.h"

//������ strPath: Ŀ¼������·����ע�ⲻҪ��'/'��β

//����ֵ�� ���ΪĿ¼�������棬���򷵻ؼ�
//�ж�һ��Ŀ¼�Ƿ����
BOOL FolderExist(CString strPath);
/*
�ж��ļ���Ŀ¼�Ƿ����

�������ļ���Ŀ¼����������(��·��)���������ļ�����Ҳ������Ŀ¼��

����ֵ: ������ڣ������棬���򷵻ؼ١�
*/
BOOL  FileExist(CString strFileName);
/*
����һ��Ŀ¼
*/
BOOL CreateFolder(CString strPath);
/*
�ļ���С��

������ �ļ�����, ע�⣬���������Ŀ¼(�ļ���)���ú�������ֵ����ݹ����Ŀ¼�������ļ���С�����Ըú���ֻ�������ļ���С��ͳ�ơ�

����ֵ�� �ļ���С����λΪByte
*/
DWORD GetFileSize(CString filepath);
/*
�����ļ��еĴ�С

�������ļ��е�����·�����ú�����ʹ�����ļ�

����ֵ�� �ļ��еĴ�С����λΪbyte
*/
long GetFolderSize(CString strDirPath);