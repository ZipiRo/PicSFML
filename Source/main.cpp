/*
    ============================================================
                            PicSFML
              SFML Project Creator & Build Utility
    ============================================================

    PicSFML is a command-line utility designed to simplify
    creating and building SFML projects with GCC/MinGW.

    Features:
    - Create ready-to-build SFML projects
    - Automatic SFML DLL management
    - Resource directory copying
    - Debug and Release build support
    - Optional VSCode configuration generation
    - Optional SFML Audio and Network modules

    Supported SFML Versions:
    - SFML 2.6.2
    - SFML 3.0.0

    Configuration:
    Uses `.picsfml_config` for project settings, libraries,
    include paths, binaries, and additional configuration.

    Author: ZipiRo
    Version: 1.0.11
    ============================================================
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <map>
#include <json.hpp>

namespace Win32
{
    #include <windows.h>
}

using json = nlohmann::json;

const std::string PROJECT_CONFIG_NAME = ".picsfml_config";
const std::string VSC_CONFIG_NAME = "c_cpp_properties.json";

std::map<int, std::string> sfml_version_core;

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
    std::cout << "Help - PicSFML SFML Project Creator & Builder\n";
    std::cout << "--------------------------------------\n";

    std::cout << "Usage:\n";
    std::cout << "picsfml [option] <path> [argumets]\n";
    std::cout << '\n';
    
    std::cout << "<path>            The path to your project <string> ('.' for current directory)\n";
    std::cout << '\n';

    std::cout << "Options:\n";
    std::cout << "  -b, --build     Build Project\n";
    std::cout << "  Arguments:\n";
    std::cout << "      -r          Release build\n";
    std::cout << "      -d          Debug build\n";
    std::cout << '\n';

    std::cout << "  -c, --create    Create project\n";
    std::cout << "  Arguments:\n";
    std::cout << "      -n          Project name <string> (default PicSFMLProject)\n";
    std::cout << "      -o          Output name <string> (default main)\n";
    std::cout << "      -m          Main file name <string> (default main.cpp)\n";
    std::cout << "      -g          GCC path <string> (path to mingwx directory)\n";
    std::cout << "      -s          SFML path <string> (path to sfml-x.x.x directory)\n";
    std::cout << "      -sv         SFML version <int> (default 300) {versions suported: 262, 300}\n";
    std::cout << "      -vs         Use VSCode properties file (default false)\n";
    std::cout << "      --audio      Use SFML audio (default false)\n";
    std::cout << "      --network    Use SFML network (default false)\n";
    std::cout << '\n';
    
    std::cout << "  -h, --help        Show help\n";
    std::cout << "  -v, --version     Show version\n";
}

void VersionOption()
{
    std::cout << "PicSFML SFML Project Creator & Builder, by ZipiRo.\n"
                "Created to simplify SFML project creation, and makeing it easier with including code\n"
                "and linking libraryes for any other things you want to add to your project.\n"
                "Surely works with SFML-2.6.2, SFML-3.0.0, more in  the future.\n"
                "Created on 05/05/2026 and Last Updated 05/20/2026.\n"
                "PicSFML Version 1.0.11\n";
}

void FlagNotExistent(const std::string &flag)
{
    std::cout << "The flag '" + flag + "' does not exist in the PicSFML workflow, try -h or --help for more information.\n";
}

bool CheckSFMLVersion(int version)
{
    if(sfml_version_core.find(version) == sfml_version_core.end())
    {
        std::cout << "This sfml version '" << version << "' does not exist in the current version of PicSFML, try -h or --help for more information.\n";
        return false;
    }

    return true;
}

int main(int argc, char** argv)
{ 
    if(argc <= 1) 
    {
        std::cout << "No option selected, try -h, --help for more information.\n";
        return 0;
    }
    
    sfml_version_core[262] = "SFML-2.6.2";
    sfml_version_core[300] = "SFML-3.0.0";

    option = Waiting;
    std::filesystem::path picsfml_path = GetLocalPath().parent_path();
    std::filesystem::path project_path;

    BuildConfig build_config{.picsfml_path = picsfml_path};
    CreateConfig create_config{.picsfml_path = picsfml_path};
    
    int index = 1;
    std::string flag(argv[index++]);
    if(flag == "-b" || flag == "--build") option = Build;
    else if(flag == "-c" || flag == "--create") option = Create;
    else if(flag == "-h" || flag == "--help") 
    {
        HelpOption();
        return 0;
    }
    else if(flag == "-v" || flag == "--version")
    {
        VersionOption();
        return 0;
    }
    else 
    {
        FlagNotExistent(flag);
        return 1;
    }

    if(argc <= 2)
    {
        std::cout << "There is no path to any directory project, try -h or --help for more information.\n";
        return 1;
    }

    project_path = std::string(argv[index++]);

    if(project_path.string()[0] == '.') project_path = std::filesystem::current_path();
    
    if(!std::filesystem::exists(project_path))
    {
        std::cout << "The given directory path does not exist in your filesystem.\n";
        return 1;
    }
    else project_path = std::filesystem::absolute(project_path);

    while (index < argc)
    {
        std::string flag(argv[index++]);

        switch (option)
        {
        case Build:
            if(flag == "-r")
            {    
                build_config.build_type = Release;
            }
            else if(flag == "-d")
            {
                build_config.build_type = Debug;
            }
            else FlagNotExistent(flag);
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
                int sfml_version = atoi(arg.c_str());
                if(!CheckSFMLVersion(sfml_version)) return 1;
                create_config.sfml_version = sfml_version;
            }
            else if(flag == "-vs")
            {
                create_config.use_vscode = true;
            }
            else if(flag == "--audio")
            {
                create_config.use_audio = true;
            }
            else if(flag == "--network")
            {
                create_config.use_network = true;
            }
            else FlagNotExistent(flag);
            break;
        default:
            break;
        }
    }

    switch (option)
    {
    case Build:
        build_config.project_path = project_path;
        if(!BuildOption(build_config)) return 1;
        break;
    case Create:
        create_config.project_path = project_path;
        if(!CreateOption(create_config)) return 1;
        break;
    default:
        break;
    }

    return 0;
}