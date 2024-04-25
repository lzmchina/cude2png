#include <iostream>
#include "lut.hpp"
#include <windows.h>
#include <string>
#include <io.h>
#include "lodepng.h"
using namespace std;

string ORIGINIMG = "datas\\original.png";
void get_need_file(string path, vector<string> &file, string ext);

void printHelp()
{
    std::cout << "直接执行xxx.exe" << std::endl;
    std::cout << "datas下面的cubes文件夹放入需要转化的cudes文件，\npngs文件夹下面是转换后的png文件" << std::endl;
}

string formatStr(string s)
{
    std::stringstream ss;
    ss << "./datas/pngs/" << s << ".png";
    return ss.str();
}

void batchProcess()
{
    string file_path = R"(./datas/cubes)";
    vector<string> my_file;
    string need_extension = ".cube";
    get_need_file(file_path, my_file, need_extension);

    std::vector<unsigned char> image; // the raw pixels
    unsigned width, height;
    // decode
    unsigned error = lodepng::decode(image, width, height, ORIGINIMG, LCT_RGB);
    // unsigned error = lodepng::load_file(image, ORIGINIMG);
    // if there's an error, display it
    if (error)
        std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

    // uint8_t *data = image.data();

    for (auto i : my_file)
    {
        std::cout << "正在转化文件：" << i << std::endl;
        auto lut = octoon::image::lut::parse(i); // load the lut from .cube file
        std::vector<unsigned char> image_trans(image.size());
        lut.lookup(image.data(), image_trans.data(), image.size(), 3);
        string::size_type iPos = i.find_last_of('/') + 1;
        string filename = i.substr(iPos, i.length() - iPos);
        // cout << filename << endl;
        string name = filename.substr(0, filename.rfind("."));
        // cout << name << endl;
        error = lodepng::encode(formatStr(name), image_trans, width, height, LCT_RGB);
        // error = lodepng::save_file(image_trans, formatStr(name));
        // if there's an error, display it
        if (error)
            std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;
    }
}

int main(int argc, char *argv[])
{
    SetConsoleOutputCP(CP_UTF8);
    if (argc <= 1)
    {
        std::cout << "开始转化！" << std::endl;
        batchProcess();
    }
    else if (argc >= 2 && (strcmp(argv[1], "-h") || strcmp(argv[1], "--help")))
    {
        printHelp();
    }

    return 0;
}

void get_need_file(string path, vector<string> &file, string ext)
{
    intptr_t file_handle = 0;
    struct _finddata_t file_info;
    string temp;
    if ((file_handle = _findfirst(temp.assign(path).append("/*" + ext).c_str(), &file_info)) != -1)
    {
        do
        {
            file.push_back(temp.assign(path).append("/").append(file_info.name));
        } while (_findnext(file_handle, &file_info) == 0);
        _findclose(file_handle);
    }
}