#pragma once
#include <string>
#include <vector>

struct PVersion
{
    std::vector<int> version;

    PVersion() {}
    PVersion(const std::string s_version)
    {
        ParseString(s_version);
    }

    int operator[](size_t index) const
    {
        return version[index];
    }

    void ParseString(const std::string& s_version)
    {
        version.clear();

        std::string current;

        for (char c : s_version)
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

    std::string AsString(char delim) const
    {
        std::string s_version;

        for (size_t i = 0; i < version.size(); i++)
        {
            s_version += std::to_string(version[i]);

            if (i + 1 < version.size())
                s_version += delim;
        }

        return s_version;
    }

    int AsInt() const
    {
        int i_version = 0;

        for (size_t i = 0; i < version.size(); i++)
            i_version = i_version * 10 + version[i];

        return i_version;
    }
};