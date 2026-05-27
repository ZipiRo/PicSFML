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

bool CreateWindowsIcon(const PicConfig &pic_config)
{
    if(!std::filesystem::exists(project_path / APPLICATION_ICON)) return false;

    std::string command;

    command += picsfml_path.string() + "/make_icon.exe " +
            "--input " + project_path.string() + "/" + APPLICATION_ICON + " "
            "--output " + project_path.string() + "/" + WINDOWS_ICON;

    if(system(command.c_str()) != 0)
    {
        std::cout << "Had some problems compiling windows resources\n";
        std::cout << "Command: " << command << '\n';
        return false;
    }

    std::cout << "Windows Icon Created\n";
    return true;
}

void CreateWindowsResource(const PicConfig &pic_config)
{
    std::ofstream file;
    
    file.open(project_path.string() + "/" + WINDOWS_RESOURCE_NAME + ".rc");

    file <<
    "#include <windows.h>\n\n"
    "IDI_ICON1 ICON \"icon.ico\"\n"
    "VS_VERSION_INFO VERSIONINFO\n"
    " FILEVERSION "
    << pic_config.application_version.AsString(',') << "\n"
    " PRODUCTVERSION "
    << pic_config.application_version.AsString(',') << "\n" 
    " FILEOS VOS_NT_WINDOWS32\n"
    " FILETYPE VFT_APP\n"
    "BEGIN\n"
    "    BLOCK \"StringFileInfo\"\n"
    "    BEGIN\n"
    "        BLOCK \"040904B0\"\n"
    "        BEGIN\n"
    "            VALUE \"ProductName\", \"" 
    << pic_config.output << "\"\n"
    "            VALUE \"FileVersion\", \"" 
    << pic_config.application_version.AsString('.') << "\"\n"
    "            VALUE \"ProductVersion\", \"" 
    << pic_config.application_version.AsString('.') << "\"\n"
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

bool CompileWindowsResource(const PicConfig &pic_config)
{
    std::string command;

    command += "windres " +
            project_path.string() + "/" + WINDOWS_RESOURCE_NAME + ".rc " + 
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

bool CompileProject(const PicConfig &pic_config)
{
    std::string command;

    command += pic_config.gcc_path.string() + "/bin/g++.exe -c " + 
            project_path.string() + "/" + pic_config.main + " "
            "-I" + pic_config.sfml_path.string() + "/include " +
            "-I" + project_path.string() + "/Include " +
            "-I" + picsfml_path.string() + "/Core/" + SFMLCoreVersions().at(pic_config.sfml_version.AsInt()) + " ";
    
    for(const auto &path : pic_config.include)
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

bool BuildProject(const PicConfig &pic_config, BuildType build_type)
{
    std::string command, libraries;

    command += pic_config.gcc_path.string() + "/bin/g++.exe ";

    if(!pic_config.flags[build_type].empty())
        command += pic_config.flags[build_type] + " ";

    if(pic_config.use_audio)
        command += AUDIO_DEFINE + " ";

    if(pic_config.use_network)
        command += NEWTWORK_DEFINE + " ";

    command += std::filesystem::path(pic_config.main).stem().string() + ".o resource.o -o " +
                project_path.string() + "/" + build_directory[build_type] + "/" + 
                pic_config.output + " ";

    for(const auto &path : pic_config.library)
        command += path.string() + " ";

    libraries += "-l" + sfml_file[0] + ((build_type == Debug) ? "-d " : " ");
    libraries += "-l" + sfml_file[1] + ((build_type == Debug) ? "-d " : " ");
    libraries += "-l" + sfml_file[2] + ((build_type == Debug) ? "-d " : " ");

    if(pic_config.use_audio)
        libraries += "-l" + sfml_file[3] + ((build_type == Debug) ? "-d " : " ");

    if(pic_config.use_network)
        libraries += "-l" + sfml_file[4] + ((build_type == Debug) ? "-d " : " ");

    if(build_type == Release)
        libraries += no_cmd_library;

    command += "-L" + pic_config.sfml_path.string() + "/lib ^ " + libraries;
    
    if(system(command.c_str()) != 0) 
    {
        std::cout << "Had some problems building\n";
        std::cout << "Command: " + command << '\n';
        return false;
    }

    std::cout << "Build sucessful\n"; 
    return true;
}

void WorkBuildDirectory(const PicConfig &pic_config, BuildType build_type)
{   
    std::filesystem::path to = project_path / build_directory[build_type];

    for(const auto &entry : std::filesystem::directory_iterator(to))
    {
        if(entry.path().extension() != ".dll") continue;
        std::filesystem::remove(entry.path());
    }

    for(int i = 0; i < sfml_file.size(); i++)
    {
        if(i == 3 && !pic_config.use_audio) continue;
        if(i == 4 && !pic_config.use_network) continue; 

        std::string binary = sfml_file[i] + ((build_type == Debug) ? "-d-" : "-") + std::to_string(pic_config.sfml_version[0]) + ".dll"; 

        std::filesystem::copy(pic_config.sfml_path / "bin" / binary, to / binary);
    }

    for(const auto &file : gcc_binary)
        std::filesystem::copy(pic_config.gcc_path / "bin" / file, to / file);

    for(const auto &path : pic_config.binary)
        std::filesystem::copy(path, to / path.filename());

    if(std::filesystem::exists(to / "Resources")) std::filesystem::remove_all(to / "Resources");

    std::filesystem::copy(project_path / "Resources", to / "Resources", std::filesystem::copy_options::recursive);

    if(std::filesystem::exists(to / "icon.png")) std::filesystem::remove(to / "icon.png");

    if(std::filesystem::exists(project_path / "icon.png")) 
        std::filesystem::copy(project_path / "icon.png", to / "icon.png");
}

void CleanProject(const PicConfig &pic_config)
{
    std::filesystem::remove(project_path / WINDOWS_ICON);
    std::filesystem::remove(project_path / (WINDOWS_RESOURCE_NAME + ".rc"));
    std::filesystem::remove(project_path / (WINDOWS_RESOURCE_NAME + ".o"));
    std::filesystem::remove(project_path / (std::filesystem::path(pic_config.main).stem().string() + ".o"));
}

bool BuildOption(BuildType build_type)
{
    PicConfig pic_config;

    if(!std::filesystem::exists(project_path / build_directory[build_type]))
        std::filesystem::create_directories(project_path / build_directory[build_type]);

    if(!GetPicSFMLConfig(pic_config, project_path))
        return false;

    std::cout << "Building PicSFML project '" + pic_config.name + "'\n";
    std::cout << "BuildType: " + std::string((build_type == Debug) ? "Debug" : "Release") + "\n";
    std::cout << "BuildVersion: " + pic_config.application_version.AsString('.') << "\n";
    std::cout << "Building for " + SFMLCoreVersions().at(pic_config.sfml_version.AsInt()) + "\n";
    
    if(!CreateWindowsIcon(pic_config))
    {
        std::cout << "Aborting...\n";
        CleanProject(pic_config);
        return false;
    }

    CreateWindowsResource(pic_config);

    if(!CompileWindowsResource(pic_config))
    {
        std::cout << "Aborting...\n";
        CleanProject(pic_config);
        return false;
    }

    if(!CompileProject(pic_config))
    {
        std::cout << "Aborting...\n";
        CleanProject(pic_config);
        return false;
    }

    if(!BuildProject(pic_config, build_type))
    {
        std::cout << "Aborting...\n";
        CleanProject(pic_config);
        return false;
    }

    WorkBuildDirectory(pic_config, build_type);

    CleanProject(pic_config);

    std::cout << "Sucessful\n";

    return true;
}