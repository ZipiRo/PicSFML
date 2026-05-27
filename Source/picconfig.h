struct PicConfig
{
    PVersion version;
    PVersion sfml_version = PVersion("3.0.0");
    PVersion application_version = PVersion("1.0.0.0");
    std::string name = "PicSFML Project";
    std::string output = "main";
    std::string main = "main.cpp";
    std::string flags[2]; 
    std::filesystem::path gcc_path;
    std::filesystem::path sfml_path;
    std::vector<std::filesystem::path> binary;
    std::vector<std::filesystem::path> include;
    std::vector<std::filesystem::path> library;
    bool use_audio = false;
    bool use_network = false;
    bool use_vscode = false;
};

std::map<int, std::string> SFMLCoreVersions()
{
    std::map<int, std::string> versions;

    versions[300] = "SFML-3.0.0";
    versions[262] = "SFML-2.6.2";

    return versions;
}