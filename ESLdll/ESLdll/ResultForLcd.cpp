#include "stdafx.h"
#include "ResultForLcd.h"


ResultForLcd::ResultForLcd():
	m_resType(RES_TYPE_OK)
{
	m_screenType = T_WHITE_SCR;  //���Ŀ������
	m_strMsg = ""; //��Ϣ
	m_targetColX = -1;    //Ŀ������λ��������
	m_targetRowY = -1;
	m_Area = -1;          //Ŀ�����
	m_lfRow1 = -1;
	m_lfCol1 = -1;
	m_lfRow2 = -1;
	m_lfCol2 = -1;
	m_strPathFullImage = ""; //��ͼ���λ��
	m_strPathTargetImage = ""; //��ͼ���λ��
}


ResultForLcd::~ResultForLcd()
{
}
