#include "calibration.h"
//#include <iostream>
#include "common.h"

int FeaturesFind(vector<Mat> img_row, vector<ImageFeatures> &features, double Calibration_scale, string features_type)
{
    Ptr<FeaturesFinder> finder;
    if (features_type == "surf")
    {
        finder = makePtr<SurfFeaturesFinder>();
    }
    else if (features_type == "orb")
    {
        finder = makePtr<OrbFeaturesFinder>();
    }
    else
    {
        cout << "Unknown 2D features type: '" << features_type << "'.\n";
        return -1;
    }

    //double Calibration_scale = 1;
    Mat img_tmp;
    //double Calibration_scale;
    //vector<ImageFeatures> features(num_images);
    for(int i=0;i<3;i++)
    {
        //Calibration_scale = min(1.0, sqrt(Calibration_megapix * 1e6 / img_row[i].size().area()));
        resize(img_row[i], img_tmp, Size(), Calibration_scale, Calibration_scale, INTER_LINEAR_EXACT);

        (*finder)(img_tmp, features[i]);
        features[i].img_idx = i;
        LOGLN("Features in image #" << i+1 << ": " << features[i].keypoints.size());
    }

    finder->collectGarbage();
    img_tmp.release();
}

int FeaturesMatch(vector<ImageFeatures> features, vector<MatchesInfo> &pairwise_matches, string matcher_type, bool try_cuda, float match_conf)
{
    int range_width = -1;
    Ptr<FeaturesMatcher> matcher;
    if (matcher_type == "affine")
        matcher = makePtr<AffineBestOf2NearestMatcher>(false, try_cuda, match_conf);
    else if (range_width == -1)
        matcher = makePtr<BestOf2NearestMatcher>(try_cuda, match_conf);
    else
        matcher = makePtr<BestOf2NearestRangeMatcher>(range_width, try_cuda, match_conf);

    (*matcher)(features, pairwise_matches);

    matcher->collectGarbage();
}

int CamEstimator(vector<ImageFeatures> features, vector<MatchesInfo> pairwise_matches, vector<CameraParams> &cameras, string Camestimator_type)
{
    Ptr<Estimator> estimator;
    if (Camestimator_type == "affine")
        estimator = makePtr<AffineBasedEstimator>();
    else
        estimator = makePtr<HomographyBasedEstimator>();
    //imshow("masks_warped",  img_row[0]);
    //waitKey();

    if (!(*estimator)(features, pairwise_matches, cameras))
    {
        cout << "Homography estimation failed.\n";
        return -1;
    }

    for (size_t i = 0; i < cameras.size(); ++i)
    {
        Mat R;
        cameras[i].R.convertTo(R, CV_32F);
        cameras[i].R = R;
        LOGLN("K:\n" << cameras[i].K() << "\nR:\n" << cameras[i].R<< "\nt:\n" << cameras[i].t);
    }
}

int CamAdjuster(vector<ImageFeatures> features, vector<MatchesInfo> pairwise_matches, vector<CameraParams> &cameras, string ba_cost_func, float conf_thresh )
{
    Ptr<detail::BundleAdjusterBase> adjuster;
    if (ba_cost_func == "reproj") adjuster = makePtr<detail::BundleAdjusterReproj>();
    else if (ba_cost_func == "ray") adjuster = makePtr<detail::BundleAdjusterRay>();
    else if (ba_cost_func == "affine") adjuster = makePtr<detail::BundleAdjusterAffinePartial>();
    else if (ba_cost_func == "no") adjuster = makePtr<NoBundleAdjuster>();
    else
    {
        cout << "Unknown bundle adjustment cost function: '" << ba_cost_func << "'.\n";
        return -1;
    }
    adjuster->setConfThresh(conf_thresh);

    string ba_refine_mask = "xxxxx";
    Mat_<uchar> refine_mask = Mat::zeros(3, 3, CV_8U);
    if (ba_refine_mask[0] == 'x') refine_mask(0,0) = 1;
    if (ba_refine_mask[1] == 'x') refine_mask(0,1) = 1;
    if (ba_refine_mask[2] == 'x') refine_mask(0,2) = 1;
    if (ba_refine_mask[3] == 'x') refine_mask(1,1) = 1;
    if (ba_refine_mask[4] == 'x') refine_mask(1,2) = 1;
    adjuster->setRefinementMask(refine_mask);

    if (!(*adjuster)(features, pairwise_matches, cameras))                                    //精确化的相机内参数矩阵K的掩码矩阵
    {
        cout << "Camera parameters adjusting failed.\n";
        return -1;
    }
    LOGLN("调整后K:\n" << cameras[0].K() << "\nR:\n" << cameras[0].R<< "\nt:\n" << cameras[0].t);
}

double Get_wif(vector<CameraParams> cameras)
{
        vector<double> focals;
        for (size_t i = 0; i < cameras.size(); ++i)
        {
            focals.push_back(cameras[i].focal);
        }

        sort(focals.begin(), focals.end());  //paixu
        float warped_image_focal;
        //if (focals.size() % 2 == 1)
            //warped_image_focal = static_cast<float>(focals[focals.size() / 2]);
        //else
            //warped_image_focal = static_cast<float>(focals[focals.size() / 2 - 1] + focals[focals.size() / 2]) * 0.5f;
        warped_image_focal = (focals[0]+focals[1]+focals[2])/focals.size();
        //warped_image_focal = 1000;

        return warped_image_focal;
        LOGLN(warped_image_focal);
}

void WaveCorrect(vector<CameraParams> &cameras, bool do_wave_correct)
{
    // ----修正R矩阵
    if (do_wave_correct)
    {
        vector<Mat> rmats;
        for (size_t i = 0; i < cameras.size(); ++i)
            rmats.push_back(cameras[i].R.clone());
        WaveCorrectKind wave_correct = detail::WAVE_CORRECT_HORIZ;
        waveCorrect(rmats, wave_correct);                          //波形矫正 试图使全景更水平
        for (size_t i = 0; i < cameras.size(); ++i)
            cameras[i].R = rmats[i];
    }

}

/*
Mat Calibraton_Output(vector<Mat> Img_row, Infor &saveinfor, CVmenber cvmenber, Constant constant)
{

    //return BlendImg(Img_row, &saveinfor, constant, cvmenber);

}
*/
