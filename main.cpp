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

#include <XnCppWrapper.h>

using namespace xn;

Context context;
DepthGenerator depthG;
DepthMetaData depthMD;
ImageGenerator imageG;

vtkSmartPointer<vtkPolyData> polyData;
vtkSmartPointer<vtkRenderWindow> renwin;

void updatePolyData(const XnDepthPixel* depth, const XnRGB24Pixel* rgb)
{
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> cellArray = vtkSmartPointer<vtkCellArray>::New();
	vtkSmartPointer<vtkUnsignedCharArray> color = vtkSmartPointer<vtkUnsignedCharArray>::New();
	color->SetName("Color");
	color->SetNumberOfComponents(3);

	for(int y = 0; y < 480; y++)
	{
		for(int x = 0; x < 640; x++)
		{
			XnDepthPixel d = depth[x+y*640];
			XnRGB24Pixel c = rgb[x+y*640];

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

	polyData->SetPoints(points);
	polyData->SetVerts(cellArray);
	polyData->GetPointData()->SetScalars(color);
	polyData->Modified();
	polyData->Update();
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
			polyData->Initialize();
			context.WaitOneUpdateAll(depthG);
			const XnDepthPixel* pDepthMap = depthG.GetDepthMap();
			const XnRGB24Pixel* pImageMap = imageG.GetRGB24ImageMap();
			updatePolyData(pDepthMap, pImageMap);
		}
};

int main(int argc, char** argv)
{
	context.Init();

	depthG.Create(context);
	imageG.Create(context);

	XnMapOutputMode mapMode;
	mapMode.nXRes = XN_VGA_X_RES;
	mapMode.nYRes = XN_VGA_Y_RES;
	mapMode.nFPS = 30;
	depthG.SetMapOutputMode(mapMode);
	imageG.SetMapOutputMode(mapMode);

	//Align depth image to RGB Camera image
	XnBool isSupported = depthG.IsCapabilitySupported("AlternativeViewPoint");

	if(TRUE == isSupported)
	{
		std::cout << "Aligning depth to RGB is supported with device." << std::endl;
		XnStatus res = depthG.GetAlternativeViewPointCap().SetViewPoint(imageG);
		if (XN_STATUS_OK != res)
			printf("Setting alternative viewpoint for depth failed: %s\n", xnGetStatusString(res));
	}

	context.StartGeneratingAll();

	//XnUInt32 nMiddleIndex = ((640*480)/2)+(640/2);

	//VTK Pipeline
	polyData = vtkSmartPointer<vtkPolyData>::New();
	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	vtkSmartPointer<vtkRenderer> ren = vtkSmartPointer<vtkRenderer>::New();
	renwin = vtkSmartPointer<vtkRenderWindow>::New();
	vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();

	mapper->SetInput(polyData);
	actor->SetMapper(mapper);
	ren->AddActor(actor);
	renwin->AddRenderer(ren);
	renwin->SetInteractor(iren);
	renwin->SetSize(1280,800);
	iren->Initialize();

	//Add timer callback
	vtkSmartPointer<UpdateData> updateCallback = vtkSmartPointer<UpdateData>::New();
	iren->AddObserver(vtkCommand::TimerEvent, updateCallback);
	iren->CreateRepeatingTimer(100);

	context.WaitOneUpdateAll(depthG);
	const XnDepthPixel* pDepthMap = depthG.GetDepthMap();
	const XnRGB24Pixel* pImageMap = imageG.GetRGB24ImageMap();
	updatePolyData(pDepthMap, pImageMap);

	//Set up camera
	ren->ResetCamera();
	ren->GetActiveCamera()->Roll(180.0);
	ren->GetActiveCamera()->Azimuth(180.0);
	ren->GetActiveCamera()->Zoom(2.0);

	iren->Start();

	context.Shutdown();
}
