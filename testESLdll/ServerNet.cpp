/*
ServerNet.cpp
*/

#include"ServerNet.h"

/*服务器初始化*/
int ServerNet::ServerInit(const char* address, int port)
{
	int rlt = 0;

	int iErrorMsg;

	//初始化WinSock
	WSAData wsaData;
	iErrorMsg = WSAStartup(MAKEWORD(1, 1), &wsaData);

	if (iErrorMsg != NO_ERROR)
	{
		//初始化WinSock失败
		printf("server wsastartup failed with error : %d\n", iErrorMsg);

		rlt = 1;
		return rlt;
	}

	// 创建服务器端socket

	m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_sock == INVALID_SOCKET)
		// 创建socket出现了异常
	{
		printf("server socket failed with error: %d\n", WSAGetLastError());
		rlt = 2;
		return rlt;
	}

	// 声明信息
	SOCKADDR_IN	servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = port;
	servaddr.sin_addr.s_addr = inet_addr(address);

	//绑定
	iErrorMsg = bind(m_sock, (SOCKADDR*)&servaddr, sizeof(servaddr));
	if (iErrorMsg < 0)
	{
		//绑定失败
		printf("bind failed with error : %d\n", iErrorMsg);
		rlt = 3;
		return rlt;
	}

	return rlt;
}

void ServerNet::ServerRun()
{
	// 公开连接
	listen(m_sock, 5);

	SOCKADDR_IN tcpAddr;
	int len = sizeof(sockaddr);
	SOCKET newSocket;
	char buf[1024];
	int rval;

	do
	{
		// 接收信息
		newSocket = accept(m_sock, (sockaddr*)&tcpAddr, &len);

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
					// 显示接收到的数据
					printf("recv msg: %s\n", buf);
			} while (rval != 0);

			// 关于接收的socket
			closesocket(newSocket);
		}
	} while (1);

	// 关闭自身socket
	closesocket(m_sock);
}

int ServerNet::ServerSend(SOCKET sk,const char* msg, int len)
{
	int rlt = 0;

	int iErrMsg = 0;

	// 指定sock发送消息
	iErrMsg = send(sk, msg, len, 0);
	if (iErrMsg < 0)
		// 发送失败
	{
		printf("send msg failed with error: %d\n", iErrMsg);
		rlt = 1;
		return rlt;
	}
	printf("send msg successfully\n");
	return rlt;
}
