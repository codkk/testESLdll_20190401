// ESLdll.h : ESLdll DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CESLdllApp
// �йش���ʵ�ֵ���Ϣ������� ESLdll.cpp
//

class CESLdllApp : public CWinApp
{
public:
	CESLdllApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
