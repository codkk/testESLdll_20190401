
// testESLdllDlg.cpp : ʵ���ļ�
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


// CtestESLdllDlg �Ի���
#define WHITESCREEN_RESULT_FILE "Result\\WhiteResult.txt"  //����ļ��ļ���
#define BLACKSCREEN_RESULT_FILE "Result\\BlackResult.txt"
#define GRAYSCALE_RESULT_FILE "Result\\GrayResult.txt"
#define SCREENREGION_RESULT_FILE "Result\\ScreenRegionResult.txt"
#define CHESSBOARD_RESULT_FILE "Result\\ChessResult.txt"//���̸������ļ�

#define NODE_WHITESCREEN_RES "WhiteScreenResult" //�ڵ�
#define NODE_BLACKSCREEN_RES "BlackScreenResult"
#define NODE_GRAYSCALE_RES "GrayScaleResult"
#define NODE_SCREENREGION_RES "ScreenRegionResult"
#define NODE_CHESSBOARD_RES "ChessBoardResult"

#define SUBNODE_COUNT "Count" //�ӽڵ�
#define SUBNODE_AREA "Area"
#define SUBNODE_MSG "Message"
#define SUBNODE_FAILPATH "FailImagePath"
#define SUBNODE_RES "Result"

#define RESULT_PASS "PASS"
#define RESULT_FAIL "FAIL"

#define RESULT_MSG_PASS "PASS"
#define RESULT_MSG_FAIL_GRAY "GRAY FAIL" //�Ҷȳ�������
#define RESULT_MSG_FAIL_DEFECT "DEFECT FAIL" //��⵽ȱ��


//�����߳�
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


// CtestESLdllDlg ��Ϣ�������

BOOL CtestESLdllDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	ShowWindow(SW_MAXIMIZE);

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��
	HObject ho_Image;
	//ReadImage(&ho_Image, "SrcImg\\white.jpg");

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	dllHandle = LoadLibrary("ESLdll.dll");
	if (dllHandle == NULL)
	{
		AfxMessageBox("LOAD FAIL");
		return TRUE;
	}

	//����������򴴽���ǰ���ڵ��ļ��б���ͼƬ

	//����������򴴽���ǰ���ڵ��ļ��б�����־

	//ÿ�����һ�Σ����ڵ�ǰ�����ļ����´���һ����־

	//����dll��ʼ������
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

	//dll��ʼ������������� ��ʾ��
	CWnd * pWnd = GetDlgItem(IDC_STATIC_SHOW);
	if (!pInitDll(pWnd))
	{
		m_bInitSuccess = FALSE;
		AfxMessageBox("�����ʧ��");
	}
	//���ý����ʾ����
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
	//����һ��������
	int iRlt = m_serverNet.ServerInit("127.0.0.1", 3000);
	if (iRlt == 0)
	{
		m_strMsg.Format("��ʼ���ɹ���");
		SendMessage(WM_MYMSG, 0, (LPARAM)&m_strMsg);
		//AfxMessageBox("������ init successful.\n");
		//m_serverNet.ServerRun();
	}
	else
	{
		CString str;
		str.Format("��������ʼ��ʧ�� error: %d\n", iRlt);
		AfxMessageBox(str);
		return false;
	}

	//����һ�����߳�
	//���������߳�
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AutoTestHandleThread, this, 0, NULL);

	bool res = true;
	return res;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CtestESLdllDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
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

//����
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

//����
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

//�ҽ�
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
	//���ļ��ж�ȡ
	iniFile.Read(pPath);
	//ʶ����
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
	m_listResult.AddString(strShowResult);//��ʾ���б���
	//�����
	int count = 0;
	iniFile.GetValue(strNode, SUBNODE_COUNT, count);
	CString strShowCount;
	strShowCount.Format("Count: %d", count);
	m_listResult.AddString(strShowCount);//��ʾ���б���
	//�Ҷ�ֵ
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
		m_listResult.AddString(strGray);	//��ʾ���б���
	}

}


void CtestESLdllDlg::OnBnClickedButtonSaveimg()
{
	
}

//����
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	AfxMessageBox("�������");
}

//��ȡ���ͼ��
void CtestESLdllDlg::OnBnClickedButtonOpenimg2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	pFuncGrabOneImage();
}


void CtestESLdllDlg::OnBnClickedButton8()
{
	CamSelDlg dlg;
	dlg.DoModal();
}

//��������
void CtestESLdllDlg::OnBnClickedButtonSetting()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}

//�Զ�����
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
	// ��������
	listen(m_serverNet.m_sock, 5);

	SOCKADDR_IN tcpAddr;
	int len = sizeof(sockaddr);
	SOCKET newSocket;
	char buf[1024];
	int rval;
	bool bClear = false;
	do
	{
		//�ȴ���ʼ�ź�
		WaitForSingleObject(m_hTestEvent, INFINITE);
		if (m_bExit) //�˳�����
		{
			break;
		}
		// ������Ϣ
		m_strMsg = "�ȴ�������һ������...";
		if(bClear)
			SendMessage(WM_MYMSG, 1, (LPARAM)&m_strMsg);
		else
			SendMessage(WM_MYMSG, 0, (LPARAM)&m_strMsg);
		newSocket = accept(m_serverNet.m_sock, (sockaddr*)&tcpAddr, &len);

		if (newSocket == INVALID_SOCKET)
		{
			// �ǿ���socket
			printf("invalid socket occured.\n");
		}
		else
		{
			// ���õ���socket����
			printf("new socket connect: %d\n", newSocket);

			// ��Ϣ����
			do
			{
				//printf("process\n");
				// ��������
				memset(buf, 0, sizeof(buf));
				rval = recv(newSocket, buf, 1024, 0);

				if (rval == SOCKET_ERROR)
					// ���쳣ͨ��������δcloseSocket���˳�ʱ
				{
					printf("recv socket error.\n");
					break;
				}

				else if (rval == 0)
					// 0��ʾ�����˳�
					printf("socket %d connect end.\n", newSocket);
				else
				{
					// ��ʾ���յ�������
					printf("recv msg: %s\n", buf);
					m_strMsg.Format("���յ�:%s", buf);
					SendMessage(WM_MYMSG, 0, (LPARAM)&m_strMsg);
					//���ݲ�ͬ���ݣ����ò�ͬ�Ľӿ�,����������
					std::string Msg = buf;
					std::string strRet = RunFromMsg(Msg);
					if ("" == strRet) //δ����
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
						//���ͷ���ֵ
						m_serverNet.ServerSend(newSocket, strRet.c_str(), strRet.length());
					}
				}

			} while (rval != 0);

			// ���ڽ��յ�socket
			closesocket(newSocket);
		}
	} while (1);

	// �ر�����socket
	closesocket(m_serverNet.m_sock);
	return 0;
}

//�㷨OK����0�� NG����1�� ��������-1
std::string CtestESLdllDlg::RunFromMsg(std::string Msg) //������Ϣ���в�ͬ�Ĺ���
{
	Sleep(11000);
	return "OK"; //����
	bool ret = false;
	if ("WHITE" == Msg)
	{
		m_strMsg.Format("��ʼ��������...");
		SendMessage(WM_MYMSG, 0, (LPARAM)&m_strMsg);
		if (pFuncLightScreen)
			ret = pFuncLightScreen();
		else
			ret = false;

		//�������ڴ����ļ���
		
	}
	else if ("RED" == Msg)
	{
		m_strMsg.Format("��ʼ��������...");
		SendMessage(WM_MYMSG, 0, (LPARAM)&m_strMsg);
		ret = pFuncRedScreen();
	}
	else if ("GREEN" == Msg)
	{
		m_strMsg.Format("��ʼ��������...");
		SendMessage(WM_MYMSG, 0, (LPARAM)&m_strMsg);
		ret = pFuncGreenScreen();
	}
	else if ("BLUE" == Msg)
	{
		m_strMsg.Format("��ʼ��������...");
		SendMessage(WM_MYMSG, 0, (LPARAM)&m_strMsg);
		ret = pFuncBlueScreen();
	}
	else if ("BLACK" == Msg)
	{
		m_strMsg.Format("��ʼ��������...");
		SendMessage(WM_MYMSG, 0, (LPARAM)&m_strMsg);
		ret = pFuncBlackScreen();
	}
	else if ("END" == Msg)
	{
		m_strMsg.Format("��������...");
		SendMessage(WM_MYMSG, 0, (LPARAM)&m_strMsg);
		return "TESTOVER";
	}
	else
	{
		m_strMsg.Format("��Ч����...");
		SendMessage(WM_MYMSG, 0, (LPARAM)&m_strMsg);
		return "";
	}

	if (ret)
	{
		//����ͼƬ������Ŀ¼��,����������򴴽�

		return "OK";
	}
	else
	{
		//����ͼƬ������Ŀ¼��NG�ļ��У�����������򴴽�

		return "NG";
	}

}


void CtestESLdllDlg::OnBnClickedButtonStopautorun()
{
	ResetEvent(m_hTestEvent);
}


void CtestESLdllDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (WAIT_OBJECT_0 == WaitForSingleObject(m_hTestEvent, 10)) //���ֹͣ
	{
		AfxMessageBox(_T("����ʹ����ֹͣ���ڹرճ���"));
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
