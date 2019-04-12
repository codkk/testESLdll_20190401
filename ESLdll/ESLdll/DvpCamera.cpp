#include "stdafx.h"
#include "DvpCamera.h"

// Convert the type of string from ANSI to UNICODE.
#ifdef _M_X64
#pragma comment(lib, "./library/Visual C++/lib/x64/DVPCamera64.lib")
#else
#pragma comment(lib, "./library/Visual C++/lib/x86/DVPCamera.lib")
#endif


DvpCamera::DvpCamera()
{
	m_handle = 0;
	m_strName = "USB2_5M@UE500901474";
	m_bSoftTrigger = false;
	m_uGrabTimeout = 5000;
	m_pImageBuffer = NULL;
	m_pDataSeparate = NULL;
}

dvpStatus DvpCamera::OpenCamera(char* name)
{
	dvpStatus status = DVP_STATUS_OK;
	//CString strName;

	if (!IsValidHandle(m_handle))
	{
		// Open the specific device by the selected FriendlyName.

		status = dvpOpenByName(name, OPEN_NORMAL, &m_handle);


		if (status != DVP_STATUS_OK)
		{
			AfxMessageBox(_T("Open the device failed!"));
		}
		return status;
	}


	return DVP_STATUS_OK;
}

dvpStatus DvpCamera::LoadConfig(char * inifile)
{
	dvpStatus status = DVP_STATUS_OK;
	//CString strName;

	if (IsValidHandle(m_handle))
	{
		// Open the specific device by the selected FriendlyName.

		status = dvpLoadConfig(m_handle, inifile);

		if (status != DVP_STATUS_OK)
		{
			AfxMessageBox(_T("Load ini config failed!"));
		}
		return status;
	}


	return status;
}

DvpCamera::~DvpCamera()
{
	CloseCamera();

	if (m_pImageBuffer)
	{
		free(m_pImageBuffer);
		m_pImageBuffer = NULL;
	}
	if (m_pDataSeparate)
	{
		free(m_pDataSeparate);
		m_pDataSeparate = NULL;
	}
}

dvpStatus DvpCamera::CloseCamera()
{
	dvpStatus status = DVP_STATUS_OK;
	if (IsValidHandle(m_handle))
	{
		dvpStreamState state;
		dvpGetStreamState(m_handle, &state);
		if (state == STATE_STARTED)
		{
			status = dvpStop(m_handle);
			ASSERT(status == DVP_STATUS_OK);
		}

		status = dvpClose(m_handle);
		ASSERT(status == DVP_STATUS_OK);
		m_handle = 0;
	}
	return status;
}

dvpStatus DvpCamera::StartCamera()
{
	dvpStatus status = DVP_STATUS_OK;
	if (IsValidHandle(m_handle))
	{
		dvpStreamState state;
		status = dvpGetStreamState(m_handle, &state);
		ASSERT(status == DVP_STATUS_OK);

		if (state != STATE_STOPED)
		{

		}
		else
		{
			bool bTrigerFlg = false;
			m_bSoftTrigger = false; //不软件触发
			status = dvpGetTriggerState(m_handle, &bTrigerFlg);

			// Set as Trigger mode before starting video stream.
			if (status == DVP_STATUS_OK)
			{
				status = dvpSetTriggerState(m_handle, m_bSoftTrigger ? TRUE : FALSE);
				if (status != DVP_STATUS_OK)
				{
					// Confirm whether the camera support the trigger function.
					AfxMessageBox(_T("Set trigger mode fails"));
				}
			}

			status = dvpStart(m_handle);
			if (status != DVP_STATUS_OK)
			{
				AfxMessageBox(_T("Start the video stream failed!"));
			}
		}
	}
	return status;
}

dvpStatus DvpCamera::StopCamera()
{
	dvpStatus status = DVP_STATUS_OK;
	if (IsValidHandle(m_handle))
	{
		dvpStreamState state;
		status = dvpGetStreamState(m_handle, &state);
		ASSERT(status == DVP_STATUS_OK);

		if (state != STATE_STOPED)
		{
			status = dvpStop(m_handle);
			ASSERT(status == DVP_STATUS_OK);
		}
	}
	return status;
}

dvpStatus DvpCamera::GrabHalconImage()
{
	dvpStatus status = DVP_STATUS_OK;
	void *pBuffer;
	dvpFrame frame;
	if (IsValidHandle(m_handle))
	{
		// Grab a frame image from the video stream and timeout should not less than the current exposure time.
		status = dvpGetFrame(m_handle, &frame, &pBuffer, m_uGrabTimeout);
		if (status != DVP_STATUS_OK)
		{
			TRACE("dvpGetFrame operation failed, status:%d\r\n", status);
		}
		else
		{
			//HObject image;
			if (0 == ConvertoHalcon((unsigned char*)pBuffer, frame, m_image))
			{
				status = DVP_STATUS_OK;
			}
			if (m_image.IsInitialized())
			{
				HalconCpp::WriteImage(m_image, "bmp", 0, "save");
			}

			//CString strFilename;
			//strFilename.Format(_T("GrabImage.bmp"));
			//status = dvpSavePicture(&frame, pBuffer, w2a(strFilename).c_str(), 100);
			//ASSERT(status == DVP_STATUS_OK);
		}


	}
	return status;
}

dvpStatus DvpCamera::GrabImageAndSave(char * path)
{
	dvpStatus status = DVP_STATUS_OK;
	void *pBuffer;
	dvpFrame frame;
	if (IsValidHandle(m_handle))
	{
		Sleep(1);
		// Grab a frame image from the video stream and timeout should not less than the current exposure time.
		status = dvpGetFrame(m_handle, &frame, &pBuffer, m_uGrabTimeout);
		if (status != DVP_STATUS_OK)
		{
			TRACE("dvpGetFrame operation failed, status:%d\r\n", status);
		}
		else
		{
			CString strFilename;
			strFilename.Format(_T("GrabImage.bmp"));
			status = dvpSavePicture(&frame, pBuffer, path, 100);
			ASSERT(status == DVP_STATUS_OK);
		}
	}
	return status;
}

int DvpCamera::ConvertoHalcon(unsigned char * pData, dvpFrame stImageInfo, HalconCpp::HObject & image)
{
	if (NULL == m_pImageBuffer)//Allocates the cache needed to transform the image，Mono8 and RGB are no need to allocate memory
	{
		if (FORMAT_MONO != stImageInfo.format)
		{
			m_pImageBuffer = (unsigned char *)malloc(sizeof(unsigned char) * stImageInfo.iWidth * stImageInfo.iHeight * 3);
		}
	}
	int nRet = 0;
	unsigned char* pTemp = NULL;
	if (FORMAT_MONO != stImageInfo.format)//Color
	{
		if (NULL == m_pDataSeparate)
		{
			m_pDataSeparate = (unsigned char *)malloc(sizeof(unsigned char) * stImageInfo.iHeight * stImageInfo.iWidth * 3);
		}

		int nWidth = stImageInfo.iWidth;
		int nHeight = stImageInfo.iHeight;
		unsigned char *dataBlue = m_pDataSeparate;
		unsigned char *dataGreen = m_pDataSeparate + (nWidth * nHeight);
		unsigned char *dataRed = m_pDataSeparate + (2 * nWidth * nHeight);
		int nSupWidth = (nWidth + 3) & 0xfffffffc;  //宽度补齐为4的倍数

		for (int row = 0; row <nHeight; row++)
		{
			unsigned char* ptr = &pData[row * nWidth * 3];
			for (int col = 0; col < nWidth; col++)
			{
				dataBlue[row * nSupWidth + col] = ptr[3 * col];
				dataGreen[row * nSupWidth + col] = ptr[3 * col + 1];
				dataRed[row * nSupWidth + col] = ptr[3 * col + 2];
			}
		}

		HalconCpp::GenImage3(&image, "byte", nWidth, nHeight, (Hlong)(dataRed), (Hlong)(dataGreen), (Hlong)(dataBlue));
		return 0;
	}
	return -1;
}
