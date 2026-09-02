#pragma once

#include <Data.hpp>

#include <string>
#include <vector>

namespace PicSFML
{
    const std::string ProjectConfigName = ".picsfml_config";

    const int ProjectConfigVersion = 1;
    
    class ProjectConfig : public Data
    {
    private:
        std::string picsfml_version;
        std::string project_name;
        std::string description;
        std::string output_name;
        std::string compiler_dir_path;
        std::string sfml_dir_path;
        std::string sfml_version;
        std::string app_version = "1.0.0.0";
        std::string debug_flags;
        std::string release_flags;
        std::string date_created;
        std::vector<std::string> include_directories;
        std::vector<std::string> library_files;
        std::vector<std::string> binary_files;
        bool create_vscode_files = false;
        bool use_sfml_audio = false;
        bool use_sfml_network = false;

    public:
        bool Check() const override;
        bool Load(const std::string &path) override;
        bool Save(const std::string &path) override;

        bool Set(const std::string &field, const std::string &value) override;
        bool SetList(const std::string &field, const std::string &value, int index) override;
        std::string Get(const std::string &field) const override;
        std::string GetList(const std::string &field, int index) const override;
        bool Add(const std::string &field, const std::string &value) override;
        bool Remove(const std::string &field, const std::string &value) override;
    };
}