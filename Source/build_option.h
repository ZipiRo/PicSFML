const std::string AUDIO_DEFINE = "-DAUDIO";
const std::string NEWTWORK_DEFINE = "-DNETWORK";

std::string build_directory[2] = {
    "Build/Release",
    "Build/Debug"
};

std::vector<std::string> gcc_binary = {
    "libstdc++-6.dll",
    "libwinpthread-1.dll",
    "libgcc_s_seh-1.dll"
};

std::vector<std::string> sfml_file = {
    "sfml-system",
    "sfml-window",
    "sfml-graphics",
    "sfml-audio",
    "sfml-network"
};

enum BuildType 
{ 
    Release, 
    Debug 
};

struct BuildConfig
{
    BuildType build_type;
    std::string project_name, project_output, build_flags[2]; 
    std::filesystem::path picsfml_path, gcc_path, sfml_path, main_source, project_path; 
    std::vector<std::filesystem::path> binaries, includes, libraries;
    bool use_audio = false, use_network = false;
    int sfml_version = 300;
};

bool GetBuildConfigData(const json &project_config, BuildConfig &build_config)
{
    if(!std::filesystem::exists(project_config["project"]["gcc"]))
    {
        std::cout << "GCC path "  << project_config["project"]["gcc"] << " not found\n";
        return false;
    } build_config.gcc_path = std::string(project_config["project"]["gcc"]);

    if(!std::filesystem::exists(project_config["project"]["sfml"]))
    {
        std::cout << "SFML path "  << project_config["project"]["sfml"] << " not found\n";
        return false;
    } build_config.sfml_path = std::string(project_config["project"]["sfml"]);

    if(!std::filesystem::exists(build_config.project_path / project_config["project"]["main"]))
    {
        std::cout << "Main file "  << project_config["project"]["main"] << " not found\n";
        return false;
    } build_config.main_source = std::string(project_config["project"]["main"]);

    build_config.project_name = project_config["project"]["name"];

    build_config.project_output = project_config["project"]["output"];
    
    build_config.use_audio = project_config["project"].value("use_audio", false);

    build_config.use_network = project_config["project"].value("use_network", false);
    
    build_config.sfml_version = project_config["project"].value("sfml_version", 300);
    
    build_config.build_flags[Debug] = project_config["build"]["debug"]["flags"];
    
    build_config.build_flags[Release] = project_config["build"]["release"]["flags"];

    for(const auto &binary : project_config["binary"])
    {
        if(!std::filesystem::exists(binary) && binary.empty()) 
        {
            std::cout << "File " << binary << " not found\n";
            return false;
        }   

        build_config.binaries.push_back(binary);
    }

    for(const auto &include : project_config["include"])
    {
        if(!std::filesystem::exists(include) && include.empty()) 
        {
            std::cout << "Include " << include << " not found\n";
            return false;
        }   

        build_config.includes.push_back(include);  
    }

    for(const auto &library : project_config["library"])
        build_config.libraries.push_back(library);

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

    if(system(command.c_str()) != 0) 
    {
        std::cout << "Had some problems compiling\n";
        std::cout << "Command: " << command << '\n';
        return false;
    }

    std::cout << "Compilation sucessful\n"; 
    return true;
}

bool BuildProject(const BuildConfig &build_config)
{
    std::string command, libraries;

    command += build_config.gcc_path.string() + "/bin/g++.exe ";

    if(!build_config.build_flags[build_config.build_type].empty())
        command += build_config.build_flags[build_config.build_type] + " ";

    if(build_config.use_audio)
        command += AUDIO_DEFINE + " ";

    if(build_config.use_network)
        command += NEWTWORK_DEFINE + " ";

    command += build_config.main_source.stem().string() + ".o -o " + 
                build_config.project_path.string() + "/" + build_directory[build_config.build_type] + "/" + 
                build_config.project_output + " ";

    for(const auto &path : build_config.libraries)
        command += path.string() + " ";

    libraries += "-l" + sfml_file[0] + ((build_config.build_type == Debug) ? "-d " : " ");
    libraries += "-l" + sfml_file[1] + ((build_config.build_type == Debug) ? "-d " : " ");
    libraries += "-l" + sfml_file[2] + ((build_config.build_type == Debug) ? "-d " : " ");

    if(build_config.use_audio)
        libraries += "-l" + sfml_file[3] + ((build_config.build_type == Debug) ? "-d " : " ");

    if(build_config.use_network)
        libraries += "-l" + sfml_file[4] + ((build_config.build_type == Debug) ? "-d " : " ");

    command += "-L" + build_config.sfml_path.string() + "/lib ^ " + libraries;
    
    if(system(command.c_str()) != 0) 
    {
        std::cout << "Had some problems building\n";
        std::cout << "Command: " + command << '\n';
        return false;
    }

    std::cout << "Build sucessful\n"; 
    return true;
}

void WorkBuildDirectory(const BuildConfig &build_config)
{   
    std::filesystem::path to = build_config.project_path / build_directory[build_config.build_type];

    for(const auto &entry : std::filesystem::directory_iterator(to))
    {
        if(entry.path().extension() != ".dll") continue;
        std::filesystem::remove(entry.path());
    }

    for(int i = 0; i < sfml_file.size(); i++)
    {
        if(i == 3 && !build_config.use_audio) continue;
        if(i == 4 && !build_config.use_network) continue; 

        std::string binary = sfml_file[i] + ((build_config.build_type == Debug) ? "-d-" : "-") + std::to_string(build_config.sfml_version / 100) + ".dll"; 

        std::filesystem::copy(build_config.sfml_path / "bin" / binary, to / binary);
    }

    for(const auto &file : gcc_binary)
        std::filesystem::copy(build_config.gcc_path / "bin" / file, to / file);

    for(const auto &path : build_config.binaries)
        std::filesystem::copy(path, to / path.filename());

    if(std::filesystem::exists(to / "Resources")) std::filesystem::remove_all(to / "Resources");

    std::filesystem::copy(build_config.project_path / "Resources/", to / "Resources/", std::filesystem::copy_options::recursive);
}

bool BuildOption(BuildConfig &build_config)
{
    json project_config;

    if(!GetConfigJSON(build_config.project_path / PROJECT_CONFIG_NAME, project_config)) return 1;

    if(!std::filesystem::exists(build_config.project_path / build_directory[build_config.build_type]))
        std::filesystem::create_directories(build_config.project_path / build_directory[build_config.build_type]);

    if(!GetBuildConfigData(project_config, build_config))
        return false;

    std::cout << "Building PicSFML project '" + build_config.project_name + "'\n";
    std::cout << "BuildType: " + std::string((build_config.build_type == Debug) ? "Debug" : "Release") + "\n";
    std::cout << "Building for " + sfml_version_core[build_config.sfml_version] + "\n";
    
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

    std::cout << "Sucessful\n";

    return true;
}