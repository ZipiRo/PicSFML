#include <Debug.hpp>
#include <ConfigDefaults.hpp>

#include <nlohmann/json.hpp>

#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <algorithm>

using json = nlohmann::json;

bool PicSFML::ConfigDefaults::Check() const
{
    Debug::Log("[ConfigDefaults::Check] Checking compiler path: " + compiler_dir_path);

    if (!std::filesystem::exists(compiler_dir_path))
    {
        Debug::Error("[ConfigDefaults::Check] Compiler path does not exist: " + compiler_dir_path);
        return false;
    }

    Debug::Log("[ConfigDefaults::Check] Checking SFML directory path: " + sfml_dir_path);

    if(!std::filesystem::exists(sfml_dir_path))
    {
        Debug::Error("[ConfigDefaults::Check] SFML path does not exist: " + sfml_dir_path);
        return false;
    }

    Debug::Log("[ConfigDefaults::Check] Checking SFML version: " + sfml_version);

    if(std::find(SFMLValidVersions.begin(), SFMLValidVersions.end(), sfml_version) == SFMLValidVersions.end())
    {
        Debug::Error("[ConfigDefaults::Check] Unsupported SFML version: " + sfml_version);
        return false;
    }

    Debug::Log("[ConfigDefaults::Check] Configuration is valid");

    return true;
}

bool PicSFML::ConfigDefaults::Save(const std::string &path)
{
    Debug::Log("[ConfigDefaults::Save] Saving defaults to: " + path + "\\" + DefaultsName);

    json j;

    j["output_name"] = output_name;
    j["compiler_dir_path"] = compiler_dir_path;
    j["sfml_dir_path"] = sfml_dir_path;
    j["sfml_version"] = sfml_version;
    j["create_vscode_files"] = create_vscode_files;

    std::ofstream file(path + "\\" + DefaultsName);

    if (!file.is_open())
    {
        Debug::Error("[ConfigDefaults::Save] Could not open defaults file for writing");
        return false;
    }

    file << j.dump(4);

    if (!file.good())
    {
        Debug::Error("[ConfigDefaults::Save] Failed to write defaults file");
        return false;
    }

    Debug::Log("[ConfigDefaults::Save] Defaults saved successfully");

    return true;
}

bool PicSFML::ConfigDefaults::Load(const std::string &path)
{
    Debug::Log("[ConfigDefaults::Load] Loading defaults from: " + path + "\\" + DefaultsName);

    std::ifstream file(path + "\\" + DefaultsName);

    if (!file.is_open())
    {
        Debug::Error("[ConfigDefaults::Load] Could not open defaults file");
        return false;
    }

    json j;

    try
    {
        Debug::DebugLog("[ConfigDefaults::Load] Parsing defaults...");

        file >> j;

        output_name = j["output_name"];
        compiler_dir_path = j["compiler_dir_path"];
        sfml_dir_path = j["sfml_dir_path"];
        sfml_version = j["sfml_version"];
        create_vscode_files = j["create_vscode_files"];
    }
    catch (const std::exception &e)
    {
        Debug::Error("[ConfigDefaults::Load] Failed to parse defaults: " + std::string(e.what()));
        return false;
    }

    Debug::Log("[ConfigDefaults::Load] Defaults loaded successfully");

    return true;
}

bool PicSFML::ConfigDefaults::Set(const std::string &field, const std::string &value)
{
    Debug::DebugLog("[ConfigDefaults::Set] Field: " + field + ", Value: " + value);

    if (field == "output_name") output_name = value;
    else if (field == "compiler_dir_path") compiler_dir_path = value;
    else if (field == "sfml_dir_path") sfml_dir_path = value;
    else if (field == "sfml_version") sfml_version = value;
    else if (field == "create_vscode_files") create_vscode_files = (value == "true");
    else
    {
        Debug::Error("[ConfigDefaults::Set] Unknown field: " + field);
        return false;
    }

    return true;
}

std::string PicSFML::ConfigDefaults::Get(const std::string &field) const
{
    Debug::DebugLog("[ConfigDefaults::Get] Field: " + field);

    if (field == "output_name")
    {
        Debug::DebugLog("[ConfigDefaults::Get] Value: " + output_name);
        return output_name;
    }
    else if (field == "compiler_dir_path")
    {
        Debug::DebugLog("[ConfigDefaults::Get] Value: " + compiler_dir_path);
        return compiler_dir_path;
    }
    else if (field == "sfml_dir_path")
    {
        Debug::DebugLog("[ConfigDefaults::Get] Value: " + sfml_dir_path);
        return sfml_dir_path;
    }
    else if (field == "sfml_version")
    {
        Debug::DebugLog("[ConfigDefaults::Get] Value: " + sfml_version);
        return sfml_version;
    }
    else if (field == "create_vscode_files")
    {
        const std::string value = create_vscode_files ? "true" : "false";

        Debug::DebugLog("[ConfigDefaults::Get] Value: " + value);

        return value;
    }

    Debug::Error("[ConfigDefaults::Get] Unknown field: " + field);

    return "";
}
