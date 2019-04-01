#pragma once
#include "afxwin.h"
# include "HalconCpp.h" //halcon
# include "HDevThread.h"
using namespace HalconCpp;
// CamSelDlg �Ի���

class CamSelDlg : public CDialog
{
	DECLARE_DYNAMIC(CamSelDlg)

public:
	CamSelDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CamSelDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SELECTCAM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_comboCameralist;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonSavecam();
};
