#ifndef OUTPUTSERVE
#define OUTPUTSERVE

#include "common.h"

typedef struct
{
    int PlineImg0;
    int PlineImg1L;
    int PlineImg1R;
    int PlineImg2;
    int PlineDstL;
    int PlineDstR;
}Pline;

Mat BlendImg(vector<Mat> img_row, Infor &infor, CVmenber &cvmenber, Constant &constant);
Mat BlendImg2(vector<Mat> img_row, Infor &infor, CVmenber &cvmenber, Constant &constant);
Mat Blend2Img(vector<Mat>img_warped_masked, Point corner0, Point corner1, int cutlength, int OptimizWidth );
void OptimizeSeam(Mat img0, Mat img1, Mat img2, Mat dst, int OptimWidth, Pline PL);
void OptimizeSeam2img(Mat img_handle0, Mat img_handle1, Mat dst, int OptimWidth, int PlineImg0, int PlineImg1);

Mat PureImg(vector<Mat> img_row, Constant &constant);

#endif // OUTPUTSERVE


