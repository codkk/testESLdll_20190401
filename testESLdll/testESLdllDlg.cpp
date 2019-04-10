
// testESLdllDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "testESLdll.h"
#include "testESLdllDlg.h"
#include "afxdialogex.h"
#include "Ini.h"
#include "CamSelDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CtestESLdllDlg 对话框
#define WHITESCREEN_RESULT_FILE "Result\\WhiteResult.txt"  //结果文件文件名
#define BLACKSCREEN_RESULT_FILE "Result\\BlackResult.txt"
#define GRAYSCALE_RESULT_FILE "Result\\GrayResult.txt"
#define SCREENREGION_RESULT_FILE "Result\\ScreenRegionResult.txt"
#define CHESSBOARD_RESULT_FILE "Result\\ChessResult.txt"//棋盘格检测结果文件

#define NODE_WHITESCREEN_RES "WhiteScreenResult" //节点
#define NODE_BLACKSCREEN_RES "BlackScreenResult"
#define NODE_GRAYSCALE_RES "GrayScaleResult"
#define NODE_SCREENREGION_RES "ScreenRegionResult"
#define NODE_CHESSBOARD_RES "ChessBoardResult"

#define SUBNODE_COUNT "Count" //子节点
#define SUBNODE_AREA "Area"
#define SUBNODE_MSG "Message"
#define SUBNODE_FAILPATH "FailImagePath"
#define SUBNODE_RES "Result"

#define RESULT_PASS "PASS"
#define RESULT_FAIL "FAIL"

#define RESULT_MSG_PASS "PASS"
#define RESULT_MSG_FAIL_GRAY "GRAY FAIL" //灰度超出公差
#define RESULT_MSG_FAIL_DEFECT "DEFECT FAIL" //检测到缺陷


//工作线程
DWORD WINAPI AutoTestHandleThread(LPVOID lpParam)
{
	CtestESLdllDlg* pMdlg = (CtestESLdllDlg*)lpParam;
	int ret = pMdlg->ServerRun();
	return ret;
}

CtestESLdllDlg::CtestESLdllDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TESTESLDLL_DIALOG, pParent)
	, m_bFromFile(FALSE)
{
	m_strPathImg = TXT_PATH_IMG;
	m_bExit = false;
	m_bInitSuccess = TRUE;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	pInitDll = NULL;
	pUnInitDll = NULL;

	pFunc = NULL;
	pFunc2 = NULL;
	pGrayscale = NULL;
	pScreenregion = NULL;
	pChessboard = NULL;

	pFuncCam = NULL;
	pFunc2Cam = NULL;
	pGrayscaleCam = NULL;
	pScreenregionCam = NULL;
	pChessboardCam = NULL;

	pFuncGrabOneImage = NULL;
	pFuncLoadOneImage = NULL;

	m_hTestEvent = CreateEventA(NULL, TRUE, FALSE, NULL);
}

CtestESLdllDlg::~CtestESLdllDlg()
{
	if (pUnInitDll)
		pUnInitDll();

	FreeLibrary(dllHandle);
}

void CtestESLdllDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_FROM_IMG, m_bFromFile);
	DDX_Control(pDX, IDC_LIST_SHOW_RESULT, m_listResult);
	
}

BEGIN_MESSAGE_MAP(CtestESLdllDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_MYMSG, &CtestESLdllDlg::ShowResult)
	ON_BN_CLICKED(IDC_BUTTON1, &CtestESLdllDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CtestESLdllDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CtestESLdllDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CtestESLdllDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CtestESLdllDlg::OnBnClickedButton5)
	//ON_BN_CLICKED(IDC_BUTTON_SAVEIMG, &CtestESLdllDlg::OnBnClickedButtonSaveimg)
	ON_BN_CLICKED(IDC_BUTTON6, &CtestESLdllDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CtestESLdllDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON_OPENIMG, &CtestESLdllDlg::OnBnClickedButtonOpenimg)
	ON_BN_CLICKED(IDC_BUTTON_RUN, &CtestESLdllDlg::OnBnClickedButtonRun)
	ON_BN_CLICKED(IDC_BUTTON_BLUE, &CtestESLdllDlg::OnBnClickedButtonBlue)
	ON_BN_CLICKED(IDC_BUTTON_RELOAD, &CtestESLdllDlg::OnBnClickedButtonReload)
	ON_BN_CLICKED(IDC_BUTTON_OPENIMG2, &CtestESLdllDlg::OnBnClickedButtonOpenimg2)
	ON_BN_CLICKED(IDC_BUTTON8, &CtestESLdllDlg::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON_SETTING, &CtestESLdllDlg::OnBnClickedButtonSetting)
	ON_BN_CLICKED(IDC_BUTTON_AUTORUN, &CtestESLdllDlg::OnBnClickedButtonAutorun)
	ON_BN_CLICKED(IDC_BUTTON_STOPAUTORUN, &CtestESLdllDlg::OnBnClickedButtonStopautorun)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CtestESLdllDlg 消息处理程序

BOOL CtestESLdllDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	ShowWindow(SW_MAXIMIZE);

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	HObject ho_Image;
	//ReadImage(&ho_Image, "SrcImg\\white.jpg");

	// TODO: 在此添加额外的初始化代码
	dllHandle = LoadLibrary("ESLdll.dll");
	if (dllHandle == NULL)
	{
		AfxMessageBox("LOAD FAIL");
		return TRUE;
	}

	//如果不存在则创建当前日期的文件夹保存图片

	//如果不存在则创建当前日期的文件夹保存日志

	//每打开软件一次，则在当前日期文件夹下创建一个日志

	//加载dll初始化函数
	pInitDll = (initDllFunc)::GetProcAddress(dllHandle, "EslInitDll");
	if (pInitDll == NULL) {
		AfxMessageBox("function  load failed!\n");
		return false;
	}
	pSetResWnd = (setResWndFunc)::GetProcAddress(dllHandle, "EslSetResWnd");
	if (pSetResWnd == NULL) {
		AfxMessageBox("function  load failed!\n");
		return false;
	}

	pUnInitDll = (UnInitDllFunc)::GetProcAddress(dllHandle, "EslUnInitDll");
	if (pUnInitDll == NULL) {
		AfxMessageBox("function  load failed!\n");
		return false;
	}

	pFunc = (whiteScreenFunc)::GetProcAddress(dllHandle, "whiteScreenFromPath");
	if (pFunc == NULL) {
		AfxMessageBox("function  load failed!\n");
		return false;
	}
	pFuncCam = (whiteScreenFuncCam)::GetProcAddress(dllHandle, "whiteScreenFromCam");
	if (pFuncCam == NULL) {
		AfxMessageBox("function  load failed!\n");
		return false;
	}

	pFunc2 = (blackScreenFunc)::GetProcAddress(dllHandle, "blackScreenFromPath");
	if (pFunc2 == NULL) {
		AfxMessageBox("function  load failed!\n");
		return false;
	}
	pFunc2Cam = (blackScreenFuncCam)::GetProcAddress(dllHandle, "blackScreenFromCam");
	if (pFunc2Cam == NULL) {
		AfxMessageBox("function  load failed!\n");
		return false;
	}

	pGrayscale = (grayScaleFunc)::GetProcAddress(dllHandle, "grayScaleFromPath");
	if (pGrayscale == NULL) {
		AfxMessageBox("function  load failed!\n");
		return false;
	}
	pGrayscaleCam = (grayScaleFuncCam)::GetProcAddress(dllHandle, "grayScaleFromCam");
	if (pGrayscaleCam == NULL) {
		AfxMessageBox("function  load failed!\n");
		return false;
	}
	
	pScreenregion = (grayScaleFunc)::GetProcAddress(dllHandle, "screenRegionFromPath");
	if (pScreenregion == NULL) {
		AfxMessageBox("function  load failed!\n");
		return false;
	}
	pScreenregionCam = (grayScaleFuncCam)::GetProcAddress(dllHandle, "screenRegionFromCam");
	if (pScreenregionCam == NULL) {
		AfxMessageBox("function  load failed!\n");
		return false;
	}

	pChessboard = (grayScaleFunc)::GetProcAddress(dllHandle, "chessBoardFromPath");
	if (pChessboard == NULL) {
		AfxMessageBox("function  load failed!\n");
		return false;
	}
	pChessboardCam = (grayScaleFuncCam)::GetProcAddress(dllHandle, "chessBoardFromCam");
	if (pChessboardCam == NULL) {
		AfxMessageBox("function  load failed!\n");
		return false;
	}

	pFuncLoadOneImage = (loadOneImage)::GetProcAddress(dllHandle, "EslLoadOneImage");
	if (pFuncLoadOneImage == NULL) {
		AfxMessageBox("function  load failed!\n");
		return false;
	}
	pFuncGrabOneImage = (grabOneImage)::GetProcAddress(dllHandle, "EslGrabOneImage");
	if (pFuncGrabOneImage == NULL) {
		AfxMessageBox("function  load failed!\n");
		return false;
	}

	pFuncRun = (runFunc)::GetProcAddress(dllHandle, "Run");
	if (pFuncRun == NULL) {
		AfxMessageBox("function  load failed!\n");
		return false;
	}
	pFuncFindScreen = (findScreenFunc)::GetProcAddress(dllHandle, "EslFindScreen");
	if (pFuncFindScreen == NULL) {
		AfxMessageBox("function  load failed!\n");
		return false;
	}

	pFuncLightScreen = (checkLightScreenFunc)::GetProcAddress(dllHandle, "EslCheckLightScreen");
	if (pFuncLightScreen == NULL) {
		AfxMessageBox("function  load failed!\n");
		return false;
	}

	pFuncRedScreen = (checkRedScreenFunc)::GetProcAddress(dllHandle, "EslCheckRedScreen");
	if (pFuncRedScreen == NULL) {
		AfxMessageBox("function  load failed!\n");
		return false;
	}

	pFuncGreenScreen = (checkGreenScreenFunc)::GetProcAddress(dllHandle, "EslCheckGreenScreen");
	if (pFuncGreenScreen == NULL) {
		AfxMessageBox("function  load failed!\n");
		return false;
	}

	pFuncBlueScreen = (checkBlueScreenFunc)::GetProcAddress(dllHandle, "EslCheckBlueScreen");
	if (pFuncBlueScreen == NULL) {
		AfxMessageBox("function  load failed!\n");
		return false;
	}
	pFuncBlackScreen = (checkBlackScreenFunc)::GetProcAddress(dllHandle, "EslCheckBlackScreen");
	if (pFuncBlackScreen == NULL) {
		AfxMessageBox("function  load failed!\n");
		return false;
	}
	pReloadPara = (reloadPara)::GetProcAddress(dllHandle, "EslReloadPara");
	if (pReloadPara == NULL) {
		AfxMessageBox("function  load failed!\n");
		return false;
	}

	//dll初始化（连接相机， 显示）
	CWnd * pWnd = GetDlgItem(IDC_STATIC_SHOW);
	if (!pInitDll(pWnd))
	{
		m_bInitSuccess = FALSE;
		AfxMessageBox("打开相机失败");
	}
	//设置结果显示窗口
	pWnd = GetDlgItem(IDC_STATIC_SCREEN_WHITE);
	if (!pSetResWnd(0, pWnd))
	{
	}
	pWnd = GetDlgItem(IDC_STATIC_SCREEN_RED);
	if (!pSetResWnd(1, pWnd))
	{
	}
	pWnd = GetDlgItem(IDC_STATIC_SCREEN_GREEN);
	if (!pSetResWnd(2, pWnd))
	{
	}
	pWnd = GetDlgItem(IDC_STATIC_SCREEN_BLUE);
	if (!pSetResWnd(3, pWnd))
	{
	}
	pWnd = GetDlgItem(IDC_STATIC_SCREEN_BLACK);
	if (!pSetResWnd(4, pWnd))
	{
	}
	//m_listFaultImg.InsertItem(0,);
	//m_ImageList.Add()
	//创建一个服务器
	int iRlt = m_serverNet.ServerInit("127.0.0.1", 3000);
	if (iRlt == 0)
	{
		m_strMsg.Format("初始化成功。");
		SendMessage(WM_MYMSG, 0, (LPARAM)&m_strMsg);
		//AfxMessageBox("服务器 init successful.\n");
		//m_serverNet.ServerRun();
	}
	else
	{
		CString str;
		str.Format("服务器初始化失败 error: %d\n", iRlt);
		AfxMessageBox(str);
		return false;
	}

	//开启一个主线程
	//开启工作线程
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AutoTestHandleThread, this, 0, NULL);

	bool res = true;
	return res;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CtestESLdllDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CtestESLdllDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CtestESLdllDlg::OnBnClickedButton1()
{
	//UpdateData();
	////HObject ho_Image;
	//////ReadImage(&ho_Image, "whiteFAKE_NG2.jpg");
	//bool res = false;
	//if (m_bFromFile || !m_bInitSuccess)
	//	res = pScreenregion("SrcImg\\ScreenRegion.bmp"/*"SrcImg\\ScreenRegion.jpg"*/, true);
	//else
	//	res = pScreenregionCam(true);

	//ReadAndShowResult(SCREENREGION_RESULT_FILE);
	if (pFuncFindScreen())
	{
	}
}

//白屏
void CtestESLdllDlg::OnBnClickedButton2()
{
	//UpdateData();
	//bool res;
	//if (m_bFromFile || !m_bInitSuccess)
	//	res = pFunc("SrcImg\\whiteFAKE_NG3.bmp", true);
	//else
	//	res = pFuncCam(true);
	//ReadAndShowResult(WHITESCREEN_RESULT_FILE);
	pFuncLightScreen();
}

//黑屏
void CtestESLdllDlg::OnBnClickedButton3()
{
	//UpdateData();
	//bool res;
	//if (m_bFromFile || !m_bInitSuccess)
	//	res = pFunc2("SrcImg\\black.jpg", true);
	//else
	//	res = pFunc2Cam(true);
	//ReadAndShowResult(BLACKSCREEN_RESULT_FILE);
	pFuncBlackScreen();
}

//灰阶
void CtestESLdllDlg::OnBnClickedButton4()
{
	UpdateData();
	bool res;
	if (m_bFromFile || !m_bInitSuccess)
		res = pGrayscale("SrcImg\\gray.jpg", true);
	else
		res = pGrayscaleCam(true); 
	ReadAndShowResult(GRAYSCALE_RESULT_FILE);
}


void CtestESLdllDlg::OnBnClickedButton5()
{
	UpdateData();
	bool res;
	if (m_bFromFile || !m_bInitSuccess)
		res = pChessboard("SrcImg\\Chessboard.jpg", true);
	else
		res = pChessboardCam(true);
	ReadAndShowResult(CHESSBOARD_RESULT_FILE);
}

void CtestESLdllDlg::ReadAndShowResult(char * pPath)
{
	m_listResult.ResetContent();
	CIni iniFile;
	//从文件中读取
	iniFile.Read(pPath);
	//识别结果
	CString strNode(pPath);
	CString str;
	if (-1 != strNode.Find("White"))
		strNode = NODE_WHITESCREEN_RES;
	else if (-1 != strNode.Find("Black"))
		strNode = NODE_BLACKSCREEN_RES;
	else if (-1 != strNode.Find("Chess"))
		strNode = NODE_CHESSBOARD_RES;
	else if (-1 != strNode.Find("Gray"))
		strNode = NODE_GRAYSCALE_RES;
	else if (-1 != strNode.Find("Screen"))
		strNode = NODE_SCREENREGION_RES;
	else
		return;

	iniFile.GetValue(strNode, SUBNODE_RES, str);
	CString strShowResult;
	strShowResult.Format("Result: %s", str);
	m_listResult.AddString(strShowResult);//显示到列表中
	//结果数
	int count = 0;
	iniFile.GetValue(strNode, SUBNODE_COUNT, count);
	CString strShowCount;
	strShowCount.Format("Count: %d", count);
	m_listResult.AddString(strShowCount);//显示到列表中
	//灰度值
	for (int irow = 3; irow >= 0; irow--)
	{
		CString strGray;
		for (int icol = 3; icol >= 0; icol--)
		{
			str.Format(_T("Gray[%d,%d]"), irow, icol);
			int igray = -1;
			
			iniFile.GetValue(strNode, str, igray);
			strGray.Format("%s  %3d", strGray, igray);
		}
		m_listResult.AddString(strGray);	//显示到列表中
	}

}


void CtestESLdllDlg::OnBnClickedButtonSaveimg()
{
	
}

//红屏
void CtestESLdllDlg::OnBnClickedButton6()
{
	//UpdateData();
	//bool res;
	//if (m_bFromFile || !m_bInitSuccess)
	//	res = pFunc("SrcImg\\redFAKE_NG3.bmp", true);
	//else
	//	res = pFuncCam(true);
	//ReadAndShowResult(WHITESCREEN_RESULT_FILE);
	pFuncRedScreen();
}


void CtestESLdllDlg::OnBnClickedButton7()
{
	/*UpdateData();
	bool res;
	if (m_bFromFile || !m_bInitSuccess)
		res = pFunc("SrcImg\\greenFAKE_NG3.bmp", true);
	else
		res = pFuncCam(true);
	ReadAndShowResult(WHITESCREEN_RESULT_FILE);*/
	pFuncGreenScreen();
}


void CtestESLdllDlg::OnBnClickedButtonOpenimg()
{
	if (pFuncLoadOneImage == NULL) return;
	pFuncLoadOneImage();
}


void CtestESLdllDlg::OnBnClickedButtonRun()
{
	// TODO: 在此添加控件通知处理程序代码
	if (pFuncRun)
	{
		pFuncRun();
	}
}


void CtestESLdllDlg::OnBnClickedButtonBlue()
{
	pFuncBlueScreen();
}


void CtestESLdllDlg::OnBnClickedButtonReload()
{
	pReloadPara();
	AfxMessageBox("加载完成");
}

//获取相机图像
void CtestESLdllDlg::OnBnClickedButtonOpenimg2()
{
	// TODO: 在此添加控件通知处理程序代码
	pFuncGrabOneImage();
}


void CtestESLdllDlg::OnBnClickedButton8()
{
	CamSelDlg dlg;
	dlg.DoModal();
}

//参数设置
void CtestESLdllDlg::OnBnClickedButtonSetting()
{
	// TODO: 在此添加控件通知处理程序代码
}

//自动运行
void CtestESLdllDlg::OnBnClickedButtonAutorun()
{
	SetEvent(m_hTestEvent);
	
	//ServerNet serverNet;
	//int iRlt = serverNet.ServerInit("127.0.0.1", 3000);
	//if (iRlt == 0)
	//{
	//	AfxMessageBox("servr init successful.\n");
	//	serverNet.ServerRun();
	//}
	//else
	//{
	//	CString str;
	//	str.Format("server init failed with error: %d\n", iRlt);
	//	AfxMessageBox(str);
	//}

}

int CtestESLdllDlg::ServerRun()
{
	// 公开连接
	listen(m_serverNet.m_sock, 5);

	SOCKADDR_IN tcpAddr;
	int len = sizeof(sockaddr);
	SOCKET newSocket;
	char buf[1024];
	int rval;
	bool bClear = false;
	do
	{
		//等待开始信号
		WaitForSingleObject(m_hTestEvent, INFINITE);
		if (m_bExit) //退出程序
		{
			break;
		}
		// 接收信息
		m_strMsg = "等待接收下一个命令...";
		if(bClear)
			SendMessage(WM_MYMSG, 1, (LPARAM)&m_strMsg);
		else
			SendMessage(WM_MYMSG, 0, (LPARAM)&m_strMsg);
		newSocket = accept(m_serverNet.m_sock, (sockaddr*)&tcpAddr, &len);

		if (newSocket == INVALID_SOCKET)
		{
			// 非可用socket
			printf("invalid socket occured.\n");
		}
		else
		{
			// 可用的新socket连接
			printf("new socket connect: %d\n", newSocket);

			// 消息处理
			do
			{
				//printf("process\n");
				// 接收数据
				memset(buf, 0, sizeof(buf));
				rval = recv(newSocket, buf, 1024, 0);

				if (rval == SOCKET_ERROR)
					// 该异常通常发生在未closeSocket就退出时
				{
					printf("recv socket error.\n");
					break;
				}

				else if (rval == 0)
					// 0表示正常退出
					printf("socket %d connect end.\n", newSocket);
				else
				{
					// 显示接收到的数据
					printf("recv msg: %s\n", buf);
					m_strMsg.Format("接收到:%s", buf);
					SendMessage(WM_MYMSG, 0, (LPARAM)&m_strMsg);
					//根据不同数据，调用不同的接口,并返回数据
					std::string Msg = buf;
					std::string strRet = RunFromMsg(Msg);
					if ("" == strRet) //未处理
					{
						//
						strRet = "Error para";
						m_serverNet.ServerSend(newSocket, strRet.c_str(), strRet.length());
					}
					else
					{
						if (strRet == "TESTOVER")
						{
							bClear = true;
						}
						else
						{
							bClear = false;
						}
						//发送返回值
						m_serverNet.ServerSend(newSocket, strRet.c_str(), strRet.length());
					}
				}

			} while (rval != 0);

			// 关于接收的socket
			closesocket(newSocket);
		}
	} while (1);

	// 关闭自身socket
	closesocket(m_serverNet.m_sock);
	return 0;
}

//算法OK返回0， NG返回1， 其他返回-1
std::string CtestESLdllDlg::RunFromMsg(std::string Msg) //根据消息运行不同的功能
{
	Sleep(11000);
	return "OK"; //调试
	bool ret = false;
	if ("WHITE" == Msg)
	{
		m_strMsg.Format("开始白屏测试...");
		SendMessage(WM_MYMSG, 0, (LPARAM)&m_strMsg);
		if (pFuncLightScreen)
			ret = pFuncLightScreen();
		else
			ret = false;

		//根据日期创建文件夹
		
	}
	else if ("RED" == Msg)
	{
		m_strMsg.Format("开始红屏测试...");
		SendMessage(WM_MYMSG, 0, (LPARAM)&m_strMsg);
		ret = pFuncRedScreen();
	}
	else if ("GREEN" == Msg)
	{
		m_strMsg.Format("开始绿屏测试...");
		SendMessage(WM_MYMSG, 0, (LPARAM)&m_strMsg);
		ret = pFuncGreenScreen();
	}
	else if ("BLUE" == Msg)
	{
		m_strMsg.Format("开始蓝屏测试...");
		SendMessage(WM_MYMSG, 0, (LPARAM)&m_strMsg);
		ret = pFuncBlueScreen();
	}
	else if ("BLACK" == Msg)
	{
		m_strMsg.Format("开始黑屏测试...");
		SendMessage(WM_MYMSG, 0, (LPARAM)&m_strMsg);
		ret = pFuncBlackScreen();
	}
	else if ("END" == Msg)
	{
		m_strMsg.Format("结束测试...");
		SendMessage(WM_MYMSG, 0, (LPARAM)&m_strMsg);
		return "TESTOVER";
	}
	else
	{
		m_strMsg.Format("无效命令...");
		SendMessage(WM_MYMSG, 0, (LPARAM)&m_strMsg);
		return "";
	}

	if (ret)
	{
		//保存图片到日期目录下,如果不存在则创建

		return "OK";
	}
	else
	{
		//保存图片到日期目录下NG文件夹，如果不存在则创建

		return "NG";
	}

}


void CtestESLdllDlg::OnBnClickedButtonStopautorun()
{
	ResetEvent(m_hTestEvent);
}


void CtestESLdllDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (WAIT_OBJECT_0 == WaitForSingleObject(m_hTestEvent, 10)) //如果停止
	{
		AfxMessageBox(_T("请先使程序停止，在关闭程序。"));
		return;
	}

	m_bExit = true;
	SetEvent(m_hTestEvent);
	Sleep(500);


	CDialogEx::OnClose();
}


LRESULT CtestESLdllDlg::ShowResult(WPARAM wParam, LPARAM lParam)
{
	CString* strData = (CString *)lParam;
	int clear = (int)wParam;
	m_listResult.AddString(*strData);
	m_listResult.AddString(" ");
	m_listResult.SetCurSel(m_listResult.GetCount() - 1);
	if (clear == 1)
	{
		m_listResult.ResetContent();
	}
	return 0;
}
