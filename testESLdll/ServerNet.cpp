/*
ServerNet.cpp
*/

#include"ServerNet.h"

/*��������ʼ��*/
int ServerNet::ServerInit(const char* address, int port)
{
	int rlt = 0;

	int iErrorMsg;

	//��ʼ��WinSock
	WSAData wsaData;
	iErrorMsg = WSAStartup(MAKEWORD(1, 1), &wsaData);

	if (iErrorMsg != NO_ERROR)
	{
		//��ʼ��WinSockʧ��
		printf("server wsastartup failed with error : %d\n", iErrorMsg);

		rlt = 1;
		return rlt;
	}

	// ������������socket

	m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_sock == INVALID_SOCKET)
		// ����socket�������쳣
	{
		printf("server socket failed with error: %d\n", WSAGetLastError());
		rlt = 2;
		return rlt;
	}

	// ������Ϣ
	SOCKADDR_IN	servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = port;
	servaddr.sin_addr.s_addr = inet_addr(address);

	//��
	iErrorMsg = bind(m_sock, (SOCKADDR*)&servaddr, sizeof(servaddr));
	if (iErrorMsg < 0)
	{
		//��ʧ��
		printf("bind failed with error : %d\n", iErrorMsg);
		rlt = 3;
		return rlt;
	}

	return rlt;
}

void ServerNet::ServerRun()
{
	// ��������
	listen(m_sock, 5);

	SOCKADDR_IN tcpAddr;
	int len = sizeof(sockaddr);
	SOCKET newSocket;
	char buf[1024];
	int rval;

	do
	{
		// ������Ϣ
		newSocket = accept(m_sock, (sockaddr*)&tcpAddr, &len);

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
					// ��ʾ���յ�������
					printf("recv msg: %s\n", buf);
			} while (rval != 0);

			// ���ڽ��յ�socket
			closesocket(newSocket);
		}
	} while (1);

	// �ر�����socket
	closesocket(m_sock);
}

int ServerNet::ServerSend(SOCKET sk,const char* msg, int len)
{
	int rlt = 0;

	int iErrMsg = 0;

	// ָ��sock������Ϣ
	iErrMsg = send(sk, msg, len, 0);
	if (iErrMsg < 0)
		// ����ʧ��
	{
		printf("send msg failed with error: %d\n", iErrMsg);
		rlt = 1;
		return rlt;
	}
	printf("send msg successfully\n");
	return rlt;
}
