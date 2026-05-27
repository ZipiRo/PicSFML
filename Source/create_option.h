bool CheckIsPicSFMLProject(const std::filesystem::path &project_path)
{
    return std::filesystem::exists(project_path / PROJECT_CONFIG);
}

void CopyApplicationBase(const PicConfig &pic_config)
{
    std::filesystem::path from = picsfml_path / "Template";

    for(const auto &entry : std::filesystem::directory_iterator(from))
    {
        std::filesystem::copy(entry, project_path / entry.path().filename());
    }
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

    if(pic_config.use_vscode && !CreateVSCodeConfigJSON(pic_config, project_path))
    {
        std::cout << "Aborting...\n";
        return false;
    }

    std::cout << "Sucessful\n";

    return true;
}