#pragma once
#pragma once

/*
ServerNet.h
����ˣ�����Socket����������Ķ˿ںź͵�ַ���󶨵�Socket��ʹ��Listen�򿪼�����
Ȼ�󲻶���Acceptȥ�鿴�Ƿ������ӣ�����У�����Socket����ͨ��Recv��ȡ��Ϣ�����ݣ�
ͨ����ɺ����CloseSocket�ر������ӦAccept����Socket�����������Ҫ�ȴ��κοͻ������ӣ���ô��CloseSocket�رյ������Socket
*/


#include<stdio.h>
#include<windows.h>

#pragma comment(lib, "Ws2_32.lib")

class ServerNet
{
public:
	SOCKET m_sock;
public:
	//  ��ʼ��������
	int ServerInit(const char* address, int port);

	// ��������
	void ServerRun();

	int ServerSend(SOCKET sk, const char* msg, int len);
};