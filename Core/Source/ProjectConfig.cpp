#include <fstream>
#include <string>
#include <nlohmann/json.hpp>

#include <Version.h>
#include <ToolResult.h>

#include <ProjectConfig.h>

using json = nlohmann::json;
using namespace PicSFML;

bool PicSFML::CheckProjectConfigPaths(const ProjectConfig &config, const std::string &project_path, ToolResult &result)
{
    bool all_good = true;

    if (!std::filesystem::exists(std::filesystem::absolute(config.compiler)))
    {
        result.errors.push_back("Compiler path does not exist: " + std::filesystem::absolute(config.compiler).string());
        all_good = false;
    }

    if (!std::filesystem::exists(std::filesystem::absolute(config.sfml)))
    {
        result.errors.push_back("SFML path does not exist: " + std::filesystem::absolute(config.sfml).string());
        all_good = false;
    }
    
    if (!std::filesystem::exists(std::filesystem::absolute(project_path + "/" + config.main)))
    {
        result.errors.push_back("Main path does not exist: " + std::filesystem::absolute(project_path + "/" + config.main).string());
        all_good = false;
    }

    for (const auto &source : config.source)
    {
        if (!std::filesystem::exists(std::filesystem::absolute(source)))
        {
            result.errors.push_back("Source path does not exist: " + std::filesystem::absolute(source).string());
            all_good = false;
        }
    }

    for (const auto &library : config.library)
    {
        if (library.rfind("-l", 0) == 0)
            continue;

        if (!std::filesystem::exists(std::filesystem::absolute(library)))
        {
            result.errors.push_back("Library path does not exist: " + std::filesystem::absolute(library).string());
            all_good = false;
        }
    }

    for (const auto &include : config.include)
    {
        if (!std::filesystem::exists(std::filesystem::absolute(include)))
        {
            result.errors.push_back("Include path does not exist: " + std::filesystem::absolute(include).string());
            all_good = false;
        }
    }

    for (const auto &binary : config.binary)
    {
        if (!std::filesystem::exists(std::filesystem::absolute(binary)))
        {
            result.errors.push_back("Binary path does not exist: " + std::filesystem::absolute(binary).string());
            all_good = false;
        }
    }

    return all_good;
}

bool PicSFML::SaveProjectConfig(const ProjectConfig &config, const std::string &path)
{
    json j;

    j["sfml_version"]    = config.sfml_version.AsString('.');
    j["app_version"]     = config.app_version.AsString('.');
    j["picsfml_version"] = config.picsfml_version.AsString('.');

    j["name"] = config.name;
    j["compiler"] = config.compiler;
    j["sfml"]     = config.sfml;
    j["output"]   = config.output;
    j["main"]     = config.main;
    j["source"]   = config.source;
    j["library"]  = config.library;
    j["include"]  = config.include;
    j["binary"]   = config.binary;
    j["flags"]    = { config.flags[0], config.flags[1] };
    j["vscode"]   = config.vscode;
    j["audio"]    = config.audio;
    j["network"]  = config.network;

    std::ofstream file(path);
    if (!file.is_open())
        return false;

    file << j.dump(4);
    return file.good();
}

bool PicSFML::LoadProjectConfig(ProjectConfig &config, const std::string &path)
{
    std::ifstream file(path);
    if (!file.is_open())
        return false;

    json j;
    try
    {
        file >> j;

        config.sfml_version.ParseString(j["sfml_version"]);    
        config.app_version.ParseString(j["app_version"]);     
        config.picsfml_version.ParseString(j["picsfml_version"]);

        config.name     = j["name"];
        config.compiler = j["compiler"];
        config.sfml     = j["sfml"];
        config.output   = j["output"];
        config.main     = j["main"];
        config.source   = j["source"].get<std::vector<std::string>>();
        config.library  = j["library"].get<std::vector<std::string>>();
        config.include  = j["include"].get<std::vector<std::string>>();
        config.binary   = j["binary"].get<std::vector<std::string>>();
        config.flags[0] = j["flags"][0];
        config.flags[1] = j["flags"][1];
        config.vscode   = j["vscode"];
        config.audio    = j["audio"];
        config.network  = j["network"];
    }
    catch (const json::exception &e)
    {
        return false;
    }

    return true;
}

bool PicSFML::CreateVSCodeProperties(const ProjectConfig &config, const std::string &tool_path, const std::string &path)
{
    std::filesystem::path vscodeDir = std::filesystem::path(path) / ".vscode";

    if (!std::filesystem::exists(vscodeDir))
        if (!std::filesystem::create_directories(vscodeDir))
            return false;

    json includePathArray = json::array();
    for (const auto &inc : config.include) includePathArray.push_back(inc);

    includePathArray.push_back("${workspaceFolder}/**");
    includePathArray.push_back(config.sfml + "/include");
    includePathArray.push_back(tool_path + "/SFML/" + config.sfml_version.AsString('.'));

    json configuration = {
        { "name", config.name },
        { "includePath", includePathArray },
        { "defines", json::array() },
        { "compilerPath", config.compiler + "/bin/g++.exe" },
        { "cStandard", "c17" },
        { "cppStandard", "c++17" },
        { "intelliSenseMode","windows-gcc-x64" }
    };

    json j = {
        { "configurations", json::array({ configuration }) },
        { "version", 4 }
    };

    std::ofstream file(vscodeDir / "c_cpp_properties.json");
    if (!file.is_open())
        return false;

    file << j.dump(4);
    return file.good();
}

ToolResult PicSFML::ListConfigValues(const ProjectConfig& config)
{
    ToolResult result;
    result.success = true;
    result.buildTimeS = 0;

    auto printList = [&](const std::string& name,
                         const std::vector<std::string>& list)
    {
        result.log += name + ":\n";

        if (list.empty())
        {
            result.log += "  (none)\n";
            return;
        }

        for (const auto& value : list)
            result.log += "  - " + value + "\n";
    };

    result.log += "Name: " + config.name + "\n";
    result.log += "Compiler: " + config.compiler + "\n";
    result.log += "SFML: " + config.sfml + "\n";
    result.log += "Output: " + config.output + "\n";
    result.log += "Main: " + config.main + "\n";

    result.log += "SFML Version: " + config.sfml_version.AsString('.') + "\n";
    result.log += "App Version: " + config.app_version.AsString('.') + "\n";
    result.log += "PicSFML Version: " + config.picsfml_version.AsString('.') + "\n";

    result.log += "VSCode: " + std::string(config.vscode ? "true" : "false") + "\n";
    result.log += "Audio: " + std::string(config.audio ? "true" : "false") + "\n";
    result.log += "Network: " + std::string(config.network ? "true" : "false") + "\n";

    printList("Source", config.source);
    printList("Library", config.library);
    printList("Include", config.include);
    printList("Binary", config.binary);

    result.log += "Debug Flags: " + config.flags[0] + "\n";
    result.log += "Release Flags: " + config.flags[1];
    
    return result;
}

ToolResult PicSFML::SetString(std::string& field, const std::string& value)
{
    field = value;

    return { true, "Value updated." };
}

ToolResult PicSFML::GetString(const std::string& field)
{
    return { true, field };
}

ToolResult PicSFML::SetBool(bool& field, bool value)
{
    field = value;

    return { true, "Value updated." };
}

ToolResult PicSFML::GetBool(bool field)
{
    return { true, field ? "true" : "false" };
}

ToolResult PicSFML::SetVersion(Version& field, const Version& value)
{
    field = value;

    return { true, "Version updated." };
}

ToolResult PicSFML::GetVersion(const Version& field)
{
    return { true, field.AsString('.') };
}

ToolResult PicSFML::AddToList(std::vector<std::string>& field, const std::string& value)
{
    if (std::find(field.begin(), field.end(), value) != field.end())
        return { false, "", { "Value already exists." } };

    field.push_back(value);

    return { true, "Value added." };
}

ToolResult PicSFML::RemoveFromList(std::vector<std::string>& field, const std::string& value)
{
    auto it = std::find(field.begin(), field.end(), value);

    if (it == field.end())
        return { false, "", { "Value not found." } };

    field.erase(it);

    return { true, "Value removed." };
}

ToolResult PicSFML::List(const std::vector<std::string>& field)
{
    ToolResult result;
    result.success = true;

    if (field.empty())
    {
        result.log = "No entries.";
        return result;
    }

    for (const auto& value : field) result.log += value + '\n';

    if (!result.log.empty()) result.log.pop_back();

    return result;
}