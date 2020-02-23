#include "outputserve.h"
#include "common.h"

Mat BlendImg(vector<Mat> img_row, Infor &infor, CVmenber &cvmenber, Constant &constant)
{

    vector<Mat> img_warped(num_images);

    for(int i = 0; i<3; i++)
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

    Size dst_sz = resultRoi(infor.corners, infor.sizes).size();
    float blend_width = sqrt(static_cast<float>(dst_sz.area())) * constant.blend_strength / 100.f;
    if (blend_width < 1.f)
        cvmenber.blender = Blender::createDefault(Blender::NO, constant.try_cuda);
    else if (constant.blend_type == Blender::MULTI_BAND)
    {
        MultiBandBlender* mb = dynamic_cast<MultiBandBlender*>(cvmenber.blender.get());
        mb->setNumBands(static_cast<int>(ceil(log(blend_width)/log(2.)) - 1.));
        LOGLN("Multi-band blender, number of bands: " << mb->numBands());

    }
    else if (constant.blend_type == Blender::FEATHER)
    {
        FeatherBlender* fb = dynamic_cast<FeatherBlender*>(cvmenber.blender.get());
        fb->setSharpness(1.f/blend_width);
        LOGLN("Feather blender, sharpness: " << fb->sharpness());
    }
    cvmenber.blender->prepare(infor.corners, infor.sizes);

    Mat img_tmp;
    for(int i = 0; i < 3; i++)
    {
        img_warped[i].convertTo(img_tmp, CV_16S);

        cvmenber.blender->feed(img_tmp,  infor.mask_warped[i], infor.corners[i]);
    }

    // Blend the current image
    //t = getTickCount();
    Mat img_blend, result_mask;
    cvmenber.blender->blend(img_blend, result_mask);


    img_blend.convertTo(img_blend,CV_8UC3);
    //imwrite("result.jpg", result);
    //resize(img_blend, img_blend, Size(1500,800), INTER_LINEAR_EXACT);

    //LOGLN("Blending, time: " << ((getTickCount() - t) / getTickFrequency()) << " sec");

    return img_blend;
}

void OptimizeSeam(Mat img0, Mat img1, Mat img2, Mat dst, int OptimWidth, Pline PL);

Mat BlendImg2(vector<Mat> img_row, Infor &infor, CVmenber &cvmenber, Constant &constant)
{
    vector<Mat> img_warped(num_images);
    vector<Mat> img_warped_masked(num_images);

    for(int i = 0; i < num_images; i++)
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

    for(int i = 0; i < num_images; i++)
    {
        img_warped[i].copyTo(img_warped_masked[i], infor.mask_warped[i]);
        //imshow("img_warped_masked[i]",img_warped_masked[i]);
        //waitKey();

    }

    int dst_width =  infor.corners[2].x + img_warped_masked[2].cols;                              //拼接图宽度
    int dst_height = max(infor.corners[0].y + img_warped_masked[0].rows,infor.corners[1].y + img_warped_masked[1].rows,infor.corners[2].y + img_warped_masked[2].rows);

    Mat dst(dst_height, dst_width, CV_8UC3);
    dst.setTo(0);

    Pline PL;    //拼缝相对于各个图片的相对位置
    PL.PlineImg0 = infor.corners[1].x + constant.leftcut;
    PL.PlineImg1L = constant.leftcut;
    PL.PlineImg1R = img_warped_masked[1].cols - constant.rightcut;
    PL.PlineImg2 = infor.corners[1].x + PL.PlineImg1R - infor.corners[2].x;
    PL.PlineDstL = PL.PlineImg0;
    PL.PlineDstR = infor.corners[1].x + PL.PlineImg1R;

    //将变换好的图片放到相同高度的图片中
    Mat img_handle0(dst_height, img_warped_masked[0].cols, CV_8UC3);
    img_handle0.setTo(0);
    img_warped_masked[0].copyTo(img_handle0(Rect(0, infor.corners[0].y, img_warped_masked[0].cols, img_warped_masked[0].rows)));

    Mat img_handle1(dst_height, img_warped_masked[1].cols, CV_8UC3);
    img_handle1.setTo(0);
    img_warped_masked[1].copyTo(img_handle1(Rect(0, infor.corners[1].y, img_warped_masked[1].cols, img_warped_masked[1].rows)));

    Mat img_handle2(dst_height, img_warped_masked[2].cols, CV_8UC3);
    img_handle2.setTo(0);
    img_warped_masked[2].copyTo(img_handle2(Rect(0, infor.corners[2].y, img_warped_masked[2].cols, img_warped_masked[2].rows)));

    //截取拼图
    Mat tmp = img_handle0.colRange(0, PL.PlineImg0);
    tmp.copyTo(dst(Rect(0, 0, tmp.cols,tmp.rows)));

    tmp = img_handle1.colRange(PL.PlineImg1L, PL.PlineImg1R);
    tmp.copyTo(dst(Rect(PL.PlineDstL, 0, tmp.cols,tmp.rows)));

    tmp = img_handle2.colRange(PL.PlineImg2, img_warped_masked[2].cols);
    tmp.copyTo(dst(Rect(PL.PlineDstR, 0, tmp.cols,tmp.rows)));

    /*
    imshow("dst1",dst);
    waitKey();
    */

    //优化拼缝
    OptimizeSeam(img_handle0, img_handle1, img_handle2, dst, constant.OptimizWidth, PL);     //优化拼缝

    /*
    imshow("dst2",dst);
    waitKey();
    */

    return dst;
}

//优化拼缝
void OptimizeSeam(Mat img0, Mat img1, Mat img2, Mat dst, int OptimWidth, Pline PL)
{
    //int rows = dst.rows;
    int rows = img1.rows;

    int p_img0 = PL.PlineImg0 - OptimWidth/2;
    int p_img1_left = PL.PlineImg1L - OptimWidth/2;
    int p_img1_right = PL.PlineImg1R - OptimWidth/2;
    int p_img2 = PL.PlineImg2 - OptimWidth/2;
    int P_dst_left = PL.PlineDstL - OptimWidth/2;
    int P_dst_right = PL.PlineDstR - OptimWidth/2;

    double alpha = 1;//img1中像素的权重
    for (int i = 0; i < rows; i++)
    {
        uchar* p0 = img0.ptr<uchar>(i);  //获取第i行的首地址
        uchar* p1 = img1.ptr<uchar>(i);
        uchar* p2 = img2.ptr<uchar>(i);
        uchar* d = dst.ptr<uchar>(i);
        for (int j = 0; j < OptimWidth; j++)
        {
            //左拼缝
            if ((p0[(p_img0 + j+2) * 3] == 0) && (p0[(p_img0 + j+2) * 3 + 1] == 0) && (p0[(p_img0 + j+2) * 3 + 2] == 0))
            {
                alpha = 0;
                //alpha = 1;
            }
            else
            {

                alpha = (OptimWidth - j)*1.0 / OptimWidth;
                //alpha = 0;
            }
            d[(P_dst_left+j) * 3] = p0[(p_img0 + j) * 3] * alpha + p1[(p_img1_left+j) * 3] * (1 - alpha);
            d[(P_dst_left+j) * 3 + 1] = p0[(p_img0 + j) * 3 + 1] * alpha + p1[(p_img1_left+j) * 3 + 1] * (1 - alpha);
            d[(P_dst_left+j) * 3 + 2] = p0[(p_img0 + j) * 3 + 2] * alpha + p1[(p_img1_left+j) * 3 + 2] * (1 - alpha);

            //右拼缝
            if (p2[(p_img2 + j) * 3] == 0 && p2[(p_img2 + j) * 3 + 1] == 0 && p2[(p_img2 + j) * 3 + 2] == 0)
            {
                alpha = 0;
            }
            else
            {
                alpha = j*1.0 / OptimWidth;
                //alpha = 0;
            }
            d[(P_dst_right+j) * 3] = p2[(p_img2 + j) * 3] * alpha + p1[(p_img1_right+j) * 3] * (1 - alpha);
            d[(P_dst_right+j) * 3 + 1] = p2[(p_img2 + j) * 3 + 1] * alpha + p1[(p_img1_right+j) * 3 + 1] * (1 - alpha);
            d[(P_dst_right+j) * 3 + 2] = p2[(p_img2 + j) * 3 + 2] * alpha + p1[(p_img1_right+j) * 3 + 2] * (1 - alpha);

        }
    }
    //return dst;

}

Mat Blend2Img(vector<Mat>img_warped_masked, Point corner0, Point corner1, int cutlength, int OptimizWidth )
{
    int dst_width =  corner1.x-corner0.x + img_warped_masked[1].cols;                              //拼接图宽度
    int dst_height = max(corner0.y + img_warped_masked[0].rows, corner1.y + img_warped_masked[1].rows, 0);

    Mat dst(dst_height, dst_width, CV_8UC3);
    dst.setTo(0);

    int PlineImg0 = corner1.x - corner0.x + cutlength;
    int PlineImg1 = cutlength;

    //将变换好的图片放到相同高度的图片中
    Mat img_handle0(dst_height, img_warped_masked[0].cols, CV_8UC3);
    img_handle0.setTo(0);
    img_warped_masked[0].copyTo(img_handle0(Rect(0, corner0.y, img_warped_masked[0].cols, img_warped_masked[0].rows)));

    Mat img_handle1(dst_height, img_warped_masked[1].cols, CV_8UC3);
    img_handle1.setTo(0);
    img_warped_masked[1].copyTo(img_handle1(Rect(0, corner1.y, img_warped_masked[1].cols, img_warped_masked[1].rows)));


    //截取拼图
    Mat tmp = img_handle0.colRange(0, PlineImg0);
    tmp.copyTo(dst(Rect(0, 0, tmp.cols,tmp.rows)));

    tmp = img_handle1.colRange(PlineImg1, img_handle1.cols);
    tmp.copyTo(dst(Rect(PlineImg0, 0, tmp.cols,tmp.rows)));

    OptimizeSeam2img(img_handle0, img_handle1, dst, OptimizWidth, PlineImg0, PlineImg1);     //优化拼缝

    return dst;

}

void OptimizeSeam2img(Mat img_handle0, Mat img_handle1, Mat dst, int OptimWidth, int PlineImg0, int PlineImg1)     //优化拼缝
{
    //int rows = dst.rows;
    int rows = dst.rows;

    int p_img0 = PlineImg0 - OptimWidth/2;
    int p_img1 = PlineImg1 - OptimWidth/2;


    double alpha = 1;//img1中像素的权重
    for (int i = 0; i < rows; i++)
    {
        uchar* p0 = img_handle0.ptr<uchar>(i);  //获取第i行的首地址
        uchar* p1 = img_handle1.ptr<uchar>(i);
        uchar* d = dst.ptr<uchar>(i);
        for (int j = 0; j < OptimWidth; j++)
        {
            //左拼缝
            if ((p0[(p_img0 + j) * 3] == 0) && (p0[(p_img0 + j) * 3 + 1] == 0) && (p0[(p_img0 + j) * 3 + 2] == 0))
            {
                alpha = 0;
                //alpha = 1;
            }
            else if ((p1[(p_img1 + j) * 3] == 0) && (p1[(p_img1 + j) * 3 + 1] == 0) && (p1[(p_img1 + j) * 3 + 2] == 0))
            {
                alpha = 1;
                //alpha = 1;
            }
            else
            {
                alpha = (OptimWidth - j)*1.0 / OptimWidth;
                //alpha = 0;
            }
            d[(p_img0+j) * 3] = p0[(p_img0 + j) * 3] * alpha + p1[(p_img1+j) * 3] * (1 - alpha);
            d[(p_img0+j) * 3 + 1] = p0[(p_img0 + j) * 3 + 1] * alpha + p1[(p_img1+j) * 3 + 1] * (1 - alpha);
            d[(p_img0+j) * 3 + 2] = p0[(p_img0 + j) * 3 + 2] * alpha + p1[(p_img1+j) * 3 + 2] * (1 - alpha);


        }
    }
    //return dst;

}

Mat PureImg(vector<Mat> img_row, Constant &constant)
{
    int num_img  = img_row.size();
    int dst_height = img_row[0].rows;
    int dst_width = 0;
    for(int i = 0;i<num_img;i++)
    {
        dst_width = dst_width + img_row[i].cols;
    }
    dst_width = dst_width + 50 * (num_img-1);
    Mat dst(dst_height, dst_width, CV_8UC3);
    dst.setTo(200);
    //合成
    int start_cols = 0;
    for(int i = 0;i<num_img;i++)
    {
        img_row[i].copyTo(dst(Rect(start_cols, 0, img_row[i].cols,img_row[i].rows)));
        start_cols = start_cols + img_row[i].cols + 50;
    }

    resize(dst, dst, Size(),constant.compose_scale, constant.compose_scale, CV_INTER_LINEAR);

    return dst;
}
