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

std::vector<std::string> sfml_binaries = {
    "sfml-system",
    "sfml-window",
    "sfml-graphics",
    "sfml-audio",
    "sfml-network"
};

std::vector<std::string> sfml_libraries = {
    "-lsfml-system",
    "-lsfml-window",
    "-lsfml-graphics",
    "-lsfml-audio",
    "-lsfml-network"
};

enum BuildType 
{ 
    Release, 
    Debug 
};

struct BuildConfig
{
    std::string build_flags[2]; 
    BuildType build_type;
    bool use_audio = false;
    bool use_network = false;
    int sfml_version = 3;
    std::string project_name, project_output; 
    std::filesystem::path picsfml_path, gcc_path, sfml_path, main_source, project_path; 
    std::vector<std::filesystem::path> binaries, includes, libraries;
};

bool GetBuildConfigData(const json &project_config, BuildConfig &build_config)
{
    build_config.project_name = project_config["project"]["name"];

    build_config.project_output = project_config["project"]["output"];

    if(!std::filesystem::exists(project_config["project"]["gcc"]))
    {
        std::cout << "File "  << project_config["project"]["gcc"] << " not found\n";
        return false;
    } 
    build_config.gcc_path = std::string(project_config["project"]["gcc"]);

    if(!std::filesystem::exists(project_config["project"]["sfml"]))
    {
        std::cout << "File "  << project_config["project"]["sfml"] << " not found\n";
        return false;
    } 
    build_config.sfml_path = std::string(project_config["project"]["sfml"]);

    if(!std::filesystem::exists(build_config.project_path / project_config["project"]["main"]))
    {
        std::cout << "File "  << project_config["project"]["main"] << " not found\n";
        return false;
    } 
    build_config.main_source = std::string(project_config["project"]["main"]);

    build_config.use_audio = project_config["project"].value("use_audio", false);
    build_config.use_network = project_config["project"].value("use_network", false);
    build_config.sfml_version = project_config["project"].value("sfml_version", 3);
    build_config.build_flags[Debug] = project_config["build"]["debug"]["flags"];
    build_config.build_flags[Release] = project_config["build"]["release"]["flags"];

    for(auto path : project_config["binary"])
    {
        if(!std::filesystem::exists(path)) 
        {
            std::cout << "File " << path << " not found\n";
            return false;
        }   

        build_config.binaries.push_back(path);
    }

    for(auto path : project_config["paths"]["include"])
    {
        if(!std::filesystem::exists(path)) 
        {
            std::cout << "File " << path << " not found\n";
            return false;
        }   

        build_config.includes.push_back(path);  
    }

    for(auto path : project_config["paths"]["library"])
    {
        if(!std::filesystem::exists(path)) 
        {
            std::cout << "File " << path << " not found\n";
            return false;
        }   

        build_config.libraries.push_back(path);
    }

    return true;
}

bool CompileProject(const BuildConfig &build_config)
{
    std::string command;

    command += build_config.gcc_path.string() + "/bin/g++.exe -c " + 
            build_config.project_path.string() + "/" + build_config.main_source.string() + " " 
            "-I" + build_config.sfml_path.string() + "/include " +
            "-I" + build_config.project_path.string() + "/Include " +
            "-I" + build_config.picsfml_path.string() + "/Core/" + sfml_version_core[build_config.sfml_version] + " ";
    
    for(const auto &path : build_config.includes)
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

bool BuildProject(const BuildConfig &build_config)
{
    std::string command;

    command += build_config.gcc_path.string() + "/bin/g++.exe " + 
                build_config.build_flags[build_config.build_type] + " ";

    if(build_config.use_audio)
        command += AUDIO_DEFINE + " ";

    if(build_config.use_network)
        command += NEWTWORK_DEFINE + " ";

    command += build_config.main_source.stem().string() + ".o -o " + 
                build_config.project_path.string() + "/" + build_directory[build_config.build_type] + "/" + 
                build_config.project_output + " ";

    std::string libraries;

    libraries += sfml_libraries[build_config.build_type][0] + (build_config.build_type == Debug) ? "-d " : " ";
    libraries += sfml_libraries[build_config.build_type][1] + (build_config.build_type == Debug) ? "-d " : " ";
    libraries += sfml_libraries[build_config.build_type][2] + (build_config.build_type == Debug) ? "-d " : " ";

    if(build_config.use_audio)
        libraries += sfml_libraries[build_config.build_type][3] + (build_config.build_type == Debug) ? "-d " : " ";

    if(build_config.use_network)
        libraries += sfml_libraries[build_config.build_type][4] + (build_config.build_type == Debug) ? "-d " : " ";

    command += "-L" + build_config.sfml_path.string() + "/lib ^ " + libraries;

    for(auto path : build_config.libraries)
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

void WorkBuildDirectory(const BuildConfig &build_config)
{   
    std::cout << "Cleaning build directory...\n";

    for(const auto &entry : std::filesystem::directory_iterator(build_config.project_path / build_directory[build_config.build_type]))
    {
        if(entry.path().extension() != ".dll") continue;
        std::filesystem::remove(entry.path());
    }

    if(std::filesystem::exists(build_config.project_path / build_directory[build_config.build_type] / "Resources"))
        std::filesystem::remove_all(build_config.project_path / build_directory[build_config.build_type] / "Resources");

    std::cout << "Adding binaries...\n";

    std::filesystem::path to = build_config.project_path / build_directory[build_config.build_type];

    for(const auto &file : gcc_binaries)
        std::filesystem::copy(build_config.gcc_path / "bin" / file, to / file);

    for(int i = 0; i < sfml_binaries[build_config.build_type].size(); i++)
    {
        if(i == 3 && !build_config.use_audio) continue;
        if(i == 4 && !build_config.use_network) continue; 

        std::string binary = sfml_binaries[build_config.build_type][i] + (build_config.build_type == Debug) ? "-d-" : "-" + 
                            std::to_string(build_config.sfml_version) + ".dll"; 
        std::filesystem::copy(build_config.sfml_path / "bin" / binary, to / binary);
    }

    for(const auto &path : build_config.binaries)
        std::filesystem::copy(path, to / path.filename());

    std::cout << "Adding resources...\n";

    std::filesystem::copy(build_config.project_path / "Resources", to / "Resources", std::filesystem::copy_options::recursive);
}

bool BuildOption(BuildConfig &build_config)
{
    if(build_config.project_path.string()[0] == '.') build_config.project_path = std::filesystem::current_path(); 
    else build_config.project_path = std::filesystem::absolute(build_config.project_path);

    std::cout << "PicSFML: " << build_config.picsfml_path.string() << '\n';
    std::cout << "Project: " << build_config.project_path.string() << '\n';

    json project_config;

    if(!GetConfigJSON(build_config.project_path / PROJECT_CONFIG_NAME, project_config)) return 1;

    if(!std::filesystem::exists(build_config.project_path / build_directory[build_config.build_type]))
        std::filesystem::create_directories(build_config.project_path / build_directory[build_config.build_type]);

    if(!GetBuildConfigData(project_config, build_config))
        return false;

    if(!CompileProject(build_config))
    {
        std::cout << "Aborting...\n";
        return false;
    }

    if(!BuildProject(build_config))
    {
        std::cout << "Aborting...\n";
        return false;
    }

    WorkBuildDirectory(build_config);

    return true;
}