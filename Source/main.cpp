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
    while (index < argc)
    {
        std::string flag(argv[index++]);

        switch (option)
        {
        case Waiting:
            if(flag == "-b") option = Build;
            else if(flag == "-c") option = Create;
            else if(flag == "-h") 
            {
                option = Help;
                break;
            } 
            else if(flag == "--version")
            {
                option = Version;
                break;  
            } 
            break;
        case Build:
            if(flag == "-p")
            {
                std::string arg(argv[index++]);
                project_path = arg;
                create_config.project_path = arg;
            }
            else if(flag == "-r")
            {    
                build_config.build_type = Release;
            }
            else if(flag == "-d")
            {
                build_config.build_type = Debug;
            }
            break;
        case Create:
            if(flag == "-p")
            {
                std::string arg(argv[index++]);
                project_path = arg;
                create_config.project_path = arg;
            }
            else if(flag == "-n")
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
        if(!BuildOption(build_config)) return 1;
        break;
    case Create:
        if(project_path.empty()) return 1;
        if(!CreateOption(create_config)) return 1;
        break;
    case Help:
        break;
    case Version:
        std::cout << "PicSFML v1.0.0" << '\n';
        break;
    default:
        break;
    }

    return 0;
}