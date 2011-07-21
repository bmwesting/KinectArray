// DeviceManager: Simplifies access and control of multiple Devices

#ifndef _DEVICEMANAGER_H
#define _DEVICEMANAGER_H

#include <XnCppWrapper.h>
#include <vector>

#include "SensorDevice.h"

class DeviceManager
{
    public:
        DeviceManager();
        ~DeviceManager();
        
        unsigned int GetNODevicesConnected();
        SensorDevice* GetFirstDevice();
        SensorDevice* GetDeviceByIndex(const unsigned int index);
        void SetImageCameraModeToIR(){};
        void SetImageCameraModeToYUV(){};
        void SetImageCameraModeToRGB(){};
        int GetCameraDataByDeviceIndex(const unsigned int index, CameraDataPacket* dataPacket);
        int GetCameraDataForAllDevices(std::vector<CameraDataPacket>& dataPackets);
        
    private:
        std::vector<SensorDevice*> devices_;
        std::vector<CameraDataPacket*> cameraDataVector_;

        xn::Context* context_;
           
        void EnumerateDevices();

};

#endif