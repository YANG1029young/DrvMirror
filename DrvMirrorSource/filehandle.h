#ifndef FILEHANDLE_H
#define FILEHANDLE_H

#include "common.h"

int saveCameraParam( string filename ,float warped_image_focal, vector<CameraParams> cameras);
int loadCameraParam( string filename ,float *warped_image_scale,vector<CameraParams> *camerainfo);
int saveCornerSize( string filename, vector<Point> corners, vector<Size> sizes);
int loadCornerAndSizes( string filename ,vector<Point> *corners_load,vector<Size> *sizes_load);

#endif // FILEHANDLE

