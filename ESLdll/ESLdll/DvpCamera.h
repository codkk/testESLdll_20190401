#pragma once
# include "HalconCpp.h" //halcon
# include "HDevThread.h"
#include "./library/Visual C++/include/DVPCamera.h"
using namespace HalconCpp;

class DvpCamera {
public:
	dvpHandle m_handle;
	CString m_strName;
	bool m_bSoftTrigger;
	UINT m_uGrabTimeout;
	unsigned char *m_pImageBuffer;
	unsigned char *m_pDataSeparate;
	HObject m_image;
public:
	DvpCamera();
	~DvpCamera();
	dvpStatus OpenCamera(char* name);
	dvpStatus LoadConfig(char* inifile);
	dvpStatus CloseCamera();
	dvpStatus StartCamera();
	dvpStatus StopCamera();
	dvpStatus GrabHalconImage();
	dvpStatus GrabImageAndSave(char* path);
	int ConvertoHalcon(unsigned char * pData, dvpFrame stImageInfo, HalconCpp::HObject& image);
	bool IsValidHandle(dvpHandle handle)
	{
		bool bValidHandle;
		dvpStatus status = dvpIsValid(handle, &bValidHandle);
		if (status == DVP_STATUS_OK)
		{
			return bValidHandle;
		}
		return false;
	}

};

