// SensorDevice: Class that encapsulates OpenNI devices

#ifndef _SENSORDEVICE_H
#define _SENSORDEVICE_H

#include <XnCppWrapper.h>
#include <string>

#include "defines.h"

struct CameraDataPacket
{
    const XnDepthPixel* depthMap_;
    const XnRGB24Pixel* imageMap_;
};

class SensorDevice
{

	public:
		SensorDevice();
		SensorDevice(xn::Context* context);
		~SensorDevice();
		
		int SetRGBCameraResolution(unsigned int x, unsigned int y);
		int SetDepthCameraResolution(unsigned int x, unsigned int y);
		void SetImageModeToRGB();
		void SetImageModeToIR();
		void SetImageModeToYUV();
		unsigned int GetDepthCameraFramerate();
		unsigned int GetRGBCameraFramerate();
		const XnDepthPixel* GetDepthData();
		const XnRGB24Pixel* GetImageData();
		xn::DepthGenerator* GetDepthGenerator(){ return &depthG_; }
		xn::ImageGenerator* GetImageGenerator(){ return &imageG_; }
		xn::DepthMetaData* GetDepthMD(){ return &depthMD_; }
		xn::ImageMetaData* GetImageMD(){ return &imageMD_; }
		void AlignSensors();
		void SetDeviceName(char* name);
		const char* GetDeviceName(){ return deviceName_.c_str(); }
				
	private:
	    xn::DepthGenerator depthG_;
	    xn::ImageGenerator imageG_;
	    xn::DepthMetaData depthMD_;
	    xn::ImageMetaData imageMD_;
	    CameraDataPacket dataPacket_;
	    std::string deviceName_;
};

#endif