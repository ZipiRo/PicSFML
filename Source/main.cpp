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
    Version: 1.1.2
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
#include <picconfig.h>

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

enum BuildType 
{ 
    Release, 
    Debug 
};

enum Option 
{
    None,
    Build,
    Create,
    Set,
    Help,
    ShowVersion
};

const PVersion PICSFML_VERSION = PVersion("1.1.2");
const std::string PROJECT_CONFIG = ".picsfml_config";
const std::string VSC_CONFIG = "c_cpp_properties.json";
const std::string APPLICATION_ICON = "icon.png";
const std::string WINDOWS_ICON = "icon.ico";
const std::string WINDOWS_RESOURCE_NAME = "resource";
const std::filesystem::path picsfml_path = Win32::GetLocalPath().parent_path();
std::filesystem::path project_path;

bool RemovePath(std::vector<std::filesystem::path> &paths, const std::string &path)
{
    for(int i = 0; i < paths.size(); i++)
    {
        if(paths[i].string() == path)
        {
            paths.erase(paths.begin() + i);
            return true;
        }
    }

    std::cout << "Path " << path << " does not exist in your list\n";

    return false;
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

bool CreateConfigJSON(const std::filesystem::path &path, const json &config_file)
{
    std::ofstream json_file(path);

    if(!json_file) return false;

    json_file << config_file.dump(4);

    json_file.close();

    return true;
}

bool CreatePicSFMLConfigJSON(const PicConfig &pic_config, const std::filesystem::path &path)
{
    std::filesystem::path from = path / PROJECT_CONFIG;

    json project_config;

    project_config["version"] = PICSFML_VERSION.AsString('.');
    project_config["binary"] = pic_config.binary;
    project_config["include"] = pic_config.include;
    project_config["library"] = pic_config.library;
    project_config["build"]["debug"]["flags"] = pic_config.flags[Debug];
    project_config["build"]["release"]["flags"] = pic_config.flags[Release];
    project_config["project"]["gcc"] = pic_config.gcc_path;
    project_config["project"]["sfml"] = pic_config.sfml_path;    
    project_config["project"]["name"] = pic_config.name;
    project_config["project"]["output"] = pic_config.output;    
    project_config["project"]["main"] = pic_config.main;
    project_config["project"]["use_audio"] = pic_config.use_audio;
    project_config["project"]["use_network"] = pic_config.use_network;
    project_config["project"]["sfml_version"] = pic_config.sfml_version.AsString('.');
    project_config["project"]["application_version"] = pic_config.application_version.AsString('.');
    project_config["use_vscode"] = pic_config.use_vscode;

    if(!CreateConfigJSON(from, project_config)) return false;

    std::cout << "PicSFML Config file creted\n";

    return true;
}

bool GetPicSFMLConfig(PicConfig &pic_config, const std::filesystem::path &path)
{
    std::filesystem::path from = path / PROJECT_CONFIG;
 
    json project_config;

    if(!GetConfigJSON(from, project_config)) return false;

    pic_config.version.ParseString(project_config["version"]);
    pic_config.name = project_config["project"]["name"];
    pic_config.output = project_config["project"]["output"];
    pic_config.use_audio = project_config["project"].value("use_audio", false);
    pic_config.use_network = project_config["project"].value("use_network", false);
    pic_config.sfml_version.ParseString(project_config["project"]["sfml_version"]);
    pic_config.application_version.ParseString(project_config["project"]["application_version"]);
    pic_config.flags[Debug] = project_config["build"]["debug"]["flags"];
    pic_config.flags[Release] = project_config["build"]["release"]["flags"];
    pic_config.use_vscode = project_config.value("use_vscode", false);

    if(!std::filesystem::exists(project_config["project"]["gcc"]))
    {
        std::cout << "GCC path "  << project_config["project"]["gcc"] << " not found\n";
        return false;
    } 
    pic_config.gcc_path = std::string(project_config["project"]["gcc"]);

    if(!std::filesystem::exists(project_config["project"]["sfml"]))
    {
        std::cout << "SFML path "  << project_config["project"]["sfml"] << " not found\n";
        return false;
    } 
    pic_config.sfml_path = std::string(project_config["project"]["sfml"]);

    if(!std::filesystem::exists(path / project_config["project"]["main"]))
    {
        std::cout << "Main file "  << project_config["project"]["main"] << " not found\n";
        return false;
    } 
    pic_config.main = project_config["project"]["main"];

    for(const auto &binary : project_config["binary"])
    {
        if(!std::filesystem::exists(binary) && binary.empty()) 
        {
            std::cout << "File " << binary << " not found\n";
            return false;
        }   

        pic_config.binary.push_back(binary);
    }

    for(const auto &include : project_config["include"])
    {
        if(!std::filesystem::exists(include) && include.empty()) 
        {
            std::cout << "Include " << include << " not found\n";
            return false;
        }   

        pic_config.include.push_back(include);  
    }

    for(const auto &library : project_config["library"])
    {
        if(std::string(library)[0] == '-')
        {
            pic_config.library.push_back(library);
            continue;
        }

        if(!std::filesystem::exists(library)) 
        {
            std::cout << "Library " << library << " not found\n";
            return false;
        }   
        
        pic_config.library.push_back(library);
    }

    return true;
}

bool CreateVSCodeConfigJSON(const PicConfig &pic_config, const std::filesystem::path &path)
{
    std::filesystem::create_directory(path / ".vscode");

    std::filesystem::path to =
        path / ".vscode" / VSC_CONFIG;

    json vscode_config;

    vscode_config["version"] = 4;
    vscode_config["configurations"] = json::array();

    json config;

    config["name"] = pic_config.name;
    config["cStandard"] = "c17";
    config["cppStandard"] = "gnu++17";
    config["intelliSenseMode"] = "windows-gcc-x64";

    if(!pic_config.gcc_path.empty())
    {
        config["compilerPath"] =
            (pic_config.gcc_path / "bin" / "g++.exe").string();
    }
    else
    {
        config["compilerPath"] = "";
    }
    config["includePath"] = json::array();
    config["includePath"].push_back("${workspaceFolder}/**");
    config["includePath"].push_back((picsfml_path / "Core" / SFMLCoreVersions().at(pic_config.sfml_version.AsInt())).string());

    if(!pic_config.sfml_path.empty())
    {
        config["includePath"].push_back(
            (pic_config.sfml_path / "include").string());
    }

    for(const auto &include : pic_config.include)
        config["includePath"].push_back(include.string());

    vscode_config["configurations"].push_back(config);

    if(!CreateConfigJSON(to, vscode_config))
        return false;

    std::cout << "VSCode properties file created\n";

    return true;
}

bool CheckSFMLVersion(const PVersion &version)
{
    if(SFMLCoreVersions().find(version.AsInt()) == SFMLCoreVersions().end())
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

#include <build_option.h>
#include <create_option.h>
#include <set_option.h>
#include <help_option.h>
#include <version_option.h>

int main(int argc, char** argv)
{ 
    if(argc <= 1) 
    {
        std::cout << "No option selected, try -h, --help for more information.\n";
        return 0;
    }

    SFMLCoreVersions(); 

    Option option;
    PicConfig pic_config;

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

    project_path = std::string(argv[index++]);

    if(project_path.string()[0] == '.') project_path = std::filesystem::current_path();
    
    if(!std::filesystem::exists(project_path))
    {
        std::cout << "The given directory path does not exist in your filesystem.\n";
        return 1;
    }
    else project_path = std::filesystem::absolute(project_path);

    if(option == Set) GetPicSFMLConfig(pic_config, project_path);

    BuildType build_type = Release;

    while (index < argc)
    {
        std::string flag(argv[index++]);

        switch (option)
        {
        case Build:
            if(flag == "-r" || flag == "--release")
            {    
                build_type = Release;
            }
            else if(flag == "-d" || flag == "--debug")
            {
                build_type = Debug;
            }
            else FlagNotExistent(flag);
            break;
        case Create:
            if(flag == "-n" || flag == "--name")
            {
                std::string arg(argv[index++]);
                pic_config.name = arg;
            }
            else if(flag == "-o" || flag == "--output")
            {
                std::string arg(argv[index++]);
                pic_config.output = arg;
            }
            else if(flag == "-g" || flag == "--gcc")
            {
                std::string arg(argv[index++]);
                pic_config.gcc_path = arg;
            }
            else if(flag == "-s" || flag == "--sfml")
            {
                std::string arg(argv[index++]);
                pic_config.sfml_path = arg;
            }
            else if(flag == "-m" || flag == "--main")
            {
                std::string arg(argv[index++]);
                pic_config.main = arg;
            }
            else if(flag == "-sv" || flag == "--sfml_version")
            {
                std::string arg(argv[index++]);
                PVersion sfml_version(arg);
                if(!CheckSFMLVersion(sfml_version)) return 1;
                pic_config.sfml_version = sfml_version;
            }
            else if(flag == "--vscode")
            {
                pic_config.use_vscode = true;
            }
            else if(flag == "--audio")
            {
                pic_config.use_audio = true;
            }
            else if(flag == "--network")
            {
                pic_config.use_network = true;
            }
            else FlagNotExistent(flag);
            break;
        case Set:
            if(flag == "-n" || flag == "--name")
            {
                std::string arg(argv[index++]);

                if(arg == "--set")
                {
                    arg = argv[index++];
                    pic_config.name = arg;
                }
                else if(arg == "--clear")
                {
                    pic_config.name = "";
                }
                else FlagNotExistent(arg);
            }
            else if(flag == "-o" || flag == "--output")
            {
                std::string arg(argv[index++]);

                if(arg == "--set")
                {
                    arg = argv[index++];
                    pic_config.output = arg;
                }
                else if(arg == "--clear")
                {
                    pic_config.output = "";
                }
                else FlagNotExistent(arg);
            }
            else if(flag == "-g" || flag == "--gcc")
            {
                std::string arg(argv[index++]);

                if(arg == "--set")
                {
                    arg = argv[index++];
                    pic_config.gcc_path = arg;
                }
                else if(arg == "--clear")
                {
                    pic_config.gcc_path = "";
                }
                else FlagNotExistent(arg);
            }
            else if(flag == "-s" || flag == "--sfml")
            {
                std::string arg(argv[index++]);

                if(arg == "--set")
                {
                    arg = argv[index++];
                    pic_config.sfml_path = arg;
                }
                else if(arg == "--clear")
                {
                    pic_config.sfml_path = "";
                }
                else FlagNotExistent(arg);
            }
            else if(flag == "-m" || flag == "--main")
            {
                std::string arg(argv[index++]);

                if(arg == "--set")
                {
                    arg = argv[index++];
                    pic_config.main = arg;
                }
                else if(arg == "--clear")
                {
                    pic_config.main = "";
                }
                else FlagNotExistent(arg);
            }
            else if(flag == "-sv" || flag == "--sfml_version")
            {
                std::string arg(argv[index++]);

                if(arg == "--set")
                {
                    arg = argv[index++];
                    PVersion sfml_version(arg);
                    if(!CheckSFMLVersion(sfml_version)) return 1;
                    pic_config.sfml_version = sfml_version;
                }
                else if(arg == "--clear")
                {
                    pic_config.sfml_version = PVersion("0");
                }
                else FlagNotExistent(arg);
            }
            else if(flag == "-av" || flag == "--application_version")
            {
                std::string arg(argv[index++]);

                if(arg == "--set")
                {
                    arg = argv[index++];
                    PVersion application_version(arg);
                    pic_config.application_version = application_version;
                }
                else if(arg == "--clear")
                {
                    pic_config.application_version = PVersion("0.0.0.0");
                }
                else FlagNotExistent(arg);
            }
            else if(flag == "-l" || flag == "--library")
            {
                std::string arg(argv[index++]);

                if(arg == "--add")
                {
                    arg = argv[index++];
                    pic_config.library.push_back(arg);
                }
                else if(arg == "--remove")
                {
                    arg = argv[index++];
                    if(arg == "--back") pic_config.library.pop_back();
                    else if(arg == "--front") pic_config.library.erase(pic_config.library.begin());
                    else RemovePath(pic_config.library, arg);
                }
                else FlagNotExistent(arg);
            }
            else if(flag == "-i" || flag == "--include")
            {
                std::string arg(argv[index++]);

                if(arg == "--add")
                {
                    arg = argv[index++];
                    pic_config.include.push_back(arg);
                }
                else if(arg == "--remove")
                {
                    arg = argv[index++];
                    if(arg == "--back") pic_config.include.pop_back();
                    else if(arg == "--front") pic_config.include.erase(pic_config.include.begin());
                    else RemovePath(pic_config.include, arg);
                }
                else FlagNotExistent(arg);
            }
            else if(flag == "-b" || flag == "--binary")
            {
                std::string arg(argv[index++]);

                if(arg == "--add")
                {
                    arg = argv[index++];
                    pic_config.binary.push_back(arg);
                }
                else if(arg == "--remove")
                {
                    arg = argv[index++];
                    if(arg == "--back") pic_config.binary.pop_back();
                    else if(arg == "--front") pic_config.binary.erase(pic_config.binary.begin());
                    else RemovePath(pic_config.binary, arg);
                }
                else FlagNotExistent(arg);
            }
            else if(flag == "-df" || flag == "--debug_flags")
            {
                std::string arg(argv[index++]);

                if(arg == "--set")
                {
                    arg = argv[index++];
                    pic_config.flags[Debug] = arg;
                }
                else if(arg == "--clear")
                {
                    pic_config.flags[Debug] = "";
                }
                else FlagNotExistent(arg);
            }
            else if(flag == "-rf" || flag == "--release_flags")
            {
                std::string arg(argv[index++]);

                if(arg == "--set")
                {
                    arg = argv[index++];
                    pic_config.flags[Release] = arg;
                }
                else if(arg == "--clear")
                {
                    pic_config.flags[Release] = "";
                }
                else FlagNotExistent(arg);
            }
            else if(flag == "--vscode")
            {
                std::string arg(argv[index++]);

                if(arg == "--true")
                {
                    pic_config.use_vscode = true;
                }
                else if(arg == "--false")
                {
                    pic_config.use_vscode = false;
                }
                else FlagNotExistent(arg);
            }
            else if(flag == "--audio")
            {
                std::string arg(argv[index++]);

                if(arg == "--true")
                {
                    pic_config.use_audio = true;
                }
                else if(arg == "--false")
                {
                    pic_config.use_audio = false;
                }
                else FlagNotExistent(arg);
            }
            else if(flag == "--network")
            {
                std::string arg(argv[index++]);

                if(arg == "--true")
                {
                    pic_config.use_network = true;
                }
                else if(arg == "--false")
                {
                    pic_config.use_network = false;
                }
                else FlagNotExistent(arg);
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
        if(!BuildOption(build_type)) return 1;
        break;
    case Create:
        if(!CreateOption(pic_config)) return 1;
        break;
    case Set:
        if(!SetOption(pic_config)) return 1;
        break;
    default:
        break;
    }

    return 0;
}