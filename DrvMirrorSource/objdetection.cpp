#include "common.h"
#include "objdetection.h"


Mat ObjDetection(Mat img_input)
{

    //配置处理图片为正方形，并适当缩小

    /*
    Mat img_handle(img_input.cols*0.6, img_input.cols, CV_8UC3);
    img_handle.setTo(0);
    img_input.copyTo(img_handle(Rect(0, 0, img_input.cols, img_input.rows)));

    //resize(img_handle,img_handle,Size(1800, 1800));
    imshow("image", img_handle);
    waitKey(0);

    //*/



    String deploy_prototxt = "MobileNetSSD_deploy.prototxt.txt";
    String modle = "MobileNetSSD_deploy.caffemodel";

    float confidenceThreshold = 0.3;
    const size_t inWidth = 300;
    const size_t inHeight = 300;

    //*
    const char* classNames[] = {"backround","aeroplane","bicycle","bird","boat","bottle","bus","car","cat",
                                "chair","cow","diningtable","dog","house","motorbike","person","pottedplant",
                                "sheep","sofa","train", "tvmonitor"};
    //*/

    //const char* classNames[] = {"house"};

    dnn::Net net = dnn::readNetFromCaffe(deploy_prototxt, modle);

    Mat myimg = img_input;
    //Mat myimg = img_handle;




    Mat blob = dnn::blobFromImage(myimg, 1.0/127.5, Size(inWidth,inHeight),(127.5,127.5,127.5), false, true);
    //Mat blob = dnn::blobFromImage(resize(myimg,(300,300)) , 0.007843, (300,300), 127.5);
    //imshow("1", blob);
    //waitKey();

    //imshow("1", myimg);
    //waitKey();

    net.setInput(blob);
    Mat output = net.forward();


    Mat detectionMat(output.size[2], output.size[3], CV_32F, output.ptr<float>());


    cout<<detectionMat.rows<<endl;

    for (int i = 0; i < detectionMat.rows; i++)
    {

        float confidence = detectionMat.at<float>(i, 2);

        if (confidence > confidenceThreshold )
        {
            //cout<<"jkas"<<endl;
            size_t objectClass = (size_t)(detectionMat.at<float>(i, 1));
            //if(objectClass == 7)                        //只检测汽车
            {
                //cout<<"jkasj"<<endl;
                int xLeftBottom = static_cast<int>(detectionMat.at<float>(i, 3) * myimg.cols);
                int yLeftBottom = static_cast<int>(detectionMat.at<float>(i, 4) * myimg.rows);
                int xRightTop = static_cast<int>(detectionMat.at<float>(i, 5) * myimg.cols);
                int yRightTop = static_cast<int>(detectionMat.at<float>(i, 6) * myimg.rows);
                ostringstream ss;
                ss << confidence;
                String conf(ss.str());
                Rect object((int)xLeftBottom, (int)yLeftBottom, (int)(xRightTop - xLeftBottom), (int)(yRightTop - yLeftBottom));
                rectangle(myimg, object, Scalar(0, 255, 0), 2);
                String label = String(classNames[objectClass]) + ": " + conf;
                int baseLine = 0;
                Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
                rectangle(myimg, Rect(Point(xLeftBottom, yLeftBottom - labelSize.height), Size(labelSize.width, labelSize.height + baseLine)), Scalar(0, 255, 0), CV_FILLED);
                putText(myimg, label, Point(xLeftBottom, yLeftBottom), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0));
            }

        }
    }
    //namedWindow("image", CV_WINDOW_NORMAL);
    //imshow("image", myimg);
    //waitKey(0);

    //imshow("image2", myimg);
    //waitKey(0);

    //还原输出图片比例



    return myimg(Rect(0, 0, img_input.cols, img_input.rows));
}
