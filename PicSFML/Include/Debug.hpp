#pragma once

#include <string>
#include <functional>
#include <fstream>

namespace PicSFML
{
    std::string GetTimestamp(const char *form);

    const std::string LogsDir = "Logs";
    const std::string LogFileExtension = ".log";

    class Debug
    {
    public:
        enum class Mode
        {
            Quiet,
            Normal,
            Verbose
        };

        enum class Type
        {
            Message,
            Log,
            DebugLog,
            Warning,
            Error
        };

        struct Entry
        {
            Type type;
            std::string message;
        };
    

    private:
        Debug() = default;
    
        bool initialized = false;
        Mode mode = Mode::Normal;

        std::string log_directory;
        std::string log_file_path;

        std::ofstream log_file;

        std::vector<std::function<void(const Entry &)>> subscribers;

        void Write(Type type, const std::string &message);

    public:
        Debug(const Debug &) = delete;
        Debug &operator=(const Debug &) = delete;

        static bool Init(const std::string &picsfmldata_dir_path);

        static void SetMode(Mode mode);
        static Mode GetMode(); 

        static std::string GetLogFilePath();
        static std::string GetLogDirectory();

        static void Message(const std::string &message);
        static void Log(const std::string &message);
        static void DebugLog(const std::string &message);
        static void Warning(const std::string &message);
        static void Error(const std::string &message);

        static void Subscribe(const std::function<void(const Entry &)> &callback);

        static Debug &GetInstance()
        {
            static Debug instance;
            return instance;
        }
    };
}