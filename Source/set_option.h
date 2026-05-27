bool SetOption(const PicConfig &pic_config)
{
    std::cout << "Setting '.picsfml_config'\n";

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

    std::cout << "Successful\n";

    return true;
}