struct CreateProjectConfig
{
    std::string name, output;
    std::filesystem::path picsfml_path, project_path, gcc_path, sfml_path;
    bool use_vscode = false;
};

void CopyApplicationBase(const CreateProjectConfig &create_config)
{
    std::filesystem::path from = create_config.picsfml_path / "Template" / "Application";

    for(const auto &entry : std::filesystem::directory_iterator(from))
        std::filesystem::copy(entry, create_config.project_path / entry.path().filename());
}

bool SetPicSFMLConfig(const CreateProjectConfig &create_config)
{
    std::filesystem::path what = create_config.picsfml_path / "Template" / ".picsfml_config";
    std::filesystem::copy(what, create_config.project_path / what.filename());

    std::ifstream i_json_file(create_config.project_path / what.filename());

    if(!i_json_file) return false;

    json project_config;
    i_json_file >> project_config;

    i_json_file.close();

    if(!create_config.name.empty())
        project_config["project"]["name"] = create_config.name;

    if(!create_config.output.empty())
        project_config["project"]["output"] = create_config.output;

    if(!create_config.gcc_path.empty())
        project_config["project"]["gcc"] = create_config.gcc_path;

    if(!create_config.sfml_path.empty())
        project_config["project"]["sfml"] = create_config.sfml_path;

    std::ofstream o_json_file(create_config.project_path / what.filename());

    if(!o_json_file) return false;

    o_json_file << project_config.dump(4);

    o_json_file.close();

    return true;
}

bool SetVSCodeProp(const CreateProjectConfig &create_config)
{
    std::filesystem::create_directory(create_config.project_path / ".vscode");

    std::filesystem::path what = create_config.picsfml_path / "Template" / "c_cpp_properties.json";
    std::filesystem::copy(what, create_config.project_path / ".vscode" / what.filename());

    std::ifstream i_json_file(create_config.project_path / ".vscode" / what.filename());

    if(!i_json_file) return false;

    json vscode_prop;
    i_json_file >> vscode_prop;

    i_json_file.close();

    if(!create_config.name.empty())
        vscode_prop["configurations"][0]["name"] = create_config.name;

    if(!create_config.gcc_path.empty())
        vscode_prop["configurations"][0]["compilerPath"] = create_config.gcc_path;

    if(!create_config.sfml_path.empty())
        vscode_prop["configurations"][0]["includePath"].push_back(create_config.sfml_path.string() + "/include");
    else vscode_prop["configurations"][0]["includePath"].push_back("Add SFML include path");

    vscode_prop["configurations"][0]["includePath"].push_back(create_config.picsfml_path.string() + "/Core");

    std::ofstream o_json_file(create_config.project_path / ".vscode" / what.filename());

    if(!o_json_file) return false;

    o_json_file << vscode_prop.dump(4);

    o_json_file.close();

    return true;
}

bool CreateOption(CreateProjectConfig &create_config)
{
    if(create_config.project_path.string()[0] == '.') create_config.project_path = std::filesystem::current_path(); 
    else create_config.project_path = std::filesystem::absolute(create_config.project_path);

    std::cout << "Project path: " << create_config.project_path.string() << '\n';

    CopyApplicationBase(create_config);
    
    if(!SetPicSFMLConfig(create_config))
    {
        std::cout << "Aborting...\n";
        return false;
    }

    if(create_config.use_vscode)
        if(!SetVSCodeProp(create_config))
        {
            std::cout << "Aborting...\n";
            return false;
        }

    return true;
}