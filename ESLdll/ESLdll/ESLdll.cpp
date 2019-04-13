// ESLdll.cpp : ���� DLL �ĳ�ʼ�����̡�
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

#define WM_MYMSG WM_USER+88   //ʵʱ��ʾ

#define WHITESCREEN_RESULT_FILE "Result\\WhiteResult.txt"  //����ļ��ļ���
#define BLACKSCREEN_RESULT_FILE "Result\\BlackResult.txt"
#define GRAYSCALE_RESULT_FILE "Result\\GrayResult.txt"
#define SCREENREGION_RESULT_FILE "Result\\ScreenRegionResult.txt"
#define CHESSBOARD_RESULT_FILE "Result\\ChessResult.txt"//���̸������ļ�


#define WHITESCREEN_RESULT_IMG "ResImg\\ResultFAIL_WhiteScreen_%d_%d__%d��%d��%d" //NGͼ����
#define BLACKSCREEN_RESULT_IMG "ResImg\\ResultFAIL_BlackScreen_%d_%d__%d��%d��%d"
#define GRAYSCALE_RESULT_IMG "ResImg\\ResultFAIL_GrayResult_%d_%d__%d��%d��%d"
#define CHESSBOARD_RESULT_IMG "ResImg\\ResultFAIL_ChessResult_%d_%d__%d��%d��%d"
#define SCREENREGION_RESULT_IMG "ResImg\\Result_ScreenRegion"

#define NODE_WHITESCREEN_RES "WhiteScreenResult" //�ڵ�
#define NODE_BLACKSCREEN_RES "BlackScreenResult"
#define NODE_GRAYSCALE_RES "GrayScaleResult"
#define NODE_SCREENREGION_RES "ScreenRegionResult"
#define NODE_CHESSBOARD_RES "ChessBoardResult"

#define SUBNODE_COUNT "Count" //�ӽڵ�
#define SUBNODE_AREA "Area"
#define SUBNODE_MSG "Message"
#define SUBNODE_FAILPATH "FailImagePath"
#define SUBNODE_RES "Result"

#define RESULT_PASS "PASS"
#define RESULT_FAIL "FAIL"

#define RESULT_MSG_PASS "PASS"
#define RESULT_MSG_FAIL_GRAY "GRAY FAIL" //�Ҷȳ�������
#define RESULT_MSG_FAIL_DEFECT "DEFECT FAIL" //��⵽ȱ��

#define CONFIG_FILE "Config\\ESLconfig.ini"  //�����ļ��ļ���
#define PATH_CAM_CONFIG "Config\\CamConfig.ini" //��������ļ���
#define NODE_WHITESCREEN_CONFIG "WhiteScreenConfig" //�ڵ�
#define NODE_BLACKSCREEN_CONFIG "BlackScreenConfig"
#define NODE_GRAYSCALE_CONFIG "GrayScaleConfig"
#define NODE_SCREENREGION_CONFIG "ScreenRegionConfig"
#define NODE_CHESSBOARD_CONFIG "ChessBoardConfig"
#define NODE_CAM_CONFIG "CamConfig" //�������

//
//TODO:  ����� DLL ����� MFC DLL �Ƕ�̬���ӵģ�
//		��Ӵ� DLL �������κε���
//		MFC �ĺ������뽫 AFX_MANAGE_STATE ����ӵ�
//		�ú�������ǰ�档
//
//		����: 
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// �˴�Ϊ��ͨ������
//		}
//
//		�˺������κ� MFC ����
//		������ÿ��������ʮ����Ҫ��  ����ζ��
//		��������Ϊ�����еĵ�һ�����
//		���֣������������ж������������
//		������Ϊ���ǵĹ��캯���������� MFC
//		DLL ���á�
//
//		�й�������ϸ��Ϣ��
//		����� MFC ����˵�� 33 �� 58��
//

// CESLdllApp

BEGIN_MESSAGE_MAP(CESLdllApp, CWinApp)
END_MESSAGE_MAP()


// CESLdllApp ����

CESLdllApp::CESLdllApp()
{
	// TODO:  �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CESLdllApp ����

CESLdllApp theApp;
HObject  /*ho_RegionScreen,*/ ho_RegionCenter, ho_RegionMargin;
HTuple hv_WindowHandle;
HTuple  hv_AcqHandle;  //���
HTuple hv_Graystd, hv_Graymax, hv_Graymin; //�ҽ׵ı�׼ֵ�����ֵ����Сֵ�� 
HTuple hv_Whitestd, hv_Whitemax, hv_Whitemin; //���� ������ ��׼ֵ�� ���ֵ�� ��Сֵ��
HTuple hv_Chessstd, hv_Chessmax, hv_Chessmin; //���̸� ��׼ֵ�� ���ֵ�� ��Сֵ��

CString m_strCamName = "default";
CString m_strCamIniFile = "";

HObject g_Image;	   //rgbͼ��
HObject g_ImageGray;	//�Ҷ�ͼ��
HObject g_ImageRed;    //��ɫ����ͼ��
HObject g_ImageGreen;  //��ɫ����ͼ��
HObject g_ImageBlue;   //��ɫ����ͼ��
HObject g_RegionScreen;//��Ļ����

HObject g_ImageParts;

ParaForLcd g_Para;     //������

ParaForLcd g_Para_White; //��ɫ��Ļ�ļ����������ʱ������g_Para;
ParaForLcd g_Para_Red;
ParaForLcd g_Para_Green;
ParaForLcd g_Para_Blue;
ParaForLcd g_Para_Black;

ResultForLcd g_Result;	//�����
std::vector<ResultForLcd> g_vctResult; //ȱ�ݵĽ������

CWnd* g_pWnd = NULL;
HTuple hv_WindowHandle_White; //�����ʾ����
HTuple hv_WindowHandle_Red;
HTuple hv_WindowHandle_Green;
HTuple hv_WindowHandle_Blue;
HTuple hv_WindowHandle_Black;

//���ʹ������
CheckMe pCheckme = NULL;
RegistMe pRegistme = NULL;

//�������
DvpCamera myCam;

//��������������Ϣ
std::string strErrorMsg[] = {
	"��������",
	"OK",
	"��Ļ����NG",
	"����",
	"����",
	"����",
	"����",
	"����",
	"����",
	"MURA",
	"�ҳ�",
	"©��" 
};
///////////////////////////////////////////////
//�㷨����
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
//�쳣������
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

//�ָ��16���֣���ȡÿ���ֵ����Ĳ���
//ho_RegionSrc		Ҫ�ָ����Ļ����
//ho_RegionRects	16�����־�������
//hv_rownum			Ҫ�ָ������
//hv_colnum			Ҫ�ָ������
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

//�ȽϻҶ�ֵ�� �������0��1��2
//hv_GrayTest  �Ƚ϶���
//hv_GrayStds  ��׼ֵ
//hv_GrayMax   ���ֵ��Ҳ����Ϊ������
//hv_GrayMin   ��Сֵ��Ҳ����Ϊ������
//mode   ģʽ 0�� �Ƚ����ֵ��Сֵ�� 1�� �Ƚϱ�׼ֵ��������
//hv_GrayDiffIdx ���س��ֳ������±�
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

//��Ļ�����Զ�ʶ��, ������4x4�ĻҶ�ֵ
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

	//)һ����Ļ����ָ�
	//ReadImage(&ho_Image, "E:/��Ļ���/������/pic/whiteFAKE_NG2.jpg");
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
	//)��ϳɱ�׼����
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

	//9)����������Ե
	CountObj(ho_SelectedRegions4, &hv_Number_edge);
	//���Ϊ���������ж����ұ�Ե�Ƿ�Ϊ����������Ϊ���ж���Ļ��û��ȥ���ӽ�ͼ���Ե
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
			//)ѡ�����±�Ե�ĵ�һ����Ե
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

			//)ѡ�����ҵĵ�2����Ե
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

			//)ѡ�����±�Ե�ĵ�2����Ե
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

			//)ѡ�����ҵĵ�1����Ե
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

			//������
			ConcatObj(ho_SelectedXLD, ho_SelectedXLD1, &ho_ObjectsConcat);
			ConcatObj(ho_ObjectsConcat, ho_SelectedXLD2, &ho_ObjectsConcat);
			ConcatObj(ho_ObjectsConcat, ho_SelectedXLD3, &ho_ObjectsConcat);
			//) ���ֱ�ߣ�ȡֱ�߽���
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
			//�����ӳ�����RegionScreen
			GenRegionPolygonFilled(&g_RegionScreen, hv_Row_Concat, hv_Col_Concat);

			//inner_rectangle1 (Region6, Row16, Column16, Row25, Column25)
			//gen_rectangle1 (Rectangle2, Row16, Column16, Row25, Column25)
			//) ���������������Ⱦ���
			ErosionRectangle1(g_RegionScreen, &ho_RegionCenter, 20, 20);
			//�����ֱ�Ե�������ȱȽϲ����ȣ������⴦��
			Difference(g_RegionScreen, ho_RegionCenter, &ho_RegionMargin);

			//����4X4�Ҷ�ֵ�����浽�ļ���
			HObject ho_RegionRects;
			HObject ho_ImageGrayMean;
			HTuple hv_Mean1, hv_Deviation;
			CIni iniFile;
			char pT[1024] = {0};
			cutpart(g_RegionScreen, &ho_RegionRects, 4, 4); //�ָ��4��4
			MeanImage(ho_ImageB, &ho_ImageGrayMean, 5, 5);   //��ֵ�˲�
			Intensity(ho_RegionRects, ho_ImageGrayMean, &hv_Mean1, &hv_Deviation); //����Ҷ�ֵ
																				   //д���ļ�����
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
			//����ʧ�ܵ�ͼƬ
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

//���⼴�������ҶȺͺ�ɫȱ�ݣ������ȵ���screenRegion ��ȡ��Ļ����
//ho_Image ����ͼ��
//bflag    �Ƿ���ʾͼ��
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
	//��2018.08.24 ��ʼʶ����Ļ�������ڰ�����������ʮ������Ļ���
	//) 2018.08.28 ����ʶ��16����Ľ��ޣ��Զ��ָ���жϣ�����ɺ���ָ��Ҫ�Ż�
	//) 2018.08.28 ��ʼ���Ӵ�ֱ����Ľ��޲���
	//) 2018.08.29 ��ʼ�з�Ϊ16��, ����� ֻ�ܸ��9�飬���Ե��ӵĲ��ָܷ��
	//) 2018.08.29

	try
	{
		if (0)
		{
			// ����4X4�����ڵĻҶ�ֵ�����ͱ�׼ֵ�Ƚϣ� ����Ҷ��Ѿ��쳣���򲻽���ȱ�ݼ��
			HalconCpp::ReduceDomain(ho_Image, g_RegionScreen, &ho_ImageGrayReducedTest);
			cutpart(g_RegionScreen, &ho_RegionRects, 4, 4); //����Ļ����ָ��4��4
			HalconCpp::MeanImage(ho_ImageGrayReducedTest, &ho_ImageGrayMean, 5, 5);  //ƽ��
			Intensity(ho_RegionRects, ho_ImageGrayMean, &hv_Mean1, &hv_Deviation); //����Ҷ�ֵ															   //���׼ֵ�Ƚ�
			compareGray(hv_Mean1, hv_Whitestd, hv_Whitemax, hv_Whitemin, 1, &hv_Diffidx);
			//д���ļ�����
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
				//�жϱ�׼ֵ
				HObject ho_ObjectSelected1;
				for (int i = 0; i < hv_Diffidx.Length(); i++)
				{
					str.Format(_T("GrayIndex%d"), i);
					iniFile.SetValue(NODE_WHITESCREEN_RES, str, hv_Diffidx[i].I());

					//����ͼ����
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
				//return true; //ֱ�ӷ��أ������ȱ��
			}
		}
		

		//�� ���������������ȱ��
		//1). ������ģ
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

		//)  ��������Ե�����ȱ��
		ReduceDomain(ho_ImageB, ho_RegionMargin, &ho_ImageMargin);
		MeanImage(ho_ImageMargin, &ho_ImageMean0, 3, 3);
		DynThreshold(ho_ImageMargin, ho_ImageMean0, &ho_RegionDynThresh0, 15, "dark");

		//)  �ģ�����
		Union2(ho_RegionHysteresis, ho_RegionDynThresh0, &ho_RegionUnion);

		//)  �壬������������

		Connection(ho_RegionUnion, &ho_ConnectedRegionUnion);
		SelectShape(ho_ConnectedRegionUnion, &ho_SelectedRegionUnion, "area", "and", 10, 99999);
		CountObj(ho_SelectedRegionUnion, &hv_Number_Blob);
		Union1(ho_SelectedRegionUnion, &ho_RegionUnion3);
		DilationCircle(ho_RegionUnion3, &ho_RegionUnion3, 5);
		if (0 != (hv_Number_Blob < 1)) //�Ҳ���
		{
			iniFile.SetValue(NODE_WHITESCREEN_RES, SUBNODE_MSG, RESULT_PASS);  //
			iniFile.SetValue(NODE_WHITESCREEN_RES, SUBNODE_COUNT, 0);  //
			iniFile.SetValue(NODE_WHITESCREEN_RES, SUBNODE_RES, RESULT_PASS);  //
		}
		else
		{
			//����ʧ�ܵ�ͼƬ
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
				iniFile.SetValue(NODE_WHITESCREEN_RES, str, hArea.I());  //���
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
		//)������ ======================================================
		//ReadImage(&ho_ImageBlackRaw, "E:/��Ļ���/������/pic/blackFAKE_NG.jpg");
		Decompose3(ho_Image, &ho_ImageBlackR, &ho_ImageBlackB, &ho_ImageBlackG);
		//)��������
		ReduceDomain(ho_ImageBlackB, ho_RegionCenter, &ho_ImageBlackCenter);
		BinaryThreshold(ho_ImageBlackCenter, &ho_RegionBlackDefect, "max_separability",
			"dark", &hv_UsedThreshold1);
		Threshold(ho_ImageBlackCenter, &ho_Regions1, hv_UsedThreshold1, 255);
		//)��Ե����
		ReduceDomain(ho_ImageBlackB, ho_RegionMargin, &ho_ImageBlackMargin);
		MeanImage(ho_ImageBlackMargin, &ho_ImageBlackMean0, 3, 3);
		DynThreshold(ho_ImageBlackMargin, ho_ImageBlackMean0, &ho_RegionBlackDynThresh0,
			10, "light");
		ClosingRectangle1(ho_RegionBlackDynThresh0, &ho_RegionBlackClosing, 3, 3);
		//)  �ģ�����
		Union2(ho_RegionBlackClosing, ho_Regions1, &ho_RegionBlackUnion);
		//)  �壬������������
		Connection(ho_RegionBlackUnion, &ho_RegionConnected);
		SelectShape(ho_RegionConnected, &ho_SelectedRegionUnion, "area", "and", 1, 99999);
		CountObj(ho_SelectedRegionUnion, &hv_Number_Blob);
		Union1(ho_SelectedRegionUnion, &ho_RegionUnion);
		DilationCircle(ho_RegionUnion, &ho_RegionUnion, 5);
		if (0 != (hv_Number_Blob<1)) //�Ҳ���ȱ��
		{
			iniFile.SetValue(NODE_BLACKSCREEN_RES, SUBNODE_MSG, RESULT_PASS);  //
			iniFile.SetValue(NODE_BLACKSCREEN_RES, SUBNODE_RES, RESULT_PASS);  //
		}
		else
		{
			//����ʧ�ܵ�ͼƬ
			GetLocalTime(&curT);
			HObject ho_ImageResult;
			HTuple hv_gray = 255;
			hv_gray.Append(0);
			hv_gray.Append(0);
			PaintRegion(ho_RegionUnion, ho_Image, &ho_ImageResult, hv_gray, "margin");
			sprintf(pT, BLACKSCREEN_RESULT_IMG, curT.wMonth, curT.wDay, curT.wHour, curT.wMinute, curT.wSecond);
			WriteImage(ho_ImageResult, "bmp", 0, (HTuple)pT);
			//�����ĵ�
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
				iniFile.SetValue(NODE_BLACKSCREEN_RES, str, hArea.I());  //���
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
	
	if (!iniFile.GetValue(NODE_CAM_CONFIG, "Name", m_strCamName)) //���ֵ
	{
		return false;
	}
	if (!iniFile.GetValue(NODE_CAM_CONFIG, "IniFile", m_strCamIniFile)) //���ֵ
	{
		return false;
	}
	return true;
}



//���ػҽײ��Ա�׼
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
			if (!iniFile.GetValue(NODE_GRAYSCALE_CONFIG, str, gray)) //��׼ֵ
			{
				return false;
			}
			hv_Graystd[cnt - 1] = gray;
			if (!iniFile.GetValue(NODE_GRAYSCALE_CONFIG, strmax, gray)) //���ֵ
			{
				return false;
			}
			hv_Graymax[cnt - 1] = gray;
			if (!iniFile.GetValue(NODE_GRAYSCALE_CONFIG, strmin, gray)) //��Сֵ
			{
				return false;
			}
			hv_Graymin[cnt - 1] = gray;
			cnt++;
		}
	}
	return true;
}

//���ر���������Ա�׼
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
			if (!iniFile.GetValue(NODE_WHITESCREEN_CONFIG, str, gray)) //��׼ֵ
			{
				return false;
			}
			hv_Whitestd[cnt - 1] = gray;
			if (!iniFile.GetValue(NODE_WHITESCREEN_CONFIG, strmax, gray)) //���ֵ,��������
			{
				return false;
			}
			hv_Whitemax[cnt - 1] = gray;
			if (!iniFile.GetValue(NODE_WHITESCREEN_CONFIG, strmin, gray)) //��Сֵ,������
			{
				return false;
			}
			hv_Whitemin[cnt - 1] = gray;
			cnt++;
		}
	}
	return true;
}

//�������̸���Ա�׼
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
			if (!iniFile.GetValue(NODE_CHESSBOARD_CONFIG, str, gray)) //��׼ֵ
			{
				return false;
			}
			hv_Chessstd[cnt - 1] = gray;
			if (!iniFile.GetValue(NODE_CHESSBOARD_CONFIG, strmax, gray)) //���ֵ,��������
			{
				return false;
			}
			hv_Chessmax[cnt - 1] = gray;
			if (!iniFile.GetValue(NODE_CHESSBOARD_CONFIG, strmin, gray)) //��Сֵ,������
			{
				return false;
			}
			hv_Chessmin[cnt - 1] = gray;
			cnt++;
		}
	}
	return true;
}

//�ҽײ���
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
		cutpart(g_RegionScreen, &ho_RegionRects, 4, 4); //�ָ��4��4
		MeanImage(ho_ImageGrayReducedTest, &ho_ImageGrayMean, 5, 5);
		Intensity(ho_RegionRects, ho_ImageGrayMean, &hv_Mean1, &hv_Deviation); //����Ҷ�ֵ

																			   //���׼ֵ�Ƚ�
		compareGray(hv_Mean1, hv_Graystd, hv_Graymax, hv_Graymin, 1, &hv_Diffidx);

		//д���ļ�����
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

			//����ͼ����

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
		cutpart(g_RegionScreen, &ho_RegionRects, 4, 4); //�ָ��4��4
		MeanImage(ho_ImageGrayReducedTest, &ho_ImageGrayMean, 5, 5);
		Intensity(ho_RegionRects, ho_ImageGrayMean, &hv_Mean1, &hv_Deviation); //����Ҷ�ֵ
		compareGray(hv_Mean1, hv_Chessstd, hv_Chessmax, hv_Chessmin, 1, &hv_Diffidx);//���׼ֵ�Ƚ�

		//д���ļ�����
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

			//����ͼ����

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
//���ý����ʾ���ڣ� 
bool EslSetResWnd(int SrcreenType, CWnd* pWnd)
{
	try
	{
		HTuple wndHandle;
		//����
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

//���ʹ������
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

	//����dll��ʼ������
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
	case 2://��ȡ���˵�ǰ���Ե�MAC
		AfxMessageBox("��ȡ���˵�ǰ�������к�"); break;
		break;
	case 3:
		AfxMessageBox("��ǰ�������кŲ�ƥ��"); break;
		break;
	case 4:
		break;
	case 5:
		break;
	case 6://д����ʱ�䷢������
		AfxMessageBox("�����������"); break;
		break;
	case 7:
		AfxMessageBox("ʱ�䲻��ȷ"); break;
		break;
	case 8:
		AfxMessageBox("����ѹ���"); break;
		break;
	case -1: //û��ע���
		AfxMessageBox("���δע��"); break;
		//if (pRegistme())
		//{
		//	AfxMessageBox("ע��fail");
		//}
		//else
		//{
		//	AfxMessageBox("ע��successed");
		//}
		break;
	default:
		break;
	}

	FreeLibrary(dllHandle);
	return re;
}

//��ʼ��dll
bool EslInitDll(CWnd* pWnd)
{
	try
	{
		//if (0 != CheckKey()) return false;
		//����������
		//����
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
		//�����㷨���
		if (!g_Para_White.LoadPara(PATH_WHITE_SRC_PARA))
		{
			AfxMessageBox("���ذ�������ʧ��");
		}
		if (!g_Para_Red.LoadPara(PATH_RED_SRC_PARA))
		{
			AfxMessageBox("���غ�������ʧ��");
		}
		if (!g_Para_Green.LoadPara(PATH_GREEN_SRC_PARA))
		{
			AfxMessageBox("������������ʧ��");
		}
		if (!g_Para_Blue.LoadPara(PATH_BLUE_SRC_PARA))
		{
			AfxMessageBox("������������ʧ��");
		}
		if (!g_Para_Black.LoadPara(PATH_BLACK_SRC_PARA))
		{
			AfxMessageBox("���غ�������ʧ��");
		}

		//�������
		loadConfig(PATH_CAM_CONFIG);
		char pstr[1024];
		memset(pstr, '\0', 1024);
		sprintf_s(pstr, "%s", m_strCamName);
		//if (hv_AcqHandle.Length())
		//	CloseFramegrabber(hv_AcqHandle);
		//AfxMessageBox(pstr);
		//�������
		if (myCam.OpenCamera(pstr) != DVP_STATUS_OK)
			return false;
		else
		{
			char pInifile[1024];
			memset(pInifile, '\0', 1024);
			sprintf_s(pInifile, "%s", m_strCamIniFile);
			myCam.LoadConfig(pInifile);
			myCam.StartCamera(); //��ʼ������
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
//����ʼ��dll
bool EslUnInitDll()
{
	//if(hv_AcqHandle.Length())
	//	CloseFramegrabber(hv_AcqHandle);//�ر�������ر�
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


//�ر���ʾ����
extern "C" _declspec(dllexport) bool closeShowWnd()
{
	if (HDevWindowStack::IsOpen())
		CloseWindow(HDevWindowStack::Pop());
	return true;
}

//
//��������·���м���ͼƬ������ʶ�����Ļ����
//������pPath ͼƬ·��
//      bflag �Ƿ���ʾ����ͼ�񴰿�
//����ֵ��false �����쳣  true �ɹ�
extern "C" _declspec(dllexport) bool screenRegionFromPath(char* pPath, bool bflag = true)
{
	HObject ho_Image;
	ReadImage(&ho_Image, pPath);
	return screenRegion(ho_Image, bflag);  //��ȡ��Ļ����
}
//������������м���ͼƬ������ʶ�����Ļ����
//������
//      bflag �Ƿ���ʾ����ͼ�񴰿�
//����ֵ��false �����쳣  true �ɹ�
extern "C" _declspec(dllexport) bool screenRegionFromCam(bool bflag = true)
{
	bool ret = false;
	HObject ho_Image;
	//ReadImage(&ho_Image, pPath);
	if(getImageFromCam(ho_Image))
		ret = screenRegion(ho_Image, bflag);//��ȡ��Ļ����
	return ret;
}

//
//��������·���м���ͼƬ��������Ļ�����ڵĺ�ɫ���㡣
//������pPath ͼƬ·��
//      bflag �Ƿ���ʾ����ͼ�񴰿�
//����ֵ��false �����쳣  true �ɹ�
extern "C" _declspec(dllexport) bool whiteScreenFromPath(char* pPath, bool bflag = true) 
{
	HObject ho_Image;
	ReadImage(&ho_Image, pPath);
	if (!loadWhiteStd(CONFIG_FILE)) //���ر�׼ֵ
	{
		return false;
	}
	return whiteScreen(ho_Image, bflag);
}
//���������������ͼƬ��������Ļ�����ڵĺ�ɫ���㡣
//������
//      bflag �Ƿ���ʾ����ͼ�񴰿�
//����ֵ��false �����쳣  true �ɹ�
extern "C" _declspec(dllexport) bool whiteScreenFromCam(bool bflag = true)
{
	bool ret = false;
	HObject ho_Image;
	//ReadImage(&ho_Image, pPath);
	
	if (!loadWhiteStd(CONFIG_FILE)) //���ر�׼ֵ
	{
		return false;
	}
	if (getImageFromCam(ho_Image))
		ret = whiteScreen(ho_Image, bflag);
	return ret;
}

//
//��������·���м���ͼƬ��������Ļ�����ڵİ�ɫ���㡣
//������pPath ͼƬ·��
//      bflag �Ƿ���ʾ����ͼ�񴰿�
//����ֵ��false �����쳣  true �ɹ�
extern "C" _declspec(dllexport) bool blackScreenFromPath(char* pPath, bool bflag = true)
{
	HObject ho_Image;
	ReadImage(&ho_Image, pPath);
	return blackScreen(ho_Image, bflag);
}
//������������м���ͼƬ��������Ļ�����ڵİ�ɫ���㡣
//������
//      bflag �Ƿ���ʾ����ͼ�񴰿�
//����ֵ��false �����쳣  true �ɹ�
extern "C" _declspec(dllexport) bool blackScreenFromCam(bool bflag = true)
{
	HObject ho_Image;
	bool ret = false;
	if (getImageFromCam(ho_Image))
		ret =  blackScreen(ho_Image, bflag);
	return ret;
}

//
//��������·���м���ͼƬ������ҽף����Ƚϱ�׼���õ��쳣�Ļҽ�
//������pPath ͼƬ·��
//      bflag �Ƿ���ʾ����ͼ�񴰿�
//����ֵ��false �����쳣  true �ɹ�
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
//��������·���м���ͼƬ������ҽף����Ƚϱ�׼���õ��쳣�Ļҽ�
//������
//      bflag �Ƿ���ʾ����ͼ�񴰿�
//����ֵ��false �����쳣  true �ɹ�
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
//��������·���м���ͼƬ������ҽף����Ƚϱ�׼���õ��쳣�Ļҽ�
//������pPath ͼƬ·��
//      bflag �Ƿ���ʾ����ͼ�񴰿�
//����ֵ��false �����쳣  true �ɹ�
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
//��������·���м���ͼƬ������ҽף����Ƚϱ�׼���õ��쳣�Ļҽ�
//������
//      bflag �Ƿ���ʾ����ͼ�񴰿�
//����ֵ��false �����쳣  true �ɹ�
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
//������ɼ�һ��ͼ��
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
//���ļ�ѡ���ͼ��
bool EslLoadOneImage()
{
	//���ļ�ѡ�񴰿�
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, NULL/*_T("λͼ�ļ�(*.BMP)|*.BMP|jpg�ļ�(*.jpg)|*.jpg||")*/);
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

//����: Ԥ����ͼ���з���Ļ����
//����ֵ�� false �����쳣  true �ɹ�
bool EslFindScreen()
{
	//����
	g_Result.m_resType = RES_TYPE_SCREEN;  //Ĭ����Ļ������
	HObject processImage;
	//תΪ��ͨ��
	Decompose3(g_Image, &g_ImageRed, &g_ImageGreen, &g_ImageBlue);
	Rgb1ToGray(g_Image, &g_ImageGray);

	switch (g_Para.m_screenType) 
	{
	case T_WHITE_SCR: //RGBת�Ҷ�
		processImage = g_ImageGray.Clone();
		break;
	case T_RED_SCR:  //�����ɫ����
		processImage = g_ImageRed.Clone();
		break;
	case T_GREEN_SCR: //������ɫ����
		processImage = g_ImageGreen.Clone();
		break;
	case T_BLUE_SCR:  //������ɫ����
		processImage = g_ImageBlue.Clone();
		break;
	case T_BLACK_SCR: //RGBת�Ҷ�
		processImage = g_ImageGray.Clone();
		break;
	case T_DUST_SCR:  //RGBת�Ҷ�
		processImage = g_ImageGray.Clone();
		break;
	default:
		g_Result.m_resType = RES_TYPE_ERROR; //��������
		return false;
	}

	//��ɫ��Ļ���÷ָ���Ļ����ֱ�ӷ���
	if (g_Para.m_screenType == T_BLACK_SCR)
	{
		g_Result.m_resType = RES_TYPE_OK;
		return true;
	}
	
	//FindScreen(processImage, &g_RegionScreen);
	
	//return true;
	//��ֵ��
	HObject ho_Regions, ho_RegionErosion, ho_RegionDilation, ho_ConnectedRegions, ho_SelectedRegions, ho_SelectedRegions1;

	Threshold(processImage, &ho_Regions, g_Para.m_FScreenBMinGray, g_Para.m_FScreenBMaxGray);
	FillUp(ho_Regions, &ho_Regions);
	ErosionRectangle1(ho_Regions, &ho_RegionErosion, g_Para.m_FScreenBEroseSize, g_Para.m_FScreenBEroseSize);
	DilationRectangle1(ho_RegionErosion, &ho_RegionDilation, g_Para.m_FScreenBEroseSize, g_Para.m_FScreenBEroseSize);
	Connection(ho_RegionDilation, &ho_ConnectedRegions);
	SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, "area", "and", g_Para.m_FScreenBSelectAreaMin, g_Para.m_FScreenBSelectAreaMax);
	SelectShape(ho_SelectedRegions, &ho_SelectedRegions1, "rectangularity", "and",
		0.85, 1);
	//)��ϳɱ�׼����
	HObject ho_RegionFillUp6, ho_RegionErosion3, ho_Contours, ho_RegionErosion4;
	HObject ho_RegionDilation5, ho_RegionDifference4, ho_RegionOpening3, ho_ConnectedRegions4;
	HObject ho_SelectedRegions4;
	HTuple  hv_Number_edge;
	//ɸѡΪ��
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

	//9)����������Ե
	CountObj(ho_SelectedRegions4, &hv_Number_edge);
	//���Ϊ���������ж����ұ�Ե�Ƿ�Ϊ����������Ϊ���ж���Ļ��û��ȥ���ӽ�ͼ���Ե
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

			//)ѡ�����±�Ե�ĵ�һ����Ե
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
			//ɸѡΪ��
			CountObj(ho_SelectedXLD, &hv_Number_edge);
			if ((hv_Number_edge == 0))
			{
				g_Result.m_resType = RES_TYPE_SCREEN;
				g_Result.m_strMsg = strErrorMsg[RES_TYPE_SCREEN];
				return true;
			}
			//)ѡ�����ҵĵ�2����Ե
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
			//)ѡ�����±�Ե�ĵ�2����Ե
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
			//)ѡ�����ҵĵ�1����Ե
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
			//������
			ConcatObj(ho_SelectedXLD, ho_SelectedXLD1, &ho_ObjectsConcat);
			ConcatObj(ho_ObjectsConcat, ho_SelectedXLD2, &ho_ObjectsConcat);
			ConcatObj(ho_ObjectsConcat, ho_SelectedXLD3, &ho_ObjectsConcat);
			//) ���ֱ�ߣ�ȡֱ�߽���
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
			//�����ӳ�����RegionScreen
			GenRegionPolygonFilled(&g_RegionScreen, hv_Row_Concat, hv_Col_Concat);

			//inner_rectangle1 (Region6, Row16, Column16, Row25, Column25)
			//gen_rectangle1 (Rectangle2, Row16, Column16, Row25, Column25)
			//) ���������������Ⱦ���
			ErosionRectangle1(g_RegionScreen, &ho_RegionCenter, g_Para.m_FScreenBEdgeWidth, g_Para.m_FScreenBEdgeWidth);
			//�����ֱ�Ե�������ȱȽϲ����ȣ������⴦��
			Difference(g_RegionScreen, ho_RegionCenter, &ho_RegionMargin);
			g_RegionScreen = g_RegionScreen.Clone();
			DispObj(g_Image, hv_WindowHandle);
			SetDraw(hv_WindowHandle, "margin");
			SetColor(hv_WindowHandle, "cyan");
			DispObj(g_RegionScreen, hv_WindowHandle);
			//DispObj(ho_RegionCenter, hv_WindowHandle);
			//���
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

//��������
int Run()
{
	//������Ļ����
	//EslFindScreen();
	bool ret = EslCheckLightScreen();
	EslCheckRedScreen();
	EslCheckGreenScreen();
	EslCheckBlueScreen();
	if (ret) //ֻ�е���������Ļ��������ʱ�Ž��к������
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

//��¼�����·��
bool LogResult(char* path, int mode)
{
	FILE* pF = NULL;
	pF = fopen(path, "a");
	if (pF == NULL) return false;

	//ʱ��
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
		if (g_vctResult.size() > 0) //��Ļ���NG
		{
			fprintf_s(pF, "  NG  \n");
			//��ʾNG����ϸ����
			for (int i = 0; i < g_vctResult.size(); i++)
			{
				fprintf_s(pF, "    %s Area: %.2lf\n", g_vctResult[i].m_strMsg.c_str(), g_vctResult[i].m_Area);
			}
		}
		else //��Ļ���OK
		{
			fprintf_s(pF, "  OK  \n");
		}
	}
	else //��Ļ����ng
	{
		fprintf_s(pF, "  NG  \n");
		//��ʾNG����ϸ����
		fprintf_s(pF, "    %s\n", g_Result.m_strMsg.c_str());
	}
	fclose(pF);
	return true;
}
//�жϽ����������Ļ����ʾ�����ͼ�� �� OKNG
bool JudgeResultAndShow(T_SCR screenType, bool show = false )
{
	//��ʾ����Ļ
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
	
	//�������Ļ����������
	if (g_Result.m_resType == RES_TYPE_SCREEN)
	{
		//��ʾͼ��
		if (wndhandle.Length())
		{
			DispObj(g_Image, wndhandle);
		}
		SetTposition(wndhandle, HTuple(0), (HTuple(0) ));
		SetColor(wndhandle, "red");
		WriteString(wndhandle, "NG");

		//����������ʾ������Ϣ
		SetTposition(hv_WindowHandle, HTuple(0 + 55), (HTuple(0)));
		SetColor(hv_WindowHandle, "red");
		WriteString(hv_WindowHandle, "NG");
		
		WriteString(hv_WindowHandle, g_Result.m_strMsg.c_str());
		LogResult("Log.txt",0);
		return false;
	}

	//����
	if (show)
	{
		//��ʾͼ��С����
		if (wndhandle.Length())
		{
			DispObj(g_Image, wndhandle);
		}
		//�Ƿ�OK
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
		if (bOk)//��ʾOK
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

			//����������ʾ������Ϣ
			SetTposition(hv_WindowHandle, HTuple(0 + 55), (HTuple(0)));
			SetColor(hv_WindowHandle, "red");
			WriteString(hv_WindowHandle, "NG");
			//SetTposition(wndhandle, HTuple(0 + 55), (HTuple(0) + 5));
			//WriteString(hv_WindowHandle, g_Result.m_strMsg.c_str());
			LogResult("Log.txt", 0);
			return false;
		}
		
	}

	//��ʾNG
	return true;
}
//������ȱ��
bool EslCheckLightScreen()
{
	g_Para = g_Para_White;
	g_Result.m_screenType = T_WHITE_SCR;
	g_Result.m_resType = RES_TYPE_OK;
	g_vctResult.clear();
	if (EslFindScreen())
	{
		//�����Ļ����ȱ�ݣ���ֱ�ӱ�NG�� ���ò������㰵��
		if (!JudgeResultAndShow(g_Result.m_screenType)) return false;
		
		//������㰵��
		g_Result.m_resType = RES_TYPE_OK;
		HObject ho_ConnectedRegions, ho_ConnectedRegions_Light;
		DetectSpot(g_ImageGray, &ho_ConnectedRegions, &ho_ConnectedRegions_Light);
		//ȱ����ʾ������
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

//������� �����㣩
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
	//�����㷨���
	if (!g_Para_White.LoadPara(PATH_WHITE_SRC_PARA))
	{
		AfxMessageBox("���ذ�������ʧ��");
	}
	if (!g_Para_Red.LoadPara(PATH_RED_SRC_PARA))
	{
		AfxMessageBox("���غ�������ʧ��");
	}
	if (!g_Para_Green.LoadPara(PATH_GREEN_SRC_PARA))
	{
		AfxMessageBox("������������ʧ��");
	}
	if (!g_Para_Blue.LoadPara(PATH_BLUE_SRC_PARA))
	{
		AfxMessageBox("������������ʧ��");
	}
	if (!g_Para_Black.LoadPara(PATH_BLACK_SRC_PARA))
	{
		AfxMessageBox("���غ�������ʧ��");
	}
	return true;
}

//��ⰵ�㣬���ߣ�mura, �����������
bool DetectSpot(HObject ImageSrc, HObject* RegionResults, HObject* RegionResults_Light)
{
	//�򵥼�ⰵ�㣬����
	HObject ho_ResultRegion;
	HTuple hv_ThrHigh, hv_MinArea, hv_Delta, hv_RoundGray;
	hv_ThrHigh	= g_Para.m_ThrHigh;
	hv_MinArea	= g_Para.m_MinArea;
	hv_Delta	= g_Para.m_Delta;
	
	SimpleDarkSpot(ImageSrc, g_RegionScreen, &ho_ResultRegion, hv_ThrHigh, hv_MinArea,
		hv_Delta, &hv_RoundGray);
	//SetColor(hv_WindowHandle, "blue");
	//DispObj(ho_ResultRegion, hv_WindowHandle);
	//�򵥼�����㣬����
	HObject ho_ResultRegionLightSpot;
	HTuple hv_ThrHigh2, hv_Delta2, hv_RoundGray2;
	hv_ThrHigh2 = g_Para.m_ThrHigh_Light;
	hv_Delta2 = g_Para.m_Delta_Light;
	SimpleLightSpot(ImageSrc, g_RegionScreen, &ho_ResultRegionLightSpot, hv_ThrHigh2,
		hv_MinArea, hv_Delta2, &hv_RoundGray2);

	//Ԥ����
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

	//����ɫMURA
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
	//����ɫMURA
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
	//ͳ�ƽ��
	//����
	HObject ho_RegionUnion1, ho_RegionUnionDark, ho_ConnectedRegions;
	ConcatObj(ho_ResultRegion, ho_ResultRegion2, &ho_RegionUnion1);
	Union1(ho_RegionUnion1, &ho_RegionUnionDark);
	Connection(ho_RegionUnionDark, &ho_ConnectedRegions);
	*RegionResults = ho_ConnectedRegions.Clone();

	//����
	HObject ho_RegionUnion2, ho_RegionUnionLight, ho_ConnectedRegions2;
	ConcatObj(ho_ResultRegionLightSpot, ho_ResultRegionLight, &ho_RegionUnion2);
	Union1(ho_RegionUnion2, &ho_RegionUnionLight);
	Connection(ho_RegionUnionLight, &ho_ConnectedRegions2);
	*RegionResults_Light = ho_ConnectedRegions2.Clone();

	return true;
}

bool DealWithResult(HObject ho_ResultRegions, HObject ho_ImageSrc, HObject * ho_ImageParts, T_SCR screentype, Type_Res type)
{
	//�ж�ȱ�ݵĴ�С���λ�ã� ����ͼ��
	HTuple Areas, CentRow, CentCol, Row1, Col1, Row2, Col2;
	JudgeResult(ho_ResultRegions, ho_ImageSrc, ho_ImageParts, 3364, 2748, 120,
		80, 20, &Areas, &CentRow, &CentCol, &Row1, &Col1, &Row2,
		&Col2);
	//������
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

	return true; //ֱ�ӷ��أ�����ͼ
	//�����ͼ
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
			//ʱ��Ӹ�������
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

	//ͼƬ��С
	GetImageSize(ho_ImageMean, &hv_Width, &hv_Height);

	//��ⰵ�ߵ�
	ScaleImageMax(ho_ImageFFT, &ho_ImageScaleMaxFFT);
	ScaleImageMax(ho_ImageMean, &ho_ImageScaleMaxMean);
	HysteresisThreshold(ho_ImageScaleMaxMean, &ho_RegionHysteresis, hv_HysteresisThrMin,
		hv_HysteresisThrMax, 5);
	//��С��Χ(��Ե����⣩
	GenRectangle1(&ho_Rectangle1, hv_MarginWidth, hv_MarginWidth, hv_Height - hv_MarginWidth,
		hv_Width - hv_MarginWidth);
	//ȡ����
	Intersection(ho_Rectangle1, ho_RegionHysteresis, &ho_RegionIntersection);
	Threshold(ho_ImageScaleMaxFFT, &ho_Regions1, hv_fftThr, 255);
	//ȡ����
	Intersection(ho_Regions1, ho_Rectangle1, &ho_RegionIntersection1);


	//����
	Union1(ho_RegionIntersection, &ho_RegionUnion);
	Union2(ho_RegionUnion, ho_RegionIntersection1, &ho_RegionUnionAll);
	//���������Ĥȡ�
	Union1(ho_MaskRegion, &ho_RegionUnionMask);
	HomMat2dIdentity(&hv_HomMat2DIdentity2);
	HomMat2dTranslate(hv_HomMat2DIdentity2, -hv_OffsetY, -hv_OffsetX, &hv_HomMat2DIdentity2);
	AffineTransRegion(ho_RegionUnionMask, &ho_RegionUnionMaskTrans, hv_HomMat2DIdentity2,
		"nearest_neighbor");
	Difference(ho_RegionUnionAll, ho_RegionUnionMaskTrans, &ho_RegionDifferenceAll);
	//���ɸѡ
	Connection(ho_RegionDifferenceAll, &ho_ConnectedRegions);
	SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, "area", "and", hv_AreaThr,
		9999999);

	//����任
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

	//�ڽӾ���
	InnerRectangle1(ho_ScreenRegion, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);
	GenRectangle1(&ho_Rectangle, hv_Row1, hv_Column1, hv_Row2, hv_Column2);
	(*hv_OffsetX) = hv_Column1;
	(*hv_OffsetY) = hv_Row1;

	ho_ImagePro = ho_ImageSrc;
	//�޲�mask����ĻҶȣ�ʹ���Ϊ����Χ�ĻҶȣ�����׼ȷ����Ҷ�任
	//dilation_circle (MaskRegion, RegionDilation, 1.5)
	Union1(ho_MaskRegion, &ho_RegionUnion);
	CountObj(ho_MaskRegion, &hv_Number);
	//�������mask֮���ƽ���Ҷ�
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

	//) ���Ա�Ե����֣���Ҫƽ���˲�
	Union1(ho_MaskRegion, &ho_RegionUnionMask);
	DilationRectangle1(ho_RegionUnionMask, &ho_RegionDilationMask, 11, 11);




	ReduceDomain(ho_ImagePro, ho_Rectangle, &ho_ImageReduced);
	CropDomain(ho_ImageReduced, &ho_ImagePart);

	//mean_image (ImagePart, ImageMean, MedianSize, MedianSize)
	//sub_image (ImagePart, ImageMean, ImageSub, 1, 194)
	MedianRect(ho_ImagePart, &ho_ImageMedian, hv_MedianSize, hv_MedianSize);
	SubImage(ho_ImagePart, ho_ImageMedian, &ho_ImageSub, 1, 194);
	//ͼƬ��С
	GetImageSize(ho_ImageSub, &hv_Width, &hv_Height);

	//����Ҷ�任
	RftGeneric(ho_ImageSub, &ho_ImageFFT, "to_freq", "none", "complex", hv_Width);
	//������˹�˲���1
	GenGaussFilter(&ho_ImageGaussHighpass, hv_GuassHigh, hv_GuassHigh, 0, "n", "rft",
		hv_Width, hv_Height);
	GenGaussFilter(&ho_ImageGaussLowpass, hv_GuassLow, hv_GuassLow, 0, "n", "rft",
		hv_Width, hv_Height);
	SubImage(ho_ImageGaussHighpass, ho_ImageGaussLowpass, &ho_ImageSub1, 1, 0);
	ConvolFft(ho_ImageFFT, ho_ImageSub1, &ho_ImageConvol);
	RftGeneric(ho_ImageConvol, &(*ho_ImageFFTRes2), "from_freq", "n", "real", hv_Width);
	MeanImage((*ho_ImageFFTRes2), &(*ho_ImageMeanRes2), hv_MeanSize, hv_MeanSize);
	//������˹�˲���2
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
	//)��ϳɱ�׼����
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

	//9)����������Ե
	CountObj(ho_SelectedRegions4, &hv_Number_edge);
	//���Ϊ���������ж����ұ�Ե�Ƿ�Ϊ����������Ϊ���ж���Ļ��û��ȥ���ӽ�ͼ���Ե
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
			//)ѡ�����±�Ե�ĵ�һ����Ե
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

			//)ѡ�����ҵĵ�2����Ե
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

			//)ѡ�����±�Ե�ĵ�2����Ե
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

			//)ѡ�����ҵĵ�1����Ե
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

			//������
			ConcatObj(ho_SelectedXLD, ho_SelectedXLD1, &ho_ObjectsConcat);
			ConcatObj(ho_ObjectsConcat, ho_SelectedXLD2, &ho_ObjectsConcat);
			ConcatObj(ho_ObjectsConcat, ho_SelectedXLD3, &ho_ObjectsConcat);
			//) ���ֱ�ߣ�ȡֱ�߽���
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
			//�����ӳ�����RegionScreen
			GenRegionPolygonFilled(&ho_RegionScreen, hv_Row_Concat, hv_Col_Concat);

			//inner_rectangle1 (Region6, Row16, Column16, Row25, Column25)
			//gen_rectangle1 (Rectangle2, Row16, Column16, Row25, Column25)
			//) ���������������Ⱦ���
			ErosionRectangle1(ho_RegionScreen, &ho_RegionCenter, 20, 20);
			//�����ֱ�Ե�������ȱȽϲ����ȣ������⴦��
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

	//�ֳɱ�Ե���������������Ϊ��Ե����̫���ӻҶȵͣ�
	ErosionRectangle1(ho_ScreenRegion, &ho_RegionCent, 13, 13);
	Difference(ho_ScreenRegion, ho_RegionCent, &ho_RegionEdge);

	ReduceDomain(ho_ImageSrc, ho_RegionEdge, &ho_ImageEdge);
	GaussFilter(ho_ImageEdge, &ho_ImageGaussEdge, 5);
	Threshold(ho_ImageGaussEdge, &ho_RegionEdge, 0, 10);
	Connection(ho_RegionEdge, &ho_ConnectedRegionsEdge);
	SelectShape(ho_ConnectedRegionsEdge, &ho_SelectedRegionsEdge, "area", "and", hv_MinArea,
		9999999);

	//�򵥼�� �ڵ�
	ReduceDomain(ho_ImageSrc, ho_RegionCent, &ho_ImageReduced);
	GaussFilter(ho_ImageReduced, &ho_ImageGauss, 5);
	Threshold(ho_ImageGauss, &ho_Regions2, 0, hv_ThrHigh);
	Connection(ho_Regions2, &ho_ConnectedRegions1);
	SelectShape(ho_ConnectedRegions1, &ho_SelectedRegions2, "area", "and", hv_MinArea,
		9999999);

	//��̬ѧ����
	//connection (SelectedRegions2, ConnectedRegions2)
	//erosion_circle (ConnectedRegions2, RegionErosion, 1.5)
	//select_shape (RegionErosion, SelectedRegions3, 'area', 'and', 1, 9999999)
	DilationCircle(ho_SelectedRegions2, &ho_RegionDilation0, 3.5);

	//���
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
	//ȱ����Χ��ƽ���Ҷ�
	GrayFeatures(ho_RegionRound, ho_ImageEmphasize, "mean", &hv_Value);
	//ȱ�ݵ�ƽ���Ҷ�
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
	//�������

	//�������

	//����Ϊ��ʾ

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

	//ͼƬ��С
	GetImageSize(ho_ImageMean, &hv_Width, &hv_Height);
	ScaleImageMax(ho_ImageFFT, &ho_ImageScaleMaxFFT);
	ScaleImageMax(ho_ImageMean, &ho_ImageScaleMaxMean);
	HysteresisThreshold(ho_ImageScaleMaxMean, &ho_RegionHysteresis, hv_HysteresisThrMin,
		hv_HysteresisThrMax, 10);

	//��С��Χ(��Ե����⣩
	GenRectangle1(&ho_Rectangle1, hv_MarginWidth, hv_MarginWidth, hv_Height - hv_MarginWidth,
		hv_Width - hv_MarginWidth);
	//ȡ����
	Intersection(ho_Rectangle1, ho_RegionHysteresis, &ho_RegionIntersection);
	Threshold(ho_ImageScaleMaxFFT, &ho_Regions1, 220, 255);
	//ȡ����
	Intersection(ho_Regions1, ho_Rectangle1, &ho_RegionIntersection1);

	//����
	Union1(ho_RegionIntersection, &ho_RegionUnion);
	Union2(ho_RegionUnion, ho_RegionIntersection1, &ho_RegionUnionAll);

	Union1(ho_MaskRegion, &ho_RegionUnionMask);
	DilationRectangle1(ho_RegionUnionMask, &ho_RegionDilationMask, 21, 21);
	HomMat2dIdentity(&hv_HomMat2DIdentity2);
	HomMat2dTranslate(hv_HomMat2DIdentity2, -hv_OffsetY, -hv_OffsetX, &hv_HomMat2DIdentity2);
	AffineTransRegion(ho_RegionDilationMask, &ho_RegionUnionMaskTrans, hv_HomMat2DIdentity2,
		"nearest_neighbor");
	Difference(ho_RegionUnionAll, ho_RegionUnionMaskTrans, &ho_RegionDifferenceAll);

	//���ɸѡ
	Connection(ho_RegionDifferenceAll, &ho_ConnectedRegions);
	SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, "area", "and", hv_AreaThr,
		9999999);

	//����任
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

	//�򵥼�� �ڵ�
	ReduceDomain(ho_ImageSrc, ho_ScreenRegion, &ho_ImageReduced);
	GaussFilter(ho_ImageReduced, &ho_ImageGauss, 5);
	Threshold(ho_ImageGauss, &ho_Regions2, hv_ThrHigh, 255);
	Connection(ho_Regions2, &ho_ConnectedRegions1);
	SelectShape(ho_ConnectedRegions1, &ho_SelectedRegions2, "area", "and", hv_MinArea,
		9999999);

	//��̬ѧ����
	//connection (SelectedRegions2, ConnectedRegions2)
	//erosion_circle (ConnectedRegions2, RegionErosion, 1.5)
	//select_shape (RegionErosion, SelectedRegions3, 'area', 'and', 1, 9999999)
	DilationCircle(ho_SelectedRegions2, &ho_RegionDilation0, 2.5);

	//���
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
	//ȱ����Χ��ƽ���Ҷ�
	GrayFeatures(ho_RegionRound, ho_ImageEmphasize, "mean", &hv_Value);
	//ȱ�ݵ�ƽ���Ҷ�
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
	//�������

	//�������

	//����Ϊ��ʾ

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

	//4) ������

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

	//0)//////// ���©��////////

	//1) �����ָ����Ļ������reduce�� ����ȫ��
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


	//���������������
	AreaCenter(ho_ResultRegions, &(*hv_Areas), &(*hv_CentRow), &(*hv_CentCol));
	SmallestRectangle1(ho_ResultRegions, &hv_roundRow1, &hv_roundCol1, &hv_roundRow2,
		&hv_roundCol2);
	hv_w = hv_roundCol2 - hv_roundCol1;
	hv_h = hv_roundRow2 - hv_roundRow1;
	//2) �������Χ���ε�λ��
	hv_boxW = hv_BoxMinW;
	hv_boxH = hv_BoxMinH;
	GenEmptyObj(&(*ho_ImageParts));
	{
		HTuple end_val10 = ((*hv_Areas).TupleLength()) - 1;
		HTuple step_val10 = 1;
		int count = 1;
		for (hv_Index = 0; hv_Index.Continue(end_val10, step_val10); hv_Index += step_val10)
		{
			//3) С��ָ��ֵ�ģ� ��ͼ��Ϊ��С��ָ�����BoxMinW,
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
			//4) ��ͼ
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
