#include <Version.hpp>

#include <cmath>
#include <string>
#include <sstream>

PicSFML::Version::Version(const std::string &version_s)
{
    Parse(version_s);
}

void PicSFML::Version::Parse(const std::string &version_s)
{
    std::stringstream ss(version_s);
    std::string part;

    while (std::getline(ss, part, '.'))
        version.push_back(std::stoi(part));
}

std::string PicSFML::Version::AsString(char delim)
{
    std::string version_s;

    for(int i = 0; i < version.size(); i++)
    {
        version_s += std::to_string(version[i]);

        if(i + 1 < version.size())
            version_s += delim;
    }

    return version_s;
}

int PicSFML::Version::AsNumber()
{
    int version_n = 0;

    for(int i = 0; i < version.size(); i++)
    {
        if(version[i] == 0) 
        {
            version_n = version_n * 10 + version[i]; 
            continue;
        }
        
        version_n = version_n * pow(10, log10(version[i])) + version[i];
    }

    return version_n;
}