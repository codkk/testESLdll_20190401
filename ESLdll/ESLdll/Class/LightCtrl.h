/*
�����ʣ�19200  Ч��Ϊ��  ֹͣλ 1  ����λ 8
ʹ�ô��ڵ�������ǰ ����ָ�� AB BA 02 01 00ָ������ͨ�ţ��������᷵��CD DC 02 50 01��֤������ͨ�ţ��������ݳɹ�����OK ���ݴ��󷵻�ERROR
���ݸ�ʽ����ʼλ���ֽ�(0xab)+��ʼλ���ֽ�(0xba)+���ݳ���(�����ʼ�����������ֽ���)+����+����    ���ݸ�ʽ16����

����壺
��EEPROM����             0x02
���õ�ͨ������           0x31
���õ�ͨ������ģʽ       0x32
����4ͨ������            0x33
����4ͨ������ģʽ        0x34

���͵�ͨ�����ȣ�0xab(��ʼλ���ֽ�)  0xba(��ʼλ���ֽ�)  3(���ݳ���)  0x31(����)  0/1/2/3(ͨ��)  0~255�����ȣ�
��������ͨ��1����255��abba033100ff
��������ͨ��2����0��abba03310100



���͵�ͨ������ģʽ��0xab(��ʼλ���ֽ�)  0xba(��ʼλ���ֽ�)  3(���ݳ���)  0x32(����)  0/1/2/3(ͨ��)  0/1������ģʽ��
��������ͨ��3�ⴥ����abba03320200
��������ͨ��3�ڴ�����abba03320201


����4ͨ�����ȣ�0xab(��ʼλ���ֽ�)  0xba(��ʼλ���ֽ�)  5(���ݳ���)  0x33(����)  data1(0~255)   data2(0~255)   data3(0~255)   data4(0~255)
��������4��ͨ������Ϊ 0,01,255,255��abba05330001FFFF


����4ͨ������ģʽ��0xab(��ʼλ���ֽ�)  0xba(��ʼλ���ֽ�)  5(���ݳ���)  0x34(����)  data1(0/1)   data2(0/1)   data3(0/1)   data4(0/1)
��������4��ͨ������ģʽ�ֱ�Ϊ �ⴥ�����ⴥ�����ڴ������ڴ�����abba053400000101


��ȡEEPROM���ã���ǰ��������������abba0102
���Ͷ�EEPROM�����ͨ�����������ݿ��Ի�ÿ�������ǰ�ڲ���������
���������ݸ�ʽΪ��0xcd(��ʼλ���ֽ�)  0xdc(��ʼλ���ֽ�)  9(���ݳ���)  0x51(����)  data1(ͨ��1����)  data2(ͨ��1����ģʽ)  data3(ͨ��2����)  data4(ͨ��2����ģʽ)  data5(ͨ��3����)  data6(ͨ��3����ģʽ)  data7(ͨ��4����)  data8(ͨ��4����ģʽ)
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
	bool SetTrigle(int idx, int mode = 1);  //����ģʽ��1���ڴ�����0���ⴥ��
	bool SendCmd1();
	bool SendCmd2();
	bool Loadini();


	itas109::CSerialPort m_sp;
	int m_light[4];	
	int m_comm;   //comm�ţ�0����com1
	long m_lbaud; //������	

	char m_cmd1[1024];
	char m_cmd2[1024];
	CString m_strCmd1;
	CString m_strCmd2;
private:
	bool send(char * pCh, int len);

	HWND m_pOwner;
};

