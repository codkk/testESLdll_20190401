// CamSelDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "testESLdll.h"
#include "CamSelDlg.h"
#include "afxdialogex.h"
#include "Ini.h"

#define PATH_CAM_CONFIG "Config\\CamConfig.ini"
#define NODE_CAM_CONFIG "CamConfig" //相机设置
//异常处理函数
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
// CamSelDlg 对话框

IMPLEMENT_DYNAMIC(CamSelDlg, CDialog)

CamSelDlg::CamSelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG_SELECTCAM, pParent)
{
	m_strCamName = "default";
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
	ON_BN_CLICKED(IDC_BUTTON_SAVECAM, &CamSelDlg::OnBnClickedButtonSavecam)
END_MESSAGE_MAP()


// CamSelDlg 消息处理程序


BOOL CamSelDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//查看当前的相机
	try
	{
		HalconCpp::HTuple Information, ValueList;
		HalconCpp::InfoFramegrabber("GenICamTL", "device", &Information, &ValueList);
		int CamNum = ValueList.Length();
		for (int i = 0; i < CamNum; i++)
		{
			CString str;
			CString strShow;
			str = ValueList[i].S();
			// | device:USB2_5M(2)@UE500901474 | interface:DVPCAMERA::USB | producer:C:\Program Files (x86)\Do3think\DVP2\DVPCameraTL.cti
			int idx1 = str.Find("|", 0);
			if (-1 != idx1)
			{
				int idx2 = str.Find("|", idx1 + 1);
				if (-1 == idx1)
					idx2 = str.GetLength() - 1;
				int cont = idx2 - idx1;
				strShow = str.Mid(idx1+1, cont-1);
				strShow.Trim();
				int idx3 = strShow.Find("device:");
				if (-1 != idx3)
				{
					strShow = strShow.Right(strShow.GetLength()-7);
				}
			}
			
			m_comboCameralist.InsertString(i, strShow);
		}
		m_comboCameralist.SetCurSel(0);
	}
	catch (HalconCpp::HException & except)
	{
		ShowException(except);
		return false;
	}
	

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

//选择的相机型号写入文件
void CamSelDlg::OnBnClickedButtonSavecam()
{
	if (IDOK != AfxMessageBox("是否保存当前选择？", MB_OKCANCEL))
	{
		return;
	}
	int nsel = m_comboCameralist.GetCurSel();
	if (nsel == -1)
	{
		AfxMessageBox("请选择相机名");
		return;
	}
	m_comboCameralist.GetLBText(nsel, m_strCamName);
	if (saveCamConfig(PATH_CAM_CONFIG))
	{
		AfxMessageBox("保存成功");
	}
	else
	{
		AfxMessageBox("保存失败");
	}
}


bool  CamSelDlg::saveCamConfig(char* pPath)
{
	CIni iniFile;
	if (!iniFile.SetValue(NODE_CAM_CONFIG, "Name", m_strCamName)) //最大值
	{
		return false;
	}
	if (!iniFile.Write(pPath))
	{
		return false;
	}
	return true;
}