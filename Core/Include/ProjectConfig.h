namespace PicSFML
{
    const std::string PicConfigFileName = ".picsfml";

    const Version PicSFMLVersion = Version("2.0.1");
    
    struct ProjectConfig
    {
        Version sfml_version;
        Version app_version = Version("1.0.0");
        Version picsfml_version = PicSFMLVersion;
        std::string name;
        std::string compiler;
        std::string sfml;
        std::string output;
        std::string main;
        std::vector<std::string> source;
        std::vector<std::string> library;
        std::vector<std::string> include;
        std::vector<std::string> binary;
        std::string flags[2];
        bool vscode = false;
        bool audio = false;
        bool network = false;
    };
    
    bool CheckProjectConfigPaths(const ProjectConfig &config, const std::string &project_path, ToolResult &result);
    bool SaveProjectConfig(const ProjectConfig &config, const std::string &path);
    bool LoadProjectConfig(ProjectConfig &config, const std::string &path);
    bool CreateVSCodeProperties(const ProjectConfig &config, const std::string &tool_path, const std::string &path);

    ToolResult ListConfigValues(const ProjectConfig &config);

    ToolResult SetString(std::string& field, const std::string& value);
    ToolResult GetString(const std::string& field);

    ToolResult SetBool(bool& field, bool value);
    ToolResult GetBool(bool field);

    ToolResult SetVersion(Version& field, const Version& value);
    ToolResult GetVersion(const Version& field);

    ToolResult AddToList(std::vector<std::string>& field, const std::string& value);
    ToolResult RemoveFromList(std::vector<std::string>& field, const std::string& value);
    ToolResult List(const std::vector<std::string>& field);
}
