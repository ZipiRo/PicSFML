#pragma once

#include <ProjectConfig.hpp>

#include <string>

namespace PicSFML
{
    class Project
    {
    private:
        ProjectConfig project_config;
        std::string project_path;

    public:
        bool Load(const std::string &from);

        std::string GetPath() const;
        ProjectConfig &GetConfig();
        const ProjectConfig &GetConfig() const;
        bool SaveConfig();
    };
}
