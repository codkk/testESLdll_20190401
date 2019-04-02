#pragma once

#define SCREEN_WHITE 0
#define SCREEN_RED 1
#define SCREEN_GREEN 2
#define SCREEN_BLUE 3
#define SCREEN_BLACK 4
//函数： 相机拍摄一张图像
//返回值：false 发生异常  true 成功
extern "C" _declspec(dllexport) bool EslGrabOneImage();

//函数： 从文件中加载一张图像
//返回值：false 发生异常  true 成功
extern "C" _declspec(dllexport) bool EslLoadOneImage();

//函数： 初始化dll
//返回值：false 发生异常  true 成功
extern "C" _declspec(dllexport) bool EslInitDll(CWnd* pWnd);

//函数： 设置结果显示窗口
//返回值：false 发生异常  true 成功
extern "C" _declspec(dllexport) bool EslSetResWnd(int SrcreenType, CWnd* pWnd);


//函数： 卸载dll资源
//返回值：false 发生异常  true 成功
extern "C" _declspec(dllexport) bool EslUnInitDll();

//函数： 运行任务
//返回值： 错误值
extern "C" _declspec(dllexport) int  Run();

//函数： 屏幕区域
//返回值： false 发生异常  true 成功
extern "C" _declspec(dllexport) bool EslFindScreen();

//函数：白屏(白灰红绿蓝)检测
//返回值： false 发生异常  true 成功
extern "C" _declspec(dllexport) bool EslCheckLightScreen();

//函数：红屏(白灰红绿蓝)检测
//返回值： false 发生异常  true 成功
extern "C" _declspec(dllexport) bool EslCheckRedScreen();

//函数：绿屏(白灰红绿蓝)检测
//返回值： false 发生异常  true 成功
extern "C" _declspec(dllexport) bool EslCheckGreenScreen();

//函数：蓝屏(白灰红绿蓝)检测
//返回值： false 发生异常  true 成功
extern "C" _declspec(dllexport) bool EslCheckBlueScreen();

//函数：黑屏检测
//返回值： false 发生异常  true 成功
extern "C" _declspec(dllexport) bool EslCheckBlackScreen();

//重新加载para文件
extern "C" _declspec(dllexport) bool EslReloadPara();