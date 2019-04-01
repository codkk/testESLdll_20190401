#include "..\stdafx.h"
#include "LightCtrl.h"

//CCriticalSection g_mycs;
CLightCtrl::CLightCtrl()
{
	m_light[0] = 0;
	m_light[1] = 0;
	m_comm = 2;   //comm号，0代表com1
	m_lbaud = 115200; //波特率

	memset(m_cmd1, 0, 1024);
	memset(m_cmd2, 0, 1024);
	sprintf_s(m_cmd1, "OPEN");
	sprintf_s(m_cmd2, "CLOSE");
	m_strCmd1 = "OPEN";
	m_strCmd2 = "CLOSE";
}


CLightCtrl::~CLightCtrl()
{
	if (m_sp.IsOpened())
		m_sp.ClosePort();
}

void CLightCtrl::SetWnd(HWND powner)
{
	m_pOwner = powner;
}

bool CLightCtrl::ConnectLight(int comm)
{
	m_sp.InitPort(m_pOwner, comm+1, m_lbaud);
	if (!m_sp.IsOpened())
	{
		//AfxMessageBox(_T("open filed"));
		return false;
	}
	else
	{
		//AfxMessageBox(_T("open comm%s successed"), 1);
	}
	m_sp.StartMonitoring();
	m_sp.SuspendMonitoring();
	
	return true;
}

bool CLightCtrl::SetLight(int idx, int nLight)
{
	if (!m_sp.IsOpened())
		return false;
	//g_mycs.Lock();
	m_light[idx] = nLight;

	char ch[7];
	memset(ch, '\0', 7);
	ch[0] = 0xab; //起始位 高字节
	ch[1] = 0xba; //起始位 低字节
	ch[2] = 0x03; //数据长度
	ch[3] = 0x31; //命令
	ch[4] = 0x00;
	ch[5] = 0xff;

	//
	ch[4] += (char)idx;  //通道0/1/2/3
	ch[5] = (char)nLight; //亮度：0~255

	//char* str = "\xab\xba\x03\x31\x00\xff";
	bool ret = send(ch, 6);
	//g_mycs.Unlock();
	return ret;

}

int CLightCtrl::GetLight(int idx)
{
	return m_light[idx];
}

bool CLightCtrl::SetTrigle(int idx,int mode)
{
	if (!m_sp.IsOpened())
		return false;
	//g_mycs.Lock();
	//m_light[idx] = nLight;

	char ch[7];
	memset(ch, '\0', 7);
	ch[0] = 0xab; //起始位 高字节
	ch[1] = 0xba; //起始位 低字节
	ch[2] = 0x03; //数据长度
	ch[3] = 0x32; //命令
	ch[4] = 0x00;
	ch[5] = 0x00;

	//
	ch[4] += (char)idx;  //通道0/1/2/3
	ch[5] = (char)mode; //0,外模式，1，内模式

						  //char* str = "\xab\xba\x03\x31\x00\xff";
	bool ret = send(ch, 6);
	//g_mycs.Unlock();
	return ret;
}

//为苏州需求
bool CLightCtrl::SendCmd1()
{
	if (!m_sp.IsOpened())
		return false;
	//添加换行符
	CString str = m_strCmd1 + "\r\n";
	char* cmd1 = (char*)str.GetBuffer(0);
	//send(cmd1, 6);
	
	m_sp.Send(cmd1);
	return true;
}

bool CLightCtrl::SendCmd2()
{
	if (!m_sp.IsOpened())
		return false;
	CString str = m_strCmd2 + "\r\n";
	char* cmd = (char*)str.GetBuffer(0);
	//send(cmd, 7);
	m_sp.Send(cmd);
	return true;
}

bool CLightCtrl::Loadini()
{
	CIni iniFile;
	CString strIniFileName;
	//strIniFileName.Format(INIFILE_PATH);
	bool ret = iniFile.Read(INIFILE_PATH);
	iniFile.GetValue(NODE_SR, SUBNODE_COMM, m_comm);
	iniFile.GetValue(NODE_SR, SUBNODE_BAUD, m_lbaud);
	iniFile.GetValue(NODE_SR, SUBNODE_CMD1, m_strCmd1);
	iniFile.GetValue(NODE_SR, SUBNODE_CMD2, m_strCmd2);
	return ret;
}

bool CLightCtrl::send(char * pCh,int len)
{
	m_sp.Send(pCh, len);
	return true;
}
