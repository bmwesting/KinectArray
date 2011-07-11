#include "WorfCamera.h"
#include "vector.h"
#include "coordinate_transform.h"
#include "log.h"
#include <math.h>

#ifdef __APPLE__
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
#else
    #include <GL/gl.h>
    #include <GL/glu.h>
#endif

WorfCamera::WorfCamera()
{
    put_flog(LOG_DEBUG, "");

    // default settings
    mode_ = perspective;

    unitLength_ = 100.;

    fovY_ = 45.;

    aspect_ = 0.;

    x1_ = 0.;
    x2_ = 1.;
    y1_ = 0.;
    y2_ = 1.;

    coordinates_[0] = unitLength_;
    coordinates_[1] = 0.;
    coordinates_[2] = unitLength_;

    speed_ = 0.;
    azimuth_ = 180.;
    elevation_ = -45.;
}

WorfCamera::~WorfCamera()
{

}

void WorfCamera::setView(int width, int height)
{
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();

    if(mode_ == perspective)
    {
        double near = NEAR_CLIPPING_FACTOR * unitLength_;
        double far = FAR_CLIPPING_FACTOR * unitLength_;

        // which aspect to use: automatic calculated or manually set
        double aspect = (double)height / (double)width;

        if(aspect_ != 0.)
            aspect = aspect_;

        double winFovY = fovY_ * aspect;

        double top = tan(0.5 * winFovY * M_PI/180.) * near;
        double bottom = -top;
        double left = 1./aspect * bottom;
        double right = 1./aspect * top;

        glFrustum(left + x1_ * (right-left), left + x2_ * (right-left), bottom + y1_ * (top-bottom), bottom + y2_ * (top-bottom), near, far);

        //  the glFrustum() call is equivalent to this gluPerspective()
        // gluPerspective(45.0 * (GLdouble)height/(GLdouble)width, (GLdouble)width/(GLdouble)height, near, far);
    }
    else if(mode_ == orthographic)
    {
        // (coordinates_[0], coordinates_[1]) represents center position
        // coordinates_[2] represents "height" above it

        // which aspect to use: automatic calculated or manually set
        double aspect = (double)height / (double)width;

        if(aspect_ != 0.)
            aspect = aspect_;

        double winFovY = fovY_ * aspect;

        double top = tan(0.5 * winFovY * M_PI/180.) * coordinates_[2];
        double bottom = -top;
        double left = 1./aspect * bottom;
        double right = 1./aspect * top;

        top += coordinates_[1];
        bottom += coordinates_[1];
        left += coordinates_[0];
        right += coordinates_[0];

        gluOrtho2D(left, right, bottom, top);
    }

    glPushMatrix();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(mode_ == perspective)
    {
        // calc new direction vector
        spherical_to_cartesian(1.0, azimuth_, elevation_, direction_[0], direction_[1], direction_[2]);

        // calc new up vector
        spherical_to_cartesian(1.0, azimuth_, elevation_ + 90.0, up_[0], up_[1], up_[2]);

        // new lookat matrix
        glLoadIdentity ();

        gluLookAt(coordinates_[0], coordinates_[1], coordinates_[2], coordinates_[0]+direction_[0], coordinates_[1]+direction_[1], coordinates_[2]+direction_[2], up_[0], up_[1], up_[2]);

        coordinates_[0] += speed_ * cos(azimuth_ * M_PI / 180.) * cos(elevation_ * M_PI / 180.);
        coordinates_[1] += speed_ * sin(azimuth_ * M_PI / 180.) * cos(elevation_ * M_PI / 180.);
        coordinates_[2] += speed_ * sin(elevation_ * M_PI / 180.);
    }

    // enable depth testing and color tracking
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);

    // setup lighting
    GLfloat LightAmbient[] = { ambientLight_, ambientLight_, ambientLight_, 1.0 };
    GLfloat LightDiffuse[] = { .99, .99, .99, 1.0 };
    GLfloat LightSpecular[] = { .9,.9,.9, 1.0 };

    GLfloat LightPosition[] = { coordinates_[0], coordinates_[1], coordinates_[2], 1.0 };

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightAmbient);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);

    glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, LightSpecular);
    glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT1);
}

void WorfCamera::motion(int mouseButton, int dx, int dy)
{
    if(mode_ == perspective)
    {
        if(mouseButton != 0)
        {
            if(mouseButton == 1)
            {
                azimuth_ -= dx * MOUSE_AIM_SENSITIVITY;
                elevation_ += dy * MOUSE_AIM_SENSITIVITY;
            }
            else if(mouseButton == 2)
            {
                // get right vector
                double right[3];

                vectorCrossProduct(direction_, up_, right);

                coordinates_[0] += dx * right[0] * MOUSE_PAN_FACTOR * unitLength_;
                coordinates_[1] += dx * right[1] * MOUSE_PAN_FACTOR * unitLength_;
                coordinates_[2] += dx * right[2] * MOUSE_PAN_FACTOR * unitLength_;

                coordinates_[0] -= dy * up_[0] * MOUSE_PAN_FACTOR * unitLength_;
                coordinates_[1] -= dy * up_[1] * MOUSE_PAN_FACTOR * unitLength_;
                coordinates_[2] -= dy * up_[2] * MOUSE_PAN_FACTOR * unitLength_;
            }
            else if(mouseButton == 3)
            {
                speed_ -= dy * MOUSE_MOVE_FACTOR * unitLength_;
            }
        }
    }
    else if(mode_ == orthographic)
    {
        if(mouseButton != 0)
        {
            if(mouseButton == 1)
            {
                coordinates_[0] += dx * unitLength_;
                coordinates_[1] -= dy * unitLength_;
            }
            else if(mouseButton == 3)
            {
                coordinates_[2] += dy * unitLength_;
            }
        }
    }
}

void WorfCamera::stopMotion()
{
    speed_ = 0.;
}

void WorfCamera::setMode(cameraMode_t mode)
{
    mode_ = mode;
}

void WorfCamera::setUnitLength(double l)
{
    unitLength_ = l;
}

void WorfCamera::setCoordinates(double x, double y, double z)
{
    coordinates_[0] = x;
    coordinates_[1] = y;
    coordinates_[2] = z;
}

void WorfCamera::setDirection(double az, double el)
{
    azimuth_ = az;
    elevation_ = el;
}

WorfCameraState WorfCamera::getState()
{
    WorfCameraState wcs;

    wcs.coordinates[0] = coordinates_[0];
    wcs.coordinates[1] = coordinates_[1];
    wcs.coordinates[2] = coordinates_[2];

    wcs.azimuth = azimuth_;
    wcs.elevation = elevation_;

    return wcs;
}

void WorfCamera::setState(WorfCameraState wcs)
{
    coordinates_[0] =  wcs.coordinates[0];
    coordinates_[1] =  wcs.coordinates[1];
    coordinates_[2] =  wcs.coordinates[2];

    azimuth_ = wcs.azimuth;
    elevation_ = wcs.elevation;
}

void WorfCamera::setFovY(double fov)
{
    fovY_ = fov;
}

void WorfCamera::setAspect(double aspect)
{
    aspect_ = aspect;
}

void WorfCamera::setViewBounds(double x1, double y1, double x2, double y2)
{
    x1_ = x1;
    y1_ = y1;
    x2_ = x2;
    y2_ = y2;
}

void WorfCamera::setAmbientLight(double ambientLight)
{
    ambientLight_ = ambientLight;
}
