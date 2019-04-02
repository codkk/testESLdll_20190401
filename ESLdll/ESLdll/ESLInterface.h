#pragma once

#define SCREEN_WHITE 0
#define SCREEN_RED 1
#define SCREEN_GREEN 2
#define SCREEN_BLUE 3
#define SCREEN_BLACK 4
//������ �������һ��ͼ��
//����ֵ��false �����쳣  true �ɹ�
extern "C" _declspec(dllexport) bool EslGrabOneImage();

//������ ���ļ��м���һ��ͼ��
//����ֵ��false �����쳣  true �ɹ�
extern "C" _declspec(dllexport) bool EslLoadOneImage();

//������ ��ʼ��dll
//����ֵ��false �����쳣  true �ɹ�
extern "C" _declspec(dllexport) bool EslInitDll(CWnd* pWnd);

//������ ���ý����ʾ����
//����ֵ��false �����쳣  true �ɹ�
extern "C" _declspec(dllexport) bool EslSetResWnd(int SrcreenType, CWnd* pWnd);


//������ ж��dll��Դ
//����ֵ��false �����쳣  true �ɹ�
extern "C" _declspec(dllexport) bool EslUnInitDll();

//������ ��������
//����ֵ�� ����ֵ
extern "C" _declspec(dllexport) int  Run();

//������ ��Ļ����
//����ֵ�� false �����쳣  true �ɹ�
extern "C" _declspec(dllexport) bool EslFindScreen();

//����������(�׻Һ�����)���
//����ֵ�� false �����쳣  true �ɹ�
extern "C" _declspec(dllexport) bool EslCheckLightScreen();

//����������(�׻Һ�����)���
//����ֵ�� false �����쳣  true �ɹ�
extern "C" _declspec(dllexport) bool EslCheckRedScreen();

//����������(�׻Һ�����)���
//����ֵ�� false �����쳣  true �ɹ�
extern "C" _declspec(dllexport) bool EslCheckGreenScreen();

//����������(�׻Һ�����)���
//����ֵ�� false �����쳣  true �ɹ�
extern "C" _declspec(dllexport) bool EslCheckBlueScreen();

//�������������
//����ֵ�� false �����쳣  true �ɹ�
extern "C" _declspec(dllexport) bool EslCheckBlackScreen();

//���¼���para�ļ�
extern "C" _declspec(dllexport) bool EslReloadPara();