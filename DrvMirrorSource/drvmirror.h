/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                          License Agreement
//            For Pan Asia Technical Automotive Center Co.,Led.
//
// Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
// Copyright (C) 2009, Willow Garage Inc., all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

#ifndef DRVMIRROR_H
#define DRVMIRROR_H

#include "common.h"
#include "drvmirror_global.h"


using namespace std;
using namespace cv;
using namespace cv::detail;

/** @brief deffirent display mode

 */
enum DisplayMode {normal, pure, ear, turnleft, turnright};

/** @brief The class to handle Mutimodal Streaming Media Rearview Mirror

 */
class DRVMIRRORSHARED_EXPORT DrvMirror
{

public:

    DrvMirror();


    /** @brief Initial class

    @return Judging if initial success
     */
    int Initial(void);


    /** @brief Initial class

    @param Display_mode Display mode
    @param Compose_scale Scale between the compose image and original image
    @param BlendSwitch Switch to select if blend or not
    @return Judging if initial success
     */
    int Initial(DisplayMode Display_mode, double Compose_scale, bool BlendSwitch);

    /** @brief Interface to calibrate
     *

    @param Img_row Images used to calibrate
    @param Compose_scale Scale between the compose image and original image
    @return The blended image to judge the quality of calibration
     */
    Mat Calibraton(vector<Mat> Img_row);

    /** @brief Interface to save the calibration information

    @return Judging if save information success
     */
    int Save_Infor(void);


    /** @brief Interface to get the result image

    @param Img_row Images used to get the result image
    @return The result image
     */
    Mat Get_ResultImg(vector<Mat> Img_row);

    /** @brief Interface to get the result image

    @param Img_row Images used to get the result image
    @param Display_mode Display mode
    @return The result image
     */
    Mat Get_ResultImg(vector<Mat> Img_row, DisplayMode Display_mode);

    /** @brief Interface to set the display mode

    @param Display_mode Display mode
     */
    void Set_Mode(DisplayMode Display_mode);

    /** @brief Interface to set the Compose scale

    @param Compose_scale Scale between the compose image and original image
     */
    void Set_ComposeScale(double Compose_scale);

    /** @brief Interface to set the switch if blend or not

    @param BlendSwitch Switch to select if blend or not
     */
    void Set_BlendSwitch(bool BlendSwitch);

    /** @brief Interface to set the switch if detection object or not

    @param ObjDetec Switch to select if detection object or not
     */
    void Set_ObjDetection(bool ObjDetec);

    /** @brief Interface to set the switch if compensat or not

    @param set Switch to select if blend or not
     */
    int Set_Compensat(string set);




};

#endif // DRVMIRROR_H
