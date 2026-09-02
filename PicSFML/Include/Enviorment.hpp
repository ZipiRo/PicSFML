#pragma once

#include <vector>
#include <string>
#include <optional>
#include <functional>

#include <Settings.hpp>
#include <ProjectConfig.hpp>
#include <ConfigDefaults.hpp>

#include <Project.hpp>

namespace PicSFML
{
    const std::string PicSFMLDataDir = "PicSFMLData";
    const std::string PicSFMLTempDir = "PicSFMLTemp";
    const std::string TemplateDir = "Template";
    const std::string BuildDir = "Build";
    const std::string SFMLApplicationDir = "PicSFML";
    const std::string UserCodeDir = "Code";
    const std::string ResourcesDir = "Resources";
    const std::string VSCodePropertiesName = "c_cpp_properties.json";

    const std::string IconName = "icon.png";

    const std::string PicSFMLVersion = "3.0.0";

    const std::string SFMLAudioDefining = "AUDIO";
    const std::string SFMLNetworkDefining = "NETWORK";

    extern const std::vector<std::string> BuildTypes; 

    enum class BuildType
    {
        Debug,
        Release
    };

    struct CreateProjectContext
    {
        std::string project_name;
        std::string output_name;
        std::string project_dir_path;
        std::string compiler_dir_path;
        std::string sfml_dir_path;
        std::string sfml_version;
        bool create_vscode_files;
    };

    class Enviorment
    {
    private:
        Enviorment() = default;

        Settings settings;
        ConfigDefaults config_defaults;
        Project project;

        std::string picsfml_dir_path;
        std::string picsfmldata_dir_path;
        std::string picsfmltemp_dir_path;

        bool initialized = false;
        bool project_set = false;

    public:
        Enviorment(const Enviorment &) = delete;
        Enviorment &operator=(const Enviorment &) = delete;

        static void Init(const std::string &picsfml_directory);
        static bool SetProject(const std::string &projct_path);
        static bool HasProject();
        static void UnsetProject();
        static bool SaveProjectConfig();
        static bool CreateProjectConfig(const std::string &path);
        static std::optional<std::reference_wrapper<ProjectConfig>> GetProjectConfig();
        
        static std::optional<std::reference_wrapper<ConfigDefaults>> GetConfigDefaults();
        static std::optional<std::reference_wrapper<Settings>> GetSettings();
        static bool SaveConfigDefaults();
        static bool SaveSettings();

        static bool CreateProject(const CreateProjectContext &create_projectd_context);
        static bool BuildProject(BuildType type);

        static Enviorment &GetInstance()
        {
            static Enviorment instance;
            return instance;
        }
    };
}