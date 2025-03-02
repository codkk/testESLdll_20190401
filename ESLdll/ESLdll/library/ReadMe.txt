2016-11-29 DVPCamera.dll 更新，版本号 2.7.8.22

一. 增加曲线LUT功能;
	1. 获取LUT曲线
		dvp2api dvpStatus dvpGetCurveLut(dvpHandle handle, dvpCurveLut *pCurveLut);
	2. 设置LUT曲线
		dvp2api dvpStatus dvpSetCurveLut(dvpHandle handle, dvpCurveLut CurveLut);

二. 增加颜色校正参数的读写功能;
	1. 获取颜色校正数据
		dvp2api dvpStatus dvpGetColorCorrection(dvpHandle handle, dvpColorCorrection *pColorCorrection);
	2. 设置颜色校正数据
		dvp2api dvpStatus dvpSetColorCorrection(dvpHandle handle, dvpColorCorrection ColorCorrection);

三. 增加在指定分辨率模式和ROI下抓图并保存图像的功能;
	1. 根据指定分辨率抓取并保存图像
		dvp2api dvpStatus dvpCaptureFile(dvpHandle handle, dvpUint32 ResoulutionModeSel, dvpRegion *roi, dvpUint32 timeout, const char *FilePath, dvpInt32 quality);
	

四. 增加了图像的正/负90度旋转的功能
	1. 获取图像旋转使能状态
		dvp2api dvpStatus dvpGetRotateState(dvpHandle handle, bool *pRotateState);
	2. 设置图像旋转使能状态
		dvp2api dvpStatus dvpSetRotateState(dvpHandle handle, bool RotateState);
	3. 获取图像旋转方向的标志（顺时针或逆时针90度）
		dvp2api dvpStatus dvpGetRotateOpposite(dvpHandle handle, bool *pRotateOpposite);
	4. 设置图像旋转方向的标志（顺时针或逆时针90度）、
		dvp2api dvpStatus dvpSetRotateOpposite(dvpHandle handle, bool RotateOpposite);

五. 扩展了帧信息结构体，增加了翻转、旋转等标志
	1. 帧信息结构体 dvpFrame

六. 增加平场、消除彩点参数的读写功能
	1. 获取平场的使能状态
		dvp2api dvpStatus dvpGetFlatFieldState(dvpHandle handle, bool *pFlatFieldState);
	2. 设置平场的使能状态
		dvp2api dvpStatus dvpSetFlatFieldState(dvpHandle handle, bool FlatFieldState);
	3. 获取消除彩点的使能状态
		dvp2api dvpStatus dvpGetDefectFixState(dvpHandle handle, bool *pDefectFixState);
	4. 设置消除彩点的使能状态
		dvp2api dvpStatus dvpSetDefectFixState(dvpHandle handle, bool DefectFixState);

七. 增加原始和目标图像格式参数的读写功能
	1. 获取当前原始图像格式选项的索引
		dvp2api dvpStatus dvpGetSourceFormatSel(dvpHandle handle, dvpUint32 *pSourceFormatSel);
	2. 设置当前原始图像格式选项的索引
		dvp2api dvpStatus dvpSetSourceFormatSel(dvpHandle handle, dvpUint32 SourceFormatSel);
	3. 获取原始图像格式选项索引的描述信息
		dvp2api dvpStatus dvpGetSourceFormatSelDescr(dvpHandle handle, dvpSelectionDescr *pSourceFormatSelDescr);
	4. 获取当前原始图像格式选项索引对应的具体信息
		dvp2api dvpStatus dvpGetSourceFormatSelDetail(dvpHandle handle, dvpUint32 SourceFormatSel, dvpFormatSelection *pSourceFormatSelDetail);
	5. 获取当前输出图像格式选项的索引
		dvp2api dvpStatus dvpGetTargetFormatSel(dvpHandle handle, dvpUint32 *pTargetFormatSel);
	6. 设置当前输出图像格式选项的索引
		dvp2api dvpStatus dvpSetTargetFormatSel(dvpHandle handle, dvpUint32 TargetFormatSel);
	7. 获取输出图像格式选项索引的描述信息
		dvp2api dvpStatus dvpGetTargetFormatSelDescr(dvpHandle handle, dvpSelectionDescr *pTargetFormatSelDescr);
	8. 获取当前输出图像格式选项索引对应的具体信息
		dvp2api dvpStatus dvpGetTargetFormatSelDetail(dvpHandle handle, dvpUint32 TargetFormatSel, dvpFormatSelection *pTargetFormatSelDetail);

八. 增加获取第一个像素对应的位置
	1. 获取第一个像素对应的位置
		dvp2api dvpStatus dvpGetFirstPosition(dvpHandle handle, dvpFirstPosition *pFirstPosition);

九. 增加同时获取时间戳和帧ID相同的原始采集帧和目标输出帧
	1. 同时获取时间戳和帧ID相同的原始采集帧和目标输出帧
		dvp2api dvpStatus dvpGetFrameBuffer(dvpHandle handle, dvpFrameBuffer *pRaw, dvpFrameBuffer *pOut, dvpUint32 timeout);

十. 增加事件通知回调函数
	1. 注册事件通知的回调函数
		dvp2api dvpStatus dvpRegisterEventCallback(dvpHandle handle, dvpEventCallback proc, dvpEvent event, void *pContext);
	2. 注销事件通知的回调函数
		dvp2api dvpStatus dvpUnregisterEventCallback(dvpHandle handle, dvpEventCallback proc, dvpEvent event, void *pContext);
	3. 事件通知回调函数
		typedef dvpInt32(dvpEventCallback)(dvpHandle handle, dvpEvent event, void *pContext, dvpInt32 param, struct dvpVariant *pVariant);

2017-02-08 DVPCamera.dll 版本号 (2.9.13)
一. 增加复位设备功能
	1. 复位设备功能
		dvpStatus dvpResetDevice(dvpHandle handle);

2017-03-22 DVPCamera.dll 版本号 (2.10.16)
一. 增加使能自动消除彩点的功能
	1. 自动消除彩点功能
		dvpStatus dvpSetAutoDefectFixState(dvpHandle handle, bool AutoDefectFixState);

2017-05-12 DVPCamera.dll 版本号 (2.11.18)
一. 增加使能自动消除彩点的功能
	1. 使能自动消除彩点
		dvpStatus dvpSetAutoDefectFixState (dvpHandle handle,bool AutoDefectFixState);
	2. 获取自动消除彩点的使能状态
		dvpStatus dvpGetAutoDefectFixState(dvpHandle handle, bool *pAutoDefectFixState);

2017-05-12 DVPCamera.dll 版本号 (2.11.18)
一. 增加中断传感器的图像采集的功能
	1. 中断传感器的图像采集
		dvpStatus dvpHold(dvpHandle handle);

二. 增加相机硬件ISP使能的功能
	1. 设置相机内置硬件ISP的使能
		dvpStatus dvpSetHardwareIspState(dvpHandle handle, bool HardwareIspState);
	2. 获取相机内置硬件ISP的使能
		dvpStatus dvpGetHardwareIspState(dvpHandle handle, bool *pHardwareIspState);

2017-06-16 DVPCamera.dll 版本号 (2.12.19)
一. 增加获取相机功能描述信息的接口
	1. 相机功能描述信息定义 dvpFunctionInfo
	2. 获取功能信息
		dvpStatus dvpGetFunctionInfo(dvpHandle handle, dvpFunctionInfo *pFunctionInfo);

二. 增加RGB32图像格式输出的支持
	1. 增加RGB32图像格式输出 dvpImageFormat

2017-07-17 DVPCamera.dll 版本号 (2.14.21)
一. 扩充了数据流格式的定义
	1. 数据流格式的定义 dvpStreamFormat

2017-09-15 DVPCamera.dll 版本号 (2.15.24)
一. 增加了根据用户命名来打开相机的功能
	1. 根据用户命名来打开相机
		dvpStatus dvpOpenByUserId(dvpStr UserId, dvpOpenMode mode, dvpHandle *pHandle);

二. 增加了加载用户设置的功能
	1. 从相机存储器加载用户设置
		dvpStatus dvpLoadUserSet(dvpHandle handle, dvpUserSet UserSet);

2018-04-11 DVPCamera.dll 版本号 (2.16.32)
一. 增加了在用户线程中处理图像的功能
	1. 增加视频流回调事件定义 dvpStreamEvent::STREAM_EVENT_FRAME_THREAD
	2. 在用户线程中对源图像进行（ISP）处理，得到目标图像
		dvpStatus dvpProcessImage(dvpHandle handle, const dvpFrame *pSourceFrame, const void *pSourceBuffer, dvpFrame *pTargetFrame, void *pTargetBuffer, dvpUint32 targetBufferSize, dvpStreamFormat targetFormat);
