
#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <libgen.h>
#include <onnxruntime/core/session/onnxruntime_cxx_api.h>
#include <jsoncpp/json/json.h>
#include <fstream>
#include <cassert>

using namespace std;

#if defined(_MSC_VER)
#include <direct.h>
#include <io.h>
#define GetCurrentDir _getcwd
#elif defined(__unix__)
#include <unistd.h>
#define GetCurrentDir getcwd
//#else
#endif

string get_current_directory();
string getOutPath(string path = "");

int main(void)
{
    std::string path = "/home/alex/code/Insulator_Identification/test.json";
    std::ofstream fout;
    fout.open(path.c_str());
    assert(fout.is_open());

    Json::Value root;
    root["name"] = "test";
    root["path"] = "/tets/file";

    Json::Value array;
    for (int i = 0; i < 4; i++)
    {
        Json::Value item;
        item["a"] = 1 + i;
        item["b"] = i;
        array.append(item);
    }
    root["array"] = array;
    std::string out = root.toStyledString();
    std::cout << out << std::endl;
    fout << out << std::endl;
    return 0;
}

int main(void)
{
    string path;
    path = getOutPath();
    cout << getOutPath() << endl;
    return 0;
}

/*
int main(int argc, char* argv[])
{
    std::cout << "Current working directory: " << get_current_directory() << endl;

    return 0;
}



int main(void)
{
    string filename = "/home/alex/code/yolov7/deploy/triton-inference-server/labels.py";
    string outname_base = filename;
    char *p = (char *)outname_base.c_str();
    string outname = basename(p); //最后斜杠之后的文件名
    string outdir = dirname(p);
    cout << outdir << " -- " << outname << endl;

    return 0;
}
*/

std::string get_current_directory()
{
    char buff[250];
    GetCurrentDir(buff, 250);
    string current_working_directory(buff);
    return current_working_directory;
}

std::string getOutPath(string path)
{
    if (path == "")
    {
        path = get_current_directory() + "/temp/file";
    }
    else
    {
        path = path;
    }
    string command;
    command = "mkdir -p " + path;
    cout << command << endl;
    system(command.c_str());
    return path;
}