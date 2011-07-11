#ifndef WORF_CAMERA
#define WORF_CAMERA

// translates pixel displacements to degree changes in azimuth / elevation
#define MOUSE_AIM_SENSITIVITY 0.1

// these scale unitLength for camera pans / moves
#define MOUSE_PAN_FACTOR 0.01
#define MOUSE_MOVE_FACTOR 0.01

// these scale unitLength to form near / far clipping distances
#define NEAR_CLIPPING_FACTOR 0.01
#define FAR_CLIPPING_FACTOR 100000.

struct WorfCameraState {
    double coordinates[3];
    double azimuth, elevation;
};

enum cameraMode_t { perspective, orthographic };

class WorfCamera {

    public:

        // mode of the camera (2D or 3D)
        cameraMode_t mode_;

        // representative length used for scaling camera movement factors
        double unitLength_;

        // field of view (vertical)
        double fovY_;

        // manually set aspect ratio, height / width, value of 0 and it won't be used
        double aspect_;

        // fraction of computed view space for this camera (used for tiled display modes, or zooming...)
        // defaults to (x1,y1,x2,y2) = (0,0,1,1) for a full view
        double x1_, x2_, y1_, y2_;

        // ambient lighting
        double ambientLight_;

        // state
        double coordinates_[3];
        double direction_[3];
        double up_[3];

        // inputs
        double speed_;
        double azimuth_;
        double elevation_;

        WorfCamera();
        ~WorfCamera();

        void setView(int width, int height);
        void motion(int mouseButton, int dx, int dy);
        void stopMotion();
        void setMode(cameraMode_t mode);
        void setUnitLength(double l);
        void setCoordinates(double x, double y, double z);
        void setDirection(double az, double el);
        WorfCameraState getState();
        void setState(WorfCameraState wcs);
        void setFovY(double fov);
        void setAspect(double aspect);
        void setViewBounds(double x1, double y1, double x2, double y2);
        void setAmbientLight(double ambientLight);
};

#endif
