#include <Debug.hpp>

#include <thread>
#include <chrono>
#include <iomanip>
#include <string>
#include <sstream>
#include <filesystem>
#include <iostream>

std::string PicSFML::GetTimestamp(const char *form)
{
    auto system_time = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(system_time);

    std::tm local_tm{};

#ifdef _WIN32
    localtime_s(&local_tm, &time);
#else
    localtime_r(&time, &local_tm);
#endif

    std::ostringstream oss;
    oss << std::put_time(&local_tm, form);
    return oss.str();
}

std::string GetTypeName(PicSFML::Debug::Type type)
{
    if(type == PicSFML::Debug::Type::Message)
        return "MESSAGE";
    else if(type == PicSFML::Debug::Type::Log)
        return "LOG";
    else if(type == PicSFML::Debug::Type::DebugLog)
        return "DEBUG";
    else if(type == PicSFML::Debug::Type::Warning)
        return "WARNING";
    else if(type == PicSFML::Debug::Type::Error)
        return "ERROR";
    
    return "LOG";
}

void PicSFML::Debug::Write(Type type, const std::string &message)
{
    auto &instance = GetInstance();

    if(!instance.initialized) return;

    std::string time = GetTimestamp("%H:%M:%S");
    std::string type_name = GetTypeName(type);

    std::string formated_message = "[" + time + "] [" + type_name + "] " + message;

    if(!instance.log_file.is_open())
        instance.log_file.open(instance.log_file_path, std::ios::app);

    instance.log_file << formated_message << '\n';

    bool output = false;

    if(instance.mode == Mode::Verbose) output = true;
    else if (instance.mode == Mode::Quiet) output = false;
    else if (instance.mode == Mode::Normal) output = (type == Type::Message || type == Type::Error);

    if(output)
    {
        std::cout << formated_message << '\n';

        for(const auto &subscriber : instance.subscribers)
            if(subscriber) 
                subscriber({type, formated_message});
    }
}

bool PicSFML::Debug::Init(const std::string &picsfmldata_dir_path)
{
    auto &instance = GetInstance();

    if(instance.initialized)
        return true;

    if(picsfmldata_dir_path.empty())
        return false;

    instance.log_directory = picsfmldata_dir_path + "\\" + LogsDir;

    std::error_code error_code;

    if(!std::filesystem::exists(instance.log_directory, error_code))
    {
        if(error_code)
            return false;

        std::filesystem::create_directories(instance.log_directory, error_code);

        if(error_code)
            return false;
    }

    instance.log_file_path = instance.log_directory + "\\" + GetTimestamp("%d-%m-%Y_%H-%M-%S") + LogFileExtension;

    instance.log_file.open(instance.log_file_path, std::ios::out | std::ios::app);

    if(!instance.log_file.is_open())
        return false;

    instance.initialized = true;

    return true;
}

void PicSFML::Debug::SetMode(Mode mode)
{
    GetInstance().mode = mode;
}

PicSFML::Debug::Mode PicSFML::Debug::GetMode()
{
    return GetInstance().mode;
}

std::string PicSFML::Debug::GetLogFilePath()
{
    return GetInstance().log_file_path;
}

std::string PicSFML::Debug::GetLogDirectory()
{
    return GetInstance().log_directory;
}

void PicSFML::Debug::Message(const std::string &message)
{
    auto &instance = GetInstance();

    if(!instance.initialized) return;

    instance.Write(Type::Message, message);
}

void PicSFML::Debug::Log(const std::string &message)
{
    auto &instance = GetInstance();

    if(!instance.initialized) return;

    instance.Write(Type::Log, message);
}

void PicSFML::Debug::DebugLog(const std::string &message)
{
    auto &instance = GetInstance();

    if(!instance.initialized) return;

    instance.Write(Type::DebugLog, message);
}

void PicSFML::Debug::Warning(const std::string &message)
{
    auto &instance = GetInstance();

    if(!instance.initialized) return;

    instance.Write(Type::Warning, message);
}

void PicSFML::Debug::Error(const std::string &message)
{
    auto &instance = GetInstance();

    if(!instance.initialized) return;

    instance.Write(Type::Error, message);
}

void PicSFML::Debug::Subscribe(const std::function<void(const Entry &)> &callback)
{
    auto &instance = GetInstance();

    if(!instance.initialized) return;

    instance.subscribers.push_back(callback);
}
