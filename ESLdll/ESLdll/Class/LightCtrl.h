/*
波特率：19200  效验为无  停止位 1  数据位 8
使用串口调试助手前 发送指令 AB BA 02 01 00指令连接通信，控制器会返回CD DC 02 50 01，证明连上通信，接收数据成功返回OK 数据错误返回ERROR
数据格式：起始位高字节(0xab)+起始位低字节(0xba)+数据长度(从命令开始到最后的数组字节数)+命令+数据    数据格式16进制

命令定义：
读EEPROM配置             0x02
设置单通道亮度           0x31
设置单通道工作模式       0x32
设置4通道亮度            0x33
设置4通道工作模式        0x34

发送单通道亮度：0xab(起始位高字节)  0xba(起始位低字节)  3(数据长度)  0x31(命令)  0/1/2/3(通道)  0~255（亮度）
例如设置通道1亮度255：abba033100ff
例如设置通道2亮度0：abba03310100



发送单通道工作模式：0xab(起始位高字节)  0xba(起始位低字节)  3(数据长度)  0x32(命令)  0/1/2/3(通道)  0/1（工作模式）
例如设置通道3外触发：abba03320200
例如设置通道3内触发：abba03320201


发送4通道亮度：0xab(起始位高字节)  0xba(起始位低字节)  5(数据长度)  0x33(命令)  data1(0~255)   data2(0~255)   data3(0~255)   data4(0~255)
例如设置4个通道亮度为 0,01,255,255：abba05330001FFFF


发送4通道工作模式：0xab(起始位高字节)  0xba(起始位低字节)  5(数据长度)  0x34(命令)  data1(0/1)   data2(0/1)   data3(0/1)   data4(0/1)
例如设置4个通道工作模式分别为 外触发，外触发，内触发，内触发：abba053400000101


读取EEPROM配置（当前控制器参数）：abba0102
发送读EEPROM命令后通过读串口数据可以获得控制器当前内部参数设置
读到的数据格式为：0xcd(起始位高字节)  0xdc(起始位低字节)  9(数据长度)  0x51(命令)  data1(通道1亮度)  data2(通道1工作模式)  data3(通道2亮度)  data4(通道2工作模式)  data5(通道3亮度)  data6(通道3工作模式)  data7(通道4亮度)  data8(通道4工作模式)
*/

#pragma once
#include "SerialPort.h"

#define INIFILE_PATH ("config.ini")
#define NODE_SR ("Serial")
#define SUBNODE_COMM ("Comm")
#define SUBNODE_BAUD ("Baud")
#define SUBNODE_CMD1 ("CMD1")
#define SUBNODE_CMD2 ("CMD2")

class CLightCtrl
{
public:
	CLightCtrl();
	~CLightCtrl();
	void SetWnd(HWND powner);
	bool ConnectLight(int comm); //com1 = 0,com2 = 1
	bool SetLight(int idx,int nLight);
	int  GetLight(int idx);
	bool SetTrigle(int idx, int mode = 1);  //触发模式，1，内触发，0，外触发
	bool SendCmd1();
	bool SendCmd2();
	bool Loadini();


	itas109::CSerialPort m_sp;
	int m_light[4];	
	int m_comm;   //comm号，0代表com1
	long m_lbaud; //波特率	

	char m_cmd1[1024];
	char m_cmd2[1024];
	CString m_strCmd1;
	CString m_strCmd2;
private:
	bool send(char * pCh, int len);

	HWND m_pOwner;
};

