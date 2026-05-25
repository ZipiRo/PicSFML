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
    - Sets the icon if there exists an icon.png file
    - Set the application version in config file 

    Supported SFML Versions:
    - SFML 2.6.2
    - SFML 3.0.0

    Configuration:
    Uses `.picsfml_config` for project settings, libraries,
    include paths, binaries, and additional configuration.

    Author: ZipiRo
    Version: 1.0.15
    ============================================================
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <map>
#include <json.hpp>
#include <pversion.h>

using json = nlohmann::json;

namespace Win32
{
    #include <windows.h>

    std::filesystem::path GetLocalPath()
    {
        char buffer[256];
        Win32::GetModuleFileNameA(NULL, buffer, 256);
        return std::filesystem::path(buffer);
    }
}

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

std::map<int, std::string> SetSFMLVersions()
{
    std::map<int, std::string> versions;

    versions[300] = "SFML-3.0.0";
    versions[262] = "SFML-2.6.2";

    return versions;
}

bool CheckSFMLVersion(const PVersion &version, const std::map<int, std::string> &versions)
{
    if(versions.find(version.AsInt()) == versions.end())
    {
        std::cout << "This sfml version '" << version.AsString('.') << "' does not exist in the current version of PicSFML\n";
        std::cout << "Try -h, --help for more information.\n";
        return false;
    }

    return true;
}

void FlagNotExistent(const std::string &flag)
{
    std::cout << "The flag '" + flag + "' does not exist in the PicSFML workflow\n";
    std::cout << "Try -h, --help for more information.\n";
}

bool YesNoOption()
{
    int option = -1;
    std::string arg;

    std::cout << "-y, --yes            Accept\n";
    std::cout << "-n, --no             Discard\n";

    while(option == -1)
    {
        std::cin >> arg;

        if(arg == "--yes" || arg == "-y") option = 1;
        else if(arg == "--no" || arg == "-n") option = 0;

        if(option == -1)
            std::cout << "Flag " << arg << " is not an option, try again\n";
    }

    return option;
}

const std::filesystem::path picsfml_path = Win32::GetLocalPath().parent_path();
const std::string PROJECT_CONFIG = ".picsfml_config";
const std::string VSC_CONFIG = "c_cpp_properties.json";
const std::string APPLICATION_ICON = "icon.png";
const std::string WINDOWS_ICON = "icon.ico";
const std::string WINDOWS_RESOURCE_NAME = "resource";
const std::map<int, std::string> sfml_versions = SetSFMLVersions();

#include <build_option.h>
#include <create_option.h>
#include <help_option.h>
#include <version_option.h>

enum OPTION 
{
    None,
    Build,
    Create,
    Set,
    Help,
    ShowVersion
} option;

int main(int argc, char** argv)
{ 
    if(argc <= 1) 
    {
        std::cout << "No option selected, try -h, --help for more information.\n";
        return 0;
    }

    SetSFMLVersions();

    BuildConfig build_config;
    CreateConfig create_config;
    
    int index = 1;
    std::string flag(argv[index++]);
    if(flag == "-b" || flag == "--build") option = Build;
    else if(flag == "-c" || flag == "--create") option = Create;
    else if(flag == "-s" || flag == "--set") option = Set;
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

    std::filesystem::path project_path = std::string(argv[index++]);

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
                PVersion sfml_version(arg);
                if(!CheckSFMLVersion(sfml_version, sfml_versions)) return 1;
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
        case Set:
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
    case Set:
        break;
    default:
        break;
    }

    return 0;
}