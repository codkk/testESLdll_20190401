// CamSelDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "testESLdll.h"
#include "CamSelDlg.h"
#include "afxdialogex.h"

//�쳣������
void MyExcepHandle(const HalconCpp::HException &except)
{
	throw except;
}
void ShowException(HalconCpp::HException & except)
{
	CString str(except.ErrorMessage());
	str.Replace(_T("HALCON"), _T(""));
	AfxMessageBox(str);
	//MessageBox((HWND)hl_wnd, str, _T("Error"), MB_OK);
}
// CamSelDlg �Ի���

IMPLEMENT_DYNAMIC(CamSelDlg, CDialog)

CamSelDlg::CamSelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG_SELECTCAM, pParent)
{

}

CamSelDlg::~CamSelDlg()
{
}

void CamSelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SEL_CAM, m_comboCameralist);
}


BEGIN_MESSAGE_MAP(CamSelDlg, CDialog)
END_MESSAGE_MAP()


// CamSelDlg ��Ϣ�������


BOOL CamSelDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	//�鿴��ǰ�����
	try
	{
		HalconCpp::HTuple Information, ValueList;
		HalconCpp::InfoFramegrabber("GenICamTL", "device", &Information, &ValueList);
		int CamNum = ValueList.Length();
		for (int i = 0; i < CamNum; i++)
		{
			CString str;
			str = ValueList[i].S();
			m_comboCameralist.InsertString(i, str);
		}
	}
	catch (HalconCpp::HException & except)
	{
		ShowException(except);
		return false;
	}
	

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}
