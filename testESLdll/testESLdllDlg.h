
// testESLdllDlg.h : 头文件
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

#define WM_MYMSG WM_USER+88   //实时显示

//extern class ParaForLcd;
typedef bool(*initDllFunc)(CWnd* pWnd);  //初始化
typedef bool(*setResWndFunc)(int screenType, CWnd* pWnd);  //初始化
typedef bool(*UnInitDllFunc)();  //反初始化
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

typedef bool (*loadOneImage)();//从文件加载图像
typedef bool(*grabOneImage)(); //从相机获取图像
typedef int(*runFunc)(); //运行
typedef bool(*findScreenFunc)(); //切分屏幕区域
typedef bool(*checkLightScreenFunc)(); //白屏检测
typedef bool(*checkRedScreenFunc)(); //红屏检测
typedef bool(*checkGreenScreenFunc)(); //绿屏检测
typedef bool(*checkBlueScreenFunc)(); //蓝屏检测
typedef bool(*checkBlackScreenFunc)(); //黑屏检测亮点
// CtestESLdllDlg 对话框
class CtestESLdllDlg : public CDialogEx
{
// 构造
public:
	CtestESLdllDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CtestESLdllDlg();
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TESTESLDLL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

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

	//流程控制
	HANDLE	m_hTestEvent;						//开始测试事件
	bool m_bExit;				//退出程序
	CString m_strMsg;
	//服务器
	ServerNet m_serverNet;
// 实现
protected:
	HICON m_hIcon;
	HMODULE dllHandle;   //图像处理库的句柄
	// 生成的消息映射函数
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
	int ServerRun();  //运行服务器
	std::string RunFromMsg(std::string Msg); //根据消息运行不同的功能
	afx_msg LRESULT ShowResult(WPARAM, LPARAM);//自定义消息处理，显示结果
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
