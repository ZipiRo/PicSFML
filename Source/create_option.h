bool CheckIsPicSFMLProject(const std::filesystem::path &project_path)
{
    return std::filesystem::exists(project_path / PROJECT_CONFIG);
}

void CopyApplicationBase(const PicConfig &pic_config)
{
    std::filesystem::path from = picsfml_path / "Template" / "Application";

    for(const auto &entry : std::filesystem::directory_iterator(from))
    {
        std::filesystem::copy(entry, project_path / entry.path().filename());
    }
}

bool SetVSCodeConfig(const PicConfig &pic_config)
{
    std::filesystem::create_directory(project_path / ".vscode");

    std::filesystem::path what = picsfml_path / "Template" / VSC_CONFIG;
    std::filesystem::path to = project_path / ".vscode" / VSC_CONFIG;
    std::filesystem::copy(what, to);

    json vscode_config;

    if(!GetConfigJSON(to, vscode_config)) return false;

    vscode_config["configurations"][0]["name"] = pic_config.name;

    vscode_config["configurations"][0]["includePath"].push_back(
        picsfml_path.string() + "/Core/" + SFMLCoreVersions().at(pic_config.sfml_version.AsInt()));

    if(!pic_config.gcc_path.empty())
    {
        vscode_config["configurations"][0]["compilerPath"] = pic_config.gcc_path.string() + "/bin/g++.exe";
    }
    else 
    {
        vscode_config["configurations"][0]["includePath"].push_back("Add G++ file path");
    }

    if(!pic_config.sfml_path.empty())
    {
        vscode_config["configurations"][0]["includePath"].push_back(pic_config.sfml_path.string() + "/include");
    }
    else 
    {
        vscode_config["configurations"][0]["includePath"].push_back("Add SFML include path");
    }

    if(!CreateConfigJSON(to, vscode_config)) return false;

    std::cout << "VSCode Properities file created\n";

    return true;
}

bool CreateOption(const PicConfig &pic_config)
{
    if(CheckIsPicSFMLProject(project_path))
    {
        std::cout << "This is already a PicSFML Project\n";
        std::cout << "Aborting...\n";
        return false;
    }

    std::cout << "Creating PicSFML project '" + pic_config.name + "'\n";
    std::cout << "SFML version " + SFMLCoreVersions().at(pic_config.sfml_version.AsInt()) + "\n";

    CopyApplicationBase(pic_config);
    
    if(!CreatePicSFMLConfigJSON(pic_config, project_path))
    {
        std::cout << "Aborting...\n";
        return false;
    }

    if(pic_config.use_vscode && !SetVSCodeConfig(pic_config))
    {
        std::cout << "Aborting...\n";
        return false;
    }

    std::cout << "Sucessful\n";

    return true;
}