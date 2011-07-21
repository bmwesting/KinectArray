// DeviceManager: Simplifies access and control of multiple Devices

#include "DeviceManager.h"
#include "defines.h"

void CHECK_RV(XnStatus rv, char *type)
{
    if(rv != XN_STATUS_OK)
    {
        printf("%s failed: %s\n", type, xnGetStatusString(rv));
    }
}

DeviceManager::DeviceManager()
{
    context_ = new xn::Context();
	context_->Init();
	
	this->EnumerateDevices();
	
}

//Iterate over all connected devices and delete their components
DeviceManager::~DeviceManager()
{
    for(unsigned int i = 0; i < devices_.size(); i++)
    {
        delete devices_[i];
    }
    
    context_->Shutdown();
    delete context_;
}

void DeviceManager::EnumerateDevices()
{

	XnStatus nRetVal = XN_STATUS_OK;

    xn::NodeInfoList deviceList;
    nRetVal = context_->EnumerateProductionTrees(XN_NODE_TYPE_DEVICE, NULL, deviceList, NULL);
    CHECK_RV(nRetVal, "Enumerate");
    
    for(xn::NodeInfoList::Iterator iter = deviceList.Begin(); iter != deviceList.End(); ++iter) 
    { 
        xn::NodeInfo node = (*iter);
        
        SensorDevice* sensor = new SensorDevice();
        char deviceName[256];
        
        //Create Device Context in the global production tree
        nRetVal = context_->CreateProductionTree(node);
        CHECK_RV(nRetVal, "Create Device");
        
        Query query;
        query.AddNeededNode(node.GetInstanceName());
        
        xnOSMemCopy(deviceName, node.GetInstanceName(), xnOSStrLen(node.GetInstanceName()));
        sensor->SetDeviceName(deviceName);
        
        //Create Image and Depth generators for this device
        nRetVal = context_->CreateAnyProductionTree(XN_NODE_TYPE_IMAGE, &query, *(sensor->GetImageGenerator()));
        CHECK_RV(nRetVal, "Create Image Generator");
        nRetVal = context_->CreateAnyProductionTree(XN_NODE_TYPE_DEPTH, &query, *(sensor->GetDepthGenerator()));
        CHECK_RV(nRetVal, "Create Depth Generator");
        
        //Align the RGB and Depth Cameras
        sensor->AlignSensors();
        
        devices_.push_back(sensor);
	}
	
	context_->StartGeneratingAll();
}

unsigned int DeviceManager::GetNODevicesConnected()
{
    return devices_.size();
}

int DeviceManager::GetCameraDataByDeviceIndex(const unsigned int index, CameraDataPacket* dataPacket)
{

	if(index > devices_.size())
	{
		printf("Index invalid, requested index %d, only %d devices available!\n", index, devices_.size());
		return -1;
	}
	
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = context_->WaitAnyUpdateAll();
	CHECK_RV(nRetVal, "WaitAnyUpdateAll() failed");
	
	if(devices_[index]->GetDepthGenerator()->IsValid())
	{
		devices_[index]->GetDepthGenerator()->GetMetaData(*(devices_[index]->GetDepthMD()));
	}
	
	if(devices_[index]->GetImageGenerator()->IsValid())
	{
		devices_[index]->GetImageGenerator()->GetMetaData(*(devices_[index]->GetImageMD()));
	}
	
	dataPacket->depthMap_ = devices_[index]->GetDepthMD()->Data();
	dataPacket->imageMap_ = devices_[index]->GetImageMD()->RGB24Data();
	
	return 0;

}

int DeviceManager::GetCameraDataForAllDevices(std::vector<CameraDataPacket>& dataPackets)
{

	dataPackets.clear();
	for (unsigned int i = 0; i < devices_.size(); i++)
	{
		CameraDataPacket dataPacket;
		GetCameraDataByDeviceIndex(i, &dataPacket);
		
		dataPackets.push_back(dataPacket);
	}
	
	return 0;
}

SensorDevice* DeviceManager::GetDeviceByIndex(const unsigned int index)
{

	if(index > devices_.size())
	{
		printf("Requested device %d, only %d devices present!\n", index, devices_.size());
		return NULL;
	}
	
	return devices_[index];
}
