#include "getinfor.h"
#include "filehandle.h"

void modifyCorners(vector<Point> &corners);
void mirror(Mat src, Mat &dst );

void Get_CalibrationInfor(vector<Mat> img_row, Infor &calibration_infor, CVmenber &cvmenber, Constant &constant )
{


    Mat mask;
    //vector<Mat> mask_warped(num_images);
    Mat img_resize;


    //constant.Calibration_scale = min(1.0, sqrt(constant.Calibration_megapix * 1e6 / img_row[0].size().area()));
    //constant.compose_scale = min(1.0, sqrt(constant.compose_megapix * 1e6 / img_row[0].size().area()));

    // Compute relative scales


    // Update warped image scale


    cvmenber.warper = cvmenber.warper_creator->create(static_cast<float>(calibration_infor.warped_image_focal));

    // Update corners and sizes
    //constant.compose_scale = constant.Calibration_scale;
    for (int i = 0; i < num_images; ++i)
    {
        if (abs(constant.compose_scale - 1) > 1e-1)
            resize(img_row[i], img_resize, Size(), constant.Calibration_scale, constant.Calibration_scale, INTER_LINEAR_EXACT);

        // Update intrinsics

        mask.create(img_resize.size(), CV_8U);
        mask.setTo(Scalar::all(255));

        Mat K;
        calibration_infor.cameras[i].K().convertTo(K, CV_32F);
        cvmenber.warper->warp(mask, K, calibration_infor.cameras[i].R, INTER_NEAREST, BORDER_CONSTANT, calibration_infor.mask_warped[i]);


        // Update corner and size
        Size sz = img_row[i].size();
        if (std::abs(constant.compose_scale - 1) > 1e-1)
        {
            //sz.width = cvRound(img_row[i].size().width * constant.compose_scale);
            //sz.height = cvRound(img_row[i].size().height * constant.compose_scale);

            sz.width = cvRound(img_row[i].size().width * constant.Calibration_scale);
            sz.height = cvRound(img_row[i].size().height * constant.Calibration_scale);
        }

        /*
        Rect roi = cvmenber.warper->warpRoi(sz, K, calibration_infor.cameras[i].R);
        calibration_infor.corners[i] = roi.tl();
        calibration_infor.sizes[i] = roi.size();

        */
        Rect roi = cvmenber.warper->warpRoi(sz, K, calibration_infor.cameras[i].R);
        calibration_infor.corners[i] = roi.tl();
        calibration_infor.sizes[i] = sz;


        if (i == 0)
            imwrite("firstMask_warped.png",calibration_infor.mask_warped[i]);
        else if (i == 1)
                  imwrite("secondMask_warped.png",calibration_infor.mask_warped[i]);
              else
                  imwrite("thirdMask_warped.png",calibration_infor.mask_warped[i]);


        //mask_warped[i].release();

    }
    img_resize.release();
    mask.release();
    //保存connors、size
    modifyCorners(calibration_infor.corners);
    //saveCornerSize( "corner_yang", corners, sizes);

    /*
     * double compose_Calibration_aspect;
    Mat mask;
    //vector<Mat> mask_warped(num_images);
    Mat img_resize;


    //constant.Calibration_scale = min(1.0, sqrt(constant.Calibration_megapix * 1e6 / img_row[0].size().area()));
    //constant.compose_scale = min(1.0, sqrt(constant.compose_megapix * 1e6 / img_row[0].size().area()));

    // Compute relative scales
    compose_Calibration_aspect = constant.compose_scale / constant.Calibration_scale;

    // Update warped image scale
    calibration_infor.warped_image_focal *= static_cast<float>(compose_Calibration_aspect);

    cvmenber.warper = cvmenber.warper_creator->create(static_cast<float>(calibration_infor.warped_image_focal));


    // Update corners and sizes
    for (int i = 0; i < num_images; ++i)
    {
        if (abs(constant.compose_scale - 1) > 1e-1)
            resize(img_row[i], img_resize, Size(), constant.compose_scale, constant.compose_scale, INTER_LINEAR_EXACT);

        // Update intrinsics
        calibration_infor.cameras[i].focal *= compose_Calibration_aspect;
        calibration_infor.cameras[i].ppx *= compose_Calibration_aspect;
        calibration_infor.cameras[i].ppy *= compose_Calibration_aspect;

        mask.create(img_resize.size(), CV_8U);
        mask.setTo(Scalar::all(255));

        Mat K;
        calibration_infor.cameras[i].K().convertTo(K, CV_32F);
        cvmenber.warper->warp(mask, K, calibration_infor.cameras[i].R, INTER_NEAREST, BORDER_CONSTANT, calibration_infor.mask_warped[i]);


        // Update corner and size
        Size sz = img_row[i].size();
        if (std::abs(constant.compose_scale - 1) > 1e-1)
        {
            sz.width = cvRound(img_row[i].size().width * constant.compose_scale);
            sz.height = cvRound(img_row[i].size().height * constant.compose_scale);
        }

        Rect roi = cvmenber.warper->warpRoi(sz, K, calibration_infor.cameras[i].R);
        calibration_infor.corners[i] = roi.tl();
        calibration_infor.sizes[i] = roi.size();



        if (i == 0)
            imwrite("firstMask_warped.png",calibration_infor.mask_warped[i]);
        else if (i == 1)
                  imwrite("secondMask_warped.png",calibration_infor.mask_warped[i]);
              else
                  imwrite("thirdMask_warped.png",calibration_infor.mask_warped[i]);


        //mask_warped[i].release();

    }
    img_resize.release();
    mask.release();
    //保存connors、size
    modifyCorners(calibration_infor.corners);
    //saveCornerSize( "corner_yang", corners, sizes);
    */
}

int Get_BlendInfor(Infor &blend_infor, CVmenber &cvmenber, Constant constant)
{

    int a = 0;
    //a = loadCameraParam( "../build-yang_test2-Desktop-Debug/camera2" ,&blend_infor.warped_image_focal, &blend_infor.cameras);
    //a = loadCornerAndSizes( "../build-yang_test2-Desktop-Debug/cornersize2" ,&blend_infor.corners, &blend_infor.sizes);
    a = loadCameraParam( "camera" ,&blend_infor.warped_image_focal, &blend_infor.cameras);
    a = loadCornerAndSizes( "cornersize" ,&blend_infor.corners, &blend_infor.sizes);
    double compose_Calibration_aspect;

    Mat mask;
    //vector<Mat> mask_warped(num_images);
    Mat mask_resize;


    //constant.Calibration_scale = min(1.0, sqrt(constant.Calibration_megapix * 1e6 / img_row[0].size().area()));
    //constant.compose_scale = min(1.0, sqrt(constant.compose_megapix * 1e6 / img_row[0].size().area()));

    // Compute relative scales
    compose_Calibration_aspect = constant.compose_scale / constant.Calibration_scale;

    // Update warped image scale
    blend_infor.warped_image_focal *= static_cast<float>(compose_Calibration_aspect);

    cvmenber.warper = cvmenber.warper_creator->create(static_cast<float>(blend_infor.warped_image_focal));


    // Update corners and sizes
    for (int i = 0; i < num_images; ++i)
    {


        // Update intrinsics
        blend_infor.cameras[i].focal *= compose_Calibration_aspect;
        blend_infor.cameras[i].ppx *= compose_Calibration_aspect;
        blend_infor.cameras[i].ppy *= compose_Calibration_aspect;

        mask.create(blend_infor.sizes[i], CV_8U);
        mask.setTo(Scalar::all(255));
        if (abs(constant.compose_scale - 1) > 1e-1)
            resize(mask, mask_resize, Size(), compose_Calibration_aspect, compose_Calibration_aspect, INTER_LINEAR_EXACT);


        Mat K;
        blend_infor.cameras[i].K().convertTo(K, CV_32F);
        cvmenber.warper->warp(mask_resize, K, blend_infor.cameras[i].R, INTER_NEAREST, BORDER_CONSTANT, blend_infor.mask_warped[i]);



        // Update corner and size
        Size sz = blend_infor.sizes[i];
        if (std::abs(constant.compose_scale - 1) > 1e-1)
        {
            sz.width = cvRound(blend_infor.sizes[i].width * compose_Calibration_aspect);
            sz.height = cvRound(blend_infor.sizes[i].height * compose_Calibration_aspect);
        }

        Rect roi = cvmenber.warper->warpRoi(sz, K, blend_infor.cameras[i].R);
        blend_infor.corners[i] = roi.tl();
        blend_infor.sizes[i] = roi.size();


    }
    mask_resize.release();
    mask.release();
    //保存connors、size
    modifyCorners(blend_infor.corners);
    //saveCornerSize( "corner_yang", corners, sizes);

}

void Get_EarmaskInfor(vector<Mat> &earmask_infor, double ear_scale_side)
{
    int width = 1920*ear_scale_side;
    int height = 1080*ear_scale_side;
    int line_width = 50*ear_scale_side;

    /*
     * earmask_infor[0]:mask_upleft
     * earmask_infor[1]:mask_upright
     * earmask_infor[2]:mask_downleft
     * earmask_infor[3]:mask_downright
     */
    for(int i=0;i<4;i++)
    {
        earmask_infor[i] = Mat(height, width, CV_8UC1);
        earmask_infor[i].setTo(0);
    }
    earmask_infor[0] = Mat(height, width, CV_8UC1);
    Mat mask_upleft(height, width, CV_8UC1);
    mask_upleft.setTo(0);
    Mat mask_upright = mask_upleft.clone();
    Mat mask_downleft = mask_upleft.clone();
    Mat mask_downright = mask_upleft.clone();


    for(int i = line_width; i < height; i++)
    {
        for(int j = 0; j< width; j++)
        {
            int arc = sqrt(pow(height-line_width, 2) - pow(i-height, 2)) + width -height;
            if(j < arc)
            {
                earmask_infor[0].at<uchar>(i,j) = 255;

            }
            else
            {
                earmask_infor[0].at<uchar>(i,j) = 0;
            }
        }
    }

    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j< width; j++)
        {
            int arc = sqrt(pow(height, 2) - pow(i-height, 2)) + width -height;
            if(j < arc)
            {
                earmask_infor[2].at<uchar>(i,j) = 0;

            }
            else
            {
                earmask_infor[2].at<uchar>(i,j) = 255;
            }
        }
    }

    mirror(earmask_infor[0], earmask_infor[1]);
    mirror(earmask_infor[2], earmask_infor[3]);

    /*
    imshow("up_left",mask_upleft);
    waitKey();
    imshow("up_right",mask_upright);
    waitKey();
    imshow("down_left",mask_downleft);
    waitKey();
    imshow("down_right",mask_downright);
    waitKey();
    */

}

void modifyCorners(vector<Point> &corners)
{
    int x_left = corners[0].x, y_top = corners[0].y;
    //mask 图片
    for(int i=0;i < num_images;i++)
    {

        if( corners[i].x < x_left ) x_left = corners[i].x;

        if( corners[i].y < y_top ) y_top = corners[i].y;

    }
    for(int i=0;i < num_images;i++)
    {
        corners[i].x = corners[i].x - x_left;
        corners[i].y = corners[i].y - y_top;
    }
}
void mirror(Mat src, Mat &dst )
{
    int col = src.cols;
    dst = src.clone();
    for(int i = 0;i<col;i++)
    {
        src.col(col-1-i).copyTo(dst.col(i));
    }

}
