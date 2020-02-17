#include <QCoreApplication>
#include "drvmirror.h"

using namespace std;
using namespace cv;
using namespace cv::detail;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //得到原始图像
    vector<Mat> img_roww(3);

    img_roww[0] = imread("./left.jpg");
    img_roww[1] = imread("./middle.jpg");
    img_roww[2] = imread("./right.jpg");


    DrvMirror dd;
    dd.Initial();

    /* 标定
    Mat aa = dd.Calibraton(img_roww);

    imshow("result",aa);
    waitKey();
    dd.Save_Infor();
//*/

    //*
    dd.Set_ComposeScale(0.5);
    dd.Set_Mode(normal);
    dd.Set_BlendSwitch(true);
    Mat bb = dd.Get_ResultImg(img_roww);
    imshow("result2",bb);
    waitKey();
    //*/


    return a.exec();
}


