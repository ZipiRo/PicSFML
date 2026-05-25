struct CreateConfig
{
    PVersion sfml_version = PVersion("3.0.0");
    std::string name = "PicSFML Project", output = "main", main = "main.cpp";
    std::filesystem::path picsfml_path, project_path, gcc_path, sfml_path;
    bool use_audio = false, use_network = false, use_vscode = false;
};

bool CheckIsPicSFMLProject(const std::filesystem::path &project_path)
{
    return std::filesystem::exists(project_path / PROJECT_CONFIG);
}

void CopyApplicationBase(const CreateConfig &create_config)
{
    std::filesystem::path from = create_config.picsfml_path / "Template" / "Application";

    for(const auto &entry : std::filesystem::directory_iterator(from))
    {
        std::filesystem::copy(entry, create_config.project_path / entry.path().filename());
    }
}

bool SetPicSFMLConfig(const CreateConfig &create_config)
{
    std::filesystem::path what = create_config.picsfml_path / "Template" / PROJECT_CONFIG;
    std::filesystem::path to = create_config.project_path / PROJECT_CONFIG;
    std::filesystem::copy(what, to);

    json project_config;

    if(!GetConfigJSON(to, project_config)) return false;

    if(!create_config.gcc_path.empty())
        project_config["project"]["gcc"] = create_config.gcc_path;

    if(!create_config.sfml_path.empty())
        project_config["project"]["sfml"] = create_config.sfml_path;
        
    project_config["project"]["name"] = create_config.name;
    project_config["project"]["output"] = create_config.output;    
    project_config["project"]["main"] = create_config.main;
    project_config["project"]["use_audio"] = create_config.use_audio;
    project_config["project"]["use_network"] = create_config.use_network;
    project_config["project"]["sfml_version"] = create_config.sfml_version.AsString('.');

    if(!SetConfigJSON(to, project_config)) return false;

    std::cout << "PicSFML Config file creted\n";

    return true;
}

bool SetVSCConfig(const CreateConfig &create_config)
{
    std::filesystem::create_directory(create_config.project_path / ".vscode");

    std::filesystem::path what = create_config.picsfml_path / "Template" / VSC_CONFIG;
    std::filesystem::path to = create_config.project_path / ".vscode" / VSC_CONFIG;
    std::filesystem::copy(what, to);

    json vscode_config;

    if(!GetConfigJSON(to, vscode_config)) return false;

    vscode_config["configurations"][0]["name"] = create_config.name;

    vscode_config["configurations"][0]["includePath"].push_back(
        create_config.picsfml_path.string() + "/Core/" + sfml_versions.at(create_config.sfml_version.AsInt()));

    if(!create_config.gcc_path.empty())
    {
        vscode_config["configurations"][0]["compilerPath"] = create_config.gcc_path.string() + "/bin/g++.exe";
    }
    else 
    {
        vscode_config["configurations"][0]["includePath"].push_back("Add G++ file path");
    }

    if(!create_config.sfml_path.empty())
    {
        vscode_config["configurations"][0]["includePath"].push_back(create_config.sfml_path.string() + "/include");
    }
    else 
    {
        vscode_config["configurations"][0]["includePath"].push_back("Add SFML include path");
    }

    if(!SetConfigJSON(to, vscode_config)) return false;

    std::cout << "VSCode Properities file created\n";

    return true;
}

bool CreateOption(CreateConfig &create_config)
{
    if(CheckIsPicSFMLProject(create_config.project_path))
    {
        std::cout << "This is already a PicSFML Project\n";
        std::cout << "Aborting...\n";
        return false;
    }

    std::cout << "Creating PicSFML project '" + create_config.name + "'\n";
    std::cout << "SFML version " + sfml_versions.at(create_config.sfml_version.AsInt()) + "\n";

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

    std::cout << "Sucessful\n";

    return true;
}