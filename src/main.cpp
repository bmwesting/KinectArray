#include <stdio.h>

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkType.h>
#include <vtkCamera.h>
#include <vtkUnsignedCharArray.h>
#include <vtkPointData.h>
#include <vtkRendererCollection.h>

#include <XnCppWrapper.h>

#include "DeviceManager.h"
#include "SensorDevice.h"

struct vtkRenderGroup
{
	vtkSmartPointer<vtkPolyData> polyData_;
	vtkSmartPointer<vtkPolyDataMapper> mapper_;
	vtkSmartPointer<vtkActor> actor_;
};

DeviceManager* devMan;
std::vector<CameraDataPacket> cameraDataVector;
std::vector<vtkRenderGroup> renderGroups;
vtkSmartPointer<vtkRenderWindow> renwin;

void updatePolyData()
{

	for(unsigned int i = 0; i < cameraDataVector.size(); i++)
	{
	
		// need to create a new render group for any new camera device
		if (renderGroups.size() <= i)
		{
			vtkRenderGroup renderGroup;
			renderGroup.polyData_ = vtkSmartPointer<vtkPolyData>::New();
			renderGroup.mapper_ = vtkSmartPointer<vtkPolyDataMapper>::New();
			renderGroup.actor_ = vtkSmartPointer<vtkActor>::New();
			
			renderGroup.mapper_->SetInput(renderGroup.polyData_);
			renderGroup.actor_->SetMapper(renderGroup.mapper_);
			renwin->GetRenderers()->GetFirstRenderer()->AddActor(renderGroup.actor_);
			
			renderGroups.push_back(renderGroup);
		}
		
		vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
		vtkSmartPointer<vtkCellArray> cellArray = vtkSmartPointer<vtkCellArray>::New();
		vtkSmartPointer<vtkUnsignedCharArray> color = vtkSmartPointer<vtkUnsignedCharArray>::New();
		color->SetName("Color");
		color->SetNumberOfComponents(3);
	
		for(int y = 0; y < 480; y++)
		{
			for(int x = 0; x < 640; x++)
			{
				XnDepthPixel d = cameraDataVector[i].depthMap_[x+y*640];
				XnRGB24Pixel c = cameraDataVector[i].imageMap_[x+y*640];
	
				vtkIdType id;
				if (d == 0)
				{
					id = points->InsertNextPoint(x,y,4096*.3);
					color->InsertNextTuple3(0,0,0);
				}
				else
				{
					id = points->InsertNextPoint(x,y,d*.3);
					color->InsertNextTuple3(c.nRed, c.nGreen, c.nBlue);
				}
	
				cellArray->InsertNextCell(1);
				cellArray->InsertCellPoint(id);
			}
		}
	
		renderGroups[i].polyData_->SetPoints(points);
		renderGroups[i].polyData_->SetVerts(cellArray);
		renderGroups[i].polyData_->GetPointData()->SetScalars(color);
		renderGroups[i].polyData_->Modified();
		renderGroups[i].polyData_->Update();
	}

	renwin->Render();
}

class UpdateData:public vtkCommand
{
	public:
		static UpdateData *New()
		{
			UpdateData* up = new UpdateData;
			return up;
		}
		virtual void Execute(vtkObject* caller, unsigned long eid, void* clientdata)
		{
			//polyData->Initialize();
			//devMan->GetCameraDataByDeviceIndex(0, &dataPacket);
			devMan->GetCameraDataForAllDevices(cameraDataVector);
			updatePolyData();
		}
};

int main(int argc, char** argv)
{
	
	devMan = new DeviceManager();
	
	unsigned int i = devMan->GetNODevicesConnected();
	
	if (i < 1)
	{
		std::cout << "No devices found." << std::endl;
		return -1;
	}
	
	printf("Number of devices connected: %d.\n", i);
	
	//VTK Pipeline
	vtkSmartPointer<vtkRenderer> ren = vtkSmartPointer<vtkRenderer>::New();
	renwin = vtkSmartPointer<vtkRenderWindow>::New();
	vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();

	renwin->AddRenderer(ren);
	renwin->SetInteractor(iren);
	renwin->SetSize(1280,800);
	iren->Initialize();

	//Add timer callback
	vtkSmartPointer<UpdateData> updateCallback = vtkSmartPointer<UpdateData>::New();
	iren->AddObserver(vtkCommand::TimerEvent, updateCallback);
	iren->CreateRepeatingTimer(100);
	
	//devMan->GetCameraDataByDeviceIndex(0, &dataPacket);
	devMan->GetCameraDataForAllDevices(cameraDataVector);
	updatePolyData();

	//Set up camera
	ren->ResetCamera();
	ren->GetActiveCamera()->Roll(180.0);
	ren->GetActiveCamera()->Azimuth(180.0);
	ren->GetActiveCamera()->Zoom(2.0);

	iren->Start();
	
	delete devMan;
	
	return 0;
}
