#include <vector>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <filesystem>

#include <ToolResult.h>
#include <Version.h>

#include <ProjectConfig.h>
#include <ToolContext.h>
#include <Creator.h>

using namespace PicSFML;

bool CheckPicSFMLProject(const std::string project_path)
{
    return std::filesystem::exists(project_path + "/" + PicConfigFileName);
}

bool CreateProjectDirectory(const ToolContext &context, ToolResult &result)
{
    std::filesystem::path project_dir = context.project_path + "/" + context.config.name;
    
    try
    {
        std::filesystem::create_directory(project_dir);
    }
    catch(const std::exception& e)
    {
        result.errors.push_back(e.what());
        return false;
    }
    
    result.log += "Project Directory created successfuly: " + project_dir.string() + "\n";

    return true;
}

bool CopyTemplate(const ToolContext &context, ToolResult &result)
{
    std::filesystem::path template_path = context.tool_path + "/Template";
    std::filesystem::path project_dir = context.project_path + "/" + context.config.name;
    
    try
    {
        for(const auto &entry : std::filesystem::directory_iterator(template_path))
            std::filesystem::copy(entry, project_dir / entry.path().filename());
    }
    catch(const std::exception& e)
    {
        result.errors.push_back(e.what());
        return false;
    }

    result.log += "Project Template copyed sucessfuly to: " + project_dir.string() + "\n";

    return true;
}

bool CreateProjectConfig(const ToolContext &context, ToolResult &result)
{
    std::string project_dir = context.project_path + "/" + context.config.name;

    if(!SaveProjectConfig(context.config, project_dir + "/" + PicConfigFileName))
    {
        result.errors.push_back("There where problems when saving the picconfig file");
        return false;
    }

    result.log += "ProjectConfig file created successfuly\n";

    if(context.config.vscode && !CreateVSCodeProperties(context.config, context.tool_path, project_dir))
    {
        result.errors.push_back("There where problems when creating the VSCode Properties file");
        return false;
    }
    else 
    {
        result.log += "VSCode Properties file created sucessfuly\n";
    }

    return true;
}

ToolResult PicSFML::Create(const ToolContext &context)
{
    ToolResult result;

    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

    if(CheckPicSFMLProject(context.project_path))
    {
        result.success = false;
        result.log += "This Directory Allready Contains A PicSFML Project\n";
        result.errors.push_back("There is a project at the given path: " + std::filesystem::absolute(context.project_path).string());
        result.buildTimeS = std::chrono::duration<double>(std::chrono::system_clock::now() - start).count();
        return result;
    }

    if(!CreateProjectDirectory(context, result))
    {
        result.success = false;
        result.log += "There Was A Problem When Creating The Project Directory\n";
        result.buildTimeS = std::chrono::duration<double>(std::chrono::system_clock::now() - start).count();
        return result;
    }

    if(!CopyTemplate(context, result))
    {
        result.success = false;
        result.log += "There Was A Problem When Copying The Project Template\n";
        result.buildTimeS = std::chrono::duration<double>(std::chrono::system_clock::now() - start).count();
        return result;
    }

    if(!CreateProjectConfig(context, result))
    {
        result.success = false;
        result.log += "There Was A Problem Creating The ProjectConfig File\n";
        result.buildTimeS = std::chrono::duration<double>(std::chrono::system_clock::now() - start).count();
        return result;
    }

    result.success = true;
    result.log += "Project Created\n";
    result.buildTimeS = std::chrono::duration<double>(std::chrono::system_clock::now() - start).count();
    return result;
}