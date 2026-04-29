const std::string PROJECT_CONFIG_NAME = ".picsfml_config";
const std::string AUDIO_DEFINE = "-DAUDIO";
const std::string NEWTWORK_DEFINE = "-DNETWORK";

std::string build_directory[2] = {
    "Build/Release",
    "Build/Debug"
};

std::vector<std::string> gcc_binaries = {
    "libstdc++-6.dll",
    "libwinpthread-1.dll",
    "libgcc_s_seh-1.dll"
};

std::vector<std::string> sfml_binaries[2] = {
    {
        "sfml-system-3.dll",
        "sfml-window-3.dll",
        "sfml-graphics-3.dll",
        "sfml-audio-3.dll",
        "sfml-network-3.dll"
    },
    {
        "sfml-system-d-3.dll",
        "sfml-window-d-3.dll",
        "sfml-graphics-d-3.dll",
        "sfml-audio-d-3.dll",
        "sfml-network-d-3.dll"
    }
};

std::vector<std::string> sfml_libraries[2] = {
    {
        "-lsfml-system",
        "-lsfml-window",
        "-lsfml-graphics",
        "-lsfml-audio",
        "-lsfml-network"
    },
    {
        "-lsfml-system-d",
        "-lsfml-window-d",
        "-lsfml-graphics-d",
        "-lsfml-audio-d",
        "-lsfml-network-d"
    }
};

enum BuildType { Release, Debug };

struct ProjectConfigData
{
    std::string build_flags[2]; 
    BuildType build_type;
    bool use_audio = false;
    bool use_network = false;
    std::string project_name, project_output; 
    std::filesystem::path picsfml_path, gcc_path, sfml_path, main_source, project_path; 
    std::vector<std::filesystem::path> binaries, includes, libraries;
};

bool GetConfigJSON(const std::filesystem::path &path, json &result)
{
    if(!std::filesystem::exists(path)) return false;

    std::ifstream json_file(path);

    if(!json_file) return false;

    json_file >> result;

    json_file.close();

    return true;
}

bool GetConfigData(const json &project_config, ProjectConfigData &config_data)
{
    config_data.project_name = project_config["project"]["name"];

    config_data.project_output = project_config["project"]["output"];

    if(!std::filesystem::exists(project_config["project"]["gcc"]))
    {
        std::cout << "File "  << project_config["project"]["gcc"] << " not found\n";
        return false;
    } 
    config_data.gcc_path = std::string(project_config["project"]["gcc"]);

    if(!std::filesystem::exists(project_config["project"]["sfml"]))
    {
        std::cout << "File "  << project_config["project"]["sfml"] << " not found\n";
        return false;
    } 
    config_data.sfml_path = std::string(project_config["project"]["sfml"]);

    if(!std::filesystem::exists(config_data.project_path / project_config["project"]["main"]))
    {
        std::cout << "File "  << project_config["project"]["main"] << " not found\n";
        return false;
    } 
    config_data.main_source = std::string(project_config["project"]["main"]);

    config_data.use_audio = project_config["project"].value("use_audio", false);
    config_data.use_network = project_config["project"].value("use_network", false);

    for(auto path : project_config["binary"])
    {
        if(!std::filesystem::exists(path)) 
        {
            std::cout << "File " << path << " not found\n";
            return false;
        }   

        config_data.binaries.push_back(path);
    }

    for(auto path : project_config["paths"]["include"])
    {
        if(!std::filesystem::exists(path)) 
        {
            std::cout << "File " << path << " not found\n";
            return false;
        }   

        config_data.includes.push_back(path);  
    }

    for(auto path : project_config["paths"]["library"])
    {
        if(!std::filesystem::exists(path)) 
        {
            std::cout << "File " << path << " not found\n";
            return false;
        }   

        config_data.libraries.push_back(path);
    }

    config_data.build_flags[Debug] = project_config["build"]["debug"]["flags"];
    config_data.build_flags[Release] = project_config["build"]["release"]["flags"];

    return true;
}

bool CompileProject(const ProjectConfigData &config_data)
{
    std::string command;

    command += config_data.gcc_path.string() + "/bin/g++.exe -c " + 
            config_data.project_path.string() + "/" + config_data.main_source.string() + " " 
            "-I" + config_data.sfml_path.string() + "/include " +
            "-I" + config_data.project_path.string() + "/Include " +
            "-I" + config_data.picsfml_path.string() + "/Core ";
    
    for(const auto &path : config_data.includes)
        command += "-I" + path.string() + " ";

    std::cout << "Compleing project...\n";

    std::cout << command << '\n';
    
    if(system(command.c_str()) != 0) 
    {
        std::cout << "Had some problems compiling\n";
        return false;
    }

    std::cout << "Compilation sucessful\n"; 
    return true;
}

bool BuildProject(const ProjectConfigData &config_data)
{
    std::string command;

    command += config_data.gcc_path.string() + "/bin/g++.exe " + 
                config_data.build_flags[config_data.build_type] + " ";

    if(config_data.use_audio)
        command += AUDIO_DEFINE + " ";

    if(config_data.use_network)
        command += NEWTWORK_DEFINE + " ";

    command += config_data.main_source.stem().string() + ".o -o " + 
                config_data.project_path.string() + "/" + build_directory[config_data.build_type] + "/" + 
                config_data.project_output + " ";

    std::string libraries;

    libraries += sfml_libraries[config_data.build_type][0] + " ";
    libraries += sfml_libraries[config_data.build_type][1] + " ";
    libraries += sfml_libraries[config_data.build_type][2] + " ";

    if(config_data.use_audio)
        libraries += sfml_libraries[config_data.build_type][3] + " ";

    if(config_data.use_network)
        libraries += sfml_libraries[config_data.build_type][4] + " ";

    command += "-L" + config_data.sfml_path.string() + "/lib ^ " + libraries;

    for(auto path : config_data.libraries)
        command += path.string();


    std::cout << "Building project...\n";

    std::cout << command << '\n';
    
    if(system(command.c_str()) != 0) 
    {
        std::cout << "Had some problems building\n";
        return false;
    }

    std::cout << "Building sucessful\n"; 
    return true;
}

void WorkBuildDirectory(const ProjectConfigData &config_data)
{   
    std::cout << "Cleaning build directory...\n";

    for(const auto &entry : std::filesystem::directory_iterator(config_data.project_path / build_directory[config_data.build_type]))
    {
        if(entry.path().extension() != ".dll") continue;
        std::filesystem::remove(entry.path());
    }

    if(std::filesystem::exists(config_data.project_path / build_directory[config_data.build_type] / "Resources"))
        std::filesystem::remove_all(config_data.project_path / build_directory[config_data.build_type] / "Resources");

    std::cout << "Adding binaries...\n";

    std::filesystem::path to = config_data.project_path / build_directory[config_data.build_type];

    for(const auto &file : gcc_binaries)
        std::filesystem::copy(config_data.gcc_path / "bin" / file, to / file);

    for(int i = 0; i < sfml_binaries[config_data.build_type].size(); i++)
    {
        if(i == 3 && !config_data.use_audio) continue;
        if(i == 4 && !config_data.use_network) continue; 

        std::string binary = sfml_binaries[config_data.build_type][i]; 
        std::filesystem::copy(config_data.sfml_path / "bin" / binary, to / binary);
    }

    for(const auto &path : config_data.binaries)
        std::filesystem::copy(path, to / path.filename());

    std::cout << "Adding resources...\n";

    std::filesystem::copy(config_data.project_path / "Resources", to / "Resources", std::filesystem::copy_options::recursive);
}

bool BuildOption(std::filesystem::path picsfml_path, std::filesystem::path project_path, BuildType build_type)
{
    if(project_path.string()[0] == '.') project_path = std::filesystem::current_path(); 
    else project_path = std::filesystem::absolute(project_path);

    std::cout << "Project path: " << project_path.string() << '\n';

    json project_config;

    if(!GetConfigJSON(project_path / PROJECT_CONFIG_NAME, project_config)) return 1;

    if(!std::filesystem::exists(project_path / build_directory[build_type]))
        std::filesystem::create_directories(project_path / build_directory[build_type]);

    ProjectConfigData config_data;
    config_data.picsfml_path = picsfml_path;
    config_data.project_path = project_path;
    config_data.build_type = build_type;

    if(!GetConfigData(project_config, config_data))
        return false;

    if(!CompileProject(config_data))
    {
        std::cout << "Aborting...\n";
        return false;
    }

    if(!BuildProject(config_data))
    {
        std::cout << "Aborting...\n";
        return false;
    }

    WorkBuildDirectory(config_data);

    return true;
}