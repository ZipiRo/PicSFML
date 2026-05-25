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

std::string no_cmd_library = "-mwindows";

enum BuildType 
{ 
    Release, 
    Debug 
};

struct BuildConfig
{
    BuildType build_type;
    std::string project_name, project_output, build_flags[2]; 
    std::filesystem::path gcc_path, sfml_path, main_source, project_path; 
    std::vector<std::filesystem::path> binaries, includes, libraries;
    bool use_audio = false, use_network = false;
    PVersion sfml_version, application_version;
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
    
    build_config.sfml_version.ParseString(project_config["project"]["sfml_version"]);

    build_config.application_version.ParseString(project_config["project"]["application_version"]);
    
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

bool CreateWindowsIcon(const BuildConfig &build_config)
{
    if(!std::filesystem::exists(build_config.project_path / APPLICATION_ICON)) return false;

    std::string command;

    command += picsfml_path.string() + "/make_icon.exe " +
            "--input " + build_config.project_path.string() + "/" + APPLICATION_ICON + " "
            "--output " + build_config.project_path.string() + "/" + WINDOWS_ICON;

    if(system(command.c_str()) != 0)
    {
        std::cout << "Had some problems compiling windows resources\n";
        std::cout << "Command: " << command << '\n';
        return false;
    }

    std::cout << "Windows Icon Created\n";
    return true;
}

void CreateWindowsResource(const BuildConfig &build_config)
{
    std::ofstream file;
    
    file.open(build_config.project_path.string() + "/" + WINDOWS_RESOURCE_NAME + ".rc");

    file <<
    "#include <windows.h>\n\n"
    "IDI_ICON1 ICON \"icon.ico\"\n"
    "VS_VERSION_INFO VERSIONINFO\n"
    " FILEVERSION "
    << build_config.application_version.AsString(',') << "\n"
    " PRODUCTVERSION "
    << build_config.application_version.AsString(',') << "\n" 
    " FILEOS VOS_NT_WINDOWS32\n"
    " FILETYPE VFT_APP\n"
    "BEGIN\n"
    "    BLOCK \"StringFileInfo\"\n"
    "    BEGIN\n"
    "        BLOCK \"040904B0\"\n"
    "        BEGIN\n"
    "            VALUE \"ProductName\", \"" 
    << build_config.project_output << "\"\n"
    "            VALUE \"FileVersion\", \"" 
    << build_config.application_version.AsString('.') << "\"\n"
    "            VALUE \"ProductVersion\", \"" 
    << build_config.application_version.AsString('.') << "\"\n"
    "            VALUE \"FileDescription\", \"PicSFML Application\"\n"
    "        END\n"
    "    END\n"
    "    BLOCK \"VarFileInfo\"\n"
    "    BEGIN\n"
    "        VALUE \"Translation\", 0x0409, 1200\n"
    "    END\n"
    "END\n";

    file.close();

    std::cout << "Windows Resource Created\n";
}

bool CompileWindowsResource(const BuildConfig &build_config)
{
    std::string command;

    command += "windres " +
            build_config.project_path.string() + "/" + WINDOWS_RESOURCE_NAME + ".rc " + 
            "-O coff -o " + WINDOWS_RESOURCE_NAME + ".o";

    if(system(command.c_str()) != 0)
    {
        std::cout << "Had some problems compiling windows resources\n";
        std::cout << "Command: " << command << '\n';
        return false;
    }

    std::cout << "Windows Resource Compiled\n";
    return true;
}

bool CompileProject(const BuildConfig &build_config)
{
    std::string command;

    command += build_config.gcc_path.string() + "/bin/g++.exe -c " + 
            build_config.project_path.string() + "/" + build_config.main_source.string() + " "
            "-I" + build_config.sfml_path.string() + "/include " +
            "-I" + build_config.project_path.string() + "/Include " +
            "-I" + picsfml_path.string() + "/Core/" + sfml_versions.at(build_config.sfml_version.AsInt()) + " ";
    
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

    command += build_config.main_source.stem().string() + ".o resource.o -o " +
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

    if(build_config.build_type == Release)
        libraries += no_cmd_library;

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

        std::string binary = sfml_file[i] + ((build_config.build_type == Debug) ? "-d-" : "-") + std::to_string(build_config.sfml_version[0]) + ".dll"; 

        std::filesystem::copy(build_config.sfml_path / "bin" / binary, to / binary);
    }

    for(const auto &file : gcc_binary)
        std::filesystem::copy(build_config.gcc_path / "bin" / file, to / file);

    for(const auto &path : build_config.binaries)
        std::filesystem::copy(path, to / path.filename());

    if(std::filesystem::exists(to / "Resources")) std::filesystem::remove_all(to / "Resources");

    std::filesystem::copy(build_config.project_path / "Resources", to / "Resources", std::filesystem::copy_options::recursive);

    if(std::filesystem::exists(to / "icon.png")) std::filesystem::remove(to / "icon.png");

    if(std::filesystem::exists(build_config.project_path / "icon.png")) 
        std::filesystem::copy(build_config.project_path / "icon.png", to / "icon.png");
}

void CleanProject(const BuildConfig &build_config)
{
    std::filesystem::remove(build_config.project_path / WINDOWS_ICON);
    std::filesystem::remove(build_config.project_path / (WINDOWS_RESOURCE_NAME + ".rc"));
    std::filesystem::remove(build_config.project_path / (WINDOWS_RESOURCE_NAME + ".o"));
    std::filesystem::remove(build_config.project_path / (build_config.main_source.stem().string() + ".o"));
}

bool BuildOption(BuildConfig &build_config)
{
    json project_config;

    if(!GetConfigJSON(build_config.project_path / PROJECT_CONFIG, project_config)) return 1;

    if(!std::filesystem::exists(build_config.project_path / build_directory[build_config.build_type]))
        std::filesystem::create_directories(build_config.project_path / build_directory[build_config.build_type]);

    if(!GetBuildConfigData(project_config, build_config))
        return false;

    std::cout << "Building PicSFML project '" + build_config.project_name + "'\n";
    std::cout << "BuildType: " + std::string((build_config.build_type == Debug) ? "Debug" : "Release") + "\n";
    std::cout << "BuildVersion: " + build_config.application_version.AsString('.') << "\n";
    std::cout << "Building for " + sfml_versions.at(build_config.sfml_version.AsInt()) + "\n";
    
    if(!CreateWindowsIcon(build_config))
    {
        std::cout << "Aborting...\n";
        CleanProject(build_config);
        return false;
    }

    CreateWindowsResource(build_config);

    if(!CompileWindowsResource(build_config))
    {
        std::cout << "Aborting...\n";
        CleanProject(build_config);
        return false;
    }

    if(!CompileProject(build_config))
    {
        std::cout << "Aborting...\n";
        CleanProject(build_config);
        return false;
    }

    if(!BuildProject(build_config))
    {
        std::cout << "Aborting...\n";
        CleanProject(build_config);
        return false;
    }

    WorkBuildDirectory(build_config);

    CleanProject(build_config);

    std::cout << "Sucessful\n";

    return true;
}