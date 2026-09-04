#include <Debug.hpp>
#include <Enviorment.hpp>
#include <Settings.hpp>
#include <ProjectConfig.hpp>
#include <ConfigDefaults.hpp>
#include <Version.hpp>

#include <Project.hpp>

#include <nlohmann/json.hpp>

#include <string>
#include <filesystem>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <functional>

using json = nlohmann::json;

const std::vector<std::string> PicSFML::SFMLValidVersions = {"2.6.2", "3.0.0"};
const std::vector<std::string> PicSFML::BuildTypes = {"Debug", "Release"};

const std::vector<std::string> sfml_dependencies = {
    "sfml-system",
    "sfml-window",
    "sfml-graphics",
    "sfml-audio",
    "sfml-network"};

const std::vector<std::string> compiler_binary = {
    "libstdc++-6.dll",
    "libwinpthread-1.dll",
    "libgcc_s_seh-1.dll"};

struct CommandResult
{
    std::string output;
    int exit_code;
};
 
CommandResult RunCommand(const std::string &command)
{
    std::string output;
    char buffer[128];

    FILE *pipe = _popen((command + " 2>&1").c_str(), "r");

    if (!pipe)
        return {};

    while (fgets(buffer, sizeof(buffer), pipe))
        output += buffer;

    int exit_code = _pclose(pipe);
    return {output, exit_code};
}

bool CreateWindowsResources(const PicSFML::Project &project, const std::string &picsfmltemp_dir_path, const std::string &picsfml_dir_path)
{
    const std::string icon_path = project.GetPath() + "\\" + PicSFML::IconName;

    PicSFML::Debug::Log("Creating Windows resources");
    PicSFML::Debug::Log("Icon path: " + icon_path);

    if (!std::filesystem::exists(icon_path))
    {
        PicSFML::Debug::Warning("There is no project icon: " + icon_path);
    }
    else
    {
        PicSFML::Debug::Message("Creating icon");

        CommandResult createicon_result = RunCommand("cd \"" + picsfmltemp_dir_path + "\" && " + "\"" + picsfml_dir_path + "\\pngtoico.exe\" " + "--input \"" + icon_path + "\" " + "--output \"" + picsfmltemp_dir_path + "\\icon.ico\"");

        if (createicon_result.exit_code != 0)
        {
            PicSFML::Debug::Error("Failed to create icon");
            PicSFML::Debug::Log("pngtoico output: " + createicon_result.output);
            return false;
        }

        PicSFML::Debug::Log("Icon created successfully");
    }

    PicSFML::Debug::Log("Building Windows resource string");

    std::string windwos_resource;

    windwos_resource += "#include <windows.h>\n\n";
    windwos_resource += "IDI_ICON1 ICON \"" + std::filesystem::path(picsfmltemp_dir_path).generic_string() + "/icon.ico\"\n";
    windwos_resource += "VS_VERSION_INFO VERSIONINFO\n";
    windwos_resource += " FILEVERSION ";
    windwos_resource += PicSFML::Version(project.GetConfig().Get("app_version")).AsString(',') + "\n";
    windwos_resource += " PRODUCTVERSION ";
    windwos_resource += PicSFML::Version(project.GetConfig().Get("app_version")).AsString(',') + "\n";
    windwos_resource += " FILEOS VOS_NT_WINDOWS32\n";
    windwos_resource += " FILETYPE VFT_APP\n";
    windwos_resource += "BEGIN\n";
    windwos_resource += "    BLOCK \"StringFileInfo\"\n";
    windwos_resource += "    BEGIN\n";
    windwos_resource += "        BLOCK \"040904B0\"\n";
    windwos_resource += "        BEGIN\n";
    windwos_resource += "            VALUE \"ProductName\", \"";
    windwos_resource += project.GetConfig().Get("output_name") + "\"\n";
    windwos_resource += "            VALUE \"FileVersion\", \"";
    windwos_resource += PicSFML::Version(project.GetConfig().Get("app_version")).AsString('.') + "\"\n";
    windwos_resource += "            VALUE \"ProductVersion\", \"";
    windwos_resource += PicSFML::Version(project.GetConfig().Get("app_version")).AsString('.') + "\"\n";
    windwos_resource += "            VALUE \"FileDescription\", \"";
    windwos_resource += project.GetConfig().Get("description") + "\"\n";
    windwos_resource += "        END\n";
    windwos_resource += "    END\n";
    windwos_resource += "    BLOCK \"VarFileInfo\"\n";
    windwos_resource += "    BEGIN\n";
    windwos_resource += "        VALUE \"Translation\", 0x0409, 1200\n";
    windwos_resource += "    END\n";
    windwos_resource += "END\n";

    PicSFML::Debug::Log("Creating resource.rc");

    std::ofstream windows_resource_file(picsfmltemp_dir_path + "\\resource.rc");

    if (!windows_resource_file.is_open())
    {
        PicSFML::Debug::Error("Could not open resource.rc for writing");
        return false;
    }

    windows_resource_file << windwos_resource;

    if (!windows_resource_file.good())
    {
        PicSFML::Debug::Error("Failed to write resource.rc");
        windows_resource_file.close();
        return false;
    }

    windows_resource_file.close();

    PicSFML::Debug::Log("Windows Resource: \n" + windwos_resource);

    PicSFML::Debug::Message("Compiling windows resources");

    CommandResult compile_windows_resource_result = RunCommand("cd \"" + picsfmltemp_dir_path + "\" && " + "\"" + project.GetConfig().Get("compiler_dir_path") + "\\bin\\windres.exe\" resource.rc -O coff -o resource.o");

    if (compile_windows_resource_result.exit_code != 0)
    {
        PicSFML::Debug::Error("windres failed");      
        PicSFML::Debug::Log("windres output: " + compile_windows_resource_result.output);
        return false;
    }

    PicSFML::Debug::Log("Windows resources created successfully");

    return true;
}

bool CompileProject(const PicSFML::Project &project, const std::string &picsfmltemp_dir_path, PicSFML::BuildType type)
{
    PicSFML::Debug::Log("Starting project compilation");

    const std::string &flags = (type == PicSFML::BuildType::Debug ? project.GetConfig().Get("debug_flags") : project.GetConfig().Get("release_flags"));

    std::string compile_command = "cd \"" + picsfmltemp_dir_path + "\" && " + "\"" + project.GetConfig().Get("compiler_dir_path") + "\\bin\\g++.exe\" -c " + flags + " ";

    if(project.GetConfig().Get("use_sfml_audio") == "true")
        compile_command += "-D" + PicSFML::SFMLAudioDefining + " ";

    if(project.GetConfig().Get("use_sfml_network") == "true")
        compile_command += "-D" + PicSFML::SFMLNetworkDefining + " ";

    compile_command += "-I\"" + project.GetConfig().Get("sfml_dir_path") + "\\include\" ";
    compile_command += "-I\"" + project.GetPath() + "\\" + PicSFML::SFMLApplicationDir + "\" ";
    compile_command += "-I\"" + project.GetPath() + "\\" + PicSFML::UserCodeDir + "\" ";

    int index = 0;
    std::string directory = project.GetConfig().GetList("include_directories", index++);
    while (!directory.empty())
    {
        compile_command += "-I\"" + directory + "\" ";
        directory = project.GetConfig().GetList("include_directories", index++);
    }

    compile_command += " \"" + project.GetPath() + "\\main.cpp\"";

    PicSFML::Debug::Message("Compiling main.cpp");
    PicSFML::Debug::Log("Compile command: " + compile_command);

    CommandResult compile_result = RunCommand(compile_command);

    if (compile_result.exit_code != 0)
    {
        PicSFML::Debug::Error("Compiler failed");
        PicSFML::Debug::Log("Compiler output: " + compile_result.output);
        return false;
    }

    PicSFML::Debug::Log("Compilation successful");

    return true;
}

bool LinkProject(const PicSFML::Project &project, const std::string &picsfmltemp_dir_path, PicSFML::BuildType type)
{
    PicSFML::Debug::Log("Starting project linking");

    const std::string sfml_suffix = type == PicSFML::BuildType::Debug ? "-d" : "";
    const std::string output_name = project.GetConfig().Get("output_name");

    std::string link_command = "cd \"" + picsfmltemp_dir_path + "\" && " + "\"" + project.GetConfig().Get("compiler_dir_path") + "\\bin\\g++.exe\" main.o resource.o -o ";

    link_command += "\"" + picsfmltemp_dir_path + "\\" + output_name + ".exe\" ";

    int index = 0;
    std::string library = project.GetConfig().GetList("library_files", index++);

    while (!library.empty())
    {
        link_command += "\"" + library + "\" ";
        library = project.GetConfig().GetList("library_files", index++);
    }

    link_command += "-L\"" + project.GetConfig().Get("sfml_dir_path") + "\\lib\" ";

    link_command += "-l" + sfml_dependencies[0] + sfml_suffix + " ";
    link_command += "-l" + sfml_dependencies[1] + sfml_suffix + " ";
    link_command += "-l" + sfml_dependencies[2] + sfml_suffix + " ";

    if (project.GetConfig().Get("use_sfml_audio") == "true")
        link_command += "-l" + sfml_dependencies[3] + sfml_suffix + " ";

    if (project.GetConfig().Get("use_sfml_network") == "true")
        link_command += "-l" + sfml_dependencies[4] + sfml_suffix + " ";

    if (type == PicSFML::BuildType::Release)
        link_command += "-mwindows";

    PicSFML::Debug::Message("Linking project");
    PicSFML::Debug::Log("Link command: " + link_command);

    CommandResult link_result = RunCommand(link_command);

    if (link_result.exit_code != 0)
    {
        PicSFML::Debug::Error("Linker failed");
        PicSFML::Debug::Log("Linker output: " + link_result.output);
        return false;
    }

    PicSFML::Debug::Log("Linking successful");

    return true;
}

bool CreateBuildDirectory(const PicSFML::Project &project, const std::string &picsfmltemp_dir_path, PicSFML::BuildType type)
{
    PicSFML::Debug::Log("Starting build directory creation");

    const std::string temp_project_build_directory = picsfmltemp_dir_path + "\\" + PicSFML::BuildDir + "\\" + (type == PicSFML::BuildType::Debug ? "Debug" : "Release");
    const std::string project_build_directory = project.GetPath() + "\\" + PicSFML::BuildDir + "\\" + (type == PicSFML::BuildType::Debug ? "Debug" : "Release");
    const std::string sfml_suffix = type == PicSFML::BuildType::Debug ? "-d-" : "-";
    const std::string project_exe = project.GetConfig().Get("output_name") + ".exe";
    const std::string temp_exe = picsfmltemp_dir_path + "\\" + project_exe;

    PicSFML::Debug::Log("Temporary build directory: " + temp_project_build_directory);
    PicSFML::Debug::Log("Project build directory: " + project_build_directory);

    std::error_code error_code;

    if (!std::filesystem::exists(temp_exe, error_code))
    {
        if (error_code)
            PicSFML::Debug::Error("Failed to check executable: " + error_code.message());
        else
            PicSFML::Debug::Error("Executable not found: " + temp_exe);

        return false;
    }

    PicSFML::Debug::Log("Creating temporary build directory");

    std::filesystem::remove_all(temp_project_build_directory, error_code);

    if (error_code)
    {
        PicSFML::Debug::Error("Failed to remove temporary build directory: " + error_code.message());
        return false;
    }

    error_code.clear();

    std::filesystem::create_directories(temp_project_build_directory, error_code);

    if (error_code)
    {
        PicSFML::Debug::Error("Failed to create temporary build directory: " + error_code.message());
        return false;
    }

    error_code.clear();

    std::filesystem::copy(temp_exe, temp_project_build_directory + "\\" + project_exe, error_code);

    if (error_code)
    {
        PicSFML::Debug::Error("Failed to copy executable: " + error_code.message());
        return false;
    }

    PicSFML::Debug::Message("Copying SFML DLLs");

    for (int i = 0; i < 5; i++)
    {
        if (i == 3 && project.GetConfig().Get("use_sfml_audio") == "false")
            continue;
        if (i == 4 && project.GetConfig().Get("use_sfml_network") == "false")
            continue;

        std::string sfml_binary = sfml_dependencies[i] + sfml_suffix + project.GetConfig().Get("sfml_version")[0] + ".dll";
        std::string source = project.GetConfig().Get("sfml_dir_path") + "\\bin\\" + sfml_binary;
        std::string destination = temp_project_build_directory + "\\" + sfml_binary;

        PicSFML::Debug::Log("Copying SFML DLL: " + source);

        error_code.clear();
        std::filesystem::copy(source, destination, error_code);

        if (error_code)
        {
            PicSFML::Debug::Error("Failed to copy SFML DLL: " + source);
            PicSFML::Debug::Log("Filesystem error: " + error_code.message());
            return false;
        }
    }

    PicSFML::Debug::Message("Copying compiler DLLs");

    for (int i = 0; i < 3; i++)
    {
        std::string source = project.GetConfig().Get("compiler_dir_path") + "\\bin\\" + compiler_binary[i];
        std::string destination = temp_project_build_directory + "\\" + compiler_binary[i];

        PicSFML::Debug::Log("Copying compiler DLL: " + source);

        error_code.clear();
        std::filesystem::copy(source, destination, error_code);

        if (error_code)
        {
            PicSFML::Debug::Error("Failed to copy compiler DLL: " + source);
            PicSFML::Debug::Log("Filesystem error: " + error_code.message());
            return false;
        }
    }

    PicSFML::Debug::Message("Copying additional binaries");

    int index = 0;
    std::string binary = project.GetConfig().GetList("binary_files", index++);
    while (!binary.empty())
    {
        std::string destination = temp_project_build_directory + "\\" + std::filesystem::path(binary).filename().string();

        PicSFML::Debug::Log("Copying binary: " + binary);

        error_code.clear();
        std::filesystem::copy(binary, destination, error_code);

        if (error_code)
        {
            PicSFML::Debug::Error("Failed to copy binary: " + binary);
            PicSFML::Debug::Log("Filesystem error: " + error_code.message());
            return false;
        }

        binary = project.GetConfig().GetList("binary_files", index++);
    }

    PicSFML::Debug::Message("Copying icon");

    std::string icon_source = project.GetPath() + "\\" + PicSFML::IconName;
    std::string icon_destination = temp_project_build_directory + "\\" + PicSFML::IconName;

    error_code.clear();
    std::filesystem::copy(icon_source, icon_destination, error_code);

    if (error_code)
    {
        PicSFML::Debug::Error("Failed to copy icon: " + error_code.message());
        return false;
    }

    PicSFML::Debug::Message("Copying resources");

    std::string resources_source = project.GetPath() + "\\" + PicSFML::ResourcesDir;
    std::string resources_destination = temp_project_build_directory + "\\" + PicSFML::ResourcesDir;

    error_code.clear();

    PicSFML::Debug::Log("Checking resources directory: " + resources_source);

    if (!std::filesystem::exists(resources_source, error_code))
    {
        if (error_code)
        {
            PicSFML::Debug::Error("Failed to check resources directory");
            PicSFML::Debug::Log("Filesystem error: " + error_code.message());
            return false;
        }

        PicSFML::Debug::Log("Resources directory does not exist, skipping copy");
    }
    else
    {
        PicSFML::Debug::Log("Resources directory exists");

        error_code.clear();
        std::filesystem::copy(resources_source, resources_destination, std::filesystem::copy_options::recursive, error_code);

        if (error_code)
        {
            PicSFML::Debug::Error("Failed to copy resources");
            PicSFML::Debug::Log("Filesystem error: " + error_code.message());
            return false;
        }
    }

    PicSFML::Debug::Log("Creating project build directory: " + project_build_directory);

    error_code.clear();
    std::filesystem::remove_all(project_build_directory, error_code);

    if (error_code)
    {
        PicSFML::Debug::Error("Failed to remove project build directory");
        PicSFML::Debug::Log("Filesystem error: " + error_code.message());
        return false;
    }

    error_code.clear();
    std::filesystem::create_directories(project_build_directory, error_code);

    if (error_code)
    {
        PicSFML::Debug::Error("Failed to create project build directory");
        PicSFML::Debug::Log("Filesystem error: " + error_code.message());
        return false;
    }

    PicSFML::Debug::Message("Copying build to project directory");

    error_code.clear();
    std::filesystem::copy(temp_project_build_directory, project_build_directory, std::filesystem::copy_options::recursive, error_code);

    if (error_code)
    {
        PicSFML::Debug::Error("Failed to copy build");
        PicSFML::Debug::Log("Filesystem error: " + error_code.message());
        return false;
    }

    error_code.clear();
    std::filesystem::remove_all(temp_project_build_directory, error_code);

    if (error_code)
    {
        PicSFML::Debug::Warning("Failed to remove temporary build directory: " + temp_project_build_directory);
        PicSFML::Debug::Log("Filesystem error: " + error_code.message());
    }

    PicSFML::Debug::Message("Build directory created successfully");

    return true;
}

bool CreateVSCodeFiles(const PicSFML::ProjectConfig &project_config, const std::string &project_path)
{
    PicSFML::Debug::Log("[CreateVSCodeFiles] Starting...");

    const std::string vscode_dir_path = project_path + "\\.vscode";
    const std::string vscode_properties_path = vscode_dir_path + "\\" + PicSFML::VSCodePropertiesName;

    std::error_code error_code;

    if (!std::filesystem::exists(vscode_dir_path, error_code))
    {
        if (error_code)
        {
            PicSFML::Debug::Error("[CreateVSCodeFiles] Failed to check .vscode directory: " + error_code.message());
            return false;
        }

        PicSFML::Debug::Log("[CreateVSCodeFiles] Creating .vscode directory...");

        std::filesystem::create_directories(vscode_dir_path, error_code);

        if (error_code)
        {
            PicSFML::Debug::Error("[CreateVSCodeFiles] Failed to create .vscode directory: " + error_code.message());
            return false;
        }
    }

    json j;

    if (std::filesystem::exists(vscode_properties_path, error_code))
    {
        if (error_code)
        {
            PicSFML::Debug::Error("[CreateVSCodeFiles] Failed to check properties file: " + error_code.message());
            return false;
        }

        PicSFML::Debug::Log("[CreateVSCodeFiles] Existing c_cpp_properties.json found, updating...");

        std::ifstream file(vscode_properties_path);

        if (!file.is_open())
        {
            PicSFML::Debug::Error("[CreateVSCodeFiles] Could not open existing c_cpp_properties.json");
            return false;
        }

        try
        {
            file >> j;
        }
        catch (const std::exception &e)
        {
            PicSFML::Debug::Error("[CreateVSCodeFiles] Failed to parse c_cpp_properties.json: " + std::string(e.what()));
            return false;
        }

        file.close();
    }
    else
    {
        PicSFML::Debug::Log("[CreateVSCodeFiles] c_cpp_properties.json does not exist, creating...");

        j["version"] = 4;
        j["configurations"] = json::array();
        j["configurations"].push_back({{"name", "Win32"},
                                       {"includePath", json::array()},
                                       {"defines", json::array()},
                                       {"compilerPath", ""},
                                       {"cStandard", "c17"},
                                       {"cppStandard", "c++17"},
                                       {"intelliSenseMode", "windows-gcc-x64"}});
    }

    if (!j.contains("configurations") || !j["configurations"].is_array())
    {
        PicSFML::Debug::Error("[CreateVSCodeFiles] Invalid c_cpp_properties.json: configurations is missing");
        return false;
    }

    if (j["configurations"].empty())
    {
        PicSFML::Debug::Warning("[CreateVSCodeFiles] No VSCode configurations found, creating one...");

        j["configurations"].push_back({{"name", "Win32"},
                                       {"includePath", json::array()}});
    }

    json &configuration = j["configurations"][0];

    if (!configuration.contains("includePath") || !configuration["includePath"].is_array())
    {
        PicSFML::Debug::Log("[CreateVSCodeFiles] Creating includePath...");
        configuration["includePath"] = json::array();
    }

    auto add_include_path = [&](const std::string &path)
    {
        if (path.empty())
            return;

        for (const auto &existing : configuration["includePath"])
            if (existing.is_string() && existing.get<std::string>() == path)
                return;

        configuration["includePath"].push_back(path);

        PicSFML::Debug::DebugLog("[CreateVSCodeFiles] Added include path: " + path);
    };

    add_include_path(project_config.Get("sfml_dir_path") + "\\include");
    add_include_path(project_path + "\\" + PicSFML::SFMLApplicationDir);
    add_include_path(project_path + "\\" + PicSFML::UserCodeDir);

    int index = 0;
    std::string directory = project_config.GetList("include_directories", index++);
    while (!directory.empty())
    {
        add_include_path(directory);
        directory = project_config.GetList("include_directories", index++);
    }

    if (!configuration.contains("compilerPath") || configuration["compilerPath"].get<std::string>().empty())
    {
        configuration["compilerPath"] = project_config.Get("compiler_dir_path") + "\\bin\\g++.exe";

        PicSFML::Debug::DebugLog("[CreateVSCodeFiles] Set compiler path: " + configuration["compilerPath"].get<std::string>());
    }

    if(project_config.Get("use_sfml_audio") == "true")
    {
        if(std::find(configuration["defines"].begin(), configuration["defines"].end(), PicSFML::SFMLAudioDefining) == configuration["defines"].end())
            configuration["defines"].push_back(PicSFML::SFMLAudioDefining);
    }
    else
    {
        configuration["defines"].erase(std::remove(configuration["defines"].begin(), configuration["defines"].end(), PicSFML::SFMLAudioDefining), 
            configuration["defines"].end());
    }

    if(project_config.Get("use_sfml_network") == "true")
    {
        if(std::find(configuration["defines"].begin(), configuration["defines"].end(), PicSFML::SFMLNetworkDefining) == configuration["defines"].end())
            configuration["defines"].push_back(PicSFML::SFMLNetworkDefining);
    }
    else
    {
        configuration["defines"].erase(std::remove(configuration["defines"].begin(), configuration["defines"].end(), PicSFML::SFMLNetworkDefining), 
            configuration["defines"].end());
    }

    std::ofstream file(vscode_properties_path);

    if (!file.is_open())
    {
        PicSFML::Debug::Error("[CreateVSCodeFiles] Could not open c_cpp_properties.json for writing");
        return false;
    }

    file << j.dump(4);

    if (!file.good())
    {
        PicSFML::Debug::Error("[CreateVSCodeFiles] Failed to write c_cpp_properties.json");
        return false;
    }

    file.close();

    PicSFML::Debug::Log("[CreateVSCodeFiles] VSCode files created/updated successfully");

    return true;
}

bool PicSFML::Enviorment::Init(const std::string &picsfml_directory)
{
    auto &instance = GetInstance();

    instance.picsfml_dir_path = std::filesystem::absolute(picsfml_directory).make_preferred().parent_path().string();
    instance.picsfmldata_dir_path = std::getenv("LOCALAPPDATA") + std::string("\\") + PicSFMLDataDir;
    instance.picsfmltemp_dir_path = std::getenv("TEMP") + std::string("\\") + PicSFMLTempDir;

    Debug::Init(instance.picsfmldata_dir_path);

    Debug::Message("Initializing PicSFML...");
    Debug::Log("PicSFML directory: " + instance.picsfml_dir_path);
    Debug::Log("PicSFML data directory: " + instance.picsfmldata_dir_path);
    Debug::Log("PicSFML temp directory: " + instance.picsfmltemp_dir_path);

    std::error_code error_code;

    if (!std::filesystem::exists(instance.picsfmldata_dir_path, error_code))
    {
        Debug::Log("PicSFML data directory does not exist, creating it...");

        std::filesystem::create_directory(instance.picsfmldata_dir_path, error_code);

        if (error_code)
        {
            Debug::Error("Failed to create PicSFML data directory: " + error_code.message());
            return false;
        }
    }

    error_code.clear();

    if (!std::filesystem::exists(instance.picsfmltemp_dir_path, error_code))
    {
        Debug::Log("PicSFML temp directory does not exist, creating it...");

        std::filesystem::create_directory(instance.picsfmltemp_dir_path, error_code);

        if (error_code)
        {
            Debug::Error("Failed to create PicSFML temp directory: " + error_code.message());
            return false;
        }
    }

    Settings &settings = instance.settings;
    ConfigDefaults &config_defaults = instance.config_defaults;

    Debug::Log("Loading settings...");

    if (!settings.Load(instance.picsfmldata_dir_path))
    {
        Debug::Warning("Settings file not found, creating settings...");

        settings.Set("debug_mode", "normal");

        if (!settings.Save(instance.picsfmldata_dir_path))
        {
            Debug::Error("Failed to create settings file");
            return false;
        }
    }

    Debug::Log("Loading config defaults...");

    if (!config_defaults.Load(instance.picsfmldata_dir_path))
    {
        Debug::Warning("Config defaults not found, creating defaults...");

        if (!config_defaults.Save(instance.picsfmldata_dir_path))
        {
            Debug::Error("Failed to create config defaults");
            return false;
        }
    }

    instance.initialized = true;

    Debug::Message("PicSFML initialized successfully");
    
    return true;
}

bool PicSFML::Enviorment::SetProject(const std::string &project_path)
{
    auto &instance = GetInstance();

    if (!instance.initialized)
    {
        Debug::Error("PicSFML was not initialized");
        return false;
    }

    if(instance.project_set)
    {
        Debug::Error("There is a PicSFML project allready set");
        return false;
    }

    Debug::Message("Loading project: " + project_path);

    Project load_project;

    if (!load_project.Load(project_path))
    {
        Debug::Error("Failed to load project: " + project_path);
        return false;
    }

    Debug::Log("Project loaded successfully");

    const std::string project_version = load_project.GetConfig().Get("picsfml_version");

    if (project_version != PicSFMLVersion)
    {
        Debug::Warning("PicSFML version mismatch, you may have problems with building your project");
        Debug::Log("Project version: " + project_version);
        Debug::Log("Current version: " + PicSFMLVersion);
    }

    if (load_project.GetConfig().Get("create_vscode_files") == "true")
        if (!CreateVSCodeFiles(load_project.GetConfig(), load_project.GetPath()))
            Debug::Error("Failed to create/update the VSCode files");

    instance.project = load_project;
    instance.project_set = true;

    Debug::Message("Project set successfully");

    return true;
}

bool PicSFML::Enviorment::HasProject()
{
    auto &instance = GetInstance();

    if (!instance.initialized)
    {
        Debug::Error("Cannot check project: PicSFML was not initialized");
        return false;
    }

    Debug::Log("Checking if a project is set");

    if (instance.project_set)
    {
        Debug::Log("Project is set");
        return true;
    }

    Debug::Log("No project is set");
    return false;
}

void PicSFML::Enviorment::UnsetProject()
{
    auto &instance = GetInstance();

    if (!instance.initialized)
    {
        Debug::Error("Cannot unset project: PicSFML was not initialized");
        return;
    }

    Debug::Message("Unsetting current project");

    if (!instance.project_set)
    {
        Debug::Log("No project is currently set");
        return;
    }

    instance.project = Project();
    instance.project_set = false;

    Debug::Message("Project unset successfully");
}

std::optional<std::reference_wrapper<PicSFML::ProjectConfig>> PicSFML::Enviorment::GetProjectConfig()
{
    auto &instance = GetInstance();

    Debug::Log("Getting project configuration");

    if (!instance.initialized)
    {
        Debug::Error("Cannot get project configuration: PicSFML was not initialized");
        return std::nullopt;
    }

    if (!instance.project_set)
    {
        Debug::Error("Cannot get project configuration: no project is set");
        return std::nullopt;
    }

    Debug::Log("Project configuration retrieved successfully");

    return instance.project.GetConfig();
}

std::optional<std::reference_wrapper<PicSFML::ConfigDefaults>> PicSFML::Enviorment::GetConfigDefaults()
{
    auto &instance = GetInstance();

    Debug::Log("Getting default configuration");

    if (!instance.initialized)
    {
        Debug::Error("Cannot get default configuration: PicSFML was not initialized");
        return std::nullopt;
    }

    Debug::Log("Default configuration retrieved successfully");

    return instance.config_defaults;
}

std::optional<std::reference_wrapper<PicSFML::Settings>> PicSFML::Enviorment::GetSettings()
{
    auto &instance = GetInstance();

    Debug::Log("Getting settings");

    if (!instance.initialized)
    {
        Debug::Error("Cannot get settings: PicSFML was not initialized");
        return std::nullopt;
    }

    Debug::Log("Settings retrieved successfully");

    return instance.settings;
}

bool PicSFML::Enviorment::SaveProjectConfig()
{
    auto &instance = GetInstance();

    Debug::Message("Saving project configuration");

    if (!instance.initialized)
    {
        Debug::Error("Cannot save project configuration: PicSFML was not initialized");
        return false;
    }

    if (!instance.project_set)
    {
        Debug::Error("Cannot save project configuration: no project is set");
        return false;
    }

    if (!instance.project.SaveConfig())
    {
        Debug::Error("Failed to save project configuration");
        return false;
    }

    if (instance.project.GetConfig().Get("create_vscode_files") == "true")
        if (!CreateVSCodeFiles(instance.project.GetConfig(), instance.project.GetPath()))
            Debug::Error("Failed to create/update the VSCode files");

    Debug::Message("Project configuration saved successfully");

    return true;
}

bool PicSFML::Enviorment::SaveConfigDefaults()
{
    auto &instance = GetInstance();

    Debug::Message("Saving default project configuration");

    if (!instance.initialized)
    {
        Debug::Error("Cannot save default project configuration: PicSFML was not initialized");
        return false;
    }

    if (!instance.config_defaults.Check())
    {
        Debug::Error("Default project configuration is not valid");
        return false;
    }

    if (!instance.config_defaults.Save(instance.picsfmldata_dir_path))
    {
        Debug::Error("Failed to save default project configuration");
        return false;
    }

    Debug::Message("Default project configuration saved successfully");

    return true;
}

bool PicSFML::Enviorment::SaveSettings()
{
    auto &instance = GetInstance();

    Debug::Message("Saving settings");

    if (!instance.initialized)
    {
        Debug::Error("Cannot save settings: PicSFML was not initialized");
        return false;
    }

    if (!instance.settings.Check())
    {
        Debug::Error("Settings are not valid");
        return false;
    }

    if (!instance.settings.Save(instance.picsfmldata_dir_path))
    {
        Debug::Error("Failed to save settings");
        return false;
    }

    Debug::Message("Settings saved successfully");

    return true;
}

bool PicSFML::Enviorment::CreateProjectConfig(const std::string &path)
{
    auto &instance = GetInstance();

    Debug::Message("Creating project configuration");
    Debug::Log("Project configuration location: " + path);

    if (!instance.initialized)
    {
        Debug::Error("Cannot create project configuration: PicSFML was not initialized");
        return false;
    }

    if (!std::filesystem::exists(path))
    {
        Debug::Error("Cannot create project configuration: directory does not exist: " + path);
        return false;
    }

    if (std::filesystem::exists(path + "\\" + ProjectConfigName))
    {
        Debug::Error("Cannot create project configuration: configuration file already exists");
        return false;
    }

    Debug::Log("Creating default project configuration");

    ProjectConfig new_project_config;

    new_project_config.Set("project_name", std::filesystem::path(path).filename().string());
    new_project_config.Set("output_name", instance.config_defaults.Get("output_name"));
    new_project_config.Set("compiler_dir_path", instance.config_defaults.Get("compiler_dir_path"));
    new_project_config.Set("sfml_dir_path", instance.config_defaults.Get("sfml_dir_path"));
    new_project_config.Set("sfml_version", instance.config_defaults.Get("sfml_version"));
    new_project_config.Set("create_vscode_files", instance.config_defaults.Get("create_vscode_files"));
    new_project_config.Set("picsfml_version", PicSFMLVersion);
    new_project_config.Set("date_created", GetTimestamp("%d/%m/%Y %H:%M"));

    Debug::Log("Saving project configuration");

    if (!new_project_config.Save(path))
    {
        Debug::Error("Failed to save project configuration");
        return false;
    }
    
    if (new_project_config.Get("create_vscode_files") == "true")
        if (!CreateVSCodeFiles(new_project_config, path))
            Debug::Error("Failed to create/update the VSCode files");

    Debug::Message("Project configuration created successfully");

    return true;
}

bool PicSFML::Enviorment::CreateProject(const CreateProjectContext &create_projectd_context)
{
    auto &instance = GetInstance();

    if (!instance.initialized)
    {
        Debug::Error("PicSFML was not initialized");
        return false;
    }

    if(create_projectd_context.project_name.empty())
    {
        Debug::Error("Project can not be created without a project name");
        return false;
    }

    const Settings &settings = instance.settings;

    std::error_code error_code;

    std::string project_dir_path = create_projectd_context.project_dir_path;

    if (!std::filesystem::exists(project_dir_path, error_code))
    {
        if (error_code)
        {
            Debug::Warning("Could not check project directory path: " + project_dir_path);
            Debug::Log("Filesystem error: " + error_code.message());
        }

        Debug::Log("Project directory path not found, using default: " + settings.Get("projects_directory"));
        project_dir_path = settings.Get("projects_directory");
    }

    error_code.clear();

    if (!std::filesystem::exists(project_dir_path, error_code))
    {
        if (error_code)
        {
            Debug::Error("Could not check project directory: " + project_dir_path);
            Debug::Log("Filesystem error: " + error_code.message());
        }
        else
        {
            Debug::Error("Project directory does not exist: " + project_dir_path);
        }

        return false;
    }

    const std::string project_path = project_dir_path + "\\" + create_projectd_context.project_name;
    const std::string temp_project_path = instance.picsfmltemp_dir_path + "\\" + create_projectd_context.project_name;

    const ConfigDefaults &config_defaults = instance.config_defaults;

    error_code.clear();

    if (std::filesystem::exists(project_path, error_code))
    {
        Debug::Error("Project already exists: " + project_path);
        return false;
    }

    if (error_code)
    {
        Debug::Error("Could not check if project exists: " + project_path);
        Debug::Log("Filesystem error: " + error_code.message());
        return false;
    }

    std::string compiler_dir_path = create_projectd_context.compiler_dir_path;

    error_code.clear();

    if (!std::filesystem::exists(compiler_dir_path, error_code))
    {
        if (error_code)
        {
            Debug::Warning("Could not check compiler path: " + compiler_dir_path);
            Debug::Log("Filesystem error: " + error_code.message());
        }

        Debug::Log("Compiler path not found, using default: " + config_defaults.Get("compiler_dir_path"));
        compiler_dir_path = config_defaults.Get("compiler_dir_path");
    }

    error_code.clear();

    if (!std::filesystem::exists(compiler_dir_path, error_code))
    {
        if (error_code)
        {
            Debug::Error("Could not check compiler directory: " + compiler_dir_path);
            Debug::Log("Filesystem error: " + error_code.message());
        }
        else
        {
            Debug::Error("Compiler directory does not exist: " + compiler_dir_path);
        }

        return false;
    }

    std::string sfml_dir_path = create_projectd_context.sfml_dir_path;

    error_code.clear();

    if (!std::filesystem::exists(sfml_dir_path, error_code))
    {
        if (error_code)
        {
            Debug::Warning("Could not check SFML path: " + sfml_dir_path);
            Debug::Log("Filesystem error: " + error_code.message());
        }

        Debug::Log("SFML path not found, using default: " + config_defaults.Get("sfml_dir_path"));
        sfml_dir_path = config_defaults.Get("sfml_dir_path");
    }

    error_code.clear();

    if (!std::filesystem::exists(sfml_dir_path, error_code))
    {
        if (error_code)
        {
            Debug::Error("Could not check SFML directory: " + sfml_dir_path);
            Debug::Log("Filesystem error: " + error_code.message());
        }
        else
        {
            Debug::Error("SFML directory does not exist: " + sfml_dir_path);
        }

        return false;
    }

    std::string sfml_version = create_projectd_context.sfml_version;

    if (sfml_version.empty())
    {
        Debug::Log("SFML version is empty, using default: " + config_defaults.Get("sfml_version"));
        sfml_version = config_defaults.Get("sfml_version");
    }

    std::string output_name = create_projectd_context.output_name;

    if (output_name.empty())
    {
        Debug::Log("Output name is empty, using default: " + config_defaults.Get("output_name"));
        output_name = config_defaults.Get("output_name");
    }

    error_code.clear();

    std::filesystem::remove_all(temp_project_path, error_code);

    if (error_code)
    {
        Debug::Error("Could not remove old temporary project: " + temp_project_path);
        Debug::Log("Filesystem error: " + error_code.message());
        return false;
    }

    error_code.clear();

    PicSFML::Debug::Message("Creating project '" + create_projectd_context.project_name + "'");

    Debug::Log("Creating temporary project directory: " + temp_project_path);

    std::filesystem::create_directory(temp_project_path, error_code);

    if (error_code)
    {
        Debug::Error("Could not create temporary project directory: " + temp_project_path);
        Debug::Log("Filesystem error: " + error_code.message());
        return false;
    }

    std::string template_path = instance.picsfml_dir_path + "\\" + TemplateDir + "\\" + "SFML-" + sfml_version;

    error_code.clear();

    if (!std::filesystem::exists(template_path, error_code))
    {
        if (error_code)
        {
            Debug::Error("Could not check template: " + template_path);
            Debug::Log("Filesystem error: " + error_code.message());
        }
        else
        {
            Debug::Error("Template does not exist: " + template_path);
        }

        return false;
    }

    Debug::Message("Copying project template");
    Debug::Log("Template: " + template_path);
    Debug::Log("Destination: " + temp_project_path);

    error_code.clear();

    std::filesystem::copy(template_path, temp_project_path, std::filesystem::copy_options::recursive, error_code);

    if (error_code)
    {
        Debug::Error("Failed to copy project template");
        Debug::Log("Filesystem error: " + error_code.message());
        return false;
    }

    ProjectConfig project_config;

    project_config.Set("project_name", create_projectd_context.project_name);
    project_config.Set("compiler_dir_path", compiler_dir_path);
    project_config.Set("sfml_dir_path", sfml_dir_path);
    project_config.Set("sfml_version", sfml_version);
    project_config.Set("output_name", output_name);
    project_config.Set("create_vscode_files", create_projectd_context.create_vscode_files ? "true" : "false");
    project_config.Set("picsfml_version", PicSFMLVersion);
    project_config.Set("date_created", GetTimestamp("%d/%m/%Y %H:%M"));

    Debug::Log("Saving project configuration");

    if (!project_config.Save(temp_project_path))
    {
        Debug::Error("Failed to save project configuration: " + temp_project_path + "\\" + ProjectConfigName);

        error_code.clear();
        std::filesystem::remove_all(temp_project_path, error_code);

        if (error_code)
            Debug::Warning("Failed to clean up temporary project: " + error_code.message());

        return false;
    }

    Debug::Message("Copying project to final location");
    Debug::Log("Source: " + temp_project_path);
    Debug::Log("Destination: " + project_path);

    error_code.clear();

    std::filesystem::copy(temp_project_path, project_path, std::filesystem::copy_options::recursive, error_code);

    if (error_code)
    {
        Debug::Error("Failed to copy project");
        Debug::Log("Filesystem error: " + error_code.message());

        error_code.clear();
        std::filesystem::remove_all(temp_project_path, error_code);

        if (error_code)
            Debug::Warning("Failed to clean up temporary project: " + error_code.message());

        return false;
    }

    if (create_projectd_context.create_vscode_files)
        if (!CreateVSCodeFiles(project_config, project_path))
            Debug::Error("Failed to create/update the VSCode files");

    error_code.clear();

    std::filesystem::remove_all(temp_project_path, error_code);

    if (error_code)
    {
        Debug::Warning("Failed to remove temporary project: " + temp_project_path);
        Debug::Log("Filesystem error: " + error_code.message());
    }

    Debug::Message("Project created successfully: " + project_path);

    return true;
}

bool PicSFML::Enviorment::BuildProject(BuildType type)
{
    auto &instance = GetInstance();

    if (!instance.initialized)
    {
        Debug::Error("PicSFML was not initialized");
        return false;
    }

    if (!instance.project_set)
    {
        Debug::Error("No project is set");
        return false;
    }

    const Project &project = instance.project;
    const std::string &picsfml_dir_path = instance.picsfml_dir_path;
    const std::string &picsfmltemp_dir_path = instance.picsfmltemp_dir_path;

    Debug::Message("Starting build");
    PicSFML::Debug::Message("Building project: " + project.GetConfig().Get("project_name"));
    PicSFML::Debug::Message("Application version: " + project.GetConfig().Get("app_version"));
    PicSFML::Debug::Message("Build type: " + PicSFML::BuildTypes[int(type)]);

    Debug::Log("Project: " + project.GetPath());

    if (!project.GetConfig().Check())
    {
        Debug::Error("Project configuration check failed");
        return false;
    }

    Debug::Message("Creating Windows resources");

    if (!CreateWindowsResources(project, picsfmltemp_dir_path, picsfml_dir_path))
    {
        Debug::Error("Failed to create Windows resources");
        return false;
    }

    Debug::Message("Compiling project");

    if (!CompileProject(project, picsfmltemp_dir_path, type))
    {
        Debug::Error("Failed to compile project");
        return false;
    }

    Debug::Message("Linking project");

    if (!LinkProject(project, picsfmltemp_dir_path, type))
    {
        Debug::Error("Failed to link project");
        return false;
    }

    Debug::Message("Creating build directory");

    if (!CreateBuildDirectory(project, picsfmltemp_dir_path, type))
    {
        Debug::Error("Failed to create build directory");
        return false;
    }

    Debug::Message("Build completed successfully");

    return true;
}
