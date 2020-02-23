#include "drvmirror.h"
#include "calibration.h"
#include "output.h"
#include "common.h"
#include "getinfor.h"
#include "filehandle.h"
#include "outputserve.h"
#include "objdetection.h"

#define DEBUG 1
#define ENABLE_LOG 1
#define LOG(msg) std::cout << msg
#define LOGLN(msg) std::cout << msg << std::endl
#define num_images 3
#define LENTH_RESULT 1280
#define HEIGHT_RESULT 268


Constant constant;
CVmenber cvmenber;
Infor calibration_infor;
Infor blend_infor;
DisplayMode display_mode;
vector<Mat> earmask_infor;

DrvMirror::DrvMirror()
{

}

int DrvMirror::Initial(void)
{
    constant.features_type = "surf";             //orb
    constant.matcher_type = "homography";        //homography" || string(argv[i + 1]) == "affine"
    constant.Camestimator_type = "homography";   //"homography" || string(argv[i + 1]) == "affine" 相机参数的计算评估
    constant.ba_cost_func = "ray";               //光束平差法使相机参数精确化(methods)
    constant.warp_type = "spherical";            //affine|plane|cylindrical|spherical|fisheye|stereographic|compressedPlaneA2B1|compressedPlaneA1.5B1|compressedPlanePortraitA2B1|compressedPlanePortraitA1.5B1|
                                              //翘曲表面类型
    constant.expos_comp_type = ExposureCompensator::GAIN_BLOCKS;          //光照补偿方法，默认是gain_blocks
    constant.blend_type = Blender::MULTI_BAND;   //融合方法，默认是多频段融合 Blender::MULTI_BAND; Blender::FEATHER;
    constant.blend_strength = 100;                //融合强度，0 - 100.默认是5.
    constant.try_cuda = false;
    constant.match_conf = 0.3f;                 //特征点检测置信等级，最近邻匹配距离与次近邻匹配距离的比值，surf默认为0.65
    constant.Calibration_megapix = 0.8 ;//0.6;        //图像匹配的分辨率大小，图像的面积尺寸变为Calibration_megapix*100000,这里用来寻找特征点
    constant.Calibration_scale = 0.8;
    constant.compose_megapix = 0.5;            //这里用来最后融合图的分辨率大小
    constant.compose_scale = constant.Calibration_scale;
    constant.range_width = -1;                    //uses range_width to limit number of images to match with，特征点匹配时用到
    constant.do_wave_correct = true;               // 是否进行波形矫正
    constant.conf_thresh = 0.45f;                //光束平差法用到，两幅图来自同一全景图的置信度根据任意两幅图匹配的置信度，将所有置信度高于conf_thresh（默认是1.0）的图像合并到一个全集中
    constant.ba_refine_mask = "xxxxx";           //光束平差法用到，相机参数精确化mask(parameter)
    constant.flag_Compensat = 0;

    constant.leftcut = 200;
    constant.rightcut = 200;
    constant.OptimizWidth = 100;

    constant.ear_heightscale_background = 0.6;
    constant.ear_scale_side = 0.35;
    constant.ear_startscale_background = 0.4;

    constant.BlendSwitch = true;
    constant.ObjDetc = true;

    //constant.Calibration_scale = min(1.0, sqrt(constant.Calibration_megapix * 1e6 / img_row[0].size().area()));
    //constant.compose_scale = min(1.0, sqrt(constant.compose_megapix * 1e6 / img_row[0].size().area()));
    {
        if (constant.warp_type == "plane")
            cvmenber.warper_creator = makePtr<cv::PlaneWarper>();
        else if (constant.warp_type == "affine")
            cvmenber.warper_creator = makePtr<cv::AffineWarper>();
        else if (constant.warp_type == "cylindrical")
            cvmenber.warper_creator = makePtr<cv::CylindricalWarper>();
        else if (constant.warp_type == "spherical")
            cvmenber.warper_creator = makePtr<cv::SphericalWarper>();
        else if (constant.warp_type == "fisheye")
            cvmenber.warper_creator = makePtr<cv::FisheyeWarper>();
        else if (constant.warp_type == "stereographic")
            cvmenber.warper_creator = makePtr<cv::StereographicWarper>();
        else if (constant.warp_type == "compressedPlaneA2B1")
            cvmenber.warper_creator = makePtr<cv::CompressedRectilinearWarper>(2.0f, 1.0f);
        else if (constant.warp_type == "compressedPlaneA1.5B1")
            cvmenber.warper_creator = makePtr<cv::CompressedRectilinearWarper>(1.5f, 1.0f);
        else if (constant.warp_type == "compressedPlanePortraitA2B1")
            cvmenber.warper_creator = makePtr<cv::CompressedRectilinearPortraitWarper>(2.0f, 1.0f);
        else if (constant.warp_type == "compressedPlanePortraitA1.5B1")
            cvmenber.warper_creator = makePtr<cv::CompressedRectilinearPortraitWarper>(1.5f, 1.0f);
        else if (constant.warp_type == "paniniA2B1")
            cvmenber.warper_creator = makePtr<cv::PaniniWarper>(2.0f, 1.0f);
        else if (constant.warp_type == "paniniA1.5B1")
            cvmenber.warper_creator = makePtr<cv::PaniniWarper>(1.5f, 1.0f);
        else if (constant.warp_type == "paniniPortraitA2B1")
            cvmenber.warper_creator = makePtr<cv::PaniniPortraitWarper>(2.0f, 1.0f);
        else if (constant.warp_type == "paniniPortraitA1.5B1")
            cvmenber.warper_creator = makePtr<cv::PaniniPortraitWarper>(1.5f, 1.0f);
        else if (constant.warp_type == "mercator")
            cvmenber.warper_creator = makePtr<cv::MercatorWarper>();
        else if (constant.warp_type == "transverseMercator")
            cvmenber.warper_creator = makePtr<cv::TransverseMercatorWarper>();
    }



    //cvmenber.warper = cvmenber.warper_creator->create(static_cast<float>(Warped_image_focal));

    //建立光照补偿器

    cvmenber.compensator = ExposureCompensator::createDefault(constant.expos_comp_type);
    cvmenber.blender = Blender::createDefault(constant.blend_type, constant.try_cuda);

    calibration_infor.mask_warped = vector<Mat>(num_images);
    calibration_infor.cameras = vector<CameraParams>(num_images);
    calibration_infor.corners = vector<Point>(num_images);
    calibration_infor.sizes = vector<Size>(num_images);
    earmask_infor = vector<Mat>(4);

    blend_infor = calibration_infor;

    return 0;
}

int DrvMirror::Initial(DisplayMode Display_mode, double Compose_scale, bool BlendSwitch)
{
    Initial();
    display_mode = Display_mode;
    constant.compose_scale = Compose_scale;
    constant.BlendSwitch = BlendSwitch;
    return 0;
}

Mat DrvMirror::Calibraton(vector<Mat> Img_row)
{
    //寻找特征点
    vector<ImageFeatures> features(num_images);
    FeaturesFind(Img_row, features, constant.Calibration_scale, constant.features_type);

    LOGLN("Features in image #" << 0 << ": " << features[0].keypoints.size());

    //特征点匹配
    vector<MatchesInfo> pairwise_matches;
    FeaturesMatch(features, pairwise_matches, constant.matcher_type, constant.try_cuda, constant.match_conf);
    //LOGLN("pairwise_matches #" << 0 << ": " << pairwise_matches.const_iterator);

    //相机参数标定
    //vector<CameraParams> cameras;
    CamEstimator( features, pairwise_matches, calibration_infor.cameras, constant.Camestimator_type);


    //光束平差法使相机参数精确化
    CamAdjuster(features, pairwise_matches, calibration_infor.cameras, constant.ba_cost_func, constant.conf_thresh);
    LOGLN("光束平叉K:\n" << calibration_infor.cameras[0].K() << "\nR:\n" << calibration_infor.cameras[0].R<< "\nt:\n" << calibration_infor.cameras[0].t);

    //统一焦距
    //float warped_image_focal;
    calibration_infor.warped_image_focal = Get_wif(calibration_infor.cameras);

    //波形矫正 试图使全景更水平
    WaveCorrect(calibration_infor.cameras, constant.do_wave_correct);
    LOGLN("波形矫正K:\n" << calibration_infor.cameras[0].K() << "\nR:\n" << calibration_infor.cameras[0].R<< "\nt:\n" << calibration_infor.cameras[0].t);

    //拼接输出图像
    Get_CalibrationInfor(Img_row, calibration_infor, cvmenber, constant);

    return BlendImg(Img_row, calibration_infor, cvmenber, constant);

    //存储相机参数  (考证)
    //saveCameraParam( "camera_yang" ,warped_image_focal, cameras);
}

//存储的都是标定大小图片的信息
int DrvMirror::Save_Infor()
{
    int re_value = 0;
    re_value = saveCameraParam( "camera",calibration_infor.warped_image_focal, calibration_infor.cameras);
    re_value = saveCornerSize( "cornersize", calibration_infor.corners,  calibration_infor.sizes);
    return re_value;
}

void DrvMirror::Set_ComposeScale(double Compose_scale)
{
    constant.compose_scale = Compose_scale;
}

void DrvMirror::Set_BlendSwitch(bool BlendSwitch)
{
    constant.BlendSwitch = BlendSwitch;
}

void DrvMirror::Set_ObjDetection(bool ObjDetec)
{
    constant.ObjDetc = ObjDetec;
}

void DrvMirror::Set_Mode(DisplayMode Display_mode)
{
    display_mode = Display_mode;
    switch (display_mode) {
    case normal:
        Get_BlendInfor(blend_infor, cvmenber, constant);

        break;

    case pure:
        //return PureImg();
        break;

    case ear:

        Get_EarmaskInfor(earmask_infor, constant.ear_scale_side);
        break;

    case turnleft:
        Get_BlendInfor(blend_infor, cvmenber, constant);
        break;

    case turnright:
        Get_BlendInfor(blend_infor, cvmenber, constant);

        break;

    default:
        break;
    }

}

Mat DrvMirror::Get_ResultImg(vector<Mat> Img_row)
{

    if(constant.ObjDetc)
    {
         ObjDetection(Img_row[0]);
         ObjDetection(Img_row[1]);
         ObjDetection(Img_row[2]);
    }

    Mat img_full_result;
    Mat img_result;

    //img_row = Img_row;
    switch (display_mode) {

    case normal:
        img_full_result = NormalImg(Img_row, blend_infor, cvmenber, constant);
        break;

    case ear:
        img_full_result = EarImg(Img_row, earmask_infor, constant);
        break;

    case turnleft:

        img_full_result = TurnleftImg(Img_row, blend_infor, cvmenber, constant);
        break;

    case turnright:

        img_full_result = TurnrightImg(Img_row, blend_infor, cvmenber, constant);
        break;

    default:
        break;
    }
    /*
    if(constant.ObjDetc)
    {
        img_result = ObjDetection(img_result);
    }

    */

    return img_full_result;
}

Mat DrvMirror::Get_ResultImg(vector<Mat> Img_row, DisplayMode Display_mode)
{
    Mat re_value;
    Set_Mode(Display_mode);
    re_value = Get_ResultImg(Img_row);
    return re_value;

}



