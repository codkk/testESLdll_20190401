#pragma once
#include "afxwin.h"
# include "HalconCpp.h" //halcon
# include "HDevThread.h"
using namespace HalconCpp;
// CamSelDlg 对话框

class CamSelDlg : public CDialog
{
	DECLARE_DYNAMIC(CamSelDlg)

public:
	CamSelDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CamSelDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SELECTCAM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_comboCameralist;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonSavecam();
};
