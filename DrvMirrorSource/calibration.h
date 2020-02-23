#ifndef CALIBRATION_H
#define CALIBRATION_H

#include "common.h"

int FeaturesFind(vector<Mat> img_row, vector<ImageFeatures> &features, double Calibration_megapix, string features_type);
int FeaturesMatch(vector<ImageFeatures> features, vector<MatchesInfo> &pairwise_matches, string matcher_type, bool try_cuda, float match_conf);
int CamEstimator(vector<ImageFeatures> features, vector<MatchesInfo> pairwise_matches, vector<CameraParams> &cameras, string Camestimator_type);
int CamAdjuster(vector<ImageFeatures> features, vector<MatchesInfo> pairwise_matches, vector<CameraParams> &cameras, string ba_cost_func, float conf_thresh );

double Get_wif(vector<CameraParams> cameras);
void WaveCorrect(vector<CameraParams> &cameras, bool do_wave_correct);

#endif // CALIBRATION

