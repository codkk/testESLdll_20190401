/*
��Ļ�����ͨ����
*/
#pragma once
#include "ParaForLcd.h"
enum Type_Res {
	RES_TYPE_ERROR = 0, //��������
	RES_TYPE_OK,		//����
	RES_TYPE_SCREEN,    //��Ļ������
	RES_TYPE_BRIGHT_SPOT, //����
	RES_TYPE_BRIGHT_THREAD, //����
	RES_TYPE_BRIGHT_BLOB,   //����
	RES_TYPE_DARK_SPOT, //����
	RES_TYPE_DARK_THREAD,//����
	RES_TYPE_DARK_BLOB, //�ڿ�
	RES_TYPE_MURA,//mura
	RES_TYPE_DUST, //�ҳ�
	RES_TYPE_LIGHT_LEAK, //©��
};



class ResultForLcd
{
public:
	ResultForLcd();
	~ResultForLcd();
public:
	//�������
	T_SCR m_screenType;  //���Ŀ������
	Type_Res m_resType;  //ȱ������
	std::string m_strMsg; //��Ϣ
	double m_targetColX;    //Ŀ������λ��������
	double m_targetRowY;
	double m_Area;          //Ŀ�����
	double m_lfRow1;
	double m_lfCol1;
	double m_lfRow2;
	double m_lfCol2;
	std::string  m_strPathFullImage; //��ͼ���λ��
	std::string  m_strPathTargetImage; //��ͼ���λ��
	
};

