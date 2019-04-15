// SystemSettingDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "testESLdll.h"
#include "SystemSettingDlg.h"
#include "afxdialogex.h"


// SystemSettingDlg 对话框

IMPLEMENT_DYNAMIC(SystemSettingDlg, CDialog)

SystemSettingDlg::SystemSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG1, pParent)
	, m_nTestDelay(0)
{

}

SystemSettingDlg::~SystemSettingDlg()
{
}

void SystemSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_TEST_DELAY, m_nTestDelay);
}


BEGIN_MESSAGE_MAP(SystemSettingDlg, CDialog)
END_MESSAGE_MAP()


// SystemSettingDlg 消息处理程序
