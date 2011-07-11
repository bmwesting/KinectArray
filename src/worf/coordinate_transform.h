#ifndef COORDINATE_TRANSFORM_H
#define COORDINATE_TRANSFORM_H

extern void spherical_to_cartesian(float r, float azimuth, float elevation, float &x, float &y, float &z);
extern void cartesian_to_spherical(float x, float y, float z, float &r, float &azimuth, float &elevation);

extern void spherical_to_cartesian(double r, double azimuth, double elevation, double &x, double &y, double &z);
extern void cartesian_to_spherical(double x, double y, double z, double &r, double &azimuth, double &elevation);

#endif
