#include <string>
#include <vector>

#include <Version.h>

using namespace PicSFML;

Version::Version(const std::string sversion) { ParseString(sversion); }

int Version::operator[](size_t i) const { return version[i]; }

void Version::ParseString(const std::string &sversion)
{
    version.clear();

    std::string current;

    for (char c : sversion)
    {
        if (c >= '0' && c <= '9')
        {
            current += c;
        }
        else if (!current.empty())
        {
            version.push_back(std::stoi(current));
            current.clear();
        }
    }

    if (!current.empty())
        version.push_back(std::stoi(current));
}

std::string Version::AsString(char delimitator) const
{
    std::string sversion;

    for (size_t i = 0; i < version.size(); i++)
    {
        sversion += std::to_string(version[i]);

        if (i + 1 < version.size())
            sversion += delimitator;
    }

    return sversion;
}

int Version::AsInt() const
{
    int iversion = 0;

    for (size_t i = 0; i < version.size(); i++)
        iversion = iversion * 10 + version[i];

    return iversion;
}