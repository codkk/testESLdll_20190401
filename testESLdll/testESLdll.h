
// testESLdll.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CtestESLdllApp: 
// �йش����ʵ�֣������ testESLdll.cpp
//

class CtestESLdllApp : public CWinApp
{
public:
	CtestESLdllApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CtestESLdllApp theApp;