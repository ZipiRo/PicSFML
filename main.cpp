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

#include "build_option.h"
#include "create_option.h"

std::filesystem::path GetLocalPath()
{
    char buffer[256];
    Win32::GetModuleFileNameA(NULL, buffer, 256);
    return std::filesystem::path(buffer);
}

int main(int argc, char** argv)
{ 
    if(std::string(argv[1]) == "-b")
    {
        if(argc <= 3) return 1; 

        BuildType build_type; 
        std::filesystem::path project_path;
        std::filesystem::path picsfml_path = GetLocalPath().parent_path();

        if(std::string(argv[2]) == "-r") build_type = Release;
        else if(std::string(argv[2]) == "-d") build_type = Debug;
        else return 1;
        
        project_path = argv[3];

        if(!BuildOption(picsfml_path, project_path, build_type))
            return 1;
    }
    else if(std::string(argv[1]) == "-c")
    {
        if(argc <= 2) return 1;

        CreateProjectConfig create_config;
        create_config.picsfml_path = GetLocalPath().parent_path();

        std::cout << "PicSFML local path: " << create_config.picsfml_path.string() << '\n';

        create_config.project_path = argv[2];

        if(argc > 3)
        {
            int index = 3;
            while (index < argc)
            {
                std::string flag(argv[index++]);

                if(flag == "-n")
                {
                    std::string args(argv[index++]);
                    create_config.name = args;
                }
                else if(flag == "-o")
                {
                    std::string args(argv[index++]);
                    create_config.output = args;
                }
                else if(flag == "-g")
                {
                    std::string args(argv[index++]);
                    create_config.gcc_path = args;
                }
                else if(flag == "-s")
                {
                    std::string args(argv[index++]);
                    create_config.sfml_path = args;
                }
                else if(flag == "-vs")
                {
                    create_config.use_vscode = true;
                }
            }
        }

        if(!CreateOption(create_config))
            return 1;
    }
    
    return 0;
}