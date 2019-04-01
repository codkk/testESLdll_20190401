#include "stdafx.h"
#include "ResultForLcd.h"


ResultForLcd::ResultForLcd():
	m_resType(RES_TYPE_OK)
{
	m_screenType = T_WHITE_SCR;  //检测目标类型
	m_strMsg = ""; //消息
	m_targetColX = -1;    //目标中心位置列坐标
	m_targetRowY = -1;
	m_Area = -1;          //目标面积
	m_lfRow1 = -1;
	m_lfCol1 = -1;
	m_lfRow2 = -1;
	m_lfCol2 = -1;
	m_strPathFullImage = ""; //整图存放位置
	m_strPathTargetImage = ""; //截图存放位置
}


ResultForLcd::~ResultForLcd()
{
}
