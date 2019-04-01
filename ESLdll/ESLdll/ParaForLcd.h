/*
检测屏幕公共参数类
*/
#pragma once

//文件节点文本定义
//文件路径
#define PATH_WHITE_SRC_PARA "Para\\White.ini"
#define PATH_RED_SRC_PARA	"Para\\Red.ini"
#define PATH_GREEN_SRC_PARA "Para\\Green.ini"
#define PATH_BLUE_SRC_PARA "Para\\Blue.ini"
#define PATH_BLACK_SRC_PARA "Para\\Black.ini"

//父节点
#define NODE_PARA   "Para"
#define NODE_SCREEN "Screen"
#define NODE_DARKSPOT "DarkSpot"
#define NODE_DARKSPOTFUZZY "DarkSpotFuzzy"
#define NODE_LIGHTSPOT "LightSpot"
#define NODE_LIGHTSPOTBLACK "LightSpotBlack"

//子节点
//屏幕类型
#define SUB_PARA_SR_TYPE "ScreenType"
#define SUB_CHECK_LIGHT_SPOT "CheckLightSpot"
#define SUB_CHECK_DARK_SPOT "CheckDarkSpot"
//屏幕算法参数
#define SUB_SR_GRAY_MIN "MinGray"
#define SUB_SR_GRAY_MAX "MaxGray"
#define SUB_SR_EROS_SIZE "EroseSize"
#define SUB_SR_AREA_MIN "SelectAreaMin"
#define SUB_SR_AREA_MAX "SelectAreaMax"
#define SUB_SR_AREA_MIN_UD "SelectAreaUDMin"
#define SUB_SR_AREA_MAX_UD "SelectAreaUDMax"
#define SUB_SR_AREA_MIN_RL "SelectAreaRLMin"
#define SUB_SR_AREA_MAX_RL "SelectAreaRLMax"
#define SUB_SR_DIAM_MIN_UD "SelectDiamUDMin"
#define SUB_SR_DIAM_MAX_UD "SelectDiamUDMax"
#define SUB_SR_DIAM_MIN_RL "SelectDiamRLMin"
#define SUB_SR_DIAM_MAX_RL "SelectDiamRLMax"
#define SUB_SR_EDGE_WITH  "EdgeWidth"
//简单暗点检测
#define SUB_DARK_SPOT_THR_HIGH "ThrHigh"
#define SUB_DARK_SPOT_MIN_AREA "MinArea"
#define SUB_DARK_SPOT_DELTA    "Delta"
//模糊暗点MURA,FUZZY的检测
#define SUB_FUZZY_DKSPOT_MEDIANSIZE "MedianSize"
#define SUB_FUZZY_DKSPOT_GAUSS_H "GuassHight"
#define SUB_FUZZY_DKSPOT_GAUSS_L "GuassLow"
#define SUB_FUZZY_DKSPOT_MEANSIZE "MeanSize"
#define SUB_FUZZY_DKSPOT_HYS_MIN "HyThrMin"
#define SUB_FUZZY_DKSPOT_HYS_MAX "HyThrMax"
#define SUB_FUZZY_DKSPOT_MARGINW "MarginWidth"
#define SUB_FUZZY_DKSPOT_FFT_THR "FftThr"
#define SUB_FUZZY_DKSPOT_AREA    "AreaThr"
//非黑屏的亮点检测
#define SUB_LIGHT_SPOT_THR_HIGH "ThrHighLight"
#define SUB_LIGHT_SPOT_DELTA    "Delta_Light"
#define SUB_LIGHT_SPOT_HYS_MIN  "HyThrMin_Light"
#define SUB_LIGHT_SPOT_HYS_MAX  "HyThrMax_Light"
#define SUB_LIGHT_SPOT_FFT_THR  "FftThr_Light"
#define SUB_LIGHT_SPOT_AREA     "AreaThr_Light"

//黑屏的亮点检测
#define SUB_LIGHT_SPOT_HYS_MIN_BLACK  "HyThrMin_Light_Black"
#define SUB_LIGHT_SPOT_HYS_MAX_BLACK  "HyThrMax_Light_Black"
#define SUB_LIGHT_SPOT_AREA_BLACK     "AreaThr_Light_Black"
#define SUB_LIGHT_SPOT_MEANSIZE_BLACK "MeanSize_Light_Black"
#define SUB_LIGHT_SPOT_DYNTHR_BLACK    "DynThr_Light_Black"

//屏幕类型
enum T_SCR {
	T_WHITE_SCR = 0,
	T_GRAY_SCR,
	T_RED_SCR,
	T_GREEN_SCR,
	T_BLUE_SCR,
	T_BLACK_SCR,
	T_DUST_SCR,
};

class ParaForLcd
{
public:
	ParaForLcd();
	~ParaForLcd();

	bool LoadPara(char* path); //加载参数
	bool SavePara(char* path);
public:
	//类型参数
	T_SCR m_screenType;
	bool m_bCheckDarkSpot; //检测暗点暗斑
	bool m_bCheckLightSpot; //检测亮点亮斑
	
	//算法参数
	///屏幕分割
	//二值化
	int  m_FScreenBMinGray;
	int  m_FScreenBMaxGray;
	int  m_FScreenBEroseSize;
	int  m_FScreenBSelectAreaMin;
	int  m_FScreenBSelectAreaMax;
	//拟合
	int  m_FScreenBSelectAreaUDMin;  //上下边缘面积筛选
	int  m_FScreenBSelectAreaUDMax;
	int  m_FScreenBSelectAreaRLMin;  //左右边缘面积筛选
	int  m_FScreenBSelectAreaRLMax;
	int  m_FScreenBSelectDiameterUDMin;  //上下边缘长度筛选
	int  m_FScreenBSelectDiameterUDMax;
	int  m_FScreenBSelectDiameterRLMin;  //左右边缘长度筛选
	int  m_FScreenBSelectDiameterRLMax;
	//划分边缘区域和中心区域（边缘区域比较复杂）
	int  m_FScreenBEdgeWidth;			 //边缘区域的宽度

	///暗点检测
	int m_ThrHigh;
	int m_MinArea;
	int m_Delta;

	///mura检测
	int m_MedianSize;//	= 80;
	int m_GuassHigh	;//	= 3;
	int m_GuassLow	;//	= 10;
	int m_MeanSize	;//	= 3;
	int m_HysteresisThrMin;// = 95;
	int m_HysteresisThrMax;// = 120;
	int m_MarginWidth;// = 30;
	int m_FftThr;// = 120;
	int m_AreaThr;// = 7;

	//非黑屏的亮点检测
	int m_ThrHigh_Light;
	int m_Delta_Light;

	int m_HysteresisThrMin_Light;// = 95;
	int m_HysteresisThrMax_Light;// = 120;
	int m_FftThr_Light;
	int m_AreaThr_Light;// = 7;

	//黑屏的亮点检测
	int m_HysteresisThrMin_Black;// = 30;
	int m_HysteresisThrMax_Black;// = 60;
	int m_AreaThr_Black;// = 2;
	int m_MeanSize_Black;//80
	int m_DynThr_Black;//10
};

