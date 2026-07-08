#include <vector>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <filesystem>

#include <Version.h>
#include <ToolResult.h>

#include <ProjectConfig.h>
#include <ToolContext.h>
#include <Builder.h>

using namespace PicSFML;
    
std::string GetBuildDirectory(BuildType type)
{
    return type == Release ? "Release" : "Debug";
}

bool CreateIcon(const ToolContext &context, ToolResult &result)
{
    if(!std::filesystem::exists(context.project_path + "/icon.png")) 
    {
        result.log += "The Icon File Was Not Found\n";
        result.errors.push_back("There is no icon.png in your project");
        return false;
    }

    std::string command;

    command += context.tool_path + "/pngtoico.exe ";
    command += "--input " + context.project_path + "/icon.png ";
    command += "--output " + context.project_path + "/icon.ico";
    
    if(system(command.c_str()) != 0)
    {
        result.log += "Had Some Problems Creating The Icon\n";
        result.errors.push_back("Command: " + command);
        return false;
    }

    result.log += "Icon Was Created Successfuly\n";
    return true;
}

bool CreateWindowsResource(const ToolContext &context, ToolResult &result)
{
    std::string resource;

    resource += "#include <windows.h>\n\n";
    resource += "IDI_ICON1 ICON \"icon.ico\"\n";
    resource += "VS_VERSION_INFO VERSIONINFO\n";
    resource += " FILEVERSION ";
    resource += context.config.app_version.AsString(',') + "\n";
    resource += " PRODUCTVERSION ";
    resource += context.config.app_version.AsString(',') + "\n" ;
    resource += " FILEOS VOS_NT_WINDOWS32\n";
    resource += " FILETYPE VFT_APP\n";
    resource += "BEGIN\n";
    resource += "    BLOCK \"StringFileInfo\"\n";
    resource += "    BEGIN\n";
    resource += "        BLOCK \"040904B0\"\n";
    resource += "        BEGIN\n";
    resource += "            VALUE \"ProductName\", \"" ;
    resource += context.config.output + "\"\n";
    resource += "            VALUE \"FileVersion\", \"" ;
    resource += context.config.app_version.AsString('.') + "\"\n";
    resource += "            VALUE \"ProductVersion\", \"" ;
    resource += context.config.app_version.AsString('.') + "\"\n";
    resource += "            VALUE \"FileDescription\", \"PicSFML Application\"\n";
    resource += "        END\n";
    resource += "    END\n";
    resource += "    BLOCK \"VarFileInfo\"\n";
    resource += "    BEGIN\n";
    resource += "        VALUE \"Translation\", 0x0409, 1200\n";
    resource += "    END\n";
    resource += "END\n";

    std::ofstream file(context.project_path + "/resource.rc");
    if(!file.is_open())
    {

        result.log += "Had Some Problems Creating The Windows Resource File\n";
        result.errors.push_back("Resource: \n" + resource);
        return false;
    }

    file << resource;

    return file.good();
}
bool Compile(const ToolContext &context, ToolResult &result)
{
    std::string command;

    command += context.config.compiler + "/bin/g++.exe -c ";
    command += context.project_path + "/" + context.config.main + " ";

    for(const auto &source : context.config.source)
        command += context.project_path + "/" + source + " ";

    command += "-I" + context.config.sfml + "/include  ";
    command += "-I" + context.project_path + "/Include ";
    
    if(context.config.sfml_version.AsInt() == 0)
    {
        result.log = "Had some problems compiling the project\n";
        result.errors.push_back("There is no 'sfml_version' set for this project");
        return false;
    }
    command += "-I" + context.tool_path + "/SFML/" + context.config.sfml_version.AsString('.') + " ";

    for(const auto &path : context.config.include)
        command += "-I" + path + " ";

    if(std::system(command.c_str()) != 0)
    {
        result.log = "Had some problems compiling the project\n";
        result.errors.push_back("Command: " + command);
        return false;
    }

    result.log += "Project Compilation Successful\n";
    return true;
}

bool Link(const ToolContext &context, BuildType type, ToolResult &result)
{
    std::string sfml_libraries[5] = {
        "sfml-system",
        "sfml-window",
        "sfml-graphics",
        "sfml-audio",
        "sfml-network"
    };

    std::filesystem::path build_directory_path = context.project_path + "/Build/" +  GetBuildDirectory(type);
    
    std::filesystem::remove_all(build_directory_path);
    std::filesystem::create_directories(build_directory_path);

    std::string command;

    command += context.config.compiler + "/bin/g++.exe ";

    if(!context.config.flags[type].empty())
        command += context.config.flags[type] + " ";

    if(context.config.audio)    
        command += "-DAUDIO ";

    if(context.config.network)    
        command += "-DNETWORK ";

    std::filesystem::path main_path(context.project_path + "/" + context.config.main);
    command += main_path.stem().string() + ".o ";

    for(const auto &source : context.config.source)
    {
        std::filesystem::path source_path(source);
        command += source_path.stem().string() + ".o ";
    }

    command += context.project_path + "/resource.rs ";

    command += "-o " + context.project_path + "/Build/" + GetBuildDirectory(type) + "/" + context.config.output + " ";

    for(const auto &library : context.config.library)
        command += library + " ";

    command += "-L" + context.config.sfml + "/lib ^ "; 

    command += "-l" + sfml_libraries[0] + ((type == Debug) ? "-d " : " ");
    command += "-l" + sfml_libraries[1] + ((type == Debug) ? "-d " : " ");
    command += "-l" + sfml_libraries[2] + ((type == Debug) ? "-d " : " ");

    if(context.config.audio)
        command += "-l" + sfml_libraries[3] + ((type == Debug) ? "-d " : " ");
    if(context.config.network)
        command += "-l" + sfml_libraries[4] + ((type == Debug) ? "-d " : " ");

    if(type == Release) command += "-mwindows";

    if(std::system(command.c_str()) != 0)
    {
        result.log = "Had some problems linking & building the project\n";
        result.errors.push_back("Command: " + command);
        return false;
    }

    result.log += "Project Link & Build Successful\n";
    return true;
}

bool CopyDLLs(const ToolContext &context, BuildType type, ToolResult &result)
{
    std::string sfml_binaries[5] = {
        "sfml-system",
        "sfml-window",
        "sfml-graphics",
        "sfml-audio",
        "sfml-network"
    };

    std::string compiler_binaries[3] = {
        "libstdc++-6.dll",
        "libwinpthread-1.dll",
        "libgcc_s_seh-1.dll"
    };
    
    std::filesystem::path build_directory_path = context.project_path + "/Build/" +  GetBuildDirectory(type);

    try
    {
        for(int i = 0; i < 5; i++)
        {
            if(i == 3 && !context.config.audio) continue;
            if(i == 4 && !context.config.network) continue; 

            std::string binary = sfml_binaries[i] + ((type == Debug) ? "-d-" : "-") + std::to_string(context.config.sfml_version[0]) + ".dll"; 

            std::filesystem::copy(context.config.sfml + "/bin/" + binary, build_directory_path / binary);
        }

        for(int i = 0; i < 3; i++)
        {
            std::string binary = compiler_binaries[i];
            std::filesystem::copy(context.config.compiler + "/bin/" + binary, build_directory_path / binary);
        }

        for(const auto &binary : context.config.binary)
        {
            std::filesystem::path binary_path(binary);
            std::filesystem::copy(binary, build_directory_path / binary_path.filename());
        }   
    }
    catch(const std::exception& e)
    {
        result.errors.push_back(e.what());
        return false;
    }
    
    result.log += "Project DLLs Successfuly Copyed\n";
    return true;
}

bool CopyResources(const ToolContext &context, BuildType type, ToolResult &result)
{
    std::filesystem::path build_directory_path = context.project_path + "/Build/" +  GetBuildDirectory(type);

    try
    {
        std::filesystem::copy(context.project_path + "/Resources", build_directory_path / "Resources", std::filesystem::copy_options::recursive);
        std::filesystem::copy(context.project_path + "/icon.png", build_directory_path / "icon.png");
    }
    catch(const std::exception& e)
    {
        result.errors.push_back(e.what());
        return false;
    }

    result.log += "Project Resources Successfuly Copyed\n";
    return true;
}

bool CompileWindowsResources(const ToolContext &context, ToolResult &result)
{
    if(!CreateIcon(context, result)) return false;
    if(!CreateWindowsResource(context, result)) return false;

    std::string command;

    command += "windres ";
    command += context.project_path + "/resource.rc ";
    command += "-O coff -o ";
    command += context.project_path + "/resource.rs";

    if(system(command.c_str()) != 0)
    {
        result.log += "Had some problems compiling windows resources\n";
        result.errors.push_back("Command: " + command);
        return false;
    }

    result.log += "Windows Resource Created & Compiled Successfuly\n";
    return true;
}

ToolResult PicSFML::Build(const ToolContext &context, BuildType type)
{
    ToolResult result;

    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

    if(!CheckProjectConfigPaths(context.config, context.project_path, result))
    {
        result.success = false;
        result.log += "There where file paths that don't exist\n";
        result.buildTimeS = std::chrono::duration<double>(std::chrono::system_clock::now() - start).count();
        return result;          
    }

    if(!CompileWindowsResources(context, result))
    {
        result.success = false;
        result.log += "Windows resources compilation failed\n";
        result.buildTimeS = std::chrono::duration<double>(std::chrono::system_clock::now() - start).count();
        return result;      
    }

    if(!Compile(context, result))
    {
        result.success = false;
        result.log += "Compilation failed\n";
        result.buildTimeS = std::chrono::duration<double>(std::chrono::system_clock::now() - start).count();
        return result;
    }

    if(!Link(context, type, result))
    {
        result.success = false;
        result.log += "Linking failed\n";
        result.buildTimeS = std::chrono::duration<double>(std::chrono::system_clock::now() - start).count();
        return result;
    }
    
    if(!CopyDLLs(context, type, result))
    {
        result.success = false;
        result.log += "DLL copying failed\n";
        result.buildTimeS = std::chrono::duration<double>(std::chrono::system_clock::now() - start).count();
        return result;
    }
    
    if(!CopyResources(context, type, result))
    {
        result.success = false;
        result.log += "Resources copy failed\n";
        result.buildTimeS = std::chrono::duration<double>(std::chrono::system_clock::now() - start).count();
        return result;
    }

    result.success = true;
    result.log += "Project Build Completed\n";
    result.buildTimeS = std::chrono::duration<double>(std::chrono::system_clock::now() - start).count();
    return result;
}