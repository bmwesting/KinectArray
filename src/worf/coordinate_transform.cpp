#include "coordinate_transform.h"
#include <math.h>

// note: angles given in degrees
void spherical_to_cartesian(float r, float azimuth, float elevation, float &x, float &y, float &z)
{
    azimuth *= M_PI / 180.;
    elevation *= M_PI / 180.;

    x = r * cosf(azimuth) * cosf(elevation);
    y = r * cosf(elevation) * sinf(azimuth);
    z = r * sinf(elevation);
}

void cartesian_to_spherical(float x, float y, float z, float &r, float &azimuth, float &elevation)
{
    r = sqrtf(x*x+y*y+z*z);
    azimuth = atan2f(y, x) * 180./M_PI;
    elevation = asinf(z/r) * 180./M_PI;
}

void spherical_to_cartesian(double r, double azimuth, double elevation, double &x, double &y, double &z)
{
    azimuth *= M_PI / 180.;
    elevation *= M_PI / 180.;

    x = r * cos(azimuth) * cos(elevation);
    y = r * cos(elevation) * sin(azimuth);
    z = r * sin(elevation);
}

void cartesian_to_spherical(double x, double y, double z, double &r, double &azimuth, double &elevation)
{
    r = sqrt(x*x+y*y+z*z);
    azimuth = atan2(y, x) * 180./M_PI;
    elevation = asin(z/r) * 180./M_PI;
}
