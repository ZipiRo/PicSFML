#include <PicSFML.hpp>
#include <Parser.hpp>

#include <iostream>
#include <string>

#ifdef _WIN32
    #include <windows.h>

    std::string GetExecutablePath() 
    {
        char buffer[MAX_PATH];
        GetModuleFileNameA(NULL, buffer, MAX_PATH);
        return std::string(buffer);
    }
#else

#endif

bool CreateCommand(const std::vector<ParsedCommand::Option> &options)
{
    PicSFML::CreateProjectContext create_context;

    for(const auto &option : options)
    {
        if(option.name == "name")
        {
            create_context.project_name = option.argument;
        }
        else if(option.name == "output")
        {
            create_context.output_name = option.argument;
        }
        else if(option.name == "project_path")
        {
            create_context.project_dir_path = option.argument;
        }
        else if(option.name == "compiler_path")
        {
            create_context.compiler_dir_path = option.argument;
        }
        else if(option.name == "sfml_path")
        {
            create_context.sfml_dir_path = option.argument;
        }
        else if(option.name == "sfml_version")
        {
            create_context.sfml_version = option.argument;
        }
        else if(option.name == "vscode")
        {
            create_context.create_vscode_files = true;
        }
    }

    return PicSFML::Enviorment::CreateProject(create_context);
}

bool BuildCommand(const std::vector<ParsedCommand::Option> &options, const std::string &project_path)
{
    if(!PicSFML::Enviorment::SetProject(project_path)) return false;

    PicSFML::BuildType build_type = PicSFML::BuildType::Debug;

    for(const auto &option : options)
    {
        if(option.name == "debug")
        {
            build_type = PicSFML::BuildType::Debug;
        }
        else if(option.name == "release")
        {
            build_type = PicSFML::BuildType::Release;
        }
    }
    
    return PicSFML::Enviorment::BuildProject(build_type);
}

bool ConfigCommand(const std::vector<ParsedCommand::Option> &options, const std::string &project_path)
{
    if(!PicSFML::Enviorment::SetProject(project_path)) return false;

    if(auto project_config_result = PicSFML::Enviorment::GetProjectConfig())
    {
        PicSFML::ProjectConfig &project_config = *project_config_result;

        for(const auto &option : options)
        {
            if(option.name == "set")
            {
                bool is_list = option.argument.find('[') != std::string::npos &&
                               option.argument.find(']') != std::string::npos;

                if(is_list)
                {
                    std::string field, value;
                    int index = 0;

                    if(!ParseListArgument(option.argument, field, value, index))
                        return false;

                    if(!project_config.SetList(field, value, index))
                        return false;
                }
                else
                {
                    std::string field, value;
                    ParseKeyValueArgument(option.argument, field, value);

                    if(value == "default")
                    {    
                        if(auto config_defaults_result = PicSFML::Enviorment::GetConfigDefaults())
                        {
                            PicSFML::ConfigDefaults config_defaults = *config_defaults_result;

                            std::string default_value = config_defaults.Get(field);

                            if(default_value.empty())
                                return false;

                            if(!project_config.Set(field, default_value))
                                return false;
                        }
                    }
                    else
                    {
                        if(!project_config.Set(field, value)) 
                            return false;
                    }
                }

                if(!PicSFML::Enviorment::SaveProjectConfig())
                    return false;
            }
            else if(option.name == "get")
            {
                bool is_list = option.argument.find('[') != std::string::npos &&
                               option.argument.find(']') != std::string::npos;

                if(is_list)
                {
                    std::string field, dummy;
                    int index = 0;

                    if(!ParseListArgument(option.argument, field, dummy, index))
                        return false;

                    std::string value = project_config.GetList(field, index);
                    
                    PicSFML::Debug::Message("");
                    PicSFML::Debug::Message("Field: " + field + " Value[" + std::to_string(index) + "]: " + value);
                    PicSFML::Debug::Message("");
                }
                else
                {
                    std::string field, dummy;
                    ParseKeyValueArgument(option.argument, field, dummy);

                    std::string value = project_config.Get(field);
                    
                    PicSFML::Debug::Message("");
                    PicSFML::Debug::Message("Field: " + field + " Value: " + value);
                    PicSFML::Debug::Message("");
                }
            }
            else if(option.name == "remove")
            {
                std::string field, value;
                ParseKeyValueArgument(option.argument, field, value);

                if(!project_config.Remove(field, value))
                    return false;

                if(!PicSFML::Enviorment::SaveProjectConfig())
                    return false;
            }
            else if(option.name == "add")
            {
                std::string field, value;
                ParseKeyValueArgument(option.argument, field, value);

                if(!project_config.Add(field, value))
                    return false;

                if(!PicSFML::Enviorment::SaveProjectConfig())
                    return false;
            }
            else if(option.name == "show")
            {
                auto show_list = [&](const std::string &field, const std::string &label) {
                    int index = 0;
                    std::string row = project_config.GetList(field, index);

                    PicSFML::Debug::Message("[" + field + "] " + label + ":");
                    while(!row.empty())
                    {
                        PicSFML::Debug::Message("      [" + std::to_string(index) + "]: " + row);
                        row = project_config.GetList(field, ++index);
                    }
                };

                auto show_field = [&](const std::string &field, const std::string &label)
                {
                    std::string padded = "[" + field + "] " + label + ":";
                    if(padded.size() < 50) padded.append(50 - padded.size(), ' ');
                    PicSFML::Debug::Message(padded + project_config.Get(field));
                };

                PicSFML::Debug::Message("");
                PicSFML::Debug::Message("Project '" + project_config.Get("project_name") + "' configuration:");
                show_field("project_name", "Name");
                show_field("description", "Description");
                show_field("output_name", "Output");
                show_field("compiler_dir_path", "Compiler Path");
                show_field("sfml_dir_path", "SFML Path");
                show_field("sfml_version", "SFML Version");
                show_field("app_version", "Application Version");
                show_field("picsfml_version", "PicSFML Version");
                show_field("debug_flags", "Debug Flags");
                show_field("release_flags", "Release Flags");
                show_field("date_created", "Date Created");
                show_field("create_vscode_files", "Create VSCode Files");
                show_field("use_sfml_audio", "SFML Audio");
                show_field("use_sfml_network", "SFML Network");
                show_list("include_directories", "Include Directories");
                show_list("library_files", "Library Files");
                show_list("binary_files", "Binary Files");
                PicSFML::Debug::Message("");
            }
        }
    }

    return true;
}

bool CreateConfigCommand(const std::string &project_path)
{
    return PicSFML::Enviorment::CreateProjectConfig(project_path);
}

bool DefaultsCommand(const std::vector<ParsedCommand::Option> &options)
{
    if(auto config_defaults_result = PicSFML::Enviorment::GetConfigDefaults())
    {
        PicSFML::ConfigDefaults &config_defaults = *config_defaults_result;

        for(const auto &option : options)
        {
            if(option.name == "set")
            {
                std::string field, value;
                ParseKeyValueArgument(option.argument, field, value);

                if(!config_defaults.Set(field, value))
                    return false;

                if(!PicSFML::Enviorment::SaveConfigDefaults())
                    return false;
            }
            else if(option.name == "get")
            {
                std::string field, dummy;
                ParseKeyValueArgument(option.argument, field, dummy);

                std::string value = config_defaults.Get(field);

                PicSFML::Debug::Message("");
                PicSFML::Debug::Message("Field: " + field + " Value: " + value);
                PicSFML::Debug::Message("");
            }
            else if(option.name == "show")
            {
                auto show_field = [&](const std::string &field, const std::string &label)
                {
                    std::string padded = "[" + field + "] " + label + ":";
                    if(padded.size() < 50) padded.append(50 - padded.size(), ' ');
                    PicSFML::Debug::Message(padded + config_defaults.Get(field));
                };

                PicSFML::Debug::Message("");
                PicSFML::Debug::Message("PicSFML configuration defaults:");
                show_field("output_name", "Output Name");
                show_field("compiler_dir_path", "Compiler Directory Path");
                show_field("sfml_dir_path", "SFML Directory Path");
                show_field("sfml_version", "SFML Version");
                show_field("create_vscode_files", "Create VSCode Files");
                PicSFML::Debug::Message("");
            }
        }
    }

    return true;
}

bool SettingsCommand(const std::vector<ParsedCommand::Option> &options)
{
    if(auto settings_result = PicSFML::Enviorment::GetSettings())
    {
        PicSFML::Settings &settings = *settings_result;

        for(const auto &option : options)
        {
            if(option.name == "set")
            {
                std::string field, value;
                ParseKeyValueArgument(option.argument, field, value);

                if(!settings.Set(field, value))
                    return false;

                if(!PicSFML::Enviorment::SaveSettings())
                    return false;
            }
            else if(option.name == "get")
            {
                std::string field, dummy;
                ParseKeyValueArgument(option.argument, field, dummy);

                std::string value = settings.Get(field);

                PicSFML::Debug::Message("");
                PicSFML::Debug::Message("Field: " + field + " Value: " + value);
                PicSFML::Debug::Message("");
            }
            else if(option.name == "show")
            {
                auto show_field = [&](const std::string &field, const std::string &label)
                {
                    std::string padded = "[" + field + "] " + label + ":";
                    if(padded.size() < 50) padded.append(50 - padded.size(), ' ');
                    PicSFML::Debug::Message(padded + settings.Get(field));
                };

                PicSFML::Debug::Message("");
                PicSFML::Debug::Message("PicSFML settings:");
                show_field("projects_directory", "Projects Directory");
                show_field("debug_mode", "Debug Mode");
                PicSFML::Debug::Message("");
            }
        }
    }

    return true;
}

void VersionCommand()
{
    PicSFML::Debug::Message("");
    PicSFML::Debug::Message("PicSFML SFML Project Creator & Builder");
    PicSFML::Debug::Message("Created to simplify SFML project creation, easier with including code and linking libraryes.");
    PicSFML::Debug::Message("Surely works with SFML-2.6.2, SFML-3.0.0, more in the future.");
    PicSFML::Debug::Message("Created on 05/05/2026 and Last Updated 09/02/2026.");
    PicSFML::Debug::Message("PicSFML version " + PicSFML::PicSFMLVersion);
    PicSFML::Debug::Message("By ZipiRo");
    PicSFML::Debug::Message("");
}

void HelpCommand(const std::vector<ParsedCommand::Option> &options)
{
    std::string requested_command;
    
    if(!options.empty())
        requested_command = !options[0].argument.empty() ? options[0].argument : options[0].name;

    if(!requested_command.empty())
    {
        auto it = CommandHelpInfo.find(requested_command);
        if(it == CommandHelpInfo.end())
        {
            PicSFML::Debug::Message("Unknown command '" + requested_command + "'. Run 'picsfml help' to see all commands.");
            return;
        }

        PicSFML::Debug::Message("");
        PicSFML::Debug::Message("Usage: " + it->second.usage);
        PicSFML::Debug::Message(it->second.description);
        PicSFML::Debug::Message("");

        if(!it->second.options.empty())
        {
            PicSFML::Debug::Message("Options:");
            for(const auto &[option, desc] : it->second.options)
            {
                std::string padded = option;
                if(padded.size() < 28) padded.append(28 - padded.size(), ' ');
                PicSFML::Debug::Message("  " + padded + desc);
            }
        }
        PicSFML::Debug::Message("");

        return;
    }

    PicSFML::Debug::Message("");
    PicSFML::Debug::Message("PicSFML - SFML Project Creator & Builder");
    PicSFML::Debug::Message("");
    PicSFML::Debug::Message("Usage: picsfml <command> [options]");
    PicSFML::Debug::Message("");
    PicSFML::Debug::Message("Commands:");

    for(const auto &[name, help] : CommandHelpInfo)
    {
        std::string padded = name;
        if(padded.size() < 16) padded.append(16 - padded.size(), ' ');
        PicSFML::Debug::Message("  " + padded + help.description);
    }

    PicSFML::Debug::Message("");
    PicSFML::Debug::Message("Run 'picsfml help --command <command>' for details on a specific command.");
    PicSFML::Debug::Message("");
}

void InterfaceCommand()
{
    
}

void ExecuteCommand(const ParsedCommand &parsed_command)
{
    const std::string executable_path = GetExecutablePath(); 

    PicSFML::Enviorment::Init(executable_path);

    if(auto settings_result = PicSFML::Enviorment::GetSettings())
    {
        PicSFML::Settings &settings = *settings_result;

        std::string settings_debug_mode = settings.Get("debug_mode");

        if(settings_debug_mode == "verbose")     PicSFML::Debug::SetMode(PicSFML::Debug::Mode::Verbose);
        else if(settings_debug_mode == "quiet")  PicSFML::Debug::SetMode(PicSFML::Debug::Mode::Quiet);
        else if(settings_debug_mode == "normal") PicSFML::Debug::SetMode(PicSFML::Debug::Mode::Normal);
    }

    if(parsed_command.debug_mode == "verbose")     PicSFML::Debug::SetMode(PicSFML::Debug::Mode::Verbose);
    else if(parsed_command.debug_mode == "quiet")  PicSFML::Debug::SetMode(PicSFML::Debug::Mode::Quiet);
    else if(parsed_command.debug_mode == "normal") PicSFML::Debug::SetMode(PicSFML::Debug::Mode::Normal);

    bool valid = true;

    switch (parsed_command.command_type)
    {
    case CommandType::Create:
        valid = CreateCommand(parsed_command.options);
        break;
    case CommandType::Build:
        valid = BuildCommand(parsed_command.options, parsed_command.project_path);
        break;
    case CommandType::Config:
        valid = ConfigCommand(parsed_command.options, parsed_command.project_path);
        break;
    case CommandType::CreateConfig:
        valid = CreateConfigCommand(parsed_command.project_path);
        break;
    case CommandType::Defaults:
        valid = DefaultsCommand(parsed_command.options);
        break;
    case CommandType::Settings:
        valid = SettingsCommand(parsed_command.options);
        break;
    case CommandType::Version:
        VersionCommand();
        break;
    case CommandType::Help:
        HelpCommand(parsed_command.options);
        break;
    case CommandType::GetPath:
        PicSFML::Debug::Message("PicSFML Directory '" + std::filesystem::path(executable_path).make_preferred().parent_path().string() + "'");
    case CommandType::Interface:
        InterfaceCommand();
        break;
    default:
        break;
    }

    if(!valid) PicSFML::Debug::Message("See the latest debug file in the log directory '" + PicSFML::Debug::GetLogDirectory() + "' ");
}

int main(int argc, char *argv[])
{
    if(auto parse_result = ParseCommand(argc, argv))
    {
        ParsedCommand parsed_command = *parse_result;
        ExecuteCommand(parsed_command);
    }

    return 0;
}