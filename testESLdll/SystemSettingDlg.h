#pragma once


// SystemSettingDlg 对话框

class SystemSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(SystemSettingDlg)

public:
	SystemSettingDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~SystemSettingDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	// 延时
	int m_nTestDelay;
};
