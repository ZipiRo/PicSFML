#pragma once
#include <string>

#include <Data.hpp>

namespace PicSFML
{
    const std::string SettingsName = ".picsfml_settings";

    const int SettingsVersion = 1;

    class Settings : public Data
    {
    private:
        std::string projects_directory;
        std::string debug_mode;

    public:
        bool Check() const override;
        bool Load(const std::string &path) override;
        bool Save(const std::string &path) override;

        bool Set(const std::string &field, const std::string &value) override;
        std::string Get(const std::string &field) const override;
    };
}