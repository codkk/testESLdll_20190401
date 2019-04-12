#pragma once
#include "afxwin.h"

//参数： strPath: 目录的完整路径，注意不要以'/'结尾

//返回值： 如果为目录，返回真，否则返回假
//判断一个目录是否存在
BOOL FolderExist(CString strPath);
/*
判断文件或目录是否存在

参数：文件或目录的完整名字(带路径)，可以是文件名，也可以是目录名

返回值: 如果存在，返回真，否则返回假。
*/
BOOL  FileExist(CString strFileName);
/*
创建一个目录
*/
BOOL CreateFolder(CString strPath);
/*
文件大小：

参数： 文件名字, 注意，如果给的是目录(文件夹)，该函数返回值不会递归计算目录下所有文件大小。所以该函数只适用于文件大小的统计。

返回值： 文件大小。单位为Byte
*/
DWORD GetFileSize(CString filepath);
/*
计算文件夹的大小

参数：文件夹的完整路径。该函数不使用与文件

返回值： 文件夹的大小，单位为byte
*/
long GetFolderSize(CString strDirPath);