struct CreateConfig
{
    std::string name = "PicSFMLProject", output = "main", main = "main.cpp";
    std::filesystem::path picsfml_path, project_path, gcc_path, sfml_path;
    bool use_audio = false, use_network = false;
    int sfml_version = 300;
    
    bool use_vscode = false;
};

void CopyApplicationBase(const CreateConfig &create_config)
{
    std::filesystem::path from = create_config.picsfml_path / "Template" / "Application";

    for(const auto &entry : std::filesystem::directory_iterator(from))
        std::filesystem::copy(entry, create_config.project_path / entry.path().filename());
}

bool SetPicSFMLConfig(const CreateConfig &create_config)
{
    std::filesystem::path what = create_config.picsfml_path / "Template" / PROJECT_CONFIG_NAME;
    std::filesystem::path to = create_config.project_path / PROJECT_CONFIG_NAME;
    std::filesystem::copy(what, to);

    json project_config;

    if(!GetConfigJSON(to, project_config)) return false;

    if(!create_config.name.empty())
        project_config["project"]["name"] = create_config.name;

    if(!create_config.output.empty())
        project_config["project"]["output"] = create_config.output;
        
    if(!create_config.main.empty())
        project_config["project"]["main"] = create_config.main;

    if(!create_config.gcc_path.empty())
        project_config["project"]["gcc"] = create_config.gcc_path;

    if(!create_config.sfml_path.empty())
        project_config["project"]["sfml"] = create_config.sfml_path;

    project_config["project"]["use_audio"] = create_config.use_audio;
    project_config["project"]["use_network"] = create_config.use_network;
    project_config["project"]["sfml_version"] = create_config.sfml_version;

    if(!SetConfigJSON(to, project_config)) return false;

    return true;
}

bool SetVSCConfig(const CreateConfig &create_config)
{
    std::filesystem::create_directory(create_config.project_path / ".vscode");

    std::filesystem::path what = create_config.picsfml_path / "Template" / VSC_CONFIG_NAME;
    std::filesystem::path to = create_config.project_path / ".vscode" / VSC_CONFIG_NAME;
    std::filesystem::copy(what, to);

    json vscode_config;

    if(!GetConfigJSON(to, vscode_config)) return false;

    if(!create_config.name.empty())
    {
        vscode_config["configurations"][0]["name"] = create_config.name;
    }

    if(!create_config.gcc_path.empty())
    {
        vscode_config["configurations"][0]["compilerPath"] = create_config.gcc_path.string() + "/bin/g++.exe";
    }

    if(!create_config.sfml_path.empty())
    {
        vscode_config["configurations"][0]["includePath"].push_back(create_config.sfml_path.string() + "/include");
    }
    else 
    {
        vscode_config["configurations"][0]["includePath"].push_back("Add SFML include path");
    }

    vscode_config["configurations"][0]["includePath"].push_back(create_config.picsfml_path.string() + "/Core/" + sfml_version_core[create_config.sfml_version]);

    if(!SetConfigJSON(to, vscode_config)) return false;

    return true;
}

bool CreateOption(CreateConfig &create_config)
{
    std::cout << "PicSFML: " << create_config.picsfml_path.string() << '\n';
    std::cout << "Project: " << create_config.project_path.string() << '\n';

    CopyApplicationBase(create_config);
    
    if(!SetPicSFMLConfig(create_config))
    {
        std::cout << "Aborting...\n";
        return false;
    }

    if(create_config.use_vscode && !SetVSCConfig(create_config))
    {
        std::cout << "Aborting...\n";
        return false;
    }

    return true;
}