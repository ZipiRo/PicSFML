#include <Debug.hpp>
#include <Settings.hpp>

#include <nlohmann/json.hpp>

#include <string>
#include <fstream>
#include <filesystem>
#include <iostream>

using json = nlohmann::json;

bool PicSFML::Settings::Check() const
{
    Debug::Log("[Settings::Check] Checking project directory path: " + projects_directory);

    if (!std::filesystem::exists(projects_directory))
    {
        Debug::Error("[Settings::Check] Projects directory path does not exist: " + projects_directory);
        return false;
    }

    Debug::Log("[Settings::Check] Settings are valid");

    return true;
}

bool PicSFML::Settings::Load(const std::string &path)
{
    const std::string settings_path = path + "\\" + SettingsName;

    Debug::Log("[Settings::Load] Loading settings from: " + settings_path);

    std::ifstream file(settings_path);

    if (!file.is_open())
    {
        Debug::Error("[Settings::Load] Could not open settings file: " + settings_path);
        return false;
    }

    json j;

    try
    {
        Debug::DebugLog("[Settings::Load] Parsing settings...");

        file >> j;
        
        projects_directory = j["projects_directory"];
        debug_mode = j["debug_mode"];
    }
    catch (const std::exception &e)
    {
        Debug::Error("[Settings::Load] Failed to parse settings: " + std::string(e.what()));
        return false;
    }

    Debug::Log("[Settings::Load] Settings loaded successfully");

    return true;
}

bool PicSFML::Settings::Save(const std::string &path)
{
    const std::string settings_path = path + "\\" + SettingsName;

    Debug::Log("[Settings::Save] Saving settings to: " + settings_path);

    json j;

    j["projects_directory"] = projects_directory;
    j["debug_mode"] = debug_mode;

    std::ofstream file(settings_path);

    if (!file.is_open())
    {
        Debug::Error("[Settings::Save] Could not open settings file for writing: " + settings_path);
        return false;
    }

    file << j.dump(4);

    if (!file.good())
    {
        Debug::Error("[Settings::Save] Failed to write settings file");
        return false;
    }

    Debug::Log("[Settings::Save] Settings saved successfully");

    return true;
}

bool PicSFML::Settings::Set(const std::string &field, const std::string &value)
{
    Debug::DebugLog("[Settings::Set] Field: " + field + ", Value: " + value);

    if (field == "projects_directory") projects_directory = value;
    else if(field == "debug_mode") debug_mode = value;
    else
    {
        Debug::Error("[Settings::Set] Unknown field: " + field);
        return false;
    }

    return true;
}

std::string PicSFML::Settings::Get(const std::string &field) const
{
    Debug::DebugLog("[Settings::Get] Field: " + field);

    if (field == "projects_directory")
    {
        Debug::DebugLog("[Settings::Get] Value: " + projects_directory);
        return projects_directory;
    }
    else if(field == "debug_mode")
    {
        Debug::DebugLog("[Settings::Get] Value: " + debug_mode);
        return debug_mode;
    }

    Debug::Error("[Settings::Get] Unknown field: " + field);

    return "";
}
