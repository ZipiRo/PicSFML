#pragma once
#include <string>
#include <vector>

namespace PicSFML
{
    class Version
    {
    private:
        std::vector<int> version;

    public:
        Version() {}
        Version(const std::string &version_s);
        void Parse(const std::string &version_s);

        std::string AsString(char delim);
        int AsNumber();
    };     
}