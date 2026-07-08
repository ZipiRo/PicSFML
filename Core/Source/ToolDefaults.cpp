#include <fstream>
#include <string>
#include <nlohmann/json.hpp>

#include <ToolResult.h>

#include <Version.h>
#include <ToolDefaults.h>

using json = nlohmann::json;
using namespace PicSFML;

bool PicSFML::SaveToolDefaults(const ToolDefaults &defaults, const std::string &path)
{
    json j;

    j["compiler"]           = defaults.compiler;
    j["sfml"]               = defaults.sfml;
    j["sfml_version"]       = defaults.sfml_version.AsString('.');
    j["flags"]              = { defaults.flags[0], defaults.flags[1] };
    j["output"]             = defaults.output;
    j["main"]               = defaults.main;
    j["projects_directory"] = defaults.projects_directory;

    std::ofstream file(path);
    if (!file.is_open())
        return false;

    file << j.dump(4);
    return file.good();
}

bool PicSFML::LoadToolDefaults(ToolDefaults &defaults, const std::string &path)
{
    std::ifstream file(path);
    if (!file.is_open())
        return false;

    json j;
    try
    {
        file >> j;

        defaults.compiler           = j["compiler"];
        defaults.sfml               = j["sfml"];
        defaults.sfml_version.ParseString(j["sfml_version"]);
        defaults.flags[0]           = j["flags"][0];
        defaults.flags[1]           = j["flags"][1];
        defaults.output             = j["output"];
        defaults.main               = j["main"];
        defaults.projects_directory = j["projects_directory"];
    }
    catch (const json::exception &e)
    {
        return false;
    }

    return true;
}

ToolResult PicSFML::ListDefaultValues(const ToolDefaults &tool_defaults)
{
    ToolResult result;
    result.success = true;
    result.buildTimeS = 0;

    result.log += "Compiler: " + tool_defaults.compiler + "\n";
    result.log += "SFML: " + tool_defaults.sfml + "\n";
    result.log += "Output: " + tool_defaults.output + "\n";
    result.log += "Main: " + tool_defaults.main + "\n";

    result.log += "SFML Version: " + tool_defaults.sfml_version.AsString('.') + "\n";

    result.log += "Debug Flags: " + tool_defaults.flags[0] + "\n";
    result.log += "Release Flags: " + tool_defaults.flags[1] + "\n";

    result.log += "Projects Directory: " + tool_defaults.projects_directory + "\n";

    return result;
}