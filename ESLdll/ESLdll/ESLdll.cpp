// ESLdll.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "ESLdll.h"
#include "ESLInterface.h"
#include "ParaForLcd.h"
#include "ResultForLcd.h"
# include "HalconCpp.h" //halcon
# include "HDevThread.h"
# include "./Class/Ini.h"
#include "DvpCamera.h"
using namespace HalconCpp;

typedef int(*CheckMe)();
typedef int(*RegistMe)();

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define IMG_W 3664
#define IMG_H 2748

#define WM_MYMSG WM_USER+88   //实时显示

#define WHITESCREEN_RESULT_FILE "Result\\WhiteResult.txt"  //结果文件文件名
#define BLACKSCREEN_RESULT_FILE "Result\\BlackResult.txt"
#define GRAYSCALE_RESULT_FILE "Result\\GrayResult.txt"
#define SCREENREGION_RESULT_FILE "Result\\ScreenRegionResult.txt"
#define CHESSBOARD_RESULT_FILE "Result\\ChessResult.txt"//棋盘格检测结果文件


#define WHITESCREEN_RESULT_IMG "ResImg\\ResultFAIL_WhiteScreen_%d_%d__%d：%d：%d" //NG图像名
#define BLACKSCREEN_RESULT_IMG "ResImg\\ResultFAIL_BlackScreen_%d_%d__%d：%d：%d"
#define GRAYSCALE_RESULT_IMG "ResImg\\ResultFAIL_GrayResult_%d_%d__%d：%d：%d"
#define CHESSBOARD_RESULT_IMG "ResImg\\ResultFAIL_ChessResult_%d_%d__%d：%d：%d"
#define SCREENREGION_RESULT_IMG "ResImg\\Result_ScreenRegion"

#define NODE_WHITESCREEN_RES "WhiteScreenResult" //节点
#define NODE_BLACKSCREEN_RES "BlackScreenResult"
#define NODE_GRAYSCALE_RES "GrayScaleResult"
#define NODE_SCREENREGION_RES "ScreenRegionResult"
#define NODE_CHESSBOARD_RES "ChessBoardResult"

#define SUBNODE_COUNT "Count" //子节点
#define SUBNODE_AREA "Area"
#define SUBNODE_MSG "Message"
#define SUBNODE_FAILPATH "FailImagePath"
#define SUBNODE_RES "Result"

#define RESULT_PASS "PASS"
#define RESULT_FAIL "FAIL"

#define RESULT_MSG_PASS "PASS"
#define RESULT_MSG_FAIL_GRAY "GRAY FAIL" //灰度超出公差
#define RESULT_MSG_FAIL_DEFECT "DEFECT FAIL" //检测到缺陷

#define CONFIG_FILE "Config\\ESLconfig.ini"  //配置文件文件名
#define PATH_CAM_CONFIG "Config\\CamConfig.ini" //相机配置文件名
#define NODE_WHITESCREEN_CONFIG "WhiteScreenConfig" //节点
#define NODE_BLACKSCREEN_CONFIG "BlackScreenConfig"
#define NODE_GRAYSCALE_CONFIG "GrayScaleConfig"
#define NODE_SCREENREGION_CONFIG "ScreenRegionConfig"
#define NODE_CHESSBOARD_CONFIG "ChessBoardConfig"
#define NODE_CAM_CONFIG "CamConfig" //相机设置

//
//TODO:  如果此 DLL 相对于 MFC DLL 是动态链接的，
//		则从此 DLL 导出的任何调入
//		MFC 的函数必须将 AFX_MANAGE_STATE 宏添加到
//		该函数的最前面。
//
//		例如: 
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。  这意味着
//		它必须作为函数中的第一个语句
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CESLdllApp

BEGIN_MESSAGE_MAP(CESLdllApp, CWinApp)
END_MESSAGE_MAP()


// CESLdllApp 构造

CESLdllApp::CESLdllApp()
{
	// TODO:  在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CESLdllApp 对象

CESLdllApp theApp;
HObject  /*ho_RegionScreen,*/ ho_RegionCenter, ho_RegionMargin;
HTuple hv_WindowHandle;
HTuple  hv_AcqHandle;  //相机
HTuple hv_Graystd, hv_Graymax, hv_Graymin; //灰阶的标准值，最大值，最小值， 
HTuple hv_Whitestd, hv_Whitemax, hv_Whitemin; //背光 白屏的 标准值， 最大值， 最小值，
HTuple hv_Chessstd, hv_Chessmax, hv_Chessmin; //棋盘格 标准值， 最大值， 最小值，

CString m_strCamName = "default";
CString m_strCamIniFile = "";

HObject g_Image;	   //rgb图像
HObject g_ImageGray;	//灰度图像
HObject g_ImageRed;    //红色分量图像
HObject g_ImageGreen;  //绿色分量图像
HObject g_ImageBlue;   //蓝色分量图像
HObject g_RegionScreen;//屏幕区域

HObject g_ImageParts;

ParaForLcd g_Para;     //检测参数

ParaForLcd g_Para_White; //白色屏幕的检测参数，检测时将付给g_Para;
ParaForLcd g_Para_Red;
ParaForLcd g_Para_Green;
ParaForLcd g_Para_Blue;
ParaForLcd g_Para_Black;

ResultForLcd g_Result;	//检测结果
std::vector<ResultForLcd> g_vctResult; //缺陷的结果链表

CWnd* g_pWnd = NULL;
HTuple hv_WindowHandle_White; //结果显示窗口
HTuple hv_WindowHandle_Red;
HTuple hv_WindowHandle_Green;
HTuple hv_WindowHandle_Blue;
HTuple hv_WindowHandle_Black;

//软件使用期限
CheckMe pCheckme = NULL;
RegistMe pRegistme = NULL;

//度申相机
DvpCamera myCam;

//发生错误的类别信息
std::string strErrorMsg[] = {
	"发生错误",
	"OK",
	"屏幕区域NG",
	"亮点",
	"亮线",
	"亮斑",
	"暗点",
	"暗线",
	"暗斑",
	"MURA",
	"灰尘",
	"漏光" 
};
///////////////////////////////////////////////
//算法声明
void CheckDarkFuzzySpot(HObject ho_ImageMean, HObject ho_ImageFFT, HObject ho_ScreenRegion,
	HObject ho_MaskRegion, HObject *ho_ResultRegion2, HTuple hv_HysteresisThrMin,
	HTuple hv_HysteresisThrMax, HTuple hv_MarginWidth, HTuple hv_fftThr, HTuple hv_AreaThr,
	HTuple hv_OffsetX, HTuple hv_OffsetY);
void PreProcess(HObject ho_ImageSrc, HObject ho_ScreenRegion, HObject ho_MaskRegion,
	HObject *ho_ImageFFTRes, HObject *ho_ImageMeanRes, HObject *ho_ImageFFTRes2,
	HObject *ho_ImageMeanRes2, HTuple hv_MedianSize, HTuple hv_GuassHigh, HTuple hv_GuassLow,
	HTuple hv_MeanSize, HTuple hv_MaskRoundGray, HTuple *hv_OffsetX, HTuple *hv_OffsetY);
void FindScreen(HObject ho_ImageSrc, HObject *ho_ScreenRegion);
void SimpleDarkSpot(HObject ho_ImageSrc, HObject ho_ScreenRegion, HObject *ho_ResultRegion,
	HTuple hv_ThrHigh, HTuple hv_MinArea, HTuple hv_Delta, HTuple *hv_RoundGray);
void CheckLightFuzzySpot(HObject ho_ImageMean, HObject ho_ImageFFT, HObject ho_ScreenRegion,
	HObject ho_MaskRegion, HObject *ho_ResultRegionLight, HTuple hv_HysteresisThrMin,
	HTuple hv_HysteresisThrMax, HTuple hv_MarginWidth, HTuple hv_fftThr, HTuple hv_AreaThr,
	HTuple hv_OffsetX, HTuple hv_OffsetY);
void SimpleLightSpot(HObject ho_ImageSrc, HObject ho_ScreenRegion, HObject *ho_ResultRegion,
	HTuple hv_ThrHigh, HTuple hv_MinArea, HTuple hv_Delta, HTuple *hv_RoundGray);
void CheckLightSpotInDarkScreen(HObject ho_ImageSrc, HObject ho_RegionScreen, HObject *ho_RegionResult,
	HTuple hv_HysteresisMin, HTuple hv_HysteresisMax, HTuple hv_AreaThr, HTuple hv_MeanSize,
	HTuple hv_DynThr);
void CheckLeakBlack(HObject ho_ImageSrc, HObject ho_RegionScreen, HObject *ho_RegionResult);
void JudgeResult(HObject ho_ResultRegions, HObject ho_ImageSrc, HObject *ho_ImageParts,
	HTuple hv_ImageW, HTuple hv_ImageH, HTuple hv_BoxMinW, HTuple hv_BoxMinH, HTuple hv_MaxShowNum,
	HTuple *hv_Areas, HTuple *hv_CentRow, HTuple *hv_CentCol, HTuple *hv_Row1, HTuple *hv_Col1,
	HTuple *hv_Row2, HTuple *hv_Col2);
///
bool DetectSpot(HObject ImageSrc, HObject* RegionResults, HObject* RegionResults_Light);
bool DealWithResult(HObject ho_ResultRegions, HObject ho_ImageSrc, HObject *ho_ImageParts, T_SCR screentype, Type_Res type);
///////////////////////////////////////
//异常处理函数
void MyExcepHandle(const HalconCpp::HException &except)
{
	throw except;
}
void ShowException(HalconCpp::HException & except)
{
	CString str(except.ErrorMessage());
	str.Replace(_T("HALCON"), _T(""));
	AfxMessageBox(str);
	//MessageBox((HWND)hl_wnd, str, _T("Error"), MB_OK);
}

BOOL CESLdllApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

//分割成16部分，并取每部分的中心部分
//ho_RegionSrc		要分割的屏幕区域
//ho_RegionRects	16个部分矩形区域
//hv_rownum			要分割的行数
//hv_colnum			要分割的列数
void cutpart(HObject ho_RegionSrc, HObject *ho_RegionRects, HTuple hv_rownum, HTuple hv_colnum)
{
	// Local iconic variables
	HObject  ho_RectangleTmp;

	// Local control variables
	HTuple  hv_Row11, hv_Column11, hv_Row2, hv_Column2;
	HTuple  hv_Retanglewidth, hv_Retangleheight, hv_rowheight;
	HTuple  hv_colwidth, hv_centerPtx, hv_centerPty, hv_half_w;
	HTuple  hv_half_h, hv_rowindex, hv_colindex, hv_x, hv_y;
	HTuple  hv_new_row1, hv_new_col1, hv_new_row2, hv_new_col2;

	SmallestRectangle1(ho_RegionSrc, &hv_Row11, &hv_Column11, &hv_Row2, &hv_Column2);
	hv_Retanglewidth = (hv_Column2 - hv_Column11).TupleAbs();
	hv_Retangleheight = (hv_Row2 - hv_Row11).TupleAbs();
	hv_rownum = 4;
	hv_colnum = 4;
	hv_rowheight = hv_Retangleheight / hv_rownum;
	hv_colwidth = hv_Retanglewidth / hv_colnum;
	hv_centerPtx = HTuple();
	hv_centerPty = HTuple();
	hv_half_w = hv_colwidth / 4;
	hv_half_h = hv_rowheight / 4;
	GenEmptyObj(&(*ho_RegionRects));
	{
		HTuple end_val12 = hv_rownum;
		HTuple step_val12 = 1;
		for (hv_rowindex = 1; hv_rowindex.Continue(end_val12, step_val12); hv_rowindex += step_val12)
		{
			{
				HTuple end_val13 = hv_colnum;
				HTuple step_val13 = 1;
				for (hv_colindex = 1; hv_colindex.Continue(end_val13, step_val13); hv_colindex += step_val13)
				{
					hv_x = ((hv_colwidth*((2 * hv_colindex) - 1)) / 2) + hv_Column11;
					hv_y = ((hv_rowheight*((2 * hv_rowindex) - 1)) / 2) + hv_Row11;
					TupleConcat(hv_centerPtx, hv_x, &hv_centerPtx);
					TupleConcat(hv_centerPty, hv_y, &hv_centerPtx);
					//gen_rectangle2 (RectangleTmp, y, x, Phi, half_w, half_h)
					hv_new_row1 = hv_y - hv_half_h;
					hv_new_col1 = hv_x - hv_half_w;
					hv_new_row2 = hv_y + hv_half_h;
					hv_new_col2 = hv_x + hv_half_w;
					GenRectangle1(&ho_RectangleTmp, hv_new_row1, hv_new_col1, hv_new_row2, hv_new_col2);
					ConcatObj((*ho_RegionRects), ho_RectangleTmp, &(*ho_RegionRects));
				}
			}
		}
	}
	return;
}

//比较灰度值， 返回序号0，1，2
//hv_GrayTest  比较对象
//hv_GrayStds  标准值
//hv_GrayMax   最大值，也可作为正公差
//hv_GrayMin   最小值，也可作为负公差
//mode   模式 0； 比较最大值最小值， 1： 比较标准值正负公差
//hv_GrayDiffIdx 返回出现超出的下标
void compareGray(HTuple hv_GrayTest, HTuple hv_GrayStds, HTuple hv_GrayMax, HTuple hv_GrayMin, int mode, HTuple *hv_GrayDiffIdx)
{
	(*hv_GrayDiffIdx) = HTuple();
	if (0 == mode)
	{
		for (int idx = 0; idx < hv_GrayTest.Length(); idx++)
		{
			if ((hv_GrayMax[idx] < hv_GrayTest[idx]) || (hv_GrayMin[idx] > hv_GrayTest[idx]))
			{
				TupleConcat((*hv_GrayDiffIdx), idx, &(*hv_GrayDiffIdx));
			}
		}
	}
	else
	{
		for (int idx = 0; idx < hv_GrayTest.Length(); idx++)
		{
			if (((hv_GrayMax[idx]+hv_GrayStds[idx]) < hv_GrayTest[idx]) || ((hv_GrayStds[idx] - hv_GrayMin[idx]) > hv_GrayTest[idx]))
			{
				TupleConcat((*hv_GrayDiffIdx), idx, &(*hv_GrayDiffIdx));
			}
		}
	}

	return;
}

//屏幕区域自动识别, 并计算4x4的灰度值
bool screenRegion(HObject ho_Image, bool bflag = true)
{
	HObject ho_ImageR, ho_ImageB, ho_ImageG;
	HObject ho_Regions, ho_RegionErosion, ho_RegionDilation;
	HObject ho_ConnectedRegions, ho_SelectedRegions, ho_SelectedRegions1;
	HObject ho_RegionFillUp6, ho_RegionErosion3, ho_Contours;
	HObject ho_RegionErosion4, ho_RegionDilation5;
	HObject ho_RegionDifference4, ho_RegionOpening3, ho_ConnectedRegions4;
	HObject ho_SelectedRegions4, ho_RegionDilation6, ho_RegionDifference5;
	HObject ho_RegionOpening4, ho_ConnectedRegions5, ho_SelectedRegions5;
	HObject ho_ObjectSelected1, ho_ClippedContours, ho_SelectedXLD;
	HObject ho_ObjectSelected2;
	HObject  ho_ClippedContours1, ho_SelectedXLD1, ho_ObjectSelected3;
	HObject  ho_ClippedContours2, ho_SelectedXLD2, ho_ObjectSelected4;
	HObject  ho_ClippedContours3, ho_SelectedXLD3, ho_ObjectsConcat;

	HTuple  hv_Number_edge, hv_Row12, hv_Column12;
	HTuple  hv_Row21, hv_Column21, hv_Row13, hv_Column13, hv_Row22;
	HTuple  hv_Column22, hv_Row14, hv_Column14, hv_Row23, hv_Column23;
	HTuple  hv_Row15, hv_Column15, hv_Row24, hv_Column24, hv_RowBegin;
	HTuple  hv_ColBegin, hv_RowEnd, hv_ColEnd, hv_Nr, hv_Nc;
	HTuple  hv_Dist, hv_Row4, hv_Column4, hv_IsOverlapping;
	HTuple  hv_Row5, hv_Column5, hv_IsOverlapping1, hv_Row6;
	HTuple  hv_Column6, hv_IsOverlapping2, hv_Row7, hv_Column7;
	HTuple  hv_IsOverlapping3, hv_Row_Concat, hv_Col_Concat;

	//)一，屏幕区域分割
	//ReadImage(&ho_Image, "E:/屏幕检测/电子书/pic/whiteFAKE_NG2.jpg");
	Decompose3(ho_Image, &ho_ImageR, &ho_ImageB, &ho_ImageG);
	//shock_filter (ImageB, SharpenedImage, 0.5, 4, 'canny', 3)
	Threshold(ho_ImageB, &ho_Regions, 81, 200);
	//ClosingRectangle1(ho_Regions, &ho_Regions, 10, 10);
	FillUp(ho_Regions, &ho_Regions);
	ErosionRectangle1(ho_Regions, &ho_RegionErosion, 21, 21);
	DilationRectangle1(ho_RegionErosion, &ho_RegionDilation, 21, 21);
	Connection(ho_RegionDilation, &ho_ConnectedRegions);
	SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, "area", "and", 2000000, 9999999);
	SelectShape(ho_SelectedRegions, &ho_SelectedRegions1, "rectangularity", "and",
		0.85, 1);
	//)拟合成标准矩形
	FillUp(ho_SelectedRegions1, &ho_RegionFillUp6);
	ErosionRectangle1(ho_RegionFillUp6, &ho_RegionErosion3, 3, 3);

	//7)
	GenContourRegionXld(ho_RegionErosion3, &ho_Contours, "border");

	ErosionRectangle1(ho_RegionErosion3, &ho_RegionErosion4, 20, 20);

	DilationRectangle1(ho_RegionErosion4, &ho_RegionDilation5, 1, 60);

	//8)
	Difference(ho_RegionDilation5, ho_RegionErosion4, &ho_RegionDifference4);

	OpeningRectangle1(ho_RegionDifference4, &ho_RegionOpening3, 10, 10);

	Connection(ho_RegionOpening3, &ho_ConnectedRegions4);
	DispObj(ho_ConnectedRegions4, hv_WindowHandle);
	SelectShape(ho_ConnectedRegions4, &ho_SelectedRegions4, "area", "and", 500, 99999);

	//9)上下两条边缘
	CountObj(ho_SelectedRegions4, &hv_Number_edge);
	//如果为两条，则判断左右边缘是否为两条。这是为了判断屏幕有没有去到接近图像边缘
	if (0 != (hv_Number_edge == 2))
	{
		DilationRectangle1(ho_RegionErosion4, &ho_RegionDilation6, 60, 1);
		Difference(ho_RegionDilation6, ho_RegionErosion4, &ho_RegionDifference5);
		OpeningRectangle1(ho_RegionDifference5, &ho_RegionOpening4, 10, 10);
		Connection(ho_RegionOpening4, &ho_ConnectedRegions5);
		SelectShape(ho_ConnectedRegions5, &ho_SelectedRegions5, "area", "and", 500, 99999);
		CountObj(ho_SelectedRegions5, &hv_Number_edge);
		if (0 != (hv_Number_edge == 2))
		{
			//)选择上下边缘的第一条边缘
			SelectObj(ho_SelectedRegions4, &ho_ObjectSelected1, 1);
			//1)
			SmallestRectangle1(ho_ObjectSelected1, &hv_Row12, &hv_Column12, &hv_Row21,
				&hv_Column21);
			//2)
			ClipContoursXld(ho_Contours, &ho_ClippedContours, hv_Row12, hv_Column12, hv_Row21,
				hv_Column21);
			//3)
			SelectShapeXld(ho_ClippedContours, &ho_SelectedXLD, "max_diameter", "and",
				150, 99999);

			//)选择左右的第2条边缘
			SelectObj(ho_SelectedRegions5, &ho_ObjectSelected2, 2);
			//1)
			SmallestRectangle1(ho_ObjectSelected2, &hv_Row13, &hv_Column13, &hv_Row22,
				&hv_Column22);
			//2)
			ClipContoursXld(ho_Contours, &ho_ClippedContours1, hv_Row13, hv_Column13, hv_Row22,
				hv_Column22);
			//3)
			SelectShapeXld(ho_ClippedContours1, &ho_SelectedXLD1, "max_diameter", "and",
				150, 99999);

			//)选择上下边缘的第2条边缘
			SelectObj(ho_SelectedRegions4, &ho_ObjectSelected3, 2);
			//1)
			SmallestRectangle1(ho_ObjectSelected3, &hv_Row14, &hv_Column14, &hv_Row23,
				&hv_Column23);
			//2)
			ClipContoursXld(ho_Contours, &ho_ClippedContours2, hv_Row14, hv_Column14, hv_Row23,
				hv_Column23);
			//3)
			SelectShapeXld(ho_ClippedContours2, &ho_SelectedXLD2, "max_diameter", "and",
				150, 99999);

			//)选择左右的第1条边缘
			SelectObj(ho_SelectedRegions5, &ho_ObjectSelected4, 1);
			//1)
			SmallestRectangle1(ho_ObjectSelected4, &hv_Row15, &hv_Column15, &hv_Row24,
				&hv_Column24);
			//2)
			ClipContoursXld(ho_Contours, &ho_ClippedContours3, hv_Row15, hv_Column15, hv_Row24,
				hv_Column24);
			//3)
			SelectShapeXld(ho_ClippedContours3, &ho_SelectedXLD3, "max_diameter", "and",
				150, 99999);

			//）集合
			ConcatObj(ho_SelectedXLD, ho_SelectedXLD1, &ho_ObjectsConcat);
			ConcatObj(ho_ObjectsConcat, ho_SelectedXLD2, &ho_ObjectsConcat);
			ConcatObj(ho_ObjectsConcat, ho_SelectedXLD3, &ho_ObjectsConcat);
			//) 拟合直线，取直线焦点
			FitLineContourXld(ho_ObjectsConcat, "tukey", -1, 0, 5, 2, &hv_RowBegin, &hv_ColBegin,
				&hv_RowEnd, &hv_ColEnd, &hv_Nr, &hv_Nc, &hv_Dist);
			IntersectionLines(HTuple(hv_RowBegin[0]), HTuple(hv_ColBegin[0]), HTuple(hv_RowEnd[0]),
				HTuple(hv_ColEnd[0]), HTuple(hv_RowBegin[1]), HTuple(hv_ColBegin[1]), HTuple(hv_RowEnd[1]),
				HTuple(hv_ColEnd[1]), &hv_Row4, &hv_Column4, &hv_IsOverlapping);
			IntersectionLines(HTuple(hv_RowBegin[1]), HTuple(hv_ColBegin[1]), HTuple(hv_RowEnd[1]),
				HTuple(hv_ColEnd[1]), HTuple(hv_RowBegin[2]), HTuple(hv_ColBegin[2]), HTuple(hv_RowEnd[2]),
				HTuple(hv_ColEnd[2]), &hv_Row5, &hv_Column5, &hv_IsOverlapping1);
			IntersectionLines(HTuple(hv_RowBegin[2]), HTuple(hv_ColBegin[2]), HTuple(hv_RowEnd[2]),
				HTuple(hv_ColEnd[2]), HTuple(hv_RowBegin[3]), HTuple(hv_ColBegin[3]), HTuple(hv_RowEnd[3]),
				HTuple(hv_ColEnd[3]), &hv_Row6, &hv_Column6, &hv_IsOverlapping2);
			IntersectionLines(HTuple(hv_RowBegin[3]), HTuple(hv_ColBegin[3]), HTuple(hv_RowEnd[3]),
				HTuple(hv_ColEnd[3]), HTuple(hv_RowBegin[0]), HTuple(hv_ColBegin[0]), HTuple(hv_RowEnd[0]),
				HTuple(hv_ColEnd[0]), &hv_Row7, &hv_Column7, &hv_IsOverlapping3);
			hv_Row_Concat = HTuple();
			hv_Col_Concat = HTuple();
			TupleConcat(hv_Row4, hv_Row5, &hv_Row_Concat);
			TupleConcat(hv_Row_Concat, hv_Row6, &hv_Row_Concat);
			TupleConcat(hv_Row_Concat, hv_Row7, &hv_Row_Concat);

			TupleConcat(hv_Column4, hv_Column5, &hv_Col_Concat);
			TupleConcat(hv_Col_Concat, hv_Column6, &hv_Col_Concat);
			TupleConcat(hv_Col_Concat, hv_Column7, &hv_Col_Concat);
			//）连接成区域RegionScreen
			GenRegionPolygonFilled(&g_RegionScreen, hv_Row_Concat, hv_Col_Concat);

			//inner_rectangle1 (Region6, Row16, Column16, Row25, Column25)
			//gen_rectangle1 (Rectangle2, Row16, Column16, Row25, Column25)
			//) 划分中心区域，亮度均匀
			ErosionRectangle1(g_RegionScreen, &ho_RegionCenter, 20, 20);
			//）划分边缘区域，亮度比较不均匀，需特殊处理
			Difference(g_RegionScreen, ho_RegionCenter, &ho_RegionMargin);

			//计算4X4灰度值，保存到文件中
			HObject ho_RegionRects;
			HObject ho_ImageGrayMean;
			HTuple hv_Mean1, hv_Deviation;
			CIni iniFile;
			char pT[1024] = {0};
			cutpart(g_RegionScreen, &ho_RegionRects, 4, 4); //分割成4乘4
			MeanImage(ho_ImageB, &ho_ImageGrayMean, 5, 5);   //均值滤波
			Intensity(ho_RegionRects, ho_ImageGrayMean, &hv_Mean1, &hv_Deviation); //计算灰度值
																				   //写入文件保存
			int count = hv_Mean1.Length();
			int cnt = 1;
			CString str;
			for (int irow = 3; irow >= 0; irow--)
			{
				for (int icol = 3; icol >= 0; icol--)
				{
					if (cnt > count) break;
					str.Format(_T("Gray[%d,%d]"), irow, icol);
					iniFile.SetValue(NODE_SCREENREGION_RES, str, hv_Mean1[cnt - 1].D());
					cnt++;
				}
			}
			//保存失败的图片
			HObject ho_ImageResult;
			HTuple hv_gray = 255;
			hv_gray.Append(0);
			hv_gray.Append(0);
			PaintRegion(g_RegionScreen, ho_Image, &ho_ImageResult, hv_gray, "margin");
			WriteImage(ho_ImageResult, "bmp", 0, SCREENREGION_RESULT_IMG);
			iniFile.SetValue(NODE_SCREENREGION_RES, SUBNODE_FAILPATH, SCREENREGION_RESULT_IMG);		//
			iniFile.SetValue(NODE_SCREENREGION_RES, SUBNODE_RES, RESULT_PASS);   //
			iniFile.Write(SCREENREGION_RESULT_FILE);
			//return true;
		}
	}
	if (bflag)
	{
		DispObj(ho_Image, hv_WindowHandle);
		//DispObj(ho_ConnectedRegions, hv_WindowHandle);
		DispObj(g_RegionScreen, hv_WindowHandle);
	}
	return true;
}

//背光即白屏检测灰度和黑色缺陷，必须先调用screenRegion 获取屏幕区域
//ho_Image 白屏图像
//bflag    是否显示图像
bool whiteScreen(HObject  ho_Image, bool bflag = true)
{
	HObject ho_ImageR, ho_ImageB, ho_ImageG;
	// Local iconic variables
	HObject  ho_ImageCenter, ho_Region, ho_ImagePlane, ho_ImageBack;
	HObject  ho_ImageSub, ho_ImageMedian, ho_RegionHysteresis;
	HObject  ho_ImageMargin, ho_ImageMean0, ho_RegionDynThresh0;
	HObject  ho_RegionUnion, ho_ImageBlackRaw, ho_ImageBlackR;
	HObject  ho_ImageBlackB, ho_ImageBlackG, ho_ImageBlackCenter;
	HObject  ho_RegionBlackDefect, ho_Regions1, ho_ImageBlackMargin;
	HObject  ho_ImageBlackMean0, ho_RegionBlackDynThresh0, ho_RegionBlackClosing;
	HObject  ho_ImageGrayRaw, ho_ImageGrayRawTest, ho_GrayImage;
	HObject  ho_GrayImageTest, ho_ROI_0, ho_ImageGrayReduced;
	HObject  ho_ImageGrayReducedTest, ho_ROI_1, ho_ROI, ho_ROI_2;
	HObject  ho_ROI_3, ho_ROI_7, ho_ROI_6, ho_ROI_5, ho_ROI_4;
	HObject  ho_ROI_8, ho_ROI_9, ho_ROI_10, ho_ROI_11, ho_ROI_12;
	HObject  ho_ROI_13, ho_ROI_14, ho_ROI_15, ho_ImageGrayMean;
	HObject  ho_ImageGrayMeanTest, ho_ObjectSelected, ho_ROI_22;
	HObject  ho_ROI_33, ho_ROI_44, ho_ROI_55, ho_ROI_66, ho_ROI_77;
	HObject  ho_ROI_88, ho_ROIROI, ho_Cross, ho_Cross2, ho_RegionLines1;
	HObject  ho_GridRegionAll, ho_RowLines, ho_ColLines, ho_GridRegion;
	HObject  ho_RegionLines, ho_RegionUnion2, ho_xldLine, ho_GridRegion2;
	HObject  ho_ConnectedRegionUnion, ho_SelectedRegionUnion, ho_RegionRects;
	// Local control variables
	HTuple  hv_Number_Blob;
	HTuple  hv_UsedThreshold, hv_Width, hv_Height, hv_MRow;
	HTuple  hv_MCol, hv_Alpha, hv_Beta, hv_Mean, hv_UsedThreshold1;
	HTuple  hv_Mean1, hv_Deviation, hv_len1, hv_d, hv_GrayRangesMax;
	HTuple  hv_deltaRanges, hv_deltaRangesMax, hv_deltaRangesMin;
	HTuple  hv_Index, hv_mid, hv_delta, hv_deltamax, hv_deltamin;
	HTuple  hv_Mean2, hv_Deviation2, hv_RangeDefectIndex, hv_DeltaDefectIndex;
	HTuple  hv_Index1, hv_Index2, hv_LeftX1, hv_LeftX2, hv_LeftX3;
	HTuple  hv_LeftX4, hv_LeftX11, hv_LeftX22, hv_LeftX33, hv_LeftX44;
	HTuple  hv_TopY1, hv_TopY2, hv_TopY3, hv_TopY4, hv_TopY11;
	HTuple  hv_TopY22, hv_TopY33, hv_TopY44, hv_Line, hv_LineLine;
	HTuple  hv_EdgeLineRow, hv_EdgeLineRow2, hv_EdgeLineColCol;
	HTuple  hv_EdgeLineColCol2, hv_totallen1, hv_totallen2;
	HTuple  hv_totallen3, hv_totallen4, hv_totallen5, hv_totallen6;
	HTuple  hv_totallen7, hv_totallen8, hv_gridw1, hv_gridw2;
	HTuple  hv_gridw3, hv_gridw4, hv_gridw11, hv_gridw22, hv_gridw33;
	HTuple  hv_gridw44, hv_gridw5, hv_gridw6, hv_gridw7, hv_gridw8;
	HTuple  hv_gridw55, hv_gridw66, hv_gridw77, hv_gridw88;
	HTuple  hv_minheight, hv_gridw, hv_LeftX, hv_coorRow, hv_coorCol;
	HTuple  hv_coorRow2, hv_coorCol2, hv_dir, hv_Index4, hv_LeftXX;
	HTuple  hv_HorProjection, hv_VertProjection, hv_HorProjection2;
	HTuple  hv_VertProjection2, hv_Projection, hv_Projection2;
	HTuple  hv_IndexEdge, hv_slip, hv_max_delta, hv_deltaGray;
	HTuple  hv_lowLim, hv_highLim, hv_VertProjectionSelected;
	HTuple  hv_VertProjectionSelected2, hv_Length, hv_Indices1;
	HTuple  hv_peak_indx, hv_VertProjectionSelected3, hv_VertProjectionSelected4;
	HTuple  hv_max_delta2, hv_slip2, hv_Index5, hv_Index3, hv_Number;

	HTuple  hv_Diffidx;
	CIni iniFile;
	CString strIniFileName;
	SYSTEMTIME curT;
	HObject ho_RegionUnion3;
	char pT[1024];
	//）2018.08.24 开始识别屏幕区域，用于白屏，黑屏，十六方格的划分
	//) 2018.08.28 增加识别16方格的界限，自动分割和判断，已完成横向分割，需要优化
	//) 2018.08.28 开始增加垂直方向的界限查找
	//) 2018.08.29 开始切分为16块, 结果： 只能割出9块，与边缘相接的不能分割出
	//) 2018.08.29

	try
	{
		if (0)
		{
			// 计算4X4方格内的灰度值，并和标准值比较， 如果灰度已经异常，则不进行缺陷检测
			HalconCpp::ReduceDomain(ho_Image, g_RegionScreen, &ho_ImageGrayReducedTest);
			cutpart(g_RegionScreen, &ho_RegionRects, 4, 4); //将屏幕区域分割成4乘4
			HalconCpp::MeanImage(ho_ImageGrayReducedTest, &ho_ImageGrayMean, 5, 5);  //平滑
			Intensity(ho_RegionRects, ho_ImageGrayMean, &hv_Mean1, &hv_Deviation); //计算灰度值															   //与标准值比较
			compareGray(hv_Mean1, hv_Whitestd, hv_Whitemax, hv_Whitemin, 1, &hv_Diffidx);
			//写入文件保存
			int count = hv_Mean1.Length();
			int cnt = 1;
			CString str;
			double temp;
			for (int irow = 3; irow >= 0; irow--)
			{
				for (int icol = 3; icol >= 0; icol--)
				{
					if (cnt > count) break;
					str.Format(_T("Gray[%d,%d]"), irow, icol);
					iniFile.SetValue(NODE_WHITESCREEN_RES, str, hv_Mean1[cnt - 1].D());
					cnt++;
				}
			}

			iniFile.SetValue(NODE_WHITESCREEN_RES, SUBNODE_COUNT, hv_Diffidx.Length());  //
			if (hv_Diffidx.Length() > 0)
			{
				//判断标准值
				HObject ho_ObjectSelected1;
				for (int i = 0; i < hv_Diffidx.Length(); i++)
				{
					str.Format(_T("GrayIndex%d"), i);
					iniFile.SetValue(NODE_WHITESCREEN_RES, str, hv_Diffidx[i].I());

					//画到图像上
					HObject ho_ImageResult;
					HTuple hv_gray = 255;
					hv_gray.Append(0);
					hv_gray.Append(0);
					SelectObj(ho_RegionRects, &ho_ObjectSelected1, hv_Diffidx[i] + 1);
					PaintRegion(ho_ObjectSelected1, ho_Image, &ho_Image, hv_gray, "margin");
				}
				GetLocalTime(&curT);
				sprintf(pT, WHITESCREEN_RESULT_IMG, curT.wMonth, curT.wDay, curT.wHour, curT.wMinute, curT.wSecond);
				WriteImage(ho_Image, "bmp", 0, (HTuple)pT);
				iniFile.SetValue(NODE_WHITESCREEN_RES, SUBNODE_FAILPATH, pT);		//

				iniFile.SetValue(NODE_WHITESCREEN_RES, SUBNODE_MSG, RESULT_MSG_FAIL_GRAY);   //
				iniFile.SetValue(NODE_WHITESCREEN_RES, SUBNODE_RES, RESULT_FAIL);   //

				iniFile.Write(WHITESCREEN_RESULT_FILE);
				//return true; //直接返回，不检测缺陷
			}
		}
		

		//） 二，检测中心区域缺陷
		//1). 背景建模
		HalconCpp::Decompose3(ho_Image, &ho_ImageR, &ho_ImageB, &ho_ImageG);
		HalconCpp::ReduceDomain(ho_ImageB, ho_RegionCenter, &ho_ImageCenter);
		HalconCpp::BinaryThreshold(ho_ImageCenter, &ho_Region, "smooth_histo", "dark", &hv_UsedThreshold);
		HalconCpp::GetImageSize(ho_ImageB, &hv_Width, &hv_Height);
		//Calculate gray value moments and approximation by a plane
		MomentsGrayPlane(ho_RegionCenter, ho_ImageCenter, &hv_MRow, &hv_MCol, &hv_Alpha,
			&hv_Beta, &hv_Mean);
		//Create a gray value ramp
		GenImageGrayRamp(&ho_ImagePlane, hv_Alpha, hv_Beta, hv_Mean, hv_MRow, hv_MCol,
			hv_Width, hv_Height);
		ReduceDomain(ho_ImagePlane, ho_RegionCenter, &ho_ImageBack);
		SubImage(ho_ImageBack, ho_ImageCenter, &ho_ImageSub, 1, 0);
		//dyn_threshold (ImageCenter, ImageBack, RegionDynThresh, 35, 'dark')
		MedianImage(ho_ImageSub, &ho_ImageMedian, "square", 3, "mirrored");
		HysteresisThreshold(ho_ImageMedian, &ho_RegionHysteresis, 10, 30, 50);

		//)  三，检测边缘区域的缺陷
		ReduceDomain(ho_ImageB, ho_RegionMargin, &ho_ImageMargin);
		MeanImage(ho_ImageMargin, &ho_ImageMean0, 3, 3);
		DynThreshold(ho_ImageMargin, ho_ImageMean0, &ho_RegionDynThresh0, 15, "dark");

		//)  四，整合
		Union2(ho_RegionHysteresis, ho_RegionDynThresh0, &ho_RegionUnion);

		//)  五，计数并保存结果

		Connection(ho_RegionUnion, &ho_ConnectedRegionUnion);
		SelectShape(ho_ConnectedRegionUnion, &ho_SelectedRegionUnion, "area", "and", 10, 99999);
		CountObj(ho_SelectedRegionUnion, &hv_Number_Blob);
		Union1(ho_SelectedRegionUnion, &ho_RegionUnion3);
		DilationCircle(ho_RegionUnion3, &ho_RegionUnion3, 5);
		if (0 != (hv_Number_Blob < 1)) //找不到
		{
			iniFile.SetValue(NODE_WHITESCREEN_RES, SUBNODE_MSG, RESULT_PASS);  //
			iniFile.SetValue(NODE_WHITESCREEN_RES, SUBNODE_COUNT, 0);  //
			iniFile.SetValue(NODE_WHITESCREEN_RES, SUBNODE_RES, RESULT_PASS);  //
		}
		else
		{
			//保存失败的图片
			HObject ho_ImageResult;
			HTuple hv_gray = 255;
			hv_gray.Append(0);
			hv_gray.Append(0);
			PaintRegion(ho_RegionUnion, ho_Image, &ho_ImageResult, hv_gray, "margin");
			GetLocalTime(&curT);
			sprintf(pT, WHITESCREEN_RESULT_IMG, curT.wMonth, curT.wDay, curT.wHour, curT.wMinute, curT.wSecond);
			WriteImage(ho_ImageResult, "bmp", 0, (HTuple)pT);

			int count = hv_Number_Blob.L();
			iniFile.SetValue(NODE_WHITESCREEN_RES, SUBNODE_MSG, RESULT_MSG_FAIL_DEFECT);  //
			iniFile.SetValue(NODE_WHITESCREEN_RES, SUBNODE_FAILPATH, pT);		//
			CString str;
			HObject ho_ObjectUnionSelected;
			HTuple hArea, hRow, hColumn;
			for (int i = 0; i < count; i++)
			{
				str.Format(_T("Area%d"), i);
				SelectObj(ho_SelectedRegionUnion, &ho_ObjectUnionSelected, i + 1);
				AreaCenter(ho_ObjectUnionSelected, &hArea, &hRow, &hColumn);
				iniFile.SetValue(NODE_WHITESCREEN_RES, str, hArea.I());  //面积
			}
			iniFile.SetValue(NODE_WHITESCREEN_RES, SUBNODE_COUNT, count);  //
			iniFile.SetValue(NODE_WHITESCREEN_RES, SUBNODE_RES, RESULT_FAIL);  //
		}
		iniFile.Write(WHITESCREEN_RESULT_FILE);
		if (bflag)
		{
			/*if (HDevWindowStack::IsOpen())
				CloseWindow(HDevWindowStack::Pop());
			SetWindowAttr("background_color", "black");
			OpenWindow(0, 0, 800, 600, 0, "", "", &hv_WindowHandle);
			SetColor(hv_WindowHandle, "red");
			SetDraw(hv_WindowHandle, "margin");
			HDevWindowStack::Push(hv_WindowHandle);*/

			DispObj(ho_Image, hv_WindowHandle);
			DispObj(ho_SelectedRegionUnion, hv_WindowHandle);
		}
	}
	catch (HalconCpp::HException & except)
	{
		ShowException(except);
		return false;
	}

	return true;
}
bool blackScreen(HObject ho_Image, bool bflag = true)
{
	HObject  ho_ImageBlackR, ho_RegionUnion;
	HObject  ho_ImageBlackB, ho_ImageBlackG, ho_ImageBlackCenter;
	HObject  ho_RegionBlackDefect, ho_Regions1, ho_ImageBlackMargin;
	HObject  ho_ImageBlackMean0, ho_RegionBlackDynThresh0, ho_RegionBlackClosing, ho_SelectedRegionUnion, ho_RegionBlackUnion, ho_RegionConnected;
	HTuple   hv_UsedThreshold1, hv_Number_Blob;
	CIni iniFile;
	SYSTEMTIME curT;
	char pT[1024];
	try
	{
		//)检测黑屏 ======================================================
		//ReadImage(&ho_ImageBlackRaw, "E:/屏幕检测/电子书/pic/blackFAKE_NG.jpg");
		Decompose3(ho_Image, &ho_ImageBlackR, &ho_ImageBlackB, &ho_ImageBlackG);
		//)中心区域
		ReduceDomain(ho_ImageBlackB, ho_RegionCenter, &ho_ImageBlackCenter);
		BinaryThreshold(ho_ImageBlackCenter, &ho_RegionBlackDefect, "max_separability",
			"dark", &hv_UsedThreshold1);
		Threshold(ho_ImageBlackCenter, &ho_Regions1, hv_UsedThreshold1, 255);
		//)边缘区域
		ReduceDomain(ho_ImageBlackB, ho_RegionMargin, &ho_ImageBlackMargin);
		MeanImage(ho_ImageBlackMargin, &ho_ImageBlackMean0, 3, 3);
		DynThreshold(ho_ImageBlackMargin, ho_ImageBlackMean0, &ho_RegionBlackDynThresh0,
			10, "light");
		ClosingRectangle1(ho_RegionBlackDynThresh0, &ho_RegionBlackClosing, 3, 3);
		//)  四，整合
		Union2(ho_RegionBlackClosing, ho_Regions1, &ho_RegionBlackUnion);
		//)  五，计数并保存结果
		Connection(ho_RegionBlackUnion, &ho_RegionConnected);
		SelectShape(ho_RegionConnected, &ho_SelectedRegionUnion, "area", "and", 1, 99999);
		CountObj(ho_SelectedRegionUnion, &hv_Number_Blob);
		Union1(ho_SelectedRegionUnion, &ho_RegionUnion);
		DilationCircle(ho_RegionUnion, &ho_RegionUnion, 5);
		if (0 != (hv_Number_Blob<1)) //找不到缺陷
		{
			iniFile.SetValue(NODE_BLACKSCREEN_RES, SUBNODE_MSG, RESULT_PASS);  //
			iniFile.SetValue(NODE_BLACKSCREEN_RES, SUBNODE_RES, RESULT_PASS);  //
		}
		else
		{
			//保存失败的图片
			GetLocalTime(&curT);
			HObject ho_ImageResult;
			HTuple hv_gray = 255;
			hv_gray.Append(0);
			hv_gray.Append(0);
			PaintRegion(ho_RegionUnion, ho_Image, &ho_ImageResult, hv_gray, "margin");
			sprintf(pT, BLACKSCREEN_RESULT_IMG, curT.wMonth, curT.wDay, curT.wHour, curT.wMinute, curT.wSecond);
			WriteImage(ho_ImageResult, "bmp", 0, (HTuple)pT);
			//保存文档
			int count = hv_Number_Blob.L();
			iniFile.SetValue(NODE_BLACKSCREEN_RES, SUBNODE_MSG, RESULT_FAIL);   //
			iniFile.SetValue(NODE_BLACKSCREEN_RES, SUBNODE_FAILPATH, pT);		//
			CString str;
			HObject ho_ObjectUnionSelected;
			HTuple hArea, hRow, hColumn;
			for (int i = 0; i < count; i++)
			{
				str.Format(_T("Area%d"), i);
				SelectObj(ho_SelectedRegionUnion, &ho_ObjectUnionSelected, i + 1);
				AreaCenter(ho_ObjectUnionSelected, &hArea, &hRow, &hColumn);
				iniFile.SetValue(NODE_BLACKSCREEN_RES, str, hArea.I());  //面积
			}
			iniFile.SetValue(NODE_BLACKSCREEN_RES, SUBNODE_COUNT, count);  //
			iniFile.SetValue(NODE_BLACKSCREEN_RES, SUBNODE_RES, RESULT_FAIL);  //
		}
		iniFile.Write(BLACKSCREEN_RESULT_FILE);
	
		if (bflag)
		{
			/*if (HDevWindowStack::IsOpen())
				CloseWindow(HDevWindowStack::Pop());
			SetWindowAttr("background_color", "black");
			OpenWindow(0, 0, 1200, 1000, 0, "", "", &hv_WindowHandle);
			SetColor(hv_WindowHandle, "red");
			SetDraw(hv_WindowHandle, "margin");
			HDevWindowStack::Push(hv_WindowHandle);*/

			DispObj(ho_Image, hv_WindowHandle);
			DispObj(ho_SelectedRegionUnion, hv_WindowHandle);
		}
	}
	catch (HalconCpp::HException & except)
	{
		ShowException(except);
		return false;
	}
	return true;
}

bool loadConfig(char* pPath)
{
	CIni iniFile;
	if (!iniFile.Read(pPath))
	{
		return false;
	}
	
	if (!iniFile.GetValue(NODE_CAM_CONFIG, "Name", m_strCamName)) //最大值
	{
		return false;
	}
	if (!iniFile.GetValue(NODE_CAM_CONFIG, "IniFile", m_strCamIniFile)) //最大值
	{
		return false;
	}
	return true;
}



//加载灰阶测试标准
bool loadGrayStd(char* pPath)
{
	CIni iniFile;
	if (!iniFile.Read(pPath))
	{
		return false;
	}
	hv_Graystd.Clear();
	hv_Graymax.Clear();
	hv_Graymin.Clear();
	CString str, strmax, strmin;
	int cnt = 1;
	for (int irow = 3; irow >= 0; irow--)
	{
		for (int icol = 3; icol >= 0; icol--)
		{
			str.Format(_T("Gray[%d,%d]"), irow, icol);
			strmax.Format(_T("GrayMax[%d,%d]"), irow, icol);
			strmin.Format(_T("GrayMin[%d,%d]"), irow, icol);
			double gray = 0;
			if (!iniFile.GetValue(NODE_GRAYSCALE_CONFIG, str, gray)) //标准值
			{
				return false;
			}
			hv_Graystd[cnt - 1] = gray;
			if (!iniFile.GetValue(NODE_GRAYSCALE_CONFIG, strmax, gray)) //最大值
			{
				return false;
			}
			hv_Graymax[cnt - 1] = gray;
			if (!iniFile.GetValue(NODE_GRAYSCALE_CONFIG, strmin, gray)) //最小值
			{
				return false;
			}
			hv_Graymin[cnt - 1] = gray;
			cnt++;
		}
	}
	return true;
}

//加载背光白屏测试标准
bool loadWhiteStd(char* pPath)
{
	CIni iniFile;
	if (!iniFile.Read(pPath))
	{
		return false;
	}
	hv_Whitestd.Clear();
	hv_Whitemax.Clear();
	hv_Whitemin.Clear();
	CString str, strmax, strmin;
	int cnt = 1;
	for (int irow = 3; irow >= 0; irow--)
	{
		for (int icol = 3; icol >= 0; icol--)
		{
			str.Format(_T("Gray[%d,%d]"), irow, icol);
			strmax.Format(_T("GrayMax[%d,%d]"), irow, icol);
			strmin.Format(_T("GrayMin[%d,%d]"), irow, icol);
			double gray = 0.0;
			if (!iniFile.GetValue(NODE_WHITESCREEN_CONFIG, str, gray)) //标准值
			{
				return false;
			}
			hv_Whitestd[cnt - 1] = gray;
			if (!iniFile.GetValue(NODE_WHITESCREEN_CONFIG, strmax, gray)) //最大值,或正公差
			{
				return false;
			}
			hv_Whitemax[cnt - 1] = gray;
			if (!iniFile.GetValue(NODE_WHITESCREEN_CONFIG, strmin, gray)) //最小值,负公差
			{
				return false;
			}
			hv_Whitemin[cnt - 1] = gray;
			cnt++;
		}
	}
	return true;
}

//加载棋盘格测试标准
bool loadChessStd(char* pPath)
{
	CIni iniFile;
	if (!iniFile.Read(pPath))
	{
		return false;
	}
	hv_Chessstd.Clear();
	hv_Chessmax.Clear();
	hv_Chessmin.Clear();
	CString str, strmax, strmin;
	int cnt = 1;
	for (int irow = 3; irow >= 0; irow--)
	{
		for (int icol = 3; icol >= 0; icol--)
		{
			str.Format(_T("Gray[%d,%d]"), irow, icol);
			strmax.Format(_T("GrayMax[%d,%d]"), irow, icol);
			strmin.Format(_T("GrayMin[%d,%d]"), irow, icol);
			double gray = 0.0;
			if (!iniFile.GetValue(NODE_CHESSBOARD_CONFIG, str, gray)) //标准值
			{
				return false;
			}
			hv_Chessstd[cnt - 1] = gray;
			if (!iniFile.GetValue(NODE_CHESSBOARD_CONFIG, strmax, gray)) //最大值,或正公差
			{
				return false;
			}
			hv_Chessmax[cnt - 1] = gray;
			if (!iniFile.GetValue(NODE_CHESSBOARD_CONFIG, strmin, gray)) //最小值,负公差
			{
				return false;
			}
			hv_Chessmin[cnt - 1] = gray;
			cnt++;
		}
	}
	return true;
}

//灰阶测试
bool grayScale(HObject ho_Image, bool bflag = true)
{
	SYSTEMTIME curT;
	char pT[1024];
	CIni iniFile;
	HObject  ho_ImageGrayMean;
	HObject  ho_ImageGrayReducedTest, ho_RegionRects;
	HTuple	hv_Mean1, hv_Deviation, hv_Diffidx;
	
	try
	{
		ReduceDomain(ho_Image, g_RegionScreen, &ho_ImageGrayReducedTest);
		cutpart(g_RegionScreen, &ho_RegionRects, 4, 4); //分割成4乘4
		MeanImage(ho_ImageGrayReducedTest, &ho_ImageGrayMean, 5, 5);
		Intensity(ho_RegionRects, ho_ImageGrayMean, &hv_Mean1, &hv_Deviation); //计算灰度值

																			   //与标准值比较
		compareGray(hv_Mean1, hv_Graystd, hv_Graymax, hv_Graymin, 1, &hv_Diffidx);

		//写入文件保存
		int count = hv_Mean1.Length();
		int cnt = 1;
		CString str;
		double temp;
		for (int irow = 3; irow >= 0; irow--)
		{
			for (int icol = 3; icol >= 0; icol--)
			{
				if (cnt > count) break;
				str.Format(_T("Gray[%d,%d]"), irow, icol);
				iniFile.SetValue(NODE_GRAYSCALE_RES, str, hv_Mean1[cnt-1].D());
				cnt++;
			}
		}
		HObject ho_ObjectSelected1, ho_ObjectSelected2;
		for (int i = 0; i < hv_Diffidx.Length(); i++)
		{
			str.Format(_T("GrayIndex%d"), i);
			iniFile.SetValue(NODE_GRAYSCALE_RES, str, hv_Diffidx[i].I());

			//画到图像上

			HObject ho_ImageResult;
			HTuple hv_gray = 255;
			hv_gray.Append(0);
			hv_gray.Append(0);
			SelectObj(ho_RegionRects, &ho_ObjectSelected1, hv_Diffidx[i] + 1);
			PaintRegion(ho_ObjectSelected1, ho_Image, &ho_Image, hv_gray, "fill");
		}
		GetLocalTime(&curT);
		sprintf(pT, GRAYSCALE_RESULT_IMG, curT.wMonth, curT.wDay, curT.wHour, curT.wMinute, curT.wSecond);
		WriteImage(ho_Image, "bmp", 0, (HTuple)pT);
		iniFile.SetValue(NODE_GRAYSCALE_RES, SUBNODE_FAILPATH, pT);		//

		iniFile.SetValue(NODE_GRAYSCALE_RES, SUBNODE_COUNT, hv_Diffidx.Length());  //
		if (hv_Diffidx.Length() > 0)
		{
			iniFile.SetValue(NODE_GRAYSCALE_RES, SUBNODE_MSG, RESULT_FAIL);   //
			iniFile.SetValue(NODE_GRAYSCALE_RES, SUBNODE_RES, RESULT_FAIL);   //
		}
		else
		{
			iniFile.SetValue(NODE_GRAYSCALE_RES, SUBNODE_MSG, RESULT_PASS);   //
			iniFile.SetValue(NODE_GRAYSCALE_RES, SUBNODE_RES, RESULT_PASS);   //
		}
		iniFile.Write(GRAYSCALE_RESULT_FILE);  

		if (bflag)
		{
			//if (HDevWindowStack::IsOpen())
			//	CloseWindow(HDevWindowStack::Pop());
			//SetWindowAttr("background_color", "black");
			//OpenWindow(0, 0, 800, 600, 0, "", "", &hv_WindowHandle);
			//SetColor(hv_WindowHandle, "red");
			//SetDraw(hv_WindowHandle, "margin");
			//HDevWindowStack::Push(hv_WindowHandle);

			DispObj(ho_Image, hv_WindowHandle);
			//DispObj(ho_RegionRects, hv_WindowHandle);
		}
	}
	catch (HalconCpp::HException & except)
	{
		ShowException(except);
		return false;
	}
	return true;
}

bool chessBoard(HObject ho_Image, bool bflag = true)
{
	SYSTEMTIME curT;
	char pT[1024];
	CIni iniFile;
	HObject  ho_ImageGrayMean;
	HObject  ho_ImageGrayReducedTest, ho_RegionRects;
	HTuple	hv_Mean1, hv_Deviation, hv_Diffidx;
	
	try
	{
		ReduceDomain(ho_Image, g_RegionScreen, &ho_ImageGrayReducedTest);
		cutpart(g_RegionScreen, &ho_RegionRects, 4, 4); //分割成4乘4
		MeanImage(ho_ImageGrayReducedTest, &ho_ImageGrayMean, 5, 5);
		Intensity(ho_RegionRects, ho_ImageGrayMean, &hv_Mean1, &hv_Deviation); //计算灰度值
		compareGray(hv_Mean1, hv_Chessstd, hv_Chessmax, hv_Chessmin, 1, &hv_Diffidx);//与标准值比较

		//写入文件保存
		int count = hv_Mean1.Length();
		int cnt = 1;
		CString str;
		double temp;
		for (int irow = 3; irow >= 0; irow--)
		{
			for (int icol = 3; icol >= 0; icol--)
			{
				if (cnt > count) break;
				str.Format(_T("Gray[%d,%d]"), irow, icol);
				iniFile.SetValue(NODE_CHESSBOARD_RES, str, hv_Mean1[cnt - 1].D());
				cnt++;
			}
		}
		HObject ho_ObjectSelected1;
		for (int i = 0; i < hv_Diffidx.Length(); i++)
		{
			str.Format(_T("GrayIndex%d"), i);
			iniFile.SetValue(NODE_CHESSBOARD_RES, str, hv_Diffidx[i].I());

			//画到图像上

			HObject ho_ImageResult;
			HTuple hv_gray = 255;
			hv_gray.Append(0);
			hv_gray.Append(0);
			SelectObj(ho_RegionRects, &ho_ObjectSelected1, hv_Diffidx[i] + 1);
			PaintRegion(ho_ObjectSelected1, ho_Image, &ho_Image, hv_gray, "fill");
		}
		GetLocalTime(&curT);
		sprintf(pT, CHESSBOARD_RESULT_IMG, curT.wMonth, curT.wDay, curT.wHour, curT.wMinute, curT.wSecond);
		WriteImage(ho_Image, "bmp", 0, (HTuple)pT);
		iniFile.SetValue(NODE_CHESSBOARD_RES, SUBNODE_FAILPATH, pT);		//

		iniFile.SetValue(NODE_CHESSBOARD_RES, SUBNODE_COUNT, hv_Diffidx.Length());  //
		if (hv_Diffidx.Length() > 0)
		{
			iniFile.SetValue(NODE_CHESSBOARD_RES, SUBNODE_MSG, RESULT_FAIL);   //
			iniFile.SetValue(NODE_CHESSBOARD_RES, SUBNODE_RES, RESULT_FAIL);   //
		}
		else
		{
			iniFile.SetValue(NODE_CHESSBOARD_RES, SUBNODE_MSG, RESULT_PASS);   //
			iniFile.SetValue(NODE_CHESSBOARD_RES, SUBNODE_RES, RESULT_PASS);   //
		}

		iniFile.Write(CHESSBOARD_RESULT_FILE);

		if (bflag)
		{
			//if (HDevWindowStack::IsOpen())
			//	CloseWindow(HDevWindowStack::Pop());
			//SetWindowAttr("background_color", "black");
			//OpenWindow(0, 0, 800, 600, 0, "", "", &hv_WindowHandle);
			//SetColor(hv_WindowHandle, "red");
			//SetDraw(hv_WindowHandle, "margin");
			//HDevWindowStack::Push(hv_WindowHandle);

			DispObj(ho_Image, hv_WindowHandle);
			//DispObj(ho_RegionRects, hv_WindowHandle);
		}
	}
	catch (HalconCpp::HException & except)
	{
		ShowException(except);
		return false;
	}
	return true;
}

bool getImageFromCam(HObject& hImage)
{
	if (hv_AcqHandle.Length())
	{
		try
		{
			GrabImage(&hImage, hv_AcqHandle);
			WriteImage(hImage, "bmp", 0, "GrabImage");
		}
		catch (HalconCpp::HException & except)
		{
			return false;
		}
	}
	else
	{
		return false;
	}
		
	return true;
}
//设置结果显示窗口， 
bool EslSetResWnd(int SrcreenType, CWnd* pWnd)
{
	try
	{
		HTuple wndHandle;
		//窗口
		CRect Rect;
		pWnd->GetWindowRect(&Rect);
		if (wndHandle.Length())
			CloseWindow(wndHandle);
		OpenWindow(0, 0, Rect.Width(), Rect.Height(), (Hlong)(pWnd->m_hWnd), "visible", "", &wndHandle);
		SetDraw(wndHandle, "margin");
		SetColor(wndHandle, "green");
		SetFont(wndHandle, "-Courier New-28-");
		HalconCpp::SetSystem("tsp_width", IMG_W);
		HalconCpp::SetSystem("tsp_height", IMG_H);
		HalconCpp::SetPart(wndHandle, 0, 0, IMG_H - 1, IMG_W - 1);

		switch (SrcreenType)
		{
		case SCREEN_WHITE:
			hv_WindowHandle_White = wndHandle;
			break;
		case SCREEN_RED:
			hv_WindowHandle_Red = wndHandle;
			break;
		case SCREEN_GREEN:
			hv_WindowHandle_Green = wndHandle;
			break;
		case SCREEN_BLUE:
			hv_WindowHandle_Blue = wndHandle;
			break;
		case SCREEN_BLACK:
			hv_WindowHandle_Black = wndHandle;
			break;
		default:
			break;
		}
	}
	catch (HalconCpp::HException & except)
	{
		ShowException(except);
		return false;
	}
	return true;
}

//软件使用期限
int CheckKey()
{
	int ret = -2;
	HMODULE dllHandle = NULL;
	dllHandle = LoadLibrary("yance0.dll");
	if (dllHandle == NULL)
	{
		AfxMessageBox("LOAD dll FAIL");
		return ret;
	}

	//加载dll初始化函数
	pCheckme = (CheckMe)::GetProcAddress(dllHandle, "check");
	if (pCheckme == NULL) {
		AfxMessageBox("function  load failed!\n");
		FreeLibrary(dllHandle);
		return ret;
	}
	pRegistme = (RegistMe)::GetProcAddress(dllHandle, "registerMe");
	if (pRegistme == NULL) {
		AfxMessageBox("function  load failed!\n");
		FreeLibrary(dllHandle);
		return ret;
	}
	int re = pCheckme();
	switch (re)
	{
	case 0:
		//AfxMessageBox("successed"); 
		break;
	case 1:
		break;
	case 2://获取不了当前电脑的MAC
		AfxMessageBox("获取不了当前电脑序列号"); break;
		break;
	case 3:
		AfxMessageBox("当前电脑序列号不匹配"); break;
		break;
	case 4:
		break;
	case 5:
		break;
	case 6://写入新时间发生错误
		AfxMessageBox("软件发生错误"); break;
		break;
	case 7:
		AfxMessageBox("时间不正确"); break;
		break;
	case 8:
		AfxMessageBox("软件已过期"); break;
		break;
	case -1: //没有注册过
		AfxMessageBox("软件未注册"); break;
		//if (pRegistme())
		//{
		//	AfxMessageBox("注册fail");
		//}
		//else
		//{
		//	AfxMessageBox("注册successed");
		//}
		break;
	default:
		break;
	}

	FreeLibrary(dllHandle);
	return re;
}

//初始化dll
bool EslInitDll(CWnd* pWnd)
{
	try
	{
		//if (0 != CheckKey()) return false;
		//加载相机相关
		//窗口
		g_pWnd = pWnd;
		CRect Rect;
		pWnd->GetWindowRect(&Rect);

		if (hv_WindowHandle.Length())
			CloseWindow(hv_WindowHandle);
		OpenWindow(0, 0, Rect.Width(), Rect.Height(), (Hlong)(pWnd->m_hWnd), "visible", "", &hv_WindowHandle);
		SetDraw(hv_WindowHandle, "margin");
		SetColor(hv_WindowHandle, "green");
		SetFont(hv_WindowHandle, "-Courier New-28-");
		HalconCpp::SetSystem("tsp_width", IMG_W);
		HalconCpp::SetSystem("tsp_height", IMG_H);
		HalconCpp::SetPart(hv_WindowHandle, 0, 0, IMG_H - 1, IMG_W - 1);

		HObject ho_Image;
		//ReadImage(&ho_Image, "SrcImg\\white.jpg");
		//DispObj(ho_Image, hv_WindowHandle);
		//加载算法相关
		if (!g_Para_White.LoadPara(PATH_WHITE_SRC_PARA))
		{
			AfxMessageBox("加载白屏参数失败");
		}
		if (!g_Para_Red.LoadPara(PATH_RED_SRC_PARA))
		{
			AfxMessageBox("加载红屏参数失败");
		}
		if (!g_Para_Green.LoadPara(PATH_GREEN_SRC_PARA))
		{
			AfxMessageBox("加载绿屏参数失败");
		}
		if (!g_Para_Blue.LoadPara(PATH_BLUE_SRC_PARA))
		{
			AfxMessageBox("加载蓝屏参数失败");
		}
		if (!g_Para_Black.LoadPara(PATH_BLACK_SRC_PARA))
		{
			AfxMessageBox("加载黑屏参数失败");
		}

		//连接相机
		loadConfig(PATH_CAM_CONFIG);
		char pstr[1024];
		memset(pstr, '\0', 1024);
		sprintf_s(pstr, "%s", m_strCamName);
		//if (hv_AcqHandle.Length())
		//	CloseFramegrabber(hv_AcqHandle);
		//AfxMessageBox(pstr);
		//链接相机
		if (myCam.OpenCamera(pstr) != DVP_STATUS_OK)
			return false;
		else
		{
			char pInifile[1024];
			memset(pInifile, '\0', 1024);
			sprintf_s(pInifile, "%s", m_strCamIniFile);
			myCam.LoadConfig(pInifile);
			myCam.StartCamera(); //开始数据流
		}
		//OpenFramegrabber("GenICamTL", 0, 0, 0, 0, 0, 0, "progressive", -1, "default", -1,
		//	"false", "default", "default"/*pstr*//*"USB2_5M@UE500901474"*/, 0, -1, &hv_AcqHandle);	
	}
	catch (HalconCpp::HException & except)
	{
		ShowException(except);
		return false;
	}
	return true;
}
//反初始化dll
bool EslUnInitDll()
{
	//if(hv_AcqHandle.Length())
	//	CloseFramegrabber(hv_AcqHandle);//关闭相机，关闭
	myCam.StopCamera();
	myCam.CloseCamera();

	if (hv_WindowHandle.Length())
		CloseWindow(hv_WindowHandle);

	if (hv_WindowHandle_White.Length())
		CloseWindow(hv_WindowHandle_White);

	if (hv_WindowHandle_Red.Length())
		CloseWindow(hv_WindowHandle_Red);

	if (hv_WindowHandle_Green.Length())
		CloseWindow(hv_WindowHandle_Green);

	if (hv_WindowHandle_Blue.Length())
		CloseWindow(hv_WindowHandle_Blue);

	if (hv_WindowHandle_Black.Length())
		CloseWindow(hv_WindowHandle_Black);

	return true;
}

bool EslSaveImage(char* path)
{
	try
	{
		if (g_Image.IsInitialized())
		{
			WriteImage(g_Image, "bmp", 0, path);
			return true;
		}
	}
	catch(HalconCpp::HException & except)
	{
		return false;
	}
	return false;
}


//关闭显示窗口
extern "C" _declspec(dllexport) bool closeShowWnd()
{
	if (HDevWindowStack::IsOpen())
		CloseWindow(HDevWindowStack::Pop());
	return true;
}

//
//函数：从路径中加载图片，并是识别出屏幕区域
//参数：pPath 图片路径
//      bflag 是否显示过程图像窗口
//返回值：false 发生异常  true 成功
extern "C" _declspec(dllexport) bool screenRegionFromPath(char* pPath, bool bflag = true)
{
	HObject ho_Image;
	ReadImage(&ho_Image, pPath);
	return screenRegion(ho_Image, bflag);  //提取屏幕区域
}
//函数：从相机中加载图片，并是识别出屏幕区域
//参数：
//      bflag 是否显示过程图像窗口
//返回值：false 发生异常  true 成功
extern "C" _declspec(dllexport) bool screenRegionFromCam(bool bflag = true)
{
	bool ret = false;
	HObject ho_Image;
	//ReadImage(&ho_Image, pPath);
	if(getImageFromCam(ho_Image))
		ret = screenRegion(ho_Image, bflag);//提取屏幕区域
	return ret;
}

//
//函数：从路径中加载图片，查找屏幕区域内的黑色坏点。
//参数：pPath 图片路径
//      bflag 是否显示过程图像窗口
//返回值：false 发生异常  true 成功
extern "C" _declspec(dllexport) bool whiteScreenFromPath(char* pPath, bool bflag = true) 
{
	HObject ho_Image;
	ReadImage(&ho_Image, pPath);
	if (!loadWhiteStd(CONFIG_FILE)) //加载标准值
	{
		return false;
	}
	return whiteScreen(ho_Image, bflag);
}
//函数：从相机加载图片，查找屏幕区域内的黑色坏点。
//参数：
//      bflag 是否显示过程图像窗口
//返回值：false 发生异常  true 成功
extern "C" _declspec(dllexport) bool whiteScreenFromCam(bool bflag = true)
{
	bool ret = false;
	HObject ho_Image;
	//ReadImage(&ho_Image, pPath);
	
	if (!loadWhiteStd(CONFIG_FILE)) //加载标准值
	{
		return false;
	}
	if (getImageFromCam(ho_Image))
		ret = whiteScreen(ho_Image, bflag);
	return ret;
}

//
//函数：从路径中加载图片，查找屏幕区域内的白色坏点。
//参数：pPath 图片路径
//      bflag 是否显示过程图像窗口
//返回值：false 发生异常  true 成功
extern "C" _declspec(dllexport) bool blackScreenFromPath(char* pPath, bool bflag = true)
{
	HObject ho_Image;
	ReadImage(&ho_Image, pPath);
	return blackScreen(ho_Image, bflag);
}
//函数：从相机中加载图片，查找屏幕区域内的白色坏点。
//参数：
//      bflag 是否显示过程图像窗口
//返回值：false 发生异常  true 成功
extern "C" _declspec(dllexport) bool blackScreenFromCam(bool bflag = true)
{
	HObject ho_Image;
	bool ret = false;
	if (getImageFromCam(ho_Image))
		ret =  blackScreen(ho_Image, bflag);
	return ret;
}

//
//函数：从路径中加载图片，计算灰阶，并比较标准，得到异常的灰阶
//参数：pPath 图片路径
//      bflag 是否显示过程图像窗口
//返回值：false 发生异常  true 成功
extern "C" _declspec(dllexport) bool grayScaleFromPath(char* pPath, bool bflag = true)
{
	HObject ho_Image;
	if (!loadGrayStd(CONFIG_FILE))
	{
		return false;
	}
	ReadImage(&ho_Image, pPath);
	return grayScale(ho_Image, bflag);
}
//函数：从路径中加载图片，计算灰阶，并比较标准，得到异常的灰阶
//参数：
//      bflag 是否显示过程图像窗口
//返回值：false 发生异常  true 成功
extern "C" _declspec(dllexport) bool grayScaleFromCam(bool bflag = true)
{
	bool ret = false;
	HObject ho_Image;
	if (!loadGrayStd(CONFIG_FILE))
	{
		return false;
	}
	if (getImageFromCam(ho_Image))
		ret = grayScale(ho_Image, bflag);
	return ret;
}

//
//函数：从路径中加载图片，计算灰阶，并比较标准，得到异常的灰阶
//参数：pPath 图片路径
//      bflag 是否显示过程图像窗口
//返回值：false 发生异常  true 成功
extern "C" _declspec(dllexport) bool chessBoardFromPath(char* pPath, bool bflag = true)
{
	HObject ho_Image;
	if (!loadChessStd(CONFIG_FILE))
	{
		return false;
	}
	ReadImage(&ho_Image, pPath);
	return chessBoard(ho_Image, bflag);
}
//函数：从路径中加载图片，计算灰阶，并比较标准，得到异常的灰阶
//参数：
//      bflag 是否显示过程图像窗口
//返回值：false 发生异常  true 成功
extern "C" _declspec(dllexport) bool chessBoardFromCam(bool bflag = true)
{
	bool ret = false;
	HObject ho_Image;
	if (!loadChessStd(CONFIG_FILE))
	{
		return false;
	}
	if (getImageFromCam(ho_Image))
		ret = chessBoard(ho_Image, bflag);
	return ret;
}


//
//从相机采集一张图像
bool EslGrabOneImage()
{
	bool ret = false;
	//ret = getImageFromCam(g_Image);
	//DispObj(g_Image, hv_WindowHandle);

	if (myCam.GrabHalconImage() != DVP_STATUS_OK)
	{
		ret = false;
	}
	else
	{
		//ReadImage(&g_Image, "Temp.bmp");
		g_Image = myCam.m_image.Clone();
		DispObj(g_Image, hv_WindowHandle);
		ret = true;
	}

	//if (myCam.GrabImageAndSave("Temp.bmp") != DVP_STATUS_OK)
	//{
	//	ret = false;
	//}
	//else
	//{
	//	ReadImage(&g_Image, "Temp.bmp");
	//	//g_Image = myCam.m_image.Clone();
	//	DispObj(g_Image, hv_WindowHandle);
	//	ret = true;
	//}

	return ret;
}
//从文件选择打开图像
bool EslLoadOneImage()
{
	//打开文件选择窗口
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, NULL/*_T("位图文件(*.BMP)|*.BMP|jpg文件(*.jpg)|*.jpg||")*/);
	if (IDOK == dlg.DoModal())
	{
		CString imgpath;
		imgpath.Format(_T("%s"), dlg.GetPathName());
		char pPath[1024];
		sprintf_s(pPath, _T("%s"), imgpath);
		try
		{
			HalconCpp::ReadImage(&g_Image, pPath);
			DispObj(g_Image, hv_WindowHandle);
		}
		catch (HalconCpp::HException & except)
		{
			ShowException(except);
			return false;
		}
		return true;
	}
	return false;
}

//函数: 预处理图像并切分屏幕区域，
//返回值： false 发生异常  true 成功
bool EslFindScreen()
{
	//以下
	g_Result.m_resType = RES_TYPE_SCREEN;  //默认屏幕不完整
	HObject processImage;
	//转为单通道
	Decompose3(g_Image, &g_ImageRed, &g_ImageGreen, &g_ImageBlue);
	Rgb1ToGray(g_Image, &g_ImageGray);

	switch (g_Para.m_screenType) 
	{
	case T_WHITE_SCR: //RGB转灰度
		processImage = g_ImageGray.Clone();
		break;
	case T_RED_SCR:  //分离红色分量
		processImage = g_ImageRed.Clone();
		break;
	case T_GREEN_SCR: //分离绿色分量
		processImage = g_ImageGreen.Clone();
		break;
	case T_BLUE_SCR:  //分离蓝色分量
		processImage = g_ImageBlue.Clone();
		break;
	case T_BLACK_SCR: //RGB转灰度
		processImage = g_ImageGray.Clone();
		break;
	case T_DUST_SCR:  //RGB转灰度
		processImage = g_ImageGray.Clone();
		break;
	default:
		g_Result.m_resType = RES_TYPE_ERROR; //发生错误
		return false;
	}

	//黑色屏幕不用分割屏幕区域，直接返回
	if (g_Para.m_screenType == T_BLACK_SCR)
	{
		g_Result.m_resType = RES_TYPE_OK;
		return true;
	}
	
	//FindScreen(processImage, &g_RegionScreen);
	
	//return true;
	//二值化
	HObject ho_Regions, ho_RegionErosion, ho_RegionDilation, ho_ConnectedRegions, ho_SelectedRegions, ho_SelectedRegions1;

	Threshold(processImage, &ho_Regions, g_Para.m_FScreenBMinGray, g_Para.m_FScreenBMaxGray);
	FillUp(ho_Regions, &ho_Regions);
	ErosionRectangle1(ho_Regions, &ho_RegionErosion, g_Para.m_FScreenBEroseSize, g_Para.m_FScreenBEroseSize);
	DilationRectangle1(ho_RegionErosion, &ho_RegionDilation, g_Para.m_FScreenBEroseSize, g_Para.m_FScreenBEroseSize);
	Connection(ho_RegionDilation, &ho_ConnectedRegions);
	SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, "area", "and", g_Para.m_FScreenBSelectAreaMin, g_Para.m_FScreenBSelectAreaMax);
	SelectShape(ho_SelectedRegions, &ho_SelectedRegions1, "rectangularity", "and",
		0.85, 1);
	//)拟合成标准矩形
	HObject ho_RegionFillUp6, ho_RegionErosion3, ho_Contours, ho_RegionErosion4;
	HObject ho_RegionDilation5, ho_RegionDifference4, ho_RegionOpening3, ho_ConnectedRegions4;
	HObject ho_SelectedRegions4;
	HTuple  hv_Number_edge;
	//筛选为空
	CountObj(ho_SelectedRegions1, &hv_Number_edge);
	if ((hv_Number_edge == 0))
	{
		g_Result.m_resType = RES_TYPE_SCREEN;
		g_Result.m_strMsg = strErrorMsg[RES_TYPE_SCREEN];
		return true;
	}
	FillUp(ho_SelectedRegions1, &ho_RegionFillUp6);
	ErosionRectangle1(ho_RegionFillUp6, &ho_RegionErosion3, 3, 3);

	//7)
	GenContourRegionXld(ho_RegionErosion3, &ho_Contours, "border");

	ErosionRectangle1(ho_RegionErosion3, &ho_RegionErosion4, 20, 20);

	DilationRectangle1(ho_RegionErosion4, &ho_RegionDilation5, 1, 60);

	//8)
	Difference(ho_RegionDilation5, ho_RegionErosion4, &ho_RegionDifference4);

	OpeningRectangle1(ho_RegionDifference4, &ho_RegionOpening3, 10, 10);

	Connection(ho_RegionOpening3, &ho_ConnectedRegions4);
	DispObj(ho_ConnectedRegions4, hv_WindowHandle);
	SelectShape(ho_ConnectedRegions4, &ho_SelectedRegions4, "area", "and", g_Para.m_FScreenBSelectAreaUDMin, g_Para.m_FScreenBSelectAreaUDMax);

	//9)上下两条边缘
	CountObj(ho_SelectedRegions4, &hv_Number_edge);
	//如果为两条，则判断左右边缘是否为两条。这是为了判断屏幕有没有去到接近图像边缘
	if (0 != (hv_Number_edge == 2))
	{
		HObject ho_RegionDilation6, ho_RegionDifference5, ho_RegionOpening4, ho_ConnectedRegions5;
		HObject ho_SelectedRegions5;

		DilationRectangle1(ho_RegionErosion4, &ho_RegionDilation6, 60, 1);
		Difference(ho_RegionDilation6, ho_RegionErosion4, &ho_RegionDifference5);
		OpeningRectangle1(ho_RegionDifference5, &ho_RegionOpening4, 10, 10);
		Connection(ho_RegionOpening4, &ho_ConnectedRegions5);
		SelectShape(ho_ConnectedRegions5, &ho_SelectedRegions5, "area", "and", g_Para.m_FScreenBSelectAreaRLMin, g_Para.m_FScreenBSelectAreaRLMax);
		CountObj(ho_SelectedRegions5, &hv_Number_edge);
		if (0 != (hv_Number_edge == 2))
		{
			HObject  ho_ObjectSelected1, ho_ClippedContours, ho_SelectedXLD;
			HObject  ho_ObjectSelected2, ho_ClippedContours1, ho_SelectedXLD1;
			HObject  ho_ObjectSelected3, ho_ClippedContours2, ho_SelectedXLD2;
			HObject  ho_ObjectSelected4, ho_ClippedContours3, ho_SelectedXLD3;
			HObject  ho_ObjectsConcat;

			HTuple hv_Row12, hv_Column12, hv_Row21, hv_Column21;
			HTuple hv_Row13, hv_Column13, hv_Row22, hv_Column22;
			HTuple hv_Row14, hv_Column14, hv_Row23, hv_Column23;
			HTuple hv_Row15, hv_Column15, hv_Row25, hv_Column25;

			//)选择上下边缘的第一条边缘
			SelectObj(ho_SelectedRegions4, &ho_ObjectSelected1, 1);
			//1)
			SmallestRectangle1(ho_ObjectSelected1, &hv_Row12, &hv_Column12, &hv_Row21,
				&hv_Column21);
			//2)
			ClipContoursXld(ho_Contours, &ho_ClippedContours, hv_Row12, hv_Column12, hv_Row21,
				hv_Column21);
			//3)
			SelectShapeXld(ho_ClippedContours, &ho_SelectedXLD, "max_diameter", "and",
				g_Para.m_FScreenBSelectDiameterUDMin, g_Para.m_FScreenBSelectDiameterUDMax);
			//筛选为空
			CountObj(ho_SelectedXLD, &hv_Number_edge);
			if ((hv_Number_edge == 0))
			{
				g_Result.m_resType = RES_TYPE_SCREEN;
				g_Result.m_strMsg = strErrorMsg[RES_TYPE_SCREEN];
				return true;
			}
			//)选择左右的第2条边缘
			SelectObj(ho_SelectedRegions5, &ho_ObjectSelected2, 2);
			//1)
			SmallestRectangle1(ho_ObjectSelected2, &hv_Row13, &hv_Column13, &hv_Row22,
				&hv_Column22);
			//2)
			ClipContoursXld(ho_Contours, &ho_ClippedContours1, hv_Row13, hv_Column13, hv_Row22,
				hv_Column22);
			//3)
			SelectShapeXld(ho_ClippedContours1, &ho_SelectedXLD1, "max_diameter", "and",
				g_Para.m_FScreenBSelectDiameterRLMin, g_Para.m_FScreenBSelectDiameterRLMax);
			CountObj(ho_SelectedXLD1, &hv_Number_edge);
			if ((hv_Number_edge == 0))
			{
				g_Result.m_resType = RES_TYPE_SCREEN;
				g_Result.m_strMsg = strErrorMsg[RES_TYPE_SCREEN];
				return true;
			}
			//)选择上下边缘的第2条边缘
			SelectObj(ho_SelectedRegions4, &ho_ObjectSelected3, 2);
			//1)
			SmallestRectangle1(ho_ObjectSelected3, &hv_Row14, &hv_Column14, &hv_Row23,
				&hv_Column23);
			//2)
			ClipContoursXld(ho_Contours, &ho_ClippedContours2, hv_Row14, hv_Column14, hv_Row23,
				hv_Column23);
			//3)
			SelectShapeXld(ho_ClippedContours2, &ho_SelectedXLD2, "max_diameter", "and",
				g_Para.m_FScreenBSelectDiameterUDMin, g_Para.m_FScreenBSelectDiameterUDMax);
			CountObj(ho_SelectedXLD2, &hv_Number_edge);
			if ((hv_Number_edge == 0))
			{
				g_Result.m_resType = RES_TYPE_SCREEN;
				g_Result.m_strMsg = strErrorMsg[RES_TYPE_SCREEN];
				return true;
			}
			//)选择左右的第1条边缘
			SelectObj(ho_SelectedRegions5, &ho_ObjectSelected4, 1);
			//1)
			SmallestRectangle1(ho_ObjectSelected4, &hv_Row15, &hv_Column15, &hv_Row25,
				&hv_Column25);
			//2)
			ClipContoursXld(ho_Contours, &ho_ClippedContours3, hv_Row15, hv_Column15, hv_Row25,
				hv_Column25);
			//3)
			SelectShapeXld(ho_ClippedContours3, &ho_SelectedXLD3, "max_diameter", "and",
				g_Para.m_FScreenBSelectDiameterRLMin, g_Para.m_FScreenBSelectDiameterRLMax);
			CountObj(ho_SelectedXLD3, &hv_Number_edge);
			if ((hv_Number_edge == 0))
			{
				g_Result.m_resType = RES_TYPE_SCREEN;
				g_Result.m_strMsg = strErrorMsg[RES_TYPE_SCREEN];
				return true;
			}
			//）集合
			ConcatObj(ho_SelectedXLD, ho_SelectedXLD1, &ho_ObjectsConcat);
			ConcatObj(ho_ObjectsConcat, ho_SelectedXLD2, &ho_ObjectsConcat);
			ConcatObj(ho_ObjectsConcat, ho_SelectedXLD3, &ho_ObjectsConcat);
			//) 拟合直线，取直线焦点
			HTuple hv_RowBegin, hv_ColBegin, hv_RowEnd, hv_ColEnd, hv_Nr, hv_Nc, hv_Dist;
			HTuple hv_IsOverlapping, hv_IsOverlapping1, hv_IsOverlapping2, hv_IsOverlapping3;
			HTuple hv_Row4, hv_Column4;
			HTuple hv_Row5, hv_Column5;
			HTuple hv_Row6, hv_Column6;
			HTuple hv_Row7, hv_Column7;
			HTuple hv_Row_Concat, hv_Col_Concat;

			FitLineContourXld(ho_ObjectsConcat, "tukey", -1, 0, 5, 2, &hv_RowBegin, &hv_ColBegin,
				&hv_RowEnd, &hv_ColEnd, &hv_Nr, &hv_Nc, &hv_Dist);
			IntersectionLines(HTuple(hv_RowBegin[0]), HTuple(hv_ColBegin[0]), HTuple(hv_RowEnd[0]),
				HTuple(hv_ColEnd[0]), HTuple(hv_RowBegin[1]), HTuple(hv_ColBegin[1]), HTuple(hv_RowEnd[1]),
				HTuple(hv_ColEnd[1]), &hv_Row4, &hv_Column4, &hv_IsOverlapping);
			IntersectionLines(HTuple(hv_RowBegin[1]), HTuple(hv_ColBegin[1]), HTuple(hv_RowEnd[1]),
				HTuple(hv_ColEnd[1]), HTuple(hv_RowBegin[2]), HTuple(hv_ColBegin[2]), HTuple(hv_RowEnd[2]),
				HTuple(hv_ColEnd[2]), &hv_Row5, &hv_Column5, &hv_IsOverlapping1);
			IntersectionLines(HTuple(hv_RowBegin[2]), HTuple(hv_ColBegin[2]), HTuple(hv_RowEnd[2]),
				HTuple(hv_ColEnd[2]), HTuple(hv_RowBegin[3]), HTuple(hv_ColBegin[3]), HTuple(hv_RowEnd[3]),
				HTuple(hv_ColEnd[3]), &hv_Row6, &hv_Column6, &hv_IsOverlapping2);
			IntersectionLines(HTuple(hv_RowBegin[3]), HTuple(hv_ColBegin[3]), HTuple(hv_RowEnd[3]),
				HTuple(hv_ColEnd[3]), HTuple(hv_RowBegin[0]), HTuple(hv_ColBegin[0]), HTuple(hv_RowEnd[0]),
				HTuple(hv_ColEnd[0]), &hv_Row7, &hv_Column7, &hv_IsOverlapping3);
			hv_Row_Concat = HTuple();
			hv_Col_Concat = HTuple();
			TupleConcat(hv_Row4, hv_Row5, &hv_Row_Concat);
			TupleConcat(hv_Row_Concat, hv_Row6, &hv_Row_Concat);
			TupleConcat(hv_Row_Concat, hv_Row7, &hv_Row_Concat);

			TupleConcat(hv_Column4, hv_Column5, &hv_Col_Concat);
			TupleConcat(hv_Col_Concat, hv_Column6, &hv_Col_Concat);
			TupleConcat(hv_Col_Concat, hv_Column7, &hv_Col_Concat);
			//）连接成区域RegionScreen
			GenRegionPolygonFilled(&g_RegionScreen, hv_Row_Concat, hv_Col_Concat);

			//inner_rectangle1 (Region6, Row16, Column16, Row25, Column25)
			//gen_rectangle1 (Rectangle2, Row16, Column16, Row25, Column25)
			//) 划分中心区域，亮度均匀
			ErosionRectangle1(g_RegionScreen, &ho_RegionCenter, g_Para.m_FScreenBEdgeWidth, g_Para.m_FScreenBEdgeWidth);
			//）划分边缘区域，亮度比较不均匀，需特殊处理
			Difference(g_RegionScreen, ho_RegionCenter, &ho_RegionMargin);
			g_RegionScreen = g_RegionScreen.Clone();
			DispObj(g_Image, hv_WindowHandle);
			SetDraw(hv_WindowHandle, "margin");
			SetColor(hv_WindowHandle, "cyan");
			DispObj(g_RegionScreen, hv_WindowHandle);
			//DispObj(ho_RegionCenter, hv_WindowHandle);
			//完成
			g_Result.m_resType = RES_TYPE_OK;
		}
		else
		{
			g_Result.m_resType = RES_TYPE_SCREEN;
			g_Result.m_strMsg = strErrorMsg[RES_TYPE_SCREEN];
			return true;
		}
	}
	else
	{
		g_Result.m_resType = RES_TYPE_SCREEN;
		g_Result.m_strMsg = strErrorMsg[RES_TYPE_SCREEN];
		return true;
	}
	return true;
}

//运行任务
int Run()
{
	//查找屏幕区域
	//EslFindScreen();
	bool ret = EslCheckLightScreen();
	EslCheckRedScreen();
	EslCheckGreenScreen();
	EslCheckBlueScreen();
	if (ret) //只有当白屏的屏幕区域正常时才进行黑屏检测
	{
		EslCheckBlackScreen();
	}

	return 0;
}

void GetScreenType(T_SCR type, std::string & name)
{
	switch (type)
	{
	case T_WHITE_SCR:
		name = "WHITE";
		break;
	case T_RED_SCR:
		name = "RED";
		break;
	case T_GREEN_SCR:
		name = "GREEN";
		break;
	case T_BLUE_SCR:
		name = "BLUE";
		break;
	case T_BLACK_SCR:
		name = "BLACK";
		break;
	default:
		name = "UNKNOWN";
		return ;
	}
}

//记录结果到路径
bool LogResult(char* path, int mode)
{
	FILE* pF = NULL;
	pF = fopen(path, "a");
	if (pF == NULL) return false;

	//时间
	SYSTEMTIME curT;
	GetLocalTime(&curT);
	fprintf_s(pF, "%d-%02d-%02d %02d:%02d:%02d ", curT.wYear, curT.wMonth, curT.wDay, curT.wHour, curT.wMinute, curT.wSecond);
	
	//switch (mode)
	//{
	//case 0:
	//	{
	std::string name;
	GetScreenType(g_Result.m_screenType, name);
	fprintf_s(pF, "  %s  ", name.c_str());
	if (g_Result.m_resType == RES_TYPE_OK)
	{
		if (g_vctResult.size() > 0) //屏幕检测NG
		{
			fprintf_s(pF, "  NG  \n");
			//显示NG的详细内容
			for (int i = 0; i < g_vctResult.size(); i++)
			{
				fprintf_s(pF, "    %s Area: %.2lf\n", g_vctResult[i].m_strMsg.c_str(), g_vctResult[i].m_Area);
			}
		}
		else //屏幕检测OK
		{
			fprintf_s(pF, "  OK  \n");
		}
	}
	else //屏幕区域ng
	{
		fprintf_s(pF, "  NG  \n");
		//显示NG的详细内容
		fprintf_s(pF, "    %s\n", g_Result.m_strMsg.c_str());
	}
	fclose(pF);
	return true;
}
//判断结果，并在屏幕上显示结果，图像 ， OKNG
bool JudgeResultAndShow(T_SCR screenType, bool show = false )
{
	//显示到屏幕
	HTuple wndhandle;
	HObject ImageTmp;
	switch (screenType)
	{
	case T_WHITE_SCR:
		wndhandle = hv_WindowHandle_White;
		break;
	case T_RED_SCR:
		wndhandle = hv_WindowHandle_Red;
		break;
	case T_GREEN_SCR:
		wndhandle = hv_WindowHandle_Green;
		break;
	case T_BLUE_SCR:
		wndhandle = hv_WindowHandle_Blue;
		break;
	case T_BLACK_SCR:
		wndhandle = hv_WindowHandle_Black;
		break;
	default:
		return false;
	}
	
	//如果是屏幕分离检测问题
	if (g_Result.m_resType == RES_TYPE_SCREEN)
	{
		//显示图像
		if (wndhandle.Length())
		{
			DispObj(g_Image, wndhandle);
		}
		SetTposition(wndhandle, HTuple(0), (HTuple(0) ));
		SetColor(wndhandle, "red");
		WriteString(wndhandle, "NG");

		//在主窗口显示错误信息
		SetTposition(hv_WindowHandle, HTuple(0 + 55), (HTuple(0)));
		SetColor(hv_WindowHandle, "red");
		WriteString(hv_WindowHandle, "NG");
		
		WriteString(hv_WindowHandle, g_Result.m_strMsg.c_str());
		LogResult("Log.txt",0);
		return false;
	}

	//其他
	if (show)
	{
		//显示图像到小窗口
		if (wndhandle.Length())
		{
			DispObj(g_Image, wndhandle);
		}
		//是否OK
		bool bOk = true;
		for (int i = 0; i < g_vctResult.size(); i++)
		{
			if (screenType == g_vctResult[i].m_screenType)
			{
				bOk = false;
				break;
			}
		}

		HalconCpp::SetDraw(wndhandle, "fill");
		SetTposition(wndhandle, HTuple(0), (HTuple(0)));
		//HObject rectangle;
		if (bOk)//显示OK
		{
			//GenRectangle1(&rectangle, 0, 0, )
			SetColor(wndhandle, "green");
			WriteString(wndhandle, "OK");
			SetTposition(hv_WindowHandle, HTuple(0 + 55), (HTuple(0)));
			SetColor(hv_WindowHandle, "green");
			WriteString(hv_WindowHandle, "OK");
			LogResult("Log.txt", 0);
			return true;
		}
		else
		{
			SetColor(wndhandle, "red");
			WriteString(wndhandle, "NG");

			//在主窗口显示错误信息
			SetTposition(hv_WindowHandle, HTuple(0 + 55), (HTuple(0)));
			SetColor(hv_WindowHandle, "red");
			WriteString(hv_WindowHandle, "NG");
			//SetTposition(wndhandle, HTuple(0 + 55), (HTuple(0) + 5));
			//WriteString(hv_WindowHandle, g_Result.m_strMsg.c_str());
			LogResult("Log.txt", 0);
			return false;
		}
		
	}

	//显示NG
	return true;
}
//检测白屏缺点
bool EslCheckLightScreen()
{
	g_Para = g_Para_White;
	g_Result.m_screenType = T_WHITE_SCR;
	g_Result.m_resType = RES_TYPE_OK;
	g_vctResult.clear();
	if (EslFindScreen())
	{
		//如果屏幕区域缺陷，则直接报NG， 不用查找亮点暗点
		if (!JudgeResultAndShow(g_Result.m_screenType)) return false;
		
		//检测亮点暗点
		g_Result.m_resType = RES_TYPE_OK;
		HObject ho_ConnectedRegions, ho_ConnectedRegions_Light;
		DetectSpot(g_ImageGray, &ho_ConnectedRegions, &ho_ConnectedRegions_Light);
		//缺陷显示到主窗
		DispObj(g_Image, hv_WindowHandle);
		SetColor(hv_WindowHandle, "green");
		DispObj(g_RegionScreen, hv_WindowHandle);
		SetColor(hv_WindowHandle, "red");
		DispObj(ho_ConnectedRegions, hv_WindowHandle);
		SetColor(hv_WindowHandle, "yellow");
		DispObj(ho_ConnectedRegions_Light, hv_WindowHandle);
		
		DealWithResult(ho_ConnectedRegions, g_Image, &g_ImageParts, T_WHITE_SCR, RES_TYPE_DARK_SPOT);
		DealWithResult(ho_ConnectedRegions_Light, g_Image, &g_ImageParts, T_WHITE_SCR, RES_TYPE_BRIGHT_SPOT);
		if (!JudgeResultAndShow(T_WHITE_SCR, true))
		{
			return false;
		}
		else
			return true;
	}
	return false;
}

bool EslCheckRedScreen()
{
	g_Result.m_screenType = T_RED_SCR;
	g_Result.m_resType = RES_TYPE_OK;
	g_vctResult.clear();
	g_Para = g_Para_Red;
	if (EslFindScreen())
	{
		if (!JudgeResultAndShow(g_Result.m_screenType)) return false;

		g_Result.m_resType = RES_TYPE_OK;
		HObject ho_ConnectedRegions, ho_ConnectedRegions_Light;
		DetectSpot(g_ImageRed, &ho_ConnectedRegions, &ho_ConnectedRegions_Light);
		
		DispObj(g_Image, hv_WindowHandle);
		//DispObj(g_ImageRed, hv_WindowHandle);
		SetColor(hv_WindowHandle, "green");
		DispObj(g_RegionScreen, hv_WindowHandle);
		SetColor(hv_WindowHandle, "red");
		DispObj(ho_ConnectedRegions, hv_WindowHandle);
		SetColor(hv_WindowHandle, "yellow");
		DispObj(ho_ConnectedRegions_Light, hv_WindowHandle);

		DealWithResult(ho_ConnectedRegions, g_Image, &g_ImageParts, T_RED_SCR, RES_TYPE_DARK_SPOT);
		DealWithResult(ho_ConnectedRegions_Light, g_Image, &g_ImageParts, T_RED_SCR, RES_TYPE_BRIGHT_SPOT);
		if (!JudgeResultAndShow(T_RED_SCR, true))
		{
			return false;
		}
		else
			return true;

	}
	else
	{
		return false;
	}
	return true;
}

bool EslCheckGreenScreen()
{
	g_Result.m_screenType = T_GREEN_SCR;
	g_Result.m_resType = RES_TYPE_OK;
	g_vctResult.clear();
	g_Para = g_Para_Green;
	if (EslFindScreen())
	{
		if (!JudgeResultAndShow(g_Result.m_screenType)) return false;

		g_Result.m_resType = RES_TYPE_OK;
		HObject ho_ConnectedRegions, ho_ConnectedRegions_Light;
		DetectSpot(g_ImageGreen, &ho_ConnectedRegions, &ho_ConnectedRegions_Light);
		
		DispObj(g_Image, hv_WindowHandle);
		SetColor(hv_WindowHandle, "green");
		DispObj(g_RegionScreen, hv_WindowHandle);
		SetColor(hv_WindowHandle, "red");
		DispObj(ho_ConnectedRegions, hv_WindowHandle);
		SetColor(hv_WindowHandle, "yellow");
		DispObj(ho_ConnectedRegions_Light, hv_WindowHandle);

		DealWithResult(ho_ConnectedRegions, g_Image, &g_ImageParts, T_GREEN_SCR, RES_TYPE_DARK_SPOT);
		DealWithResult(ho_ConnectedRegions_Light, g_Image, &g_ImageParts, T_GREEN_SCR, RES_TYPE_BRIGHT_SPOT);
		if (!JudgeResultAndShow(T_GREEN_SCR, true))
		{
			return false;
		}
		else
			return true;
	}
	else
	{
		return false;
	}
	return true;
}

bool EslCheckBlueScreen()
{
	g_Result.m_screenType = T_BLUE_SCR;
	g_Result.m_resType = RES_TYPE_OK;
	g_vctResult.clear();
	g_Para = g_Para_Blue;
	if (EslFindScreen())
	{
		if (!JudgeResultAndShow(g_Result.m_screenType)) return false;

		g_Result.m_resType = RES_TYPE_OK;
		HObject ho_ConnectedRegions, ho_ConnectedRegions_Light;
		DetectSpot(g_ImageBlue, &ho_ConnectedRegions, &ho_ConnectedRegions_Light);

		DispObj(g_Image, hv_WindowHandle);
		SetColor(hv_WindowHandle, "green");
		DispObj(g_RegionScreen, hv_WindowHandle);
		SetColor(hv_WindowHandle, "red");
		DispObj(ho_ConnectedRegions, hv_WindowHandle);
		SetColor(hv_WindowHandle, "yellow");
		DispObj(ho_ConnectedRegions_Light, hv_WindowHandle);

		DealWithResult(ho_ConnectedRegions, g_Image, &g_ImageParts, T_BLUE_SCR, RES_TYPE_DARK_SPOT);
		DealWithResult(ho_ConnectedRegions_Light, g_Image, &g_ImageParts, T_BLUE_SCR, RES_TYPE_BRIGHT_SPOT);
		if (!JudgeResultAndShow(T_BLUE_SCR, true))
		{
			return false;
		}
		else
			return true;
	}
	else
	{
		return false;
	}
	return true;
}

//黑屏检测 （亮点）
bool EslCheckBlackScreen()
{
	g_Result.m_screenType = T_BLACK_SCR;
	g_Result.m_resType = RES_TYPE_OK;
	g_vctResult.clear();
	g_Para = g_Para_Black;
	if (EslFindScreen())
	{
		if (!JudgeResultAndShow(g_Result.m_screenType)) return false;
		
		g_Result.m_resType = RES_TYPE_OK;
		HObject RegionResult;
		HTuple hv_HysteresisMin, hv_HysteresisMax, hv_AreaThr1, hv_MeanSize1, hv_DynThr;
		hv_HysteresisMin = g_Para.m_HysteresisThrMin_Black;
		hv_HysteresisMax = g_Para.m_HysteresisThrMax_Black;
		hv_AreaThr1 = g_Para.m_AreaThr;
		hv_MeanSize1 = g_Para.m_MeanSize_Black;
		hv_DynThr = g_Para.m_DynThr_Black;
		CheckLightSpotInDarkScreen(g_ImageGray, g_RegionScreen, &RegionResult, hv_HysteresisMin,
			hv_HysteresisMax, hv_AreaThr1, hv_MeanSize1, hv_DynThr);
		
		DispObj(g_Image, hv_WindowHandle);
		SetColor(hv_WindowHandle, "green");
		DispObj(g_RegionScreen, hv_WindowHandle);
		SetColor(hv_WindowHandle, "red");
		DispObj(RegionResult, hv_WindowHandle);

		DealWithResult(RegionResult, g_Image, &g_ImageParts, T_BLACK_SCR, RES_TYPE_BRIGHT_SPOT);
		if (!JudgeResultAndShow(T_BLACK_SCR, true))
		{
			return false;
		}
		else
			return true;
	}


	return false;
}

bool EslReloadPara()
{
	//加载算法相关
	if (!g_Para_White.LoadPara(PATH_WHITE_SRC_PARA))
	{
		AfxMessageBox("加载白屏参数失败");
	}
	if (!g_Para_Red.LoadPara(PATH_RED_SRC_PARA))
	{
		AfxMessageBox("加载红屏参数失败");
	}
	if (!g_Para_Green.LoadPara(PATH_GREEN_SRC_PARA))
	{
		AfxMessageBox("加载绿屏参数失败");
	}
	if (!g_Para_Blue.LoadPara(PATH_BLUE_SRC_PARA))
	{
		AfxMessageBox("加载蓝屏参数失败");
	}
	if (!g_Para_Black.LoadPara(PATH_BLACK_SRC_PARA))
	{
		AfxMessageBox("加载黑屏参数失败");
	}
	return true;
}

//检测暗点，暗斑，mura, 检测亮点亮斑
bool DetectSpot(HObject ImageSrc, HObject* RegionResults, HObject* RegionResults_Light)
{
	//简单检测暗点，暗斑
	HObject ho_ResultRegion;
	HTuple hv_ThrHigh, hv_MinArea, hv_Delta, hv_RoundGray;
	hv_ThrHigh	= g_Para.m_ThrHigh;
	hv_MinArea	= g_Para.m_MinArea;
	hv_Delta	= g_Para.m_Delta;
	
	SimpleDarkSpot(ImageSrc, g_RegionScreen, &ho_ResultRegion, hv_ThrHigh, hv_MinArea,
		hv_Delta, &hv_RoundGray);
	//SetColor(hv_WindowHandle, "blue");
	//DispObj(ho_ResultRegion, hv_WindowHandle);
	//简单检测亮点，亮斑
	HObject ho_ResultRegionLightSpot;
	HTuple hv_ThrHigh2, hv_Delta2, hv_RoundGray2;
	hv_ThrHigh2 = g_Para.m_ThrHigh_Light;
	hv_Delta2 = g_Para.m_Delta_Light;
	SimpleLightSpot(ImageSrc, g_RegionScreen, &ho_ResultRegionLightSpot, hv_ThrHigh2,
		hv_MinArea, hv_Delta2, &hv_RoundGray2);

	//预处理
	HObject ho_MaskRegion;
	HObject ho_ImageFFTRes, ho_ImageMeanRes, ho_ImageFFTRes2, ho_ImageMeanRes2;
	HTuple hv_MedianSize, hv_GuassHigh, hv_GuassLow, hv_MeanSize, hv_OffsetX, hv_OffsetY;
	hv_MedianSize	= g_Para.m_MedianSize;//80;
	hv_GuassHigh	= g_Para.m_GuassHigh;//3;
	hv_GuassLow		= g_Para.m_GuassLow;//10;
	hv_MeanSize		= g_Para.m_MeanSize;//3;
	
	ConcatObj(ho_ResultRegion, ho_ResultRegionLightSpot, &ho_MaskRegion);
	PreProcess(ImageSrc, g_RegionScreen, ho_MaskRegion, &ho_ImageFFTRes, &ho_ImageMeanRes,
		&ho_ImageFFTRes2, &ho_ImageMeanRes2, hv_MedianSize, hv_GuassHigh, hv_GuassLow,
		hv_MeanSize, hv_RoundGray, &hv_OffsetX, &hv_OffsetY);

	//检测黑色MURA
	HObject ho_ResultRegion2;
	HTuple hv_HysteresisThrMin, hv_HysteresisThrMax, hv_MarginWidth, hv_FftThr, hv_AreaThr;
	hv_HysteresisThrMin = g_Para.m_HysteresisThrMin;//95;
	hv_HysteresisThrMax = g_Para.m_HysteresisThrMax;//120;
	hv_MarginWidth		= g_Para.m_MarginWidth;//30;
	hv_FftThr			= g_Para.m_FftThr;//120;
	hv_AreaThr			= g_Para.m_AreaThr;//7;
	if (g_Para.m_bCheckDarkSpot)
	{
		CheckDarkFuzzySpot(ho_ImageMeanRes, ho_ImageFFTRes, g_RegionScreen,ho_MaskRegion, &ho_ResultRegion2,
			hv_HysteresisThrMin, hv_HysteresisThrMax, hv_MarginWidth, hv_FftThr, hv_AreaThr,
			hv_OffsetX, hv_OffsetY);
	}
	//检测白色MURA
	HObject ho_ResultRegionLight;
	HTuple hv_HysteresisThrMinLight, hv_HysteresisThrMaxLight, hv_MarginWidthLight, hv_FftThrLight, hv_AreaThrLight;
	hv_HysteresisThrMinLight = g_Para.m_HysteresisThrMin_Light;
	hv_HysteresisThrMaxLight = g_Para.m_HysteresisThrMax_Light;
	hv_MarginWidthLight = g_Para.m_MarginWidth;//30;
	hv_FftThrLight = g_Para.m_FftThr_Light;//120;
	hv_AreaThrLight = g_Para.m_AreaThr_Light;//7;
	if (g_Para.m_bCheckLightSpot)
	{
	CheckLightFuzzySpot(ho_ImageMeanRes2, ho_ImageFFTRes2, g_RegionScreen, ho_MaskRegion, &ho_ResultRegionLight,
		hv_HysteresisThrMinLight, hv_HysteresisThrMaxLight, hv_MarginWidthLight, hv_FftThrLight, hv_AreaThrLight,
		hv_OffsetX, hv_OffsetY);
	}
	//统计结果
	//暗点
	HObject ho_RegionUnion1, ho_RegionUnionDark, ho_ConnectedRegions;
	ConcatObj(ho_ResultRegion, ho_ResultRegion2, &ho_RegionUnion1);
	Union1(ho_RegionUnion1, &ho_RegionUnionDark);
	Connection(ho_RegionUnionDark, &ho_ConnectedRegions);
	*RegionResults = ho_ConnectedRegions.Clone();

	//亮点
	HObject ho_RegionUnion2, ho_RegionUnionLight, ho_ConnectedRegions2;
	ConcatObj(ho_ResultRegionLightSpot, ho_ResultRegionLight, &ho_RegionUnion2);
	Union1(ho_RegionUnion2, &ho_RegionUnionLight);
	Connection(ho_RegionUnionLight, &ho_ConnectedRegions2);
	*RegionResults_Light = ho_ConnectedRegions2.Clone();

	return true;
}

bool DealWithResult(HObject ho_ResultRegions, HObject ho_ImageSrc, HObject * ho_ImageParts, T_SCR screentype, Type_Res type)
{
	//判断缺陷的大小面积位置， 并截图。
	HTuple Areas, CentRow, CentCol, Row1, Col1, Row2, Col2;
	JudgeResult(ho_ResultRegions, ho_ImageSrc, ho_ImageParts, 3364, 2748, 120,
		80, 20, &Areas, &CentRow, &CentCol, &Row1, &Col1, &Row2,
		&Col2);
	//保存结果
	int num = Areas.Length();
	for (int i = 0; i < num; i++)
	{
		//
		ResultForLcd resTmp;
		resTmp.m_screenType = screentype;
		resTmp.m_resType = type;
		resTmp.m_targetColX = CentCol[i];
		resTmp.m_targetRowY = CentRow[i];
		resTmp.m_Area = Areas[i];
		resTmp.m_lfRow1 = Row1[i];
		resTmp.m_lfCol1 = Col1[i];
		resTmp.m_lfRow2 = Row2[i];
		resTmp.m_lfCol2 = Col2[i];
		resTmp.m_strMsg = strErrorMsg[type];
		g_vctResult.push_back(resTmp);
	}

	return true; //直接返回，不截图
	//保存截图
	if ((*ho_ImageParts).IsInitialized())
	{
		HTuple num, hv_Index;
		HObject selectObj;
		CountObj((*ho_ImageParts), &num);
		HTuple step_val10 = 1;
		int count = 1;
		for (hv_Index = 1; hv_Index.Continue(num, step_val10); hv_Index += step_val10)
		{
			SelectObj((*ho_ImageParts), &selectObj, hv_Index);
			//时间加个数命名
			char name[1024];
			if(type== RES_TYPE_BRIGHT_SPOT)
				sprintf_s(name, "LightSpot_%d.bmp", count);
			else
				sprintf_s(name, "DarkSpot%d.bmp", count);
			WriteImage(selectObj, "bmp", 0, name);
			count++;
		}
	}
	return true;
}

void CheckDarkFuzzySpot(HObject ho_ImageMean, HObject ho_ImageFFT, HObject ho_ScreenRegion,
	HObject ho_MaskRegion, HObject *ho_ResultRegion2, HTuple hv_HysteresisThrMin,
	HTuple hv_HysteresisThrMax, HTuple hv_MarginWidth, HTuple hv_fftThr, HTuple hv_AreaThr,
	HTuple hv_OffsetX, HTuple hv_OffsetY)
{

	// Local iconic variables
	HObject  ho_ImageScaleMaxFFT, ho_ImageScaleMaxMean;
	HObject  ho_RegionHysteresis, ho_Rectangle1, ho_RegionIntersection;
	HObject  ho_Regions1, ho_RegionIntersection1, ho_RegionUnion;
	HObject  ho_RegionUnionAll, ho_RegionUnionMask, ho_RegionUnionMaskTrans;
	HObject  ho_RegionDifferenceAll, ho_ConnectedRegions, ho_SelectedRegions;

	// Local control variables
	HTuple  hv_Width, hv_Height, hv_HomMat2DIdentity2;
	HTuple  hv_HomMat2DIdentity;

	//图片大小
	GetImageSize(ho_ImageMean, &hv_Width, &hv_Height);

	//检测暗斑点
	ScaleImageMax(ho_ImageFFT, &ho_ImageScaleMaxFFT);
	ScaleImageMax(ho_ImageMean, &ho_ImageScaleMaxMean);
	HysteresisThreshold(ho_ImageScaleMaxMean, &ho_RegionHysteresis, hv_HysteresisThrMin,
		hv_HysteresisThrMax, 5);
	//缩小范围(边缘不检测）
	GenRectangle1(&ho_Rectangle1, hv_MarginWidth, hv_MarginWidth, hv_Height - hv_MarginWidth,
		hv_Width - hv_MarginWidth);
	//取交集
	Intersection(ho_Rectangle1, ho_RegionHysteresis, &ho_RegionIntersection);
	Threshold(ho_ImageScaleMaxFFT, &ho_Regions1, hv_fftThr, 255);
	//取交集
	Intersection(ho_Regions1, ho_Rectangle1, &ho_RegionIntersection1);


	//汇总
	Union1(ho_RegionIntersection, &ho_RegionUnion);
	Union2(ho_RegionUnion, ho_RegionIntersection1, &ho_RegionUnionAll);
	//与输入的掩膜取差集
	Union1(ho_MaskRegion, &ho_RegionUnionMask);
	HomMat2dIdentity(&hv_HomMat2DIdentity2);
	HomMat2dTranslate(hv_HomMat2DIdentity2, -hv_OffsetY, -hv_OffsetX, &hv_HomMat2DIdentity2);
	AffineTransRegion(ho_RegionUnionMask, &ho_RegionUnionMaskTrans, hv_HomMat2DIdentity2,
		"nearest_neighbor");
	Difference(ho_RegionUnionAll, ho_RegionUnionMaskTrans, &ho_RegionDifferenceAll);
	//最后筛选
	Connection(ho_RegionDifferenceAll, &ho_ConnectedRegions);
	SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, "area", "and", hv_AreaThr,
		9999999);

	//仿射变换
	HomMat2dIdentity(&hv_HomMat2DIdentity);
	HomMat2dTranslate(hv_HomMat2DIdentity, hv_OffsetY, hv_OffsetX, &hv_HomMat2DIdentity);
	AffineTransRegion(ho_SelectedRegions, &ho_SelectedRegions, hv_HomMat2DIdentity,
		"nearest_neighbor");
	(*ho_ResultRegion2) = ho_SelectedRegions;
	return;
}

void PreProcess(HObject ho_ImageSrc, HObject ho_ScreenRegion, HObject ho_MaskRegion,
	HObject *ho_ImageFFTRes, HObject *ho_ImageMeanRes, HObject *ho_ImageFFTRes2,
	HObject *ho_ImageMeanRes2, HTuple hv_MedianSize, HTuple hv_GuassHigh, HTuple hv_GuassLow,
	HTuple hv_MeanSize, HTuple hv_MaskRoundGray, HTuple *hv_OffsetX, HTuple *hv_OffsetY)
{

	// Local iconic variables
	HObject  ho_Rectangle, ho_ImagePro, ho_RegionUnion;
	HObject  ho_RegionDifference, ho_ObjectSelected, ho_RegionUnionMask;
	HObject  ho_RegionDilationMask, ho_ImageReduced, ho_ImagePart;
	HObject  ho_ImageSub, ho_ImageMedian, ho_ImageFFT, ho_ImageGaussHighpass;
	HObject  ho_ImageGaussLowpass, ho_ImageSub1, ho_ImageConvol;
	HObject  ho_ImageSub2, ho_ImageConvol2;

	// Local control variables
	HTuple  hv_Row1, hv_Column1, hv_Row2, hv_Column2;
	HTuple  hv_Number, hv_Mean, hv_Deviation, hv_Index, hv_Width;
	HTuple  hv_Height;

	//内接矩形
	InnerRectangle1(ho_ScreenRegion, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);
	GenRectangle1(&ho_Rectangle, hv_Row1, hv_Column1, hv_Row2, hv_Column2);
	(*hv_OffsetX) = hv_Column1;
	(*hv_OffsetY) = hv_Row1;

	ho_ImagePro = ho_ImageSrc;
	//修补mask区域的灰度，使其变为其周围的灰度，便于准确傅里叶变换
	//dilation_circle (MaskRegion, RegionDilation, 1.5)
	Union1(ho_MaskRegion, &ho_RegionUnion);
	CountObj(ho_MaskRegion, &hv_Number);
	//求出除了mask之外的平均灰度
	Difference(ho_ScreenRegion, ho_RegionUnion, &ho_RegionDifference);
	Intensity(ho_RegionDifference, ho_ImageSrc, &hv_Mean, &hv_Deviation);

	{
		HTuple end_val15 = hv_Number;
		HTuple step_val15 = 1;
		for (hv_Index = 1; hv_Index.Continue(end_val15, step_val15); hv_Index += step_val15)
		{
			SelectObj(ho_MaskRegion, &ho_ObjectSelected, hv_Index);
			PaintRegion(ho_ObjectSelected, ho_ImagePro, &ho_ImagePro, hv_Mean, "fill");
		}
	}

	//) 填充对边缘不充分，需要平滑滤波
	Union1(ho_MaskRegion, &ho_RegionUnionMask);
	DilationRectangle1(ho_RegionUnionMask, &ho_RegionDilationMask, 11, 11);




	ReduceDomain(ho_ImagePro, ho_Rectangle, &ho_ImageReduced);
	CropDomain(ho_ImageReduced, &ho_ImagePart);

	//mean_image (ImagePart, ImageMean, MedianSize, MedianSize)
	//sub_image (ImagePart, ImageMean, ImageSub, 1, 194)
	MedianRect(ho_ImagePart, &ho_ImageMedian, hv_MedianSize, hv_MedianSize);
	SubImage(ho_ImagePart, ho_ImageMedian, &ho_ImageSub, 1, 194);
	//图片大小
	GetImageSize(ho_ImageSub, &hv_Width, &hv_Height);

	//傅里叶变换
	RftGeneric(ho_ImageSub, &ho_ImageFFT, "to_freq", "none", "complex", hv_Width);
	//创建高斯滤波器1
	GenGaussFilter(&ho_ImageGaussHighpass, hv_GuassHigh, hv_GuassHigh, 0, "n", "rft",
		hv_Width, hv_Height);
	GenGaussFilter(&ho_ImageGaussLowpass, hv_GuassLow, hv_GuassLow, 0, "n", "rft",
		hv_Width, hv_Height);
	SubImage(ho_ImageGaussHighpass, ho_ImageGaussLowpass, &ho_ImageSub1, 1, 0);
	ConvolFft(ho_ImageFFT, ho_ImageSub1, &ho_ImageConvol);
	RftGeneric(ho_ImageConvol, &(*ho_ImageFFTRes2), "from_freq", "n", "real", hv_Width);
	MeanImage((*ho_ImageFFTRes2), &(*ho_ImageMeanRes2), hv_MeanSize, hv_MeanSize);
	//创建高斯滤波器2
	SubImage(ho_ImageGaussLowpass, ho_ImageGaussHighpass, &ho_ImageSub2, 1, 0);
	ConvolFft(ho_ImageFFT, ho_ImageSub2, &ho_ImageConvol2);
	RftGeneric(ho_ImageConvol2, &(*ho_ImageFFTRes), "from_freq", "n", "real", hv_Width);
	MeanImage((*ho_ImageFFTRes), &(*ho_ImageMeanRes), hv_MeanSize, hv_MeanSize);


	return;
}

void FindScreen(HObject ho_ImageSrc, HObject *ho_ScreenRegion)
{

	// Local iconic variables
	HObject  ho_Regions, ho_RegionErosion, ho_RegionDilation;
	HObject  ho_ConnectedRegions, ho_SelectedRegions, ho_RegionFillUp7;
	HObject  ho_SelectedRegions1, ho_RegionFillUp6, ho_RegionErosion3;
	HObject  ho_Contours, ho_RegionErosion4, ho_RegionDilation5;
	HObject  ho_RegionDifference4, ho_RegionOpening3, ho_ConnectedRegions4;
	HObject  ho_SelectedRegions4, ho_RegionDilation6, ho_RegionDifference5;
	HObject  ho_RegionOpening4, ho_ConnectedRegions5, ho_SelectedRegions5;
	HObject  ho_ObjectSelected1, ho_ClippedContours, ho_SelectedXLD;
	HObject  ho_ObjectSelected2, ho_ClippedContours1, ho_SelectedXLD1;
	HObject  ho_ObjectSelected3, ho_ClippedContours2, ho_SelectedXLD2;
	HObject  ho_ObjectSelected4, ho_ClippedContours3, ho_SelectedXLD3;
	HObject  ho_ObjectsConcat, ho_RegionScreen, ho_RegionCenter;
	HObject  ho_RegionMargin;

	// Local control variables
	HTuple  hv_Number_edge, hv_Row12, hv_Column12;
	HTuple  hv_Row21, hv_Column21, hv_Row13, hv_Column13, hv_Row22;
	HTuple  hv_Column22, hv_Row14, hv_Column14, hv_Row23, hv_Column23;
	HTuple  hv_Row15, hv_Column15, hv_Row24, hv_Column24, hv_RowBegin;
	HTuple  hv_ColBegin, hv_RowEnd, hv_ColEnd, hv_Nr, hv_Nc;
	HTuple  hv_Dist, hv_Row4, hv_Column4, hv_IsOverlapping;
	HTuple  hv_Row5, hv_Column5, hv_IsOverlapping1, hv_Row6;
	HTuple  hv_Column6, hv_IsOverlapping2, hv_Row7, hv_Column7;
	HTuple  hv_IsOverlapping3, hv_Row_Concat, hv_Col_Concat;

	Threshold(ho_ImageSrc, &ho_Regions, 81, 200);

	ErosionRectangle1(ho_Regions, &ho_RegionErosion, 21, 21);
	DilationRectangle1(ho_RegionErosion, &ho_RegionDilation, 21, 21);
	Connection(ho_RegionDilation, &ho_ConnectedRegions);
	SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, "area", "and", 2000000, 9999999);
	FillUp(ho_SelectedRegions, &ho_RegionFillUp7);
	SelectShape(ho_RegionFillUp7, &ho_SelectedRegions1, "rectangularity", "and", 0.85,
		1);
	//)拟合成标准矩形
	FillUp(ho_SelectedRegions1, &ho_RegionFillUp6);
	ErosionRectangle1(ho_RegionFillUp6, &ho_RegionErosion3, 3, 3);

	//7)
	GenContourRegionXld(ho_RegionErosion3, &ho_Contours, "border");

	ErosionRectangle1(ho_RegionErosion3, &ho_RegionErosion4, 20, 20);

	DilationRectangle1(ho_RegionErosion4, &ho_RegionDilation5, 1, 60);

	//8)
	Difference(ho_RegionDilation5, ho_RegionErosion4, &ho_RegionDifference4);

	OpeningRectangle1(ho_RegionDifference4, &ho_RegionOpening3, 10, 10);

	Connection(ho_RegionOpening3, &ho_ConnectedRegions4);

	SelectShape(ho_ConnectedRegions4, &ho_SelectedRegions4, "area", "and", 500, 99999);

	//9)上下两条边缘
	CountObj(ho_SelectedRegions4, &hv_Number_edge);
	//如果为两条，则判断左右边缘是否为两条。这是为了判断屏幕有没有去到接近图像边缘
	if (0 != (hv_Number_edge == 2))
	{
		DilationRectangle1(ho_RegionErosion4, &ho_RegionDilation6, 60, 1);
		Difference(ho_RegionDilation6, ho_RegionErosion4, &ho_RegionDifference5);
		OpeningRectangle1(ho_RegionDifference5, &ho_RegionOpening4, 10, 10);
		Connection(ho_RegionOpening4, &ho_ConnectedRegions5);
		SelectShape(ho_ConnectedRegions5, &ho_SelectedRegions5, "area", "and", 20000,
			99999);
		CountObj(ho_SelectedRegions5, &hv_Number_edge);
		if (0 != (hv_Number_edge == 2))
		{
			//)选择上下边缘的第一条边缘
			SelectObj(ho_SelectedRegions4, &ho_ObjectSelected1, 1);
			//1)
			SmallestRectangle1(ho_ObjectSelected1, &hv_Row12, &hv_Column12, &hv_Row21,
				&hv_Column21);
			//2)
			ClipContoursXld(ho_Contours, &ho_ClippedContours, hv_Row12, hv_Column12, hv_Row21,
				hv_Column21);
			//3)
			SelectShapeXld(ho_ClippedContours, &ho_SelectedXLD, "max_diameter", "and",
				150, 99999);

			//)选择左右的第2条边缘
			SelectObj(ho_SelectedRegions5, &ho_ObjectSelected2, 2);
			//1)
			SmallestRectangle1(ho_ObjectSelected2, &hv_Row13, &hv_Column13, &hv_Row22,
				&hv_Column22);
			//2)
			ClipContoursXld(ho_Contours, &ho_ClippedContours1, hv_Row13, hv_Column13, hv_Row22,
				hv_Column22);
			//3)
			SelectShapeXld(ho_ClippedContours1, &ho_SelectedXLD1, "max_diameter", "and",
				150, 99999);

			//)选择上下边缘的第2条边缘
			SelectObj(ho_SelectedRegions4, &ho_ObjectSelected3, 2);
			//1)
			SmallestRectangle1(ho_ObjectSelected3, &hv_Row14, &hv_Column14, &hv_Row23,
				&hv_Column23);
			//2)
			ClipContoursXld(ho_Contours, &ho_ClippedContours2, hv_Row14, hv_Column14, hv_Row23,
				hv_Column23);
			//3)
			SelectShapeXld(ho_ClippedContours2, &ho_SelectedXLD2, "max_diameter", "and",
				150, 99999);

			//)选择左右的第1条边缘
			SelectObj(ho_SelectedRegions5, &ho_ObjectSelected4, 1);
			//1)
			SmallestRectangle1(ho_ObjectSelected4, &hv_Row15, &hv_Column15, &hv_Row24,
				&hv_Column24);
			//2)
			ClipContoursXld(ho_Contours, &ho_ClippedContours3, hv_Row15, hv_Column15, hv_Row24,
				hv_Column24);
			//3)
			SelectShapeXld(ho_ClippedContours3, &ho_SelectedXLD3, "max_diameter", "and",
				150, 99999);

			//）集合
			ConcatObj(ho_SelectedXLD, ho_SelectedXLD1, &ho_ObjectsConcat);
			ConcatObj(ho_ObjectsConcat, ho_SelectedXLD2, &ho_ObjectsConcat);
			ConcatObj(ho_ObjectsConcat, ho_SelectedXLD3, &ho_ObjectsConcat);
			//) 拟合直线，取直线焦点
			FitLineContourXld(ho_ObjectsConcat, "tukey", -1, 0, 5, 2, &hv_RowBegin, &hv_ColBegin,
				&hv_RowEnd, &hv_ColEnd, &hv_Nr, &hv_Nc, &hv_Dist);
			IntersectionLines(HTuple(hv_RowBegin[0]), HTuple(hv_ColBegin[0]), HTuple(hv_RowEnd[0]),
				HTuple(hv_ColEnd[0]), HTuple(hv_RowBegin[1]), HTuple(hv_ColBegin[1]), HTuple(hv_RowEnd[1]),
				HTuple(hv_ColEnd[1]), &hv_Row4, &hv_Column4, &hv_IsOverlapping);
			IntersectionLines(HTuple(hv_RowBegin[1]), HTuple(hv_ColBegin[1]), HTuple(hv_RowEnd[1]),
				HTuple(hv_ColEnd[1]), HTuple(hv_RowBegin[2]), HTuple(hv_ColBegin[2]), HTuple(hv_RowEnd[2]),
				HTuple(hv_ColEnd[2]), &hv_Row5, &hv_Column5, &hv_IsOverlapping1);
			IntersectionLines(HTuple(hv_RowBegin[2]), HTuple(hv_ColBegin[2]), HTuple(hv_RowEnd[2]),
				HTuple(hv_ColEnd[2]), HTuple(hv_RowBegin[3]), HTuple(hv_ColBegin[3]), HTuple(hv_RowEnd[3]),
				HTuple(hv_ColEnd[3]), &hv_Row6, &hv_Column6, &hv_IsOverlapping2);
			IntersectionLines(HTuple(hv_RowBegin[3]), HTuple(hv_ColBegin[3]), HTuple(hv_RowEnd[3]),
				HTuple(hv_ColEnd[3]), HTuple(hv_RowBegin[0]), HTuple(hv_ColBegin[0]), HTuple(hv_RowEnd[0]),
				HTuple(hv_ColEnd[0]), &hv_Row7, &hv_Column7, &hv_IsOverlapping3);
			hv_Row_Concat = HTuple();
			hv_Col_Concat = HTuple();
			TupleConcat(hv_Row4, hv_Row5, &hv_Row_Concat);
			TupleConcat(hv_Row_Concat, hv_Row6, &hv_Row_Concat);
			TupleConcat(hv_Row_Concat, hv_Row7, &hv_Row_Concat);

			TupleConcat(hv_Column4, hv_Column5, &hv_Col_Concat);
			TupleConcat(hv_Col_Concat, hv_Column6, &hv_Col_Concat);
			TupleConcat(hv_Col_Concat, hv_Column7, &hv_Col_Concat);
			//）连接成区域RegionScreen
			GenRegionPolygonFilled(&ho_RegionScreen, hv_Row_Concat, hv_Col_Concat);

			//inner_rectangle1 (Region6, Row16, Column16, Row25, Column25)
			//gen_rectangle1 (Rectangle2, Row16, Column16, Row25, Column25)
			//) 划分中心区域，亮度均匀
			ErosionRectangle1(ho_RegionScreen, &ho_RegionCenter, 20, 20);
			//）划分边缘区域，亮度比较不均匀，需特殊处理
			Difference(ho_RegionScreen, ho_RegionCenter, &ho_RegionMargin);

			(*ho_ScreenRegion) = ho_RegionScreen;
		}

	}



	return;
}

void SimpleDarkSpot(HObject ho_ImageSrc, HObject ho_ScreenRegion, HObject *ho_ResultRegion,
	HTuple hv_ThrHigh, HTuple hv_MinArea, HTuple hv_Delta, HTuple *hv_RoundGray)
{

	// Local iconic variables
	HObject  ho_RegionCent, ho_RegionEdge, ho_ImageEdge;
	HObject  ho_ImageGaussEdge, ho_ConnectedRegionsEdge, ho_SelectedRegionsEdge;
	HObject  ho_ImageReduced, ho_ImageGauss, ho_Regions2, ho_ConnectedRegions1;
	HObject  ho_SelectedRegions2, ho_RegionDilation0, ho_RegionFillUp;
	HObject  ho_RegionDilation, ho_RegionRound, ho_ObjectSelected1;
	HObject  ho_ObjectSelected2, ho_RegionDifference, ho_RegionIntersection;
	HObject  ho_ImageEmphasize, ho_ObjectSelected, ho_ObjectsConcat;

	// Local control variables
	HTuple  hv_Number, hv_Index, hv_Value, hv_Value2;
	HTuple  hv_Greater, hv_Indices, hv_selectIdx, hv_Selected;

	//分成边缘区域和中心区域（因为边缘区域太复杂灰度低）
	ErosionRectangle1(ho_ScreenRegion, &ho_RegionCent, 13, 13);
	Difference(ho_ScreenRegion, ho_RegionCent, &ho_RegionEdge);

	ReduceDomain(ho_ImageSrc, ho_RegionEdge, &ho_ImageEdge);
	GaussFilter(ho_ImageEdge, &ho_ImageGaussEdge, 5);
	Threshold(ho_ImageGaussEdge, &ho_RegionEdge, 0, 10);
	Connection(ho_RegionEdge, &ho_ConnectedRegionsEdge);
	SelectShape(ho_ConnectedRegionsEdge, &ho_SelectedRegionsEdge, "area", "and", hv_MinArea,
		9999999);

	//简单检测 黑点
	ReduceDomain(ho_ImageSrc, ho_RegionCent, &ho_ImageReduced);
	GaussFilter(ho_ImageReduced, &ho_ImageGauss, 5);
	Threshold(ho_ImageGauss, &ho_Regions2, 0, hv_ThrHigh);
	Connection(ho_Regions2, &ho_ConnectedRegions1);
	SelectShape(ho_ConnectedRegions1, &ho_SelectedRegions2, "area", "and", hv_MinArea,
		9999999);

	//形态学处理
	//connection (SelectedRegions2, ConnectedRegions2)
	//erosion_circle (ConnectedRegions2, RegionErosion, 1.5)
	//select_shape (RegionErosion, SelectedRegions3, 'area', 'and', 1, 9999999)
	DilationCircle(ho_SelectedRegions2, &ho_RegionDilation0, 3.5);

	//封闭
	FillUp(ho_RegionDilation0, &ho_RegionFillUp);
	DilationCircle(ho_RegionFillUp, &ho_RegionDilation, 10.5);

	CountObj(ho_RegionDilation, &hv_Number);
	GenEmptyRegion(&ho_RegionRound);
	{
		HTuple end_val29 = hv_Number;
		HTuple step_val29 = 1;
		for (hv_Index = 1; hv_Index.Continue(end_val29, step_val29); hv_Index += step_val29)
		{
			SelectObj(ho_RegionDilation, &ho_ObjectSelected1, hv_Index);
			SelectObj(ho_RegionDilation0, &ho_ObjectSelected2, hv_Index);
			Difference(ho_ObjectSelected1, ho_ObjectSelected2, &ho_RegionDifference);
			Intersection(ho_RegionCent, ho_RegionDifference, &ho_RegionIntersection);
			if (0 != (1 == hv_Index))
			{
				ho_RegionRound = ho_RegionIntersection;
			}
			else
			{
				ConcatObj(ho_RegionIntersection, ho_RegionRound, &ho_RegionRound);
			}

		}
	}



	//difference (RegionDilation, SelectedRegions3, RegionDifference)
	Emphasize(ho_ImageGauss, &ho_ImageEmphasize, 5, 5, 1);
	//缺陷周围的平均灰度
	GrayFeatures(ho_RegionRound, ho_ImageEmphasize, "mean", &hv_Value);
	//缺陷的平均灰度
	GrayFeatures(ho_RegionDilation0, ho_ImageEmphasize, "mean", &hv_Value2);
	TupleGreaterElem(hv_Value - hv_Value2, hv_Delta, &hv_Greater);
	TupleFind(hv_Greater, 1, &hv_Indices);
	hv_selectIdx = hv_Indices + 1;
	GenEmptyObj(&ho_ObjectSelected);
	hv_Selected = HTuple();
	if (0 != (HTuple(1 == (hv_selectIdx.TupleLength())).TupleAnd(0 == hv_selectIdx)))
	{

	}
	else
	{
		SelectObj(ho_RegionDilation0, &ho_ObjectSelected, hv_selectIdx);
		TupleSelectMask(hv_Value, hv_Greater, &hv_Selected);
	}
	ConcatObj(ho_SelectedRegionsEdge, ho_ObjectSelected, &ho_ObjectsConcat);
	(*ho_ResultRegion) = ho_ObjectsConcat;
	(*hv_RoundGray) = hv_Selected;
	//计算个数

	//计算面积

	//保存为显示

	return;
}

void CheckLightFuzzySpot(HObject ho_ImageMean, HObject ho_ImageFFT, HObject ho_ScreenRegion,
	HObject ho_MaskRegion, HObject *ho_ResultRegionLight, HTuple hv_HysteresisThrMin,
	HTuple hv_HysteresisThrMax, HTuple hv_MarginWidth, HTuple hv_fftThr, HTuple hv_AreaThr,
	HTuple hv_OffsetX, HTuple hv_OffsetY)
{

	// Local iconic variables
	HObject  ho_ImageScaleMaxFFT, ho_ImageScaleMaxMean;
	HObject  ho_RegionHysteresis, ho_Rectangle1, ho_RegionIntersection;
	HObject  ho_Regions1, ho_RegionIntersection1, ho_RegionUnion;
	HObject  ho_RegionUnionAll, ho_RegionUnionMask, ho_RegionDilationMask;
	HObject  ho_RegionUnionMaskTrans, ho_RegionDifferenceAll;
	HObject  ho_ConnectedRegions, ho_SelectedRegions;

	// Local control variables
	HTuple  hv_Width, hv_Height, hv_HomMat2DIdentity2;
	HTuple  hv_HomMat2DIdentity;

	//图片大小
	GetImageSize(ho_ImageMean, &hv_Width, &hv_Height);
	ScaleImageMax(ho_ImageFFT, &ho_ImageScaleMaxFFT);
	ScaleImageMax(ho_ImageMean, &ho_ImageScaleMaxMean);
	HysteresisThreshold(ho_ImageScaleMaxMean, &ho_RegionHysteresis, hv_HysteresisThrMin,
		hv_HysteresisThrMax, 10);

	//缩小范围(边缘不检测）
	GenRectangle1(&ho_Rectangle1, hv_MarginWidth, hv_MarginWidth, hv_Height - hv_MarginWidth,
		hv_Width - hv_MarginWidth);
	//取交集
	Intersection(ho_Rectangle1, ho_RegionHysteresis, &ho_RegionIntersection);
	Threshold(ho_ImageScaleMaxFFT, &ho_Regions1, 220, 255);
	//取交集
	Intersection(ho_Regions1, ho_Rectangle1, &ho_RegionIntersection1);

	//汇总
	Union1(ho_RegionIntersection, &ho_RegionUnion);
	Union2(ho_RegionUnion, ho_RegionIntersection1, &ho_RegionUnionAll);

	Union1(ho_MaskRegion, &ho_RegionUnionMask);
	DilationRectangle1(ho_RegionUnionMask, &ho_RegionDilationMask, 21, 21);
	HomMat2dIdentity(&hv_HomMat2DIdentity2);
	HomMat2dTranslate(hv_HomMat2DIdentity2, -hv_OffsetY, -hv_OffsetX, &hv_HomMat2DIdentity2);
	AffineTransRegion(ho_RegionDilationMask, &ho_RegionUnionMaskTrans, hv_HomMat2DIdentity2,
		"nearest_neighbor");
	Difference(ho_RegionUnionAll, ho_RegionUnionMaskTrans, &ho_RegionDifferenceAll);

	//最后筛选
	Connection(ho_RegionDifferenceAll, &ho_ConnectedRegions);
	SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, "area", "and", hv_AreaThr,
		9999999);

	//仿射变换
	HomMat2dIdentity(&hv_HomMat2DIdentity);
	HomMat2dTranslate(hv_HomMat2DIdentity, hv_OffsetY, hv_OffsetX, &hv_HomMat2DIdentity);
	AffineTransRegion(ho_SelectedRegions, &ho_SelectedRegions, hv_HomMat2DIdentity,
		"nearest_neighbor");
	(*ho_ResultRegionLight) = ho_SelectedRegions;
	return;
}

void SimpleLightSpot(HObject ho_ImageSrc, HObject ho_ScreenRegion, HObject *ho_ResultRegion,
	HTuple hv_ThrHigh, HTuple hv_MinArea, HTuple hv_Delta, HTuple *hv_RoundGray)
{

	// Local iconic variables
	HObject  ho_ImageReduced, ho_ImageGauss, ho_Regions2;
	HObject  ho_ConnectedRegions1, ho_SelectedRegions2, ho_RegionDilation0;
	HObject  ho_RegionFillUp, ho_RegionDilation, ho_RegionRound;
	HObject  ho_ObjectSelected1, ho_ObjectSelected2, ho_RegionDifference;
	HObject  ho_RegionIntersection, ho_ImageEmphasize, ho_ObjectSelected;

	// Local control variables
	HTuple  hv_Number, hv_Index, hv_Value, hv_Value2;
	HTuple  hv_Greater, hv_Indices, hv_selectIdx, hv_Selected;

	//简单检测 黑点
	ReduceDomain(ho_ImageSrc, ho_ScreenRegion, &ho_ImageReduced);
	GaussFilter(ho_ImageReduced, &ho_ImageGauss, 5);
	Threshold(ho_ImageGauss, &ho_Regions2, hv_ThrHigh, 255);
	Connection(ho_Regions2, &ho_ConnectedRegions1);
	SelectShape(ho_ConnectedRegions1, &ho_SelectedRegions2, "area", "and", hv_MinArea,
		9999999);

	//形态学处理
	//connection (SelectedRegions2, ConnectedRegions2)
	//erosion_circle (ConnectedRegions2, RegionErosion, 1.5)
	//select_shape (RegionErosion, SelectedRegions3, 'area', 'and', 1, 9999999)
	DilationCircle(ho_SelectedRegions2, &ho_RegionDilation0, 2.5);

	//封闭
	FillUp(ho_RegionDilation0, &ho_RegionFillUp);
	DilationCircle(ho_RegionFillUp, &ho_RegionDilation, 10.5);

	CountObj(ho_RegionDilation, &hv_Number);
	GenEmptyRegion(&ho_RegionRound);
	{
		HTuple end_val19 = hv_Number;
		HTuple step_val19 = 1;
		for (hv_Index = 1; hv_Index.Continue(end_val19, step_val19); hv_Index += step_val19)
		{
			SelectObj(ho_RegionDilation, &ho_ObjectSelected1, hv_Index);
			SelectObj(ho_RegionDilation0, &ho_ObjectSelected2, hv_Index);
			Difference(ho_ObjectSelected1, ho_ObjectSelected2, &ho_RegionDifference);
			Intersection(ho_RegionDifference, ho_RegionDifference, &ho_RegionIntersection
			);
			if (0 != (1 == hv_Index))
			{
				ho_RegionRound = ho_RegionIntersection;
			}
			else
			{
				ConcatObj(ho_RegionIntersection, ho_RegionRound, &ho_RegionRound);
			}

		}
	}



	//difference (RegionDilation, SelectedRegions3, RegionDifference)
	Emphasize(ho_ImageGauss, &ho_ImageEmphasize, 5, 5, 1);
	//缺陷周围的平均灰度
	GrayFeatures(ho_RegionRound, ho_ImageEmphasize, "mean", &hv_Value);
	//缺陷的平均灰度
	GrayFeatures(ho_RegionDilation0, ho_ImageEmphasize, "mean", &hv_Value2);
	TupleGreaterElem(hv_Value2 - hv_Value, hv_Delta, &hv_Greater);
	TupleFind(hv_Greater, 1, &hv_Indices);
	hv_selectIdx = hv_Indices + 1;
	GenEmptyObj(&ho_ObjectSelected);
	hv_Selected = HTuple();
	if (0 != (HTuple(1 == (hv_selectIdx.TupleLength())).TupleAnd(0 == hv_selectIdx)))
	{

	}
	else
	{
		SelectObj(ho_RegionDilation0, &ho_ObjectSelected, hv_selectIdx);
		TupleSelectMask(hv_Value, hv_Greater, &hv_Selected);
	}
	(*ho_ResultRegion) = ho_ObjectSelected;
	(*hv_RoundGray) = hv_Selected;
	//计算个数

	//计算面积

	//保存为显示

	return;
}

void CheckLightSpotInDarkScreen(HObject ho_ImageSrc, HObject ho_RegionScreen, HObject *ho_RegionResult,
	HTuple hv_HysteresisMin, HTuple hv_HysteresisMax, HTuple hv_AreaThr, HTuple hv_MeanSize,
	HTuple hv_DynThr)
{

	// Local iconic variables
	HObject  ho_ImageReduced, ho_RegionHysteresis;
	HObject  ho_ConnectedRegions, ho_SelectedRegions, ho_ImageMean;
	HObject  ho_RegionDynThresh, ho_ConnectedRegions1, ho_SelectedRegions1;
	HObject  ho_ObjectsConcat, ho_RegionUnion, ho_ConnectedRegions2;

	// Local control variables
	HTuple  hv_Number, hv_hv_Dynthr;


	//1)
	CountObj(ho_RegionScreen, &hv_Number);
	GenEmptyObj(&ho_ImageReduced);
	if (0 != hv_Number)
	{
		ReduceDomain(ho_ImageSrc, ho_RegionScreen, &ho_ImageReduced);
	}
	else
	{
		CopyImage(ho_ImageSrc, &ho_ImageReduced);
	}

	//reduce_domain (ImageSrc, RegionScreen, ImageReduced)
	HysteresisThreshold(ho_ImageReduced, &ho_RegionHysteresis, hv_HysteresisMin, hv_HysteresisMax,
		10);
	Connection(ho_RegionHysteresis, &ho_ConnectedRegions);
	SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, "area", "and", hv_AreaThr,
		9999999);

	//2)
	hv_hv_Dynthr = 10;
	MeanImage(ho_ImageReduced, &ho_ImageMean, hv_MeanSize, hv_MeanSize);
	DynThreshold(ho_ImageReduced, ho_ImageMean, &ho_RegionDynThresh, hv_DynThr, "light");
	Connection(ho_RegionDynThresh, &ho_ConnectedRegions1);
	SelectShape(ho_ConnectedRegions1, &ho_SelectedRegions1, "area", "and", hv_AreaThr,
		9999999);

	//3)
	ConcatObj(ho_SelectedRegions, ho_SelectedRegions1, &ho_ObjectsConcat);
	Union1(ho_ObjectsConcat, &ho_RegionUnion);

	//4) 除尘等

	//5)
	Connection(ho_RegionUnion, &ho_ConnectedRegions2);
	(*ho_RegionResult) = ho_ConnectedRegions2;
	return;
}
void CheckLeakBlack(HObject ho_ImageSrc, HObject ho_RegionScreen, HObject *ho_RegionResult)
{

	// Local iconic variables
	HObject  ho_ImageReduced, ho_ImageGauss, ho_ImageZoomed;
	HObject  ho_ImageMedian, ho_RegionErosion, ho_ImageReduced1;
	HObject  ho_ExpandedImage, ho_ImageReduced2;

	// Local control variables
	HTuple  hv_Number;

	//0)//////// 检测漏光////////

	//1) 如果有指定屏幕区域则reduce， 否则全部
	CountObj(ho_RegionScreen, &hv_Number);
	GenEmptyObj(&ho_ImageReduced);
	if (0 != (hv_Number>0))
	{
		ReduceDomain(ho_ImageSrc, ho_RegionScreen, &ho_ImageReduced);
	}
	else
	{
		CopyImage(ho_ImageSrc, &ho_ImageReduced);
	}

	//2)
	GaussFilter(ho_ImageReduced, &ho_ImageGauss, 5);
	ZoomImageFactor(ho_ImageGauss, &ho_ImageZoomed, 0.5, 0.5, "constant");
	MedianImage(ho_ImageZoomed, &ho_ImageMedian, "circle", 20, "mirrored");
	ErosionRectangle1(ho_ImageMedian, &ho_RegionErosion, 200, 200);
	ReduceDomain(ho_ImageMedian, ho_RegionErosion, &ho_ImageReduced1);
	ExpandDomainGray(ho_ImageReduced1, &ho_ExpandedImage, 120);
	ReduceDomain(ho_ExpandedImage, ho_ImageMedian, &ho_ImageReduced2);
	return;
}
void JudgeResult(HObject ho_ResultRegions, HObject ho_ImageSrc, HObject *ho_ImageParts,
	HTuple hv_ImageW, HTuple hv_ImageH, HTuple hv_BoxMinW, HTuple hv_BoxMinH, HTuple hv_MaxShowNum,
	HTuple *hv_Areas, HTuple *hv_CentRow, HTuple *hv_CentCol, HTuple *hv_Row1, HTuple *hv_Col1,
	HTuple *hv_Row2, HTuple *hv_Col2)
{

	// Local iconic variables
	HObject  ho_ImagePart;

	// Local control variables
	HTuple  hv_roundRow1, hv_roundCol1, hv_roundRow2;
	HTuple  hv_roundCol2, hv_w, hv_h, hv_boxW, hv_boxH, hv_Index;


	//计算面积中心坐标
	AreaCenter(ho_ResultRegions, &(*hv_Areas), &(*hv_CentRow), &(*hv_CentCol));
	SmallestRectangle1(ho_ResultRegions, &hv_roundRow1, &hv_roundCol1, &hv_roundRow2,
		&hv_roundCol2);
	hv_w = hv_roundCol2 - hv_roundCol1;
	hv_h = hv_roundRow2 - hv_roundRow1;
	//2) 计算外包围矩形的位置
	hv_boxW = hv_BoxMinW;
	hv_boxH = hv_BoxMinH;
	GenEmptyObj(&(*ho_ImageParts));
	{
		HTuple end_val10 = ((*hv_Areas).TupleLength()) - 1;
		HTuple step_val10 = 1;
		int count = 1;
		for (hv_Index = 0; hv_Index.Continue(end_val10, step_val10); hv_Index += step_val10)
		{
			//3) 小于指定值的， 截图框为最小的指定宽度BoxMinW,
			if (0 != (HTuple(HTuple(hv_w[hv_Index])<hv_BoxMinW).TupleAnd(HTuple(hv_h[hv_Index])<hv_BoxMinH)))
			{
				hv_boxW = hv_BoxMinW;
				hv_boxH = hv_BoxMinH;
			}
			else if (0 != (HTuple(HTuple(hv_w[hv_Index])<hv_BoxMinW).TupleAnd(HTuple(hv_h[hv_Index]) >= hv_BoxMinH)))
			{
				hv_boxW = hv_BoxMinW;
				hv_boxH = ((const HTuple&)hv_h)[hv_Index];
			}
			else if (0 != (HTuple(HTuple(hv_w[hv_Index]) >= hv_BoxMinW).TupleAnd(HTuple(hv_h[hv_Index]) == hv_BoxMinH)))
			{
				hv_boxW = ((const HTuple&)hv_w)[hv_Index];
				hv_boxH = hv_BoxMinH;
			}
			else
			{
				hv_boxW = ((const HTuple&)hv_w)[hv_Index];
				hv_boxH = ((const HTuple&)hv_h)[hv_Index];
			}
			HTuple R1, C1, R2, C2;
			R1 = HTuple((*hv_CentRow)[hv_Index]) - (hv_boxH / 2.0);
			C1 = HTuple((*hv_CentCol)[hv_Index]) - (hv_boxW / 2.0);
			R2 = HTuple((*hv_CentRow)[hv_Index]) + (hv_boxH / 2.0);
			C2 = HTuple((*hv_CentCol)[hv_Index]) + (hv_boxW / 2.0);

			if (0 != (R1<0))
			{
				R1 = 0;
			}
			if (0 != (C1<0))
			{
				C1 = 0;
			}
			if (0 != (R2 >= hv_ImageH))
			{
				R2 = hv_ImageH - 1;
			}
			if (0 != (C2 >= hv_ImageW))
			{
				C2 = hv_ImageW - 1;
			}

			(*hv_Row1).Append(R1);
			(*hv_Col1).Append(C1);
			(*hv_Row2).Append(R2);
			(*hv_Col2).Append(C2);
			//4) 截图
			//CropPart(ho_ImageSrc, &ho_ImagePart, R1, C1, hv_boxW, hv_boxH);
			//ConcatObj((*ho_ImageParts), ho_ImagePart, &(*ho_ImageParts));
			//if (count >= hv_MaxShowNum)
			//{
			//	break;
			//}
			count++;
		}
	}





	return;
}
