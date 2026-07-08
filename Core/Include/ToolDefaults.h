namespace PicSFML
{
    const std::string ToolDefaultsFileName = ".tooldefaults";

    struct ToolDefaults
    {
        Version sfml_version;
        std::string compiler;
        std::string sfml;
        std::string flags[2];
        std::string output = "PicSFML_app";
        std::string main = "main.cpp";
        std::string projects_directory;
    };

    bool SaveToolDefaults(const ToolDefaults &defaults, const std::string &path);
    bool LoadToolDefaults(ToolDefaults &defaults, const std::string &path);

    ToolResult ListDefaultValues(const ToolDefaults& tool_defaults);
}