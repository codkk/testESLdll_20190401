/*
屏幕检测结果通用类
*/
#pragma once
#include "ParaForLcd.h"
enum Type_Res {
	RES_TYPE_ERROR = 0, //发生错误
	RES_TYPE_OK,		//正常
	RES_TYPE_SCREEN,    //屏幕不完整
	RES_TYPE_BRIGHT_SPOT, //亮点
	RES_TYPE_BRIGHT_THREAD, //亮线
	RES_TYPE_BRIGHT_BLOB,   //亮斑
	RES_TYPE_DARK_SPOT, //暗点
	RES_TYPE_DARK_THREAD,//暗线
	RES_TYPE_DARK_BLOB, //黑块
	RES_TYPE_MURA,//mura
	RES_TYPE_DUST, //灰尘
	RES_TYPE_LIGHT_LEAK, //漏光
};



class ResultForLcd
{
public:
	ResultForLcd();
	~ResultForLcd();
public:
	//结果分类
	T_SCR m_screenType;  //检测目标类型
	Type_Res m_resType;  //缺陷类型
	std::string m_strMsg; //消息
	double m_targetColX;    //目标中心位置列坐标
	double m_targetRowY;
	double m_Area;          //目标面积
	double m_lfRow1;
	double m_lfCol1;
	double m_lfRow2;
	double m_lfCol2;
	std::string  m_strPathFullImage; //整图存放位置
	std::string  m_strPathTargetImage; //截图存放位置
	
};

