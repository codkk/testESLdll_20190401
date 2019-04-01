#include "stdafx.h"
#include "ParaForLcd.h"
# include "./Class/Ini.h"

ParaForLcd::ParaForLcd()
	:m_screenType(T_WHITE_SCR)
	, m_bCheckDarkSpot(true)
	, m_bCheckLightSpot(true) 
	, m_FScreenBMinGray(80)
	, m_FScreenBMaxGray(200)
	, m_FScreenBEroseSize(21)
	, m_FScreenBSelectAreaMin(2000000)
	, m_FScreenBSelectAreaMax(9999999)
	, m_FScreenBSelectAreaUDMin(500)
	, m_FScreenBSelectAreaUDMax(99999)
	, m_FScreenBSelectAreaRLMin(20000)
	, m_FScreenBSelectAreaRLMax(46000)
	, m_FScreenBSelectDiameterUDMin(3000)
	, m_FScreenBSelectDiameterUDMax(6000)
	, m_FScreenBSelectDiameterRLMin(1400)
	, m_FScreenBSelectDiameterRLMax(3000)
	, m_FScreenBEdgeWidth(20)
	//暗点检测（对比度高的）
	, m_ThrHigh(54)
	, m_MinArea(7)
	, m_Delta(85)
	//mura检测（对比度低的）
	, m_MedianSize (80)
	, m_GuassHigh  (3)
	, m_GuassLow   (10)
	, m_MeanSize   (3)
	, m_HysteresisThrMin(110/*95*/)
	, m_HysteresisThrMax(120)
	, m_MarginWidth(30)
	, m_FftThr(120)
	, m_AreaThr(7)
	//非黑屏亮点检测
	, m_ThrHigh_Light(240)
	, m_Delta_Light(45)
	, m_HysteresisThrMin_Light(200)
	, m_HysteresisThrMax_Light(210)
	, m_FftThr_Light(120)
	, m_AreaThr_Light(7)
	//黑色屏的亮点检测参数
	, m_HysteresisThrMin_Black(30)// = 30;
	, m_HysteresisThrMax_Black(60)// = 60;
	, m_AreaThr_Black(2)// = 2;
	, m_MeanSize_Black(80)//80
	, m_DynThr_Black(10)//10
{
}


ParaForLcd::~ParaForLcd()
{
}

bool ParaForLcd::LoadPara(char * path)
{
	//打开文件
	CIni iniFile;
	if (!iniFile.Read(path))
	{
		return false;
	}
	//para
	int value = -1;
	int boolvalue = false;
	if (!iniFile.GetValue(NODE_PARA, SUB_PARA_SR_TYPE, value)) return false;//类型
	m_screenType = (T_SCR)value;
	if (!iniFile.GetValue(NODE_PARA, SUB_CHECK_LIGHT_SPOT, m_bCheckLightSpot)) return false; //亮点检测
	if (!iniFile.GetValue(NODE_PARA, SUB_CHECK_DARK_SPOT, m_bCheckDarkSpot)) return false;  //暗点检测
	//屏幕检测
	if (!iniFile.GetValue(NODE_SCREEN, SUB_SR_GRAY_MIN, m_FScreenBMinGray)) return false;
	if (!iniFile.GetValue(NODE_SCREEN, SUB_SR_GRAY_MAX, m_FScreenBMaxGray)) return false;
	if (!iniFile.GetValue(NODE_SCREEN, SUB_SR_EROS_SIZE, m_FScreenBEroseSize)) return false;
	if (!iniFile.GetValue(NODE_SCREEN, SUB_SR_AREA_MIN, m_FScreenBSelectAreaMin)) return false;
	if (!iniFile.GetValue(NODE_SCREEN, SUB_SR_AREA_MAX, m_FScreenBSelectAreaMax)) return false;
	if (!iniFile.GetValue(NODE_SCREEN, SUB_SR_AREA_MIN_UD, m_FScreenBSelectAreaUDMin)) return false;
	if (!iniFile.GetValue(NODE_SCREEN, SUB_SR_AREA_MAX_UD, m_FScreenBSelectAreaUDMax)) return false;
	if (!iniFile.GetValue(NODE_SCREEN, SUB_SR_AREA_MIN_RL, m_FScreenBSelectAreaRLMin)) return false;
	if (!iniFile.GetValue(NODE_SCREEN, SUB_SR_AREA_MAX_RL, m_FScreenBSelectAreaRLMax)) return false;
	if (!iniFile.GetValue(NODE_SCREEN, SUB_SR_DIAM_MIN_UD, m_FScreenBSelectDiameterUDMin)) return false;
	if (!iniFile.GetValue(NODE_SCREEN, SUB_SR_DIAM_MAX_UD, m_FScreenBSelectDiameterUDMax)) return false;
	if (!iniFile.GetValue(NODE_SCREEN, SUB_SR_DIAM_MIN_RL, m_FScreenBSelectDiameterRLMin)) return false;
	if (!iniFile.GetValue(NODE_SCREEN, SUB_SR_DIAM_MAX_RL, m_FScreenBSelectDiameterRLMax)) return false;
	if (!iniFile.GetValue(NODE_SCREEN, SUB_SR_EDGE_WITH, m_FScreenBEdgeWidth)) return false;
	
	//暗点检测
	if (!iniFile.GetValue(NODE_DARKSPOT, SUB_DARK_SPOT_THR_HIGH, m_ThrHigh)) return false;
	if (!iniFile.GetValue(NODE_DARKSPOT, SUB_DARK_SPOT_MIN_AREA, m_MinArea)) return false;
	if (!iniFile.GetValue(NODE_DARKSPOT, SUB_DARK_SPOT_DELTA, m_Delta)) return false;

	//暗点模糊检测
	if (!iniFile.GetValue(NODE_DARKSPOTFUZZY, SUB_FUZZY_DKSPOT_MEDIANSIZE, m_MedianSize)) return false;
	if (!iniFile.GetValue(NODE_DARKSPOTFUZZY, SUB_FUZZY_DKSPOT_GAUSS_H, m_GuassHigh)) return false;
	if (!iniFile.GetValue(NODE_DARKSPOTFUZZY, SUB_FUZZY_DKSPOT_GAUSS_L, m_GuassLow)) return false;
	if (!iniFile.GetValue(NODE_DARKSPOTFUZZY, SUB_FUZZY_DKSPOT_MEANSIZE, m_MeanSize)) return false;
	if (!iniFile.GetValue(NODE_DARKSPOTFUZZY, SUB_FUZZY_DKSPOT_HYS_MIN, m_HysteresisThrMin)) return false;
	if (!iniFile.GetValue(NODE_DARKSPOTFUZZY, SUB_FUZZY_DKSPOT_HYS_MAX, m_HysteresisThrMax)) return false;
	if (!iniFile.GetValue(NODE_DARKSPOTFUZZY, SUB_FUZZY_DKSPOT_MARGINW, m_MarginWidth)) return false;
	if (!iniFile.GetValue(NODE_DARKSPOTFUZZY, SUB_FUZZY_DKSPOT_FFT_THR, m_FftThr)) return false;
	if (!iniFile.GetValue(NODE_DARKSPOTFUZZY, SUB_FUZZY_DKSPOT_AREA, m_AreaThr)) return false;
	
	//非黑屏的亮点检测
	if (!iniFile.GetValue(NODE_LIGHTSPOT, SUB_LIGHT_SPOT_THR_HIGH, m_ThrHigh_Light)) return false;
	if (!iniFile.GetValue(NODE_LIGHTSPOT, SUB_LIGHT_SPOT_DELTA, m_Delta_Light)) return false;
	if (!iniFile.GetValue(NODE_LIGHTSPOT, SUB_LIGHT_SPOT_HYS_MIN, m_HysteresisThrMin_Light)) return false;
	if (!iniFile.GetValue(NODE_LIGHTSPOT, SUB_LIGHT_SPOT_HYS_MAX, m_HysteresisThrMax_Light)) return false;
	if (!iniFile.GetValue(NODE_LIGHTSPOT, SUB_LIGHT_SPOT_FFT_THR, m_FftThr_Light)) return false;
	if (!iniFile.GetValue(NODE_LIGHTSPOT, SUB_LIGHT_SPOT_AREA, m_AreaThr_Light)) return false;

	//黑屏的亮点检测
	if (!iniFile.GetValue(NODE_LIGHTSPOTBLACK, SUB_LIGHT_SPOT_HYS_MIN_BLACK, m_HysteresisThrMin_Black)) return false;
	if (!iniFile.GetValue(NODE_LIGHTSPOTBLACK, SUB_LIGHT_SPOT_HYS_MAX_BLACK, m_HysteresisThrMax_Black)) return false;
	if (!iniFile.GetValue(NODE_LIGHTSPOTBLACK, SUB_LIGHT_SPOT_AREA_BLACK, m_AreaThr_Black)) return false;
	if (!iniFile.GetValue(NODE_LIGHTSPOTBLACK, SUB_LIGHT_SPOT_MEANSIZE_BLACK, m_MeanSize_Black)) return false;
	if (!iniFile.GetValue(NODE_LIGHTSPOTBLACK, SUB_LIGHT_SPOT_DYNTHR_BLACK, m_DynThr_Black)) return false;
	
	return true;
}

bool ParaForLcd::SavePara(char * path)
{
	CIni iniFile;

	int value = m_screenType;
	if (!iniFile.SetValue(NODE_PARA, SUB_PARA_SR_TYPE, value)) return false;//类型
	if (!iniFile.SetValue(NODE_PARA, SUB_CHECK_LIGHT_SPOT, m_bCheckLightSpot)) return false; //亮点检测
	if (!iniFile.SetValue(NODE_PARA, SUB_CHECK_DARK_SPOT, m_bCheckDarkSpot)) return false;  //暗点检测																					//屏幕检测
	if (!iniFile.SetValue(NODE_SCREEN, SUB_SR_GRAY_MIN, m_FScreenBMinGray)) return false;
	if (!iniFile.SetValue(NODE_SCREEN, SUB_SR_GRAY_MAX, m_FScreenBMaxGray)) return false;
	if (!iniFile.SetValue(NODE_SCREEN, SUB_SR_EROS_SIZE, m_FScreenBEroseSize)) return false;
	if (!iniFile.SetValue(NODE_SCREEN, SUB_SR_AREA_MIN, m_FScreenBSelectAreaMin)) return false;
	if (!iniFile.SetValue(NODE_SCREEN, SUB_SR_AREA_MAX, m_FScreenBSelectAreaMax)) return false;
	if (!iniFile.SetValue(NODE_SCREEN, SUB_SR_AREA_MIN_UD, m_FScreenBSelectAreaUDMin)) return false;
	if (!iniFile.SetValue(NODE_SCREEN, SUB_SR_AREA_MAX_UD, m_FScreenBSelectAreaUDMax)) return false;
	if (!iniFile.SetValue(NODE_SCREEN, SUB_SR_AREA_MIN_RL, m_FScreenBSelectAreaRLMin)) return false;
	if (!iniFile.SetValue(NODE_SCREEN, SUB_SR_AREA_MAX_RL, m_FScreenBSelectAreaRLMax)) return false;
	if (!iniFile.SetValue(NODE_SCREEN, SUB_SR_DIAM_MIN_UD, m_FScreenBSelectDiameterUDMin)) return false;
	if (!iniFile.SetValue(NODE_SCREEN, SUB_SR_DIAM_MAX_UD, m_FScreenBSelectDiameterUDMax)) return false;
	if (!iniFile.SetValue(NODE_SCREEN, SUB_SR_DIAM_MIN_RL, m_FScreenBSelectDiameterRLMin)) return false;
	if (!iniFile.SetValue(NODE_SCREEN, SUB_SR_DIAM_MAX_RL, m_FScreenBSelectDiameterRLMax)) return false;
	if (!iniFile.SetValue(NODE_SCREEN, SUB_SR_EDGE_WITH, m_FScreenBEdgeWidth)) return false;

	//暗点检测
	if (!iniFile.SetValue(NODE_DARKSPOT, SUB_DARK_SPOT_THR_HIGH, m_ThrHigh)) return false;
	if (!iniFile.SetValue(NODE_DARKSPOT, SUB_DARK_SPOT_MIN_AREA, m_MinArea)) return false;
	if (!iniFile.SetValue(NODE_DARKSPOT, SUB_DARK_SPOT_DELTA, m_Delta)) return false;

	//暗点模糊检测
	if (!iniFile.SetValue(NODE_DARKSPOTFUZZY, SUB_FUZZY_DKSPOT_MEDIANSIZE, m_MedianSize)) return false;
	if (!iniFile.SetValue(NODE_DARKSPOTFUZZY, SUB_FUZZY_DKSPOT_GAUSS_H, m_GuassHigh)) return false;
	if (!iniFile.SetValue(NODE_DARKSPOTFUZZY, SUB_FUZZY_DKSPOT_GAUSS_L, m_GuassLow)) return false;
	if (!iniFile.SetValue(NODE_DARKSPOTFUZZY, SUB_FUZZY_DKSPOT_MEANSIZE, m_MeanSize)) return false;
	if (!iniFile.SetValue(NODE_DARKSPOTFUZZY, SUB_FUZZY_DKSPOT_HYS_MIN, m_HysteresisThrMin)) return false;
	if (!iniFile.SetValue(NODE_DARKSPOTFUZZY, SUB_FUZZY_DKSPOT_HYS_MAX, m_HysteresisThrMax)) return false;
	if (!iniFile.SetValue(NODE_DARKSPOTFUZZY, SUB_FUZZY_DKSPOT_MARGINW, m_MarginWidth)) return false;
	if (!iniFile.SetValue(NODE_DARKSPOTFUZZY, SUB_FUZZY_DKSPOT_FFT_THR, m_FftThr)) return false;
	if (!iniFile.SetValue(NODE_DARKSPOTFUZZY, SUB_FUZZY_DKSPOT_AREA, m_AreaThr)) return false;

	//非黑屏的亮点检测
	if (!iniFile.SetValue(NODE_LIGHTSPOT, SUB_LIGHT_SPOT_THR_HIGH, m_ThrHigh_Light)) return false;
	if (!iniFile.SetValue(NODE_LIGHTSPOT, SUB_LIGHT_SPOT_DELTA, m_Delta_Light)) return false;
	if (!iniFile.SetValue(NODE_LIGHTSPOT, SUB_LIGHT_SPOT_HYS_MIN, m_HysteresisThrMin_Light)) return false;
	if (!iniFile.SetValue(NODE_LIGHTSPOT, SUB_LIGHT_SPOT_HYS_MAX, m_HysteresisThrMax_Light)) return false;
	if (!iniFile.SetValue(NODE_LIGHTSPOT, SUB_LIGHT_SPOT_FFT_THR, m_FftThr_Light)) return false;
	if (!iniFile.SetValue(NODE_LIGHTSPOT, SUB_LIGHT_SPOT_AREA, m_AreaThr_Light)) return false;

	//黑屏的亮点检测
	if (!iniFile.SetValue(NODE_LIGHTSPOTBLACK, SUB_LIGHT_SPOT_HYS_MIN_BLACK, m_HysteresisThrMin_Black)) return false;
	if (!iniFile.SetValue(NODE_LIGHTSPOTBLACK, SUB_LIGHT_SPOT_HYS_MAX_BLACK, m_HysteresisThrMax_Black)) return false;
	if (!iniFile.SetValue(NODE_LIGHTSPOTBLACK, SUB_LIGHT_SPOT_AREA_BLACK, m_AreaThr_Black)) return false;
	if (!iniFile.SetValue(NODE_LIGHTSPOTBLACK, SUB_LIGHT_SPOT_MEANSIZE_BLACK, m_MeanSize_Black)) return false;
	if (!iniFile.SetValue(NODE_LIGHTSPOTBLACK, SUB_LIGHT_SPOT_DYNTHR_BLACK, m_DynThr_Black)) return false;
	
	iniFile.Write(path);
	return true;
}
