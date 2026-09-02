#include <Debug.hpp>
#include <ProjectConfig.hpp>

#include <nlohmann/json.hpp>

#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <algorithm>

using json = nlohmann::json;

bool PicSFML::ProjectConfig::Check() const
{
    Debug::Log("[ProjectConfig::Check] Checking compiler directory path: " + compiler_dir_path);

    if (!std::filesystem::exists(compiler_dir_path))
    {
        Debug::Error("[ProjectConfig::Check] Compiler path does not exist: " + compiler_dir_path);
        return false;
    }

    Debug::Log("[ProjectConfig::Check] Checking SFML directory path: " + sfml_dir_path);

    if(!std::filesystem::exists(sfml_dir_path))
    {
        Debug::Error("[ProjectConfig::Check] SFML path does not exist: " + sfml_dir_path);
        return false;
    }

    Debug::Log("[ProjectConfig::Check] Checking SFML version: " + sfml_version);

    if(std::find(SFMLValidVersions.begin(), SFMLValidVersions.end(), sfml_version) == SFMLValidVersions.end())
    {
        Debug::Error("[ProjectConfig::Check] Unsupported SFML version: " + sfml_version);
        return false;
    }

    for (const auto &directory : include_directories)
    {
        if (!std::filesystem::exists(directory))
        {
            Debug::Error("[ProjectConfig::Check] Include directory does not exist: " + directory);
            return false;
        }
    }

    for (const auto &file : library_files)
    {
        if (!std::filesystem::exists(file) && file.find("-l") == std::string::npos)
        {
            Debug::Error("[ProjectConfig::Check] Library file does not exist: " + file);
            return false;
        }
    }

    for (const auto &file : binary_files)
    {
        if (!std::filesystem::exists(file))
        {
            Debug::Error("[ProjectConfig::Check] Binary file does not exist: " + file);
            return false;
        }
    }

    Debug::Log("[ProjectConfig::Check] Configuration is valid");

    return true;
}

bool PicSFML::ProjectConfig::Save(const std::string &path)
{
    Debug::Log("[ProjectConfig::Save] Saving config to: " + path + "\\" + ProjectConfigName);

    json j;

    j["project_name"] = project_name;
    j["description"] = description;
    j["output_name"] = output_name;
    j["compiler_dir_path"] = compiler_dir_path;
    j["sfml_dir_path"] = sfml_dir_path;
    j["sfml_version"] = sfml_version;
    j["picsfml_version"] = picsfml_version;
    j["create_vscode_files"] = create_vscode_files;
    j["use_sfml_audio"] = use_sfml_audio;
    j["use_sfml_network"] = use_sfml_network;
    j["app_version"] = app_version;
    j["debug_flags"] = debug_flags;
    j["release_flags"] = release_flags;
    j["date_created"] = date_created;
    j["include_directories"] = include_directories;
    j["library_files"] = library_files;
    j["binary_files"] = binary_files;

    std::ofstream file(path + "\\" + ProjectConfigName);

    if (!file.is_open())
    {
        Debug::Error("[ProjectConfig::Save] Could not open config file for writing: " + path + "\\" + ProjectConfigName);
        return false;
    }

    file << j.dump(4);

    if (!file.good())
    {
        Debug::Error("[ProjectConfig::Save] Failed to write config file");
        return false;
    }

    Debug::Log("[ProjectConfig::Save] Config saved successfully");

    return true;
}

bool PicSFML::ProjectConfig::Load(const std::string &path)
{
    const std::string config_path = path + "\\" + ProjectConfigName;

    Debug::Log("[ProjectConfig::Load] Loading config from: " + config_path);

    std::ifstream file(config_path);

    if (!file.is_open())
    {
        Debug::Error("[ProjectConfig::Load] Could not open config file: " + config_path);
        return false;
    }

    json j;

    try
    {
        Debug::DebugLog("[ProjectConfig::Load] Parsing config...");

        file >> j;

        project_name = j["project_name"];
        description = j["description"];
        output_name = j["output_name"];
        compiler_dir_path = j["compiler_dir_path"];
        sfml_dir_path = j["sfml_dir_path"];
        sfml_version = j["sfml_version"];
        picsfml_version = j["picsfml_version"];
        create_vscode_files = j["create_vscode_files"];
        use_sfml_audio = j["use_sfml_audio"];
        use_sfml_network = j["use_sfml_network"];
        app_version = j["app_version"];
        debug_flags = j["debug_flags"];
        release_flags = j["release_flags"];
        date_created = j["date_created"];
        include_directories = j["include_directories"];
        library_files = j["library_files"];
        binary_files = j["binary_files"];
    }
    catch (const std::exception &e)
    {
        Debug::Error(
            "[ProjectConfig::Load] Failed to parse config: " +
            std::string(e.what())
        );

        return false;
    }

    Debug::Log("[ProjectConfig::Load] Config loaded successfully");

    return true;
}

bool PicSFML::ProjectConfig::Set(const std::string &field, const std::string &value)
{
    Debug::DebugLog("[ProjectConfig::Set] Field: " + field + ", Value: " + value);

    if (field == "project_name") project_name = value;
    else if (field == "output_name") output_name = value;
    else if (field == "description") description = value;
    else if (field == "compiler_dir_path") compiler_dir_path = value;
    else if (field == "sfml_dir_path") sfml_dir_path = value;
    else if (field == "sfml_version") sfml_version = value;
    else if (field == "app_version") app_version = value;
    else if (field == "picsfml_version") picsfml_version = value;
    else if (field == "debug_flags") debug_flags = value;
    else if (field == "release_flags") release_flags = value;
    else if (field == "date_created") date_created = value;
    else if (field == "create_vscode_files") create_vscode_files = (value == "true");
    else if (field == "use_sfml_audio") use_sfml_audio = (value == "true");
    else if (field == "use_sfml_network") use_sfml_network = (value == "true");
    else
    {
        Debug::Error("[ProjectConfig::Set] Unknown field: " + field);
        return false;
    }

    return true;
}

bool PicSFML::ProjectConfig::SetList(const std::string &field, const std::string &value, int index)
{
    if (index < 0)
    {
        Debug::DebugLog("[ProjectConfig::SetList] Index is out of bounds: " + std::to_string(index));
        return false;
    }

    Debug::DebugLog("[ProjectConfig::SetList] Field: " + field + ", Index: " + std::to_string(index) + ", Value: " + value);

    if (field == "include_directories")
    {
        if (index >= static_cast<int>(include_directories.size()))
        {
            Debug::DebugLog("[ProjectConfig::SetList] Index is out of bounds");
            return false;
        }

        include_directories[index] = value;
    }
    else if (field == "library_files")
    {
        if (index >= static_cast<int>(library_files.size()))
        {
            Debug::DebugLog("[ProjectConfig::SetList] Index is out of bounds");
            return false;
        }

        library_files[index] = value;
    }
    else if (field == "binary_files")
    {
        if (index >= static_cast<int>(binary_files.size()))
        {
            Debug::DebugLog("[ProjectConfig::SetList] Index is out of bounds");
            return false;
        }

        binary_files[index] = value;
    }
    else
    {
        Debug::Error("[ProjectConfig::SetList] Unknown field: " + field);
        return false;
    }

    return true;
}

std::string PicSFML::ProjectConfig::Get(const std::string &field) const
{
    Debug::DebugLog("[ProjectConfig::Get] Field: " + field);

    if (field == "project_name")
    {
        Debug::DebugLog("[ProjectConfig::Get] Value: " + project_name);
        return project_name;
    }
    else if (field == "output_name")
    {
        Debug::DebugLog("[ProjectConfig::Get] Value: " + output_name);
        return output_name;
    }
    else if (field == "description")
    {
        Debug::DebugLog("[ProjectConfig::Get] Value: " + description);
        return description;
    }
    else if (field == "compiler_dir_path")
    {
        Debug::DebugLog("[ProjectConfig::Get] Value: " + compiler_dir_path);
        return compiler_dir_path;
    }
    else if (field == "sfml_dir_path")
    {
        Debug::DebugLog("[ProjectConfig::Get] Value: " + sfml_dir_path);
        return sfml_dir_path;
    }
    else if (field == "sfml_version")
    {
        Debug::DebugLog("[ProjectConfig::Get] Value: " + sfml_version);
        return sfml_version;
    }
    else if (field == "app_version")
    {
        Debug::DebugLog("[ProjectConfig::Get] Value: " + app_version);
        return app_version;
    }
    else if (field == "picsfml_version")
    {
        Debug::DebugLog("[ProjectConfig::Get] Value: " + picsfml_version);
        return picsfml_version;
    }
    else if (field == "debug_flags")
    {
        Debug::DebugLog("[ProjectConfig::Get] Value: " + debug_flags);
        return debug_flags;
    }
    else if (field == "release_flags")
    {
        Debug::DebugLog("[ProjectConfig::Get] Value: " + release_flags);
        return release_flags;
    }
    else if (field == "date_created")
    {
        Debug::DebugLog("[ProjectConfig::Get] Value: " + date_created);
        return date_created;
    }
    else if (field == "create_vscode_files")
    {
        const std::string value = create_vscode_files ? "true" : "false";

        Debug::DebugLog("[ProjectConfig::Get] Value: " + value);

        return value;
    }
    else if (field == "use_sfml_audio")
    {
        const std::string value = use_sfml_audio ? "true" : "false";

        Debug::DebugLog("[ProjectConfig::Get] Value: " + value);

        return value;
    }
    else if (field == "use_sfml_network")
    {
        const std::string value = use_sfml_network ? "true" : "false";

        Debug::DebugLog("[ProjectConfig::Get] Value: " + value);

        return value;
    }

    Debug::Error("[ProjectConfig::Get] Unknown field: " + field);

    return "";
}

std::string PicSFML::ProjectConfig::GetList(const std::string &field, int index) const
{
    if (index < 0)
    {
        Debug::DebugLog("[ProjectConfig::GetList] Index is out of bounds: " + std::to_string(index));
        return "";
    }

    Debug::DebugLog("[ProjectConfig::GetList] Field: " + field +
                    ", Index: " + std::to_string(index));

    if (field == "include_directories")
    {
        if (index >= static_cast<int>(include_directories.size()))
        {
            Debug::DebugLog("[ProjectConfig::GetList] Index is out of bounds");
            return "";
        }

        Debug::DebugLog("[ProjectConfig::GetList] Value: " + include_directories[index]);
        return include_directories[index];
    }
    else if (field == "library_files")
    {
        if (index >= static_cast<int>(library_files.size()))
        {
            Debug::DebugLog("[ProjectConfig::GetList] Index is out of bounds");
            return "";
        }

        Debug::DebugLog("[ProjectConfig::GetList] Value: " + library_files[index]);
        return library_files[index];
    }
    else if (field == "binary_files")
    {
        if (index >= static_cast<int>(binary_files.size()))
        {
            Debug::DebugLog("[ProjectConfig::GetList] Index is out of bounds");
            return "";
        }

        Debug::DebugLog("[ProjectConfig::GetList] Value: " + binary_files[index]);
        return binary_files[index];
    }

    Debug::Error("[ProjectConfig::GetList] Unknown field: " + field);

    return "";
}

bool PicSFML::ProjectConfig::Add(const std::string &field, const std::string &value)
{
    Debug::DebugLog("[ProjectConfig::Add] Field: " + field + ", Value: " + value);

    if (field == "include_directories")
        include_directories.push_back(value);
    else if (field == "library_files")
        library_files.push_back(value);
    else if (field == "binary_files")
        binary_files.push_back(value);
    else
    {
        Debug::Error("[ProjectConfig::Add] Unknown field: " + field);
        return false;
    }

    return true;
}

bool PicSFML::ProjectConfig::Remove(const std::string &field, const std::string &value)
{
    Debug::DebugLog("[ProjectConfig::Remove] Field: " + field + ", Value: " + value);

    std::vector<std::string> *list_using;

    if (field == "include_directories")
        list_using = &include_directories;
    else if (field == "library_files")
        list_using = &library_files;
    else if (field == "binary_files")
        list_using = &binary_files;
    else
    {
        Debug::Error("[ProjectConfig::Remove] Unknown field: " + field);
        return false;
    }

    for (int i = 0; i < static_cast<int>(list_using->size()); i++)
    {
        if ((*list_using)[i] == value)
        {
            Debug::DebugLog("[ProjectConfig::Remove] Removing item at index: " + std::to_string(i));
            list_using->erase(list_using->begin() + i);
            return true;
        }
    }

    Debug::Error("[ProjectConfig::Remove] Value not found: " + value);

    return false;
}