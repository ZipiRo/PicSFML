#include <Debug.hpp>
#include <Project.hpp>
#include <ProjectConfig.hpp>

#include <string>
#include <filesystem>
#include <iostream>

bool PicSFML::Project::Load(const std::string &from)
{
    Debug::Log("[Project::Load] Loading project from: " + from);

    if (!std::filesystem::exists(from))
    {
        Debug::Error("[Project::Load] Project directory does not exist: " + from);
        return false;
    }

    if (!std::filesystem::is_directory(from))
    {
        Debug::Error("[Project::Load] Project path is not a directory: " + from);
        return false;
    }

    const std::string config_path = from + "\\" + ProjectConfigName;

    if (!std::filesystem::exists(config_path))
    {
        Debug::Error("[Project::Load] Project config does not exist: " + config_path);
        return false;
    }

    Debug::DebugLog("[Project::Load] Loading config: " + config_path);

    if (!project_config.Load(from))
    {
        Debug::Error("[Project::Load] Failed to load project config");
        return false;
    }

    project_path = from;

    Debug::Log("[Project::Load] Project loaded successfully");

    return true;
}

std::string PicSFML::Project::GetPath() const
{
    Debug::DebugLog("[Project::GetPath] Project path: " + project_path);

    return project_path;
}

PicSFML::ProjectConfig &PicSFML::Project::GetConfig()
{
    Debug::DebugLog("[Project::GetConfig] Returning project config");

    return project_config;
}

const PicSFML::ProjectConfig &PicSFML::Project::GetConfig() const
{
    Debug::DebugLog("[Project::GetConfig] Returning const project config");

    return project_config;
}

bool PicSFML::Project::SaveConfig()
{
    Debug::Log("[Project::SaveConfig] Saving project configuration...");

    if (!project_config.Check())
    {
        Debug::Error("[Project::SaveConfig] Project configuration is not valid");
        return false;
    }

    if (!project_config.Save(project_path))
    {
        Debug::Error("[Project::SaveConfig] Project configuration could not be saved");
        return false;
    }

    Debug::Log("[Project::SaveConfig] Project configuration successfully saved");

    return true;
}
