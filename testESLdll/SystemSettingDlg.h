#pragma once


// SystemSettingDlg �Ի���

class SystemSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(SystemSettingDlg)

public:
	SystemSettingDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~SystemSettingDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	// ��ʱ
	int m_nTestDelay;
};
