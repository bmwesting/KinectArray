// SensorDevice: Abstract class to encapsulate OpenNI devices

#include <iostream>

#include "SensorDevice.h"

SensorDevice::SensorDevice(){}

SensorDevice::~SensorDevice(){}

void SensorDevice::AlignSensors()
{

	XnStatus nRetVal = XN_STATUS_OK;
	
	//Align depth image to RGB Camera image
	XnBool isSupported = depthG_.IsCapabilitySupported("AlternativeViewPoint");

	if(TRUE == isSupported)
	{
		std::cout << "Aligning depth to RGB is supported with device." << std::endl;
		nRetVal = depthG_.GetAlternativeViewPointCap().SetViewPoint(imageG_);
		CHECK_RV(nRetVal, "AlignSensors");
	}
}

int SensorDevice::SetRGBCameraResolution(unsigned int x, unsigned int y)
{
    
}

int SensorDevice::SetDepthCameraResolution(unsigned int x, unsigned int y)
{

}

unsigned int SensorDevice::GetDepthCameraFramerate()
{
	return (unsigned int)depthMD_.FPS();
}

unsigned int SensorDevice::GetRGBCameraFramerate()
{
	return (unsigned int)imageMD_.FPS();
}

const XnDepthPixel* SensorDevice::GetDepthData()
{
	
	if(depthG_.IsValid())
	{
		depthG_.GetMetaData(depthMD_);
	}
	
	return depthMD_.Data();
	
}


const XnRGB24Pixel* SensorDevice::GetImageData()
{
	
	if(imageG_.IsValid())
	{
		imageG_.GetMetaData(imageMD_);
	}
	
	return imageMD_.RGB24Data();
}

void SensorDevice::SetImageModeToRGB()
{

}

void SensorDevice::SetImageModeToIR()
{

}

void SensorDevice::SetImageModeToYUV()
{

}

void SensorDevice::SetDeviceName(char* name)
{
	deviceName_ = name;
}