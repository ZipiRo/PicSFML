#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <json.hpp>

namespace Win32
{
    #include <windows.h>
}

using json = nlohmann::json;

const std::string PROJECT_CONFIG_NAME = ".picsfml_config";
const std::string VSC_CONFIG_NAME = "c_cpp_properties.json";

std::string sfml_version_core[5] = {
    "",
    "",
    "SFML-2.6.2",
    "SFML-3.0.0",
};

bool GetConfigJSON(const std::filesystem::path &path, json &result)
{
    if(!std::filesystem::exists(path)) return false;

    std::ifstream json_file(path);

    if(!json_file) return false;

    json_file >> result;

    json_file.close();

    return true;
}

bool SetConfigJSON(const std::filesystem::path &path, const json &config_file)
{
    std::ofstream json_file(path);

    if(!json_file) return false;

    json_file << config_file.dump(4);

    json_file.close();

    return true;
}
#include "build_option.h"
#include "create_option.h"

enum OPTION 
{
    Waiting,
    Build,
    Create,
    Help,
    Version
} option;

std::filesystem::path GetLocalPath()
{
    char buffer[256];
    Win32::GetModuleFileNameA(NULL, buffer, 256);
    return std::filesystem::path(buffer);
}

void HelpOption()
{
    std::cout << "PicSFML SFML Project Creator & Builder\n";
    std::cout << "--------------------------------------\n";

    std::cout << "Usage:\n";
    std::cout << "picsfml [option] <path> [argumets]\n";
    std::cout << '\n';
    
    std::cout << "Options:\n";
    std::cout << "  -b Build Project\n";
    std::cout << "Arguments:\n";
    std::cout << "      -r Release build\n";
    std::cout << "      -d Debug build\n";
    std::cout << '\n';

    std::cout << "  -c Create project\n";
    std::cout << "Arguments:\n";
    std::cout << "      -n Project name <string> (default PicSFMLProject)\n";
    std::cout << "      -o Output name <string> (default main)\n";
    std::cout << "      -m Main file name <string> (default main.cpp)\n";
    std::cout << "      -g GCC path <string> (path to mingwx directory)\n";
    std::cout << "      -s SFML path <string> (path to sfml-x.x.x directory)\n";
    std::cout << "      -sv SFML version <int> (default 3) (available versions 2, 3)\n";
    std::cout << "      -vs Use VSCode properties file (default false)\n";
    std::cout << "      -audio Use SFML audio (default false)\n";
    std::cout << "      -network Use SFML network (default false)\n";
    std::cout << '\n';
    
    std::cout << "-h, --help Show help message\n";
    std::cout << "-v, --version Show version\n";
}

int main(int argc, char** argv)
{ 
    option = Waiting;
    std::filesystem::path picsfml_path = GetLocalPath().parent_path();
    std::filesystem::path project_path;

    BuildConfig build_config{.picsfml_path = picsfml_path};
    CreateConfig create_config{.picsfml_path = picsfml_path};

    if(argc <= 1) 
    {
        
        return 0;
    }
    
    int index = 1;

    std::string flag(argv[index++]);
    if(flag == "-b") option = Build;
    else if(flag == "-c") option = Create;
    else if(flag == "-h" || flag == "--help") 
    {
        HelpOption();
        return 0;
    }
    else if(flag == "-v" || flag == "--version")
    {
        return 0;
    }

    std::string arg(argv[index++]);
    project_path = arg;
    
    while (index < argc)
    {
        std::string flag(argv[index++]);

        switch (option)
        {
        case Waiting:
            break;
        case Build:
            if(flag == "-r")
            {    
                build_config.build_type = Release;
            }
            else if(flag == "-d")
            {
                build_config.build_type = Debug;
            }
            break;
        case Create:
            if(flag == "-n")
            {
                std::string arg(argv[index++]);
                create_config.name = arg;
            }
            else if(flag == "-o")
            {
                std::string arg(argv[index++]);
                create_config.output = arg;
            }
            else if(flag == "-g")
            {
                std::string arg(argv[index++]);
                create_config.gcc_path = arg;
            }
            else if(flag == "-s")
            {
                std::string arg(argv[index++]);
                create_config.sfml_path = arg;
            }
            else if(flag == "-m")
            {
                std::string arg(argv[index++]);
                create_config.main = arg;
            }
            else if(flag == "-sv")
            {
                std::string arg(argv[index++]);
                create_config.sfml_version = atoi(arg.c_str());
            }
            else if(flag == "-vs")
            {
                create_config.use_vscode = true;
            }
            else if(flag == "-audio")
            {
                create_config.use_audio = true;
            }
            else if(flag == "-network")
            {
                create_config.use_network = true;
            }
            break;
        default:
            break;
        }
    }

    switch (option)
    {
    case Build:
        if(project_path.empty()) return 1;
        build_config.project_path = project_path;
        if(!BuildOption(build_config)) return 1;
        break;
    case Create:
        if(project_path.empty()) return 1;
        create_config.project_path = project_path;
        if(!CreateOption(create_config)) return 1;
        break;
    default:
        break;
    }

    return 0;
}