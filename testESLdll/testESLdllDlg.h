
// testESLdllDlg.h : ͷ�ļ�
//

#pragma once
# include "HalconCpp.h" //halcon
# include "HDevThread.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "../ESLdll/ESLdll/ResultForLcd.h"
#include "../ESLdll/ESLdll/ParaForLcd.h"
#include "ServerNet.h"
using namespace HalconCpp;

#define WM_MYMSG WM_USER+88   //ʵʱ��ʾ

//extern class ParaForLcd;
typedef bool(*initDllFunc)(CWnd* pWnd);  //��ʼ��
typedef bool(*setResWndFunc)(int screenType, CWnd* pWnd);  //��ʼ��
typedef bool(*UnInitDllFunc)();  //����ʼ��
typedef bool(*whiteScreenFunc)(char* pPath, bool bflag);
typedef bool(*blackScreenFunc)(char* pPath, bool bflag);
typedef bool(*grayScaleFunc)(char* pPath, bool bflag);
typedef bool(*screenFunc)(char* pPath, bool bflag);
typedef bool(*chessBoardFunc)(char* pPath, bool bflag);

typedef bool(*whiteScreenFuncCam)(bool bflag);
typedef bool(*blackScreenFuncCam)(bool bflag);
typedef bool(*grayScaleFuncCam)(bool bflag);
typedef bool(*screenFuncCam)(bool bflag);
typedef bool(*chessBoardFuncCam)(bool bflag);
typedef bool(*reloadPara)();

typedef bool (*loadOneImage)();//���ļ�����ͼ��
typedef bool(*grabOneImage)(); //�������ȡͼ��
typedef int(*runFunc)(); //����
typedef bool(*findScreenFunc)(); //�з���Ļ����
typedef bool(*checkLightScreenFunc)(); //�������
typedef bool(*checkRedScreenFunc)(); //�������
typedef bool(*checkGreenScreenFunc)(); //�������
typedef bool(*checkBlueScreenFunc)(); //�������
typedef bool(*checkBlackScreenFunc)(); //�����������
// CtestESLdllDlg �Ի���
class CtestESLdllDlg : public CDialogEx
{
// ����
public:
	CtestESLdllDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CtestESLdllDlg();
// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TESTESLDLL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

	initDllFunc pInitDll;
	UnInitDllFunc pUnInitDll;
	setResWndFunc pSetResWnd;
	whiteScreenFunc pFunc;
	blackScreenFunc pFunc2;
	grayScaleFunc pGrayscale;
	screenFunc pScreenregion;
	chessBoardFunc pChessboard;

	whiteScreenFuncCam pFuncCam;
	blackScreenFuncCam pFunc2Cam;
	grayScaleFuncCam pGrayscaleCam;
	screenFuncCam pScreenregionCam;
	chessBoardFuncCam pChessboardCam;
	reloadPara pReloadPara;
	loadOneImage pFuncLoadOneImage;
	grabOneImage pFuncGrabOneImage;
	runFunc pFuncRun;
	findScreenFunc pFuncFindScreen;
	checkLightScreenFunc pFuncLightScreen;
	checkRedScreenFunc pFuncRedScreen;
	checkGreenScreenFunc pFuncGreenScreen;
	checkBlueScreenFunc pFuncBlueScreen;
	checkBlackScreenFunc pFuncBlackScreen;

	//���̿���
	HANDLE	m_hTestEvent;						//��ʼ�����¼�
	bool m_bExit;				//�˳�����
	CString m_strMsg;
	//������
	ServerNet m_serverNet;
// ʵ��
protected:
	HICON m_hIcon;
	HMODULE dllHandle;   //ͼ�����ľ��
	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	BOOL m_bFromFile;
	BOOL m_bInitSuccess;
	CListBox m_listResult;
	CListCtrl m_listFaultImg;


	void ReadAndShowResult(char* pPath);
	int ServerRun();  //���з�����
	std::string RunFromMsg(std::string Msg); //������Ϣ���в�ͬ�Ĺ���
	afx_msg LRESULT ShowResult(WPARAM, LPARAM);//�Զ�����Ϣ������ʾ���
	afx_msg void OnBnClickedButtonSaveimg();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButtonOpenimg();
	

	afx_msg void OnBnClickedButtonRun();
	afx_msg void OnBnClickedButtonBlue();
	afx_msg void OnBnClickedButtonReload();
	afx_msg void OnBnClickedButtonOpenimg2();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButtonSetting();
	afx_msg void OnBnClickedButtonAutorun();
	afx_msg void OnBnClickedButtonStopautorun();
	afx_msg void OnClose();
};
