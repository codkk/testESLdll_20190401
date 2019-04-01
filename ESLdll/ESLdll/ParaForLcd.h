/*
�����Ļ����������
*/
#pragma once

//�ļ��ڵ��ı�����
//�ļ�·��
#define PATH_WHITE_SRC_PARA "Para\\White.ini"
#define PATH_RED_SRC_PARA	"Para\\Red.ini"
#define PATH_GREEN_SRC_PARA "Para\\Green.ini"
#define PATH_BLUE_SRC_PARA "Para\\Blue.ini"
#define PATH_BLACK_SRC_PARA "Para\\Black.ini"

//���ڵ�
#define NODE_PARA   "Para"
#define NODE_SCREEN "Screen"
#define NODE_DARKSPOT "DarkSpot"
#define NODE_DARKSPOTFUZZY "DarkSpotFuzzy"
#define NODE_LIGHTSPOT "LightSpot"
#define NODE_LIGHTSPOTBLACK "LightSpotBlack"

//�ӽڵ�
//��Ļ����
#define SUB_PARA_SR_TYPE "ScreenType"
#define SUB_CHECK_LIGHT_SPOT "CheckLightSpot"
#define SUB_CHECK_DARK_SPOT "CheckDarkSpot"
//��Ļ�㷨����
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
//�򵥰�����
#define SUB_DARK_SPOT_THR_HIGH "ThrHigh"
#define SUB_DARK_SPOT_MIN_AREA "MinArea"
#define SUB_DARK_SPOT_DELTA    "Delta"
//ģ������MURA,FUZZY�ļ��
#define SUB_FUZZY_DKSPOT_MEDIANSIZE "MedianSize"
#define SUB_FUZZY_DKSPOT_GAUSS_H "GuassHight"
#define SUB_FUZZY_DKSPOT_GAUSS_L "GuassLow"
#define SUB_FUZZY_DKSPOT_MEANSIZE "MeanSize"
#define SUB_FUZZY_DKSPOT_HYS_MIN "HyThrMin"
#define SUB_FUZZY_DKSPOT_HYS_MAX "HyThrMax"
#define SUB_FUZZY_DKSPOT_MARGINW "MarginWidth"
#define SUB_FUZZY_DKSPOT_FFT_THR "FftThr"
#define SUB_FUZZY_DKSPOT_AREA    "AreaThr"
//�Ǻ�����������
#define SUB_LIGHT_SPOT_THR_HIGH "ThrHighLight"
#define SUB_LIGHT_SPOT_DELTA    "Delta_Light"
#define SUB_LIGHT_SPOT_HYS_MIN  "HyThrMin_Light"
#define SUB_LIGHT_SPOT_HYS_MAX  "HyThrMax_Light"
#define SUB_LIGHT_SPOT_FFT_THR  "FftThr_Light"
#define SUB_LIGHT_SPOT_AREA     "AreaThr_Light"

//������������
#define SUB_LIGHT_SPOT_HYS_MIN_BLACK  "HyThrMin_Light_Black"
#define SUB_LIGHT_SPOT_HYS_MAX_BLACK  "HyThrMax_Light_Black"
#define SUB_LIGHT_SPOT_AREA_BLACK     "AreaThr_Light_Black"
#define SUB_LIGHT_SPOT_MEANSIZE_BLACK "MeanSize_Light_Black"
#define SUB_LIGHT_SPOT_DYNTHR_BLACK    "DynThr_Light_Black"

//��Ļ����
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

	bool LoadPara(char* path); //���ز���
	bool SavePara(char* path);
public:
	//���Ͳ���
	T_SCR m_screenType;
	bool m_bCheckDarkSpot; //��ⰵ�㰵��
	bool m_bCheckLightSpot; //�����������
	
	//�㷨����
	///��Ļ�ָ�
	//��ֵ��
	int  m_FScreenBMinGray;
	int  m_FScreenBMaxGray;
	int  m_FScreenBEroseSize;
	int  m_FScreenBSelectAreaMin;
	int  m_FScreenBSelectAreaMax;
	//���
	int  m_FScreenBSelectAreaUDMin;  //���±�Ե���ɸѡ
	int  m_FScreenBSelectAreaUDMax;
	int  m_FScreenBSelectAreaRLMin;  //���ұ�Ե���ɸѡ
	int  m_FScreenBSelectAreaRLMax;
	int  m_FScreenBSelectDiameterUDMin;  //���±�Ե����ɸѡ
	int  m_FScreenBSelectDiameterUDMax;
	int  m_FScreenBSelectDiameterRLMin;  //���ұ�Ե����ɸѡ
	int  m_FScreenBSelectDiameterRLMax;
	//���ֱ�Ե������������򣨱�Ե����Ƚϸ��ӣ�
	int  m_FScreenBEdgeWidth;			 //��Ե����Ŀ��

	///������
	int m_ThrHigh;
	int m_MinArea;
	int m_Delta;

	///mura���
	int m_MedianSize;//	= 80;
	int m_GuassHigh	;//	= 3;
	int m_GuassLow	;//	= 10;
	int m_MeanSize	;//	= 3;
	int m_HysteresisThrMin;// = 95;
	int m_HysteresisThrMax;// = 120;
	int m_MarginWidth;// = 30;
	int m_FftThr;// = 120;
	int m_AreaThr;// = 7;

	//�Ǻ�����������
	int m_ThrHigh_Light;
	int m_Delta_Light;

	int m_HysteresisThrMin_Light;// = 95;
	int m_HysteresisThrMax_Light;// = 120;
	int m_FftThr_Light;
	int m_AreaThr_Light;// = 7;

	//������������
	int m_HysteresisThrMin_Black;// = 30;
	int m_HysteresisThrMax_Black;// = 60;
	int m_AreaThr_Black;// = 2;
	int m_MeanSize_Black;//80
	int m_DynThr_Black;//10
};

