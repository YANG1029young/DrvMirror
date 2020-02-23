#ifndef OUTPUT_H
#define OUTPUT_H

#include "common.h"

Mat NormalImg(vector<Mat> img_row, Infor &blend_infor, CVmenber &cvmenber, Constant &constant);
Mat TurnleftImg(vector<Mat> img_row, Infor &infor, CVmenber &cvmenber, Constant &constant);
Mat TurnrightImg(vector<Mat> img_row, Infor &infor, CVmenber &cvmenber, Constant &constant);
Mat EarImg(vector<Mat> img_row, vector<Mat> &earmask_infor, Constant &constant);

#endif // BLENDIMG

