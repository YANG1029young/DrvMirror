#include "common.h"
#include "output.h"
#include "outputserve.h"
#include "objdetection.h"


Mat NormalImg(vector<Mat> img_row, Infor &blend_infor, CVmenber &cvmenber, Constant &constant)
{
    LOGLN("Curent display mode is normal!");

    Mat blendimg;
    if(constant.BlendSwitch)
        blendimg = BlendImg2(img_row, blend_infor, cvmenber, constant);
    else
        blendimg = PureImg( img_row, constant);

    return blendimg;
}

Mat TurnleftImg(vector<Mat> img_row, Infor &infor, CVmenber &cvmenber, Constant &constant)
{
    LOGLN("Curent display mode is turnleft!");
    Mat blendimg;
    if(constant.BlendSwitch)
    {
        vector<Mat> img_warped(num_images-1);
        vector<Mat> img_warped_masked(num_images-1);

        for(int i = 0; i < num_images-1; i++)
        {
            Mat K, img_tmp;
            resize(img_row[i], img_tmp, Size(), constant.compose_scale, constant.compose_scale, INTER_LINEAR_EXACT);

            CameraParams camera = infor.cameras[i];

            camera.K().convertTo(K, CV_32F);
            cvmenber.warper->warp(img_tmp, K, camera.R, INTER_LINEAR, BORDER_REFLECT, img_warped[i]);

            /*
            imshow("img_warped",img_warped[i]);
            waitKey();
            imshow("mask_warped",infor.mask_warped[i]);
            waitKey();
            */

            if(constant.flag_Compensat == 2)
            {
                cvmenber.compensator->apply(i, infor.corners[i], img_warped[i], infor.mask_warped[i]);
            }
        }

        for(int i = 0; i < num_images-1; i++)
        {
            img_warped[i].copyTo(img_warped_masked[i], infor.mask_warped[i]);
            //imshow("img_warped_masked[i]",img_warped_masked[i]);
            //waitKey();
        }

        blendimg = Blend2Img(img_warped_masked,  infor.corners[0],  infor.corners[1], constant.leftcut, constant.OptimizWidth);
    }
    else
    {
        vector<Mat> img_pure_row(num_images-1);
        img_pure_row[0] = img_row[0];
        img_pure_row[1] = img_row[1];

        blendimg = PureImg( img_pure_row, constant);
    }



    return blendimg;
}

Mat TurnrightImg(vector<Mat> img_row, Infor &infor, CVmenber &cvmenber, Constant &constant)
{
    LOGLN("Curent display mode is turnright!");
    Mat blendimg;
    if(constant.BlendSwitch)
    {
        vector<Mat> img_warped(num_images-1);
        vector<Mat> img_warped_masked(num_images-1);

        for(int i = 1; i < num_images; i++)
        {
            Mat K, img_tmp;
            resize(img_row[i], img_tmp, Size(), constant.compose_scale, constant.compose_scale, INTER_LINEAR_EXACT);

            CameraParams camera = infor.cameras[i];

            camera.K().convertTo(K, CV_32F);
            cvmenber.warper->warp(img_tmp, K, camera.R, INTER_LINEAR, BORDER_REFLECT, img_warped[i-1]);

            /*
            imshow("img_warped",img_warped[i-1]);
            waitKey();
            imshow("mask_warped",infor.mask_warped[i]);
            waitKey();
            */

            if(constant.flag_Compensat == 2)
            {
                cvmenber.compensator->apply(i, infor.corners[i], img_warped[i], infor.mask_warped[i]);
            }
        }

        for(int i = 1; i < num_images; i++)
        {
            img_warped[i-1].copyTo(img_warped_masked[i-1], infor.mask_warped[i]);
        }


        blendimg = Blend2Img(img_warped_masked,  infor.corners[1],  infor.corners[2], infor.corners[2].x-infor.corners[1].x-constant.rightcut, constant.OptimizWidth);

    }
    else
    {
        vector<Mat> img_pure_row(num_images-1);
        img_pure_row[0] = img_row[1];
        img_pure_row[1] = img_row[2];
        blendimg = PureImg( img_pure_row, constant);
    }


    return blendimg;
}


///
/// \brief EarImg
/// \param img_row
/// \param earmask_infor
/// \param ear_startscale_background
/// \param ear_heightscale_background
/// \param ear_scale_side
/// \return
///
Mat EarImg(vector<Mat> img_row, vector<Mat> &earmask_infor,  Constant &constant)
{
    LOGLN("Curent display mode is ear!");
    //改变图像大小


    resize(img_row[1],img_row[1], Size(1920,1080),CV_INTER_LINEAR);
    Mat img_background = img_row[1](Rect(0, 1080*constant.ear_startscale_background,1920,1080*constant.ear_heightscale_background));

    resize(img_row[0],img_row[0], Size(1920*constant.ear_scale_side,1080*constant.ear_scale_side), 0, 0, CV_INTER_LINEAR);
    resize(img_row[2],img_row[2], Size(1920*constant.ear_scale_side,1080*constant.ear_scale_side), 0, 0, CV_INTER_LINEAR);
    //读取各个mask
    /*
    Mat mask_upleft = imread("../build-yang_test2-Desktop-Debug/mask_upleft.png");
    Mat mask_upright = imread("../build-yang_test2-Desktop-Debug/mask_upright.png");
    Mat mask_downleft = imread("../build-yang_test2-Desktop-Debug/mask_downleft.png");
    Mat mask_downright = imread("../build-yang_test2-Desktop-Debug/mask_downright.png");

    imshow("mask_upleft",mask_upleft);
    waitKey();
    */

    //mask两侧图
    Mat img_leftmasked, img_rightmasked;
    img_row[0].copyTo(img_leftmasked, earmask_infor[0]);
    img_row[2].copyTo(img_rightmasked, earmask_infor[1]);

    /*
    imshow("img_rightmasked",img_rightmasked);
    waitKey();
    */
    //抠出middle图的两侧，并mask
    Mat img_bg_left, img_bg_right, img_bg_leftmasked, img_bg_rightmasked;
    img_bg_left = img_background(Rect(0, img_background.rows - img_row[0].rows, img_row[0].cols, img_row[0].rows));
    img_bg_right = img_background(Rect(img_background.cols - img_row[0].cols, img_background.rows - img_row[0].rows, img_row[0].cols, img_row[0].rows));

    img_bg_left.copyTo(img_bg_leftmasked, earmask_infor[2]);
    img_bg_right.copyTo(img_bg_rightmasked, earmask_infor[3]);

    /*
    imshow("img_bg_rightmasked", img_bg_rightmasked);
    waitKey();
    */
    //相加
    Mat img_left, img_right;
    img_left = img_leftmasked + img_bg_leftmasked;
    img_right = img_rightmasked + img_bg_rightmasked;

    /*
    imshow("img_left", img_left);
    waitKey();
    */
    //将相加的图贴到背景上
    img_left.copyTo(img_background(Rect(0, img_background.rows - img_row[0].rows, img_row[0].cols, img_row[0].rows)));
    img_right.copyTo(img_background(Rect(img_background.cols - img_row[0].cols, img_background.rows - img_row[0].rows, img_row[0].cols, img_row[0].rows)));

    /*
    imshow("img_background", img_background);
    waitKey();
    */


    return img_background;
}




