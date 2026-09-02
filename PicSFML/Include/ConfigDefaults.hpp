#pragma once
#include <string>

#include <Data.hpp>

namespace PicSFML 
{
    const std::string DefaultsName = ".picsfml_config_defaults";

    const int ConfigDefaultsVersion = 1;

    class ConfigDefaults : public Data
    {
    private:
        std::string output_name;
        std::string compiler_dir_path;
        std::string sfml_dir_path;
        std::string sfml_version;
        bool create_vscode_files;

    public:
        bool Check() const override;
        bool Load(const std::string &path) override;
        bool Save(const std::string &path) override;

        bool Set(const std::string &field, const std::string &value) override;
        std::string Get(const std::string &field) const override;
    };
}