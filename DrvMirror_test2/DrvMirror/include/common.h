#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <fstream>
#include <string>

#include "opencv2/opencv_modules.hpp"
#include <opencv2/core/utility.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/stitching/detail/autocalib.hpp"
#include "opencv2/stitching/detail/blenders.hpp"
#include "opencv2/stitching/detail/timelapsers.hpp"
#include "opencv2/stitching/detail/camera.hpp"
#include "opencv2/stitching/detail/exposure_compensate.hpp"
#include "opencv2/stitching/detail/matchers.hpp"
#include "opencv2/stitching/detail/motion_estimators.hpp"
#include "opencv2/stitching/detail/seam_finders.hpp"
#include "opencv2/stitching/detail/warpers.hpp"
#include "opencv2/stitching/warpers.hpp"

using namespace std;
using namespace cv;
using namespace cv::detail;

#define DEBUG 1
#define ENABLE_LOG 1
#define LOG(msg) std::cout << msg
#define LOGLN(msg) std::cout << msg << std::endl
#define num_images 3

typedef struct
{
    string features_type ;             //orb
    string matcher_type ;        //homography" || string(argv[i + 1]) == "affine"
    string Camestimator_type ;   //"homography" || string(argv[i + 1]) == "affine" 相机参数的计算评估
    string ba_cost_func ;               //光束平差法使相机参数精确化(methods)
    string warp_type ;            //affine|plane|cylindrical|spherical|fisheye|stereographic|compressedPlaneA2B1|compressedPlaneA1.5B1|compressedPlanePortraitA2B1|compressedPlanePortraitA1.5B1|
                                              //翘曲表面类型
    int expos_comp_type ;          //光照补偿方法，默认是gain_blocks
    int blend_type ;   //融合方法，默认是多频段融合 Blender::MULTI_BAND; Blender::FEATHER;
    float blend_strength;                //融合强度，0 - 100.默认是5.
    bool try_cuda;
    float match_conf;                 //特征点检测置信等级，最近邻匹配距离与次近邻匹配距离的比值，surf默认为0.65
    double Calibration_megapix ;//0.6;        //图像匹配的分辨率大小，图像的面积尺寸变为Calibration_megapix*100000,这里用来寻找特征点
    double Calibration_scale;
    double compose_megapix;            //这里用来最后融合图的分辨率大小
    double compose_scale;
    int range_width ;                    //uses range_width to limit number of images to match with，特征点匹配时用到
    bool do_wave_correct ;               // 是否进行波形矫正
    float conf_thresh ;                //光束平差法用到，两幅图来自同一全景图的置信度根据任意两幅图匹配的置信度，将所有置信度高于conf_thresh（默认是1.0）的图像合并到一个全集中
    string ba_refine_mask ;           //光束平差法用到，相机参数精确化mask(parameter)
    int flag_Compensat;

    int leftcut;
    int rightcut;
    int OptimizWidth;

    double ear_heightscale_background;
    double ear_scale_side;
    double ear_startscale_background;

    bool BlendSwitch;

}Constant;

typedef struct
{
    Ptr<WarperCreator> warper_creator;
    Ptr<RotationWarper> warper;
    Ptr<ExposureCompensator> compensator;
    Ptr<Blender> blender;

}CVmenber;

typedef struct
{
    vector<CameraParams> cameras;
    vector<Size> sizes;
    vector<Point> corners;
    vector<Mat> mask_warped;
    float warped_image_focal;

}Infor;

int max(int a,int b,int c);
#endif // COMMON

