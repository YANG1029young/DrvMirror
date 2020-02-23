#ifndef GETINFOR_H
#define GETINFOR_H
#include "common.h"

extern void Get_Saveinfor(vector<Mat> img_row, Infor &saveinfor, CVmenber &cvmenber, Constant constant);
void Get_CalibrationInfor(vector<Mat> img_row, Infor &calibration_infor, CVmenber &cvmenber, Constant &constant );
int Get_BlendInfor(Infor &blend_infor, CVmenber &cvmenber, Constant constant);
void Get_EarmaskInfor(vector<Mat> &earmask_infor, double ear_scale_side);

#endif // GETMASKWARPED

