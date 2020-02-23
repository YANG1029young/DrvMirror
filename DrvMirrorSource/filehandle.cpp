#include "filehandle.h"


//	保存摄像机参数，文件格式如下：
//	第一行是中间焦距median_focal_len_
//	之后每一行是一个相机--
//	数据依次是focal、aspect、ppx、ppy、R、t
int saveCameraParam( string filename ,float warped_image_focal, vector<CameraParams> cameras)
{
    ofstream cp_file(filename.c_str());
    cp_file << warped_image_focal << endl;
    for(uint i = 0; i < cameras.size(); i++)
    {
        CameraParams cp = cameras[i];
        cp_file << cp.focal << " " << cp.aspect << " " << cp.ppx << " " << cp.ppy;
        for(int r = 0; r < 3; r++)
            for(int c = 0; c < 3; c++)
                cp_file << " " << cp.R.at<float>(r, c);
        for(int r = 0; r < 3; r++)
            cp_file << " " << cp.t.at<double>(r, 0);
        cp_file << endl;
    }
    cp_file.close();
}

int loadCameraParam( string filename ,float *warped_image_scale,vector<CameraParams> *camerainfo)
{
    ifstream cp_file(filename.c_str());
    string line;

    //	median_focal_len_
    if(!getline(cp_file, line))
        return -1;
    stringstream mfl_string_stream;
    mfl_string_stream << line;
    mfl_string_stream >> *warped_image_scale;

    //	每行一个摄像机
    camerainfo->clear();

    while(getline(cp_file, line))
    {
        stringstream cp_string_stream;
        cp_string_stream << line;
        CameraParams cp;
        cp.R.create(3, 3, CV_32F);
        cp.t.create(3, 1, CV_64F);
        cp_string_stream >> cp.focal >> cp.aspect >> cp.ppx >> cp.ppy;
        for(int r = 0; r < 3; r++)
            for(int c = 0; c < 3; c++)
                cp_string_stream >> cp.R.at<float>(r, c);
        for(int r = 0; r < 3; r++)
            cp_string_stream >> cp.t.at<double>(r, 0);
        camerainfo->push_back(cp);
    }

    //    camera.erase(camera.begin());//删除camera的第一个元素
    //    camerainfo->swap(camera);
    return 0;
}

/*
void saveCornerSize( string filename, vector<Point> corners, vector<Size> sizes, double compose_scale)
{
    ofstream cp_file(filename.c_str());

    cp_file << compose_scale <<endl ;
    for(int i = 0; i < sizes.size(); i++)
    {
        Size ab = sizes[i];
        cp_file << ab.height  << " " << ab.width<< " " ;
    }
    cp_file << endl;
    for(int i = 0; i < corners.size(); i++)
    {
        Point ab = corners[i];
        cp_file << ab.x << " " << ab.y << " ";
    }
    cp_file << endl;

    cp_file.close();
}
*/
int saveCornerSize( string filename, vector<Point> corners, vector<Size> sizes)
{
    ofstream cp_file(filename.c_str());

    for(uint i = 0; i < sizes.size(); i++)
    {
        Size ab = sizes[i];
        cp_file << ab.height  << " " << ab.width<< " " ;
    }
    cp_file << endl;
    for(uint i = 0; i < corners.size(); i++)
    {
        Point ab = corners[i];
        cp_file << ab.x << " " << ab.y << " ";
    }
    cp_file << endl;

    cp_file.close();
}
/*
int loadCornerAndSizes( string filename ,vector<Point> *corners_load,vector<Size> *sizes_load , double *scale)
{
    ifstream cp_file(filename.c_str());
    string line;
    sizes_load->clear();
    corners_load->clear();
    //	median_focal_len_
    if(!getline(cp_file, line))
        return -1;
    stringstream cp_string_stream;
    cp_string_stream << line;
    cp_string_stream >> *scale;

    getline(cp_file, line);
    stringstream string_stream;
    string_stream << line;
    for(int i = 0; i < 3; i++)
    {
        Size ab;
        string_stream >> ab.height >>ab.width;
        sizes_load->push_back(ab);
    }
    stringstream string_stream1;
    getline(cp_file, line);
    string_stream1 << line;
    for(int i = 0; i < 3; i++)
    {
        Point ha ;
        string_stream1 >>  ha.x >> ha.y ;
        corners_load->push_back(ha);
    }
    return 0;
}
*/
int loadCornerAndSizes( string filename ,vector<Point> *corners_load,vector<Size> *sizes_load)
{
    ifstream cp_file(filename.c_str());
    string line;
    sizes_load->clear();
    corners_load->clear();
    //	median_focal_len_
    if(!getline(cp_file, line))
        return -1;

    stringstream string_stream;
    string_stream << line;
    for(int i = 0; i < 3; i++)
    {
        Size ab;
        string_stream >> ab.height >>ab.width;
        sizes_load->push_back(ab);
    }
    stringstream string_stream1;
    getline(cp_file, line);
    string_stream1 << line;
    for(int i = 0; i < 3; i++)
    {
        Point ha ;
        string_stream1 >>  ha.x >> ha.y ;
        corners_load->push_back(ha);
    }
    return 0;
}
