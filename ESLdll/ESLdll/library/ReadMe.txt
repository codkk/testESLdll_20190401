2016-11-29 DVPCamera.dll ���£��汾�� 2.7.8.22

һ. ��������LUT����;
	1. ��ȡLUT����
		dvp2api dvpStatus dvpGetCurveLut(dvpHandle handle, dvpCurveLut *pCurveLut);
	2. ����LUT����
		dvp2api dvpStatus dvpSetCurveLut(dvpHandle handle, dvpCurveLut CurveLut);

��. ������ɫУ�������Ķ�д����;
	1. ��ȡ��ɫУ������
		dvp2api dvpStatus dvpGetColorCorrection(dvpHandle handle, dvpColorCorrection *pColorCorrection);
	2. ������ɫУ������
		dvp2api dvpStatus dvpSetColorCorrection(dvpHandle handle, dvpColorCorrection ColorCorrection);

��. ������ָ���ֱ���ģʽ��ROI��ץͼ������ͼ��Ĺ���;
	1. ����ָ���ֱ���ץȡ������ͼ��
		dvp2api dvpStatus dvpCaptureFile(dvpHandle handle, dvpUint32 ResoulutionModeSel, dvpRegion *roi, dvpUint32 timeout, const char *FilePath, dvpInt32 quality);
	

��. ������ͼ�����/��90����ת�Ĺ���
	1. ��ȡͼ����תʹ��״̬
		dvp2api dvpStatus dvpGetRotateState(dvpHandle handle, bool *pRotateState);
	2. ����ͼ����תʹ��״̬
		dvp2api dvpStatus dvpSetRotateState(dvpHandle handle, bool RotateState);
	3. ��ȡͼ����ת����ı�־��˳ʱ�����ʱ��90�ȣ�
		dvp2api dvpStatus dvpGetRotateOpposite(dvpHandle handle, bool *pRotateOpposite);
	4. ����ͼ����ת����ı�־��˳ʱ�����ʱ��90�ȣ���
		dvp2api dvpStatus dvpSetRotateOpposite(dvpHandle handle, bool RotateOpposite);

��. ��չ��֡��Ϣ�ṹ�壬�����˷�ת����ת�ȱ�־
	1. ֡��Ϣ�ṹ�� dvpFrame

��. ����ƽ���������ʵ�����Ķ�д����
	1. ��ȡƽ����ʹ��״̬
		dvp2api dvpStatus dvpGetFlatFieldState(dvpHandle handle, bool *pFlatFieldState);
	2. ����ƽ����ʹ��״̬
		dvp2api dvpStatus dvpSetFlatFieldState(dvpHandle handle, bool FlatFieldState);
	3. ��ȡ�����ʵ��ʹ��״̬
		dvp2api dvpStatus dvpGetDefectFixState(dvpHandle handle, bool *pDefectFixState);
	4. ���������ʵ��ʹ��״̬
		dvp2api dvpStatus dvpSetDefectFixState(dvpHandle handle, bool DefectFixState);

��. ����ԭʼ��Ŀ��ͼ���ʽ�����Ķ�д����
	1. ��ȡ��ǰԭʼͼ���ʽѡ�������
		dvp2api dvpStatus dvpGetSourceFormatSel(dvpHandle handle, dvpUint32 *pSourceFormatSel);
	2. ���õ�ǰԭʼͼ���ʽѡ�������
		dvp2api dvpStatus dvpSetSourceFormatSel(dvpHandle handle, dvpUint32 SourceFormatSel);
	3. ��ȡԭʼͼ���ʽѡ��������������Ϣ
		dvp2api dvpStatus dvpGetSourceFormatSelDescr(dvpHandle handle, dvpSelectionDescr *pSourceFormatSelDescr);
	4. ��ȡ��ǰԭʼͼ���ʽѡ��������Ӧ�ľ�����Ϣ
		dvp2api dvpStatus dvpGetSourceFormatSelDetail(dvpHandle handle, dvpUint32 SourceFormatSel, dvpFormatSelection *pSourceFormatSelDetail);
	5. ��ȡ��ǰ���ͼ���ʽѡ�������
		dvp2api dvpStatus dvpGetTargetFormatSel(dvpHandle handle, dvpUint32 *pTargetFormatSel);
	6. ���õ�ǰ���ͼ���ʽѡ�������
		dvp2api dvpStatus dvpSetTargetFormatSel(dvpHandle handle, dvpUint32 TargetFormatSel);
	7. ��ȡ���ͼ���ʽѡ��������������Ϣ
		dvp2api dvpStatus dvpGetTargetFormatSelDescr(dvpHandle handle, dvpSelectionDescr *pTargetFormatSelDescr);
	8. ��ȡ��ǰ���ͼ���ʽѡ��������Ӧ�ľ�����Ϣ
		dvp2api dvpStatus dvpGetTargetFormatSelDetail(dvpHandle handle, dvpUint32 TargetFormatSel, dvpFormatSelection *pTargetFormatSelDetail);

��. ���ӻ�ȡ��һ�����ض�Ӧ��λ��
	1. ��ȡ��һ�����ض�Ӧ��λ��
		dvp2api dvpStatus dvpGetFirstPosition(dvpHandle handle, dvpFirstPosition *pFirstPosition);

��. ����ͬʱ��ȡʱ�����֡ID��ͬ��ԭʼ�ɼ�֡��Ŀ�����֡
	1. ͬʱ��ȡʱ�����֡ID��ͬ��ԭʼ�ɼ�֡��Ŀ�����֡
		dvp2api dvpStatus dvpGetFrameBuffer(dvpHandle handle, dvpFrameBuffer *pRaw, dvpFrameBuffer *pOut, dvpUint32 timeout);

ʮ. �����¼�֪ͨ�ص�����
	1. ע���¼�֪ͨ�Ļص�����
		dvp2api dvpStatus dvpRegisterEventCallback(dvpHandle handle, dvpEventCallback proc, dvpEvent event, void *pContext);
	2. ע���¼�֪ͨ�Ļص�����
		dvp2api dvpStatus dvpUnregisterEventCallback(dvpHandle handle, dvpEventCallback proc, dvpEvent event, void *pContext);
	3. �¼�֪ͨ�ص�����
		typedef dvpInt32(dvpEventCallback)(dvpHandle handle, dvpEvent event, void *pContext, dvpInt32 param, struct dvpVariant *pVariant);

2017-02-08 DVPCamera.dll �汾�� (2.9.13)
һ. ���Ӹ�λ�豸����
	1. ��λ�豸����
		dvpStatus dvpResetDevice(dvpHandle handle);

2017-03-22 DVPCamera.dll �汾�� (2.10.16)
һ. ����ʹ���Զ������ʵ�Ĺ���
	1. �Զ������ʵ㹦��
		dvpStatus dvpSetAutoDefectFixState(dvpHandle handle, bool AutoDefectFixState);

2017-05-12 DVPCamera.dll �汾�� (2.11.18)
һ. ����ʹ���Զ������ʵ�Ĺ���
	1. ʹ���Զ������ʵ�
		dvpStatus dvpSetAutoDefectFixState (dvpHandle handle,bool AutoDefectFixState);
	2. ��ȡ�Զ������ʵ��ʹ��״̬
		dvpStatus dvpGetAutoDefectFixState(dvpHandle handle, bool *pAutoDefectFixState);

2017-05-12 DVPCamera.dll �汾�� (2.11.18)
һ. �����жϴ�������ͼ��ɼ��Ĺ���
	1. �жϴ�������ͼ��ɼ�
		dvpStatus dvpHold(dvpHandle handle);

��. �������Ӳ��ISPʹ�ܵĹ���
	1. �����������Ӳ��ISP��ʹ��
		dvpStatus dvpSetHardwareIspState(dvpHandle handle, bool HardwareIspState);
	2. ��ȡ�������Ӳ��ISP��ʹ��
		dvpStatus dvpGetHardwareIspState(dvpHandle handle, bool *pHardwareIspState);

2017-06-16 DVPCamera.dll �汾�� (2.12.19)
һ. ���ӻ�ȡ�������������Ϣ�Ľӿ�
	1. �������������Ϣ���� dvpFunctionInfo
	2. ��ȡ������Ϣ
		dvpStatus dvpGetFunctionInfo(dvpHandle handle, dvpFunctionInfo *pFunctionInfo);

��. ����RGB32ͼ���ʽ�����֧��
	1. ����RGB32ͼ���ʽ��� dvpImageFormat

2017-07-17 DVPCamera.dll �汾�� (2.14.21)
һ. ��������������ʽ�Ķ���
	1. ��������ʽ�Ķ��� dvpStreamFormat

2017-09-15 DVPCamera.dll �汾�� (2.15.24)
һ. �����˸����û�������������Ĺ���
	1. �����û������������
		dvpStatus dvpOpenByUserId(dvpStr UserId, dvpOpenMode mode, dvpHandle *pHandle);

��. �����˼����û����õĹ���
	1. ������洢�������û�����
		dvpStatus dvpLoadUserSet(dvpHandle handle, dvpUserSet UserSet);

2018-04-11 DVPCamera.dll �汾�� (2.16.32)
һ. ���������û��߳��д���ͼ��Ĺ���
	1. ������Ƶ���ص��¼����� dvpStreamEvent::STREAM_EVENT_FRAME_THREAD
	2. ���û��߳��ж�Դͼ����У�ISP�������õ�Ŀ��ͼ��
		dvpStatus dvpProcessImage(dvpHandle handle, const dvpFrame *pSourceFrame, const void *pSourceBuffer, dvpFrame *pTargetFrame, void *pTargetBuffer, dvpUint32 targetBufferSize, dvpStreamFormat targetFormat);
