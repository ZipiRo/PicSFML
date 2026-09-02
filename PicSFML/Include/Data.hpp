#pragma once

#include <vector>
#include <string>


namespace PicSFML
{
    extern const std::vector<std::string> SFMLValidVersions;
 
    class Data
    {
    public:
        virtual bool Check() const = 0;
        virtual bool Load(const std::string &path) = 0;
        virtual bool Save(const std::string &path) = 0;

        virtual bool Set(const std::string &field, const std::string &value) { return false; }
        virtual bool SetList(const std::string &field, const std::string &value, int index) { return false; }
        virtual std::string Get(const std::string &field) const { return ""; }
        virtual std::string GetList(const std::string &field, int index) const { return ""; }
        virtual bool Add(const std::string &field, const std::string &value) { return false; }
        virtual bool Remove(const std::string &field, const std::string &value) { return false; }
    };
}