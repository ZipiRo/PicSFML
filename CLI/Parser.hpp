#pragma once

#include <iostream>
#include <filesystem>
#include <unordered_map>
#include <string>
#include <vector>
#include <optional>

enum class CommandType
{
    Interface,
    Create,
    CreateConfig,
    Build,
    Version,
    Help,
    Config,
    Defaults,
    Settings
};

enum class ArgType
{
    None,
    Int,
    String,
    Bool
};

struct OptionInfo
{
    std::string name; 
    ArgType type = ArgType::None;
    bool takes_argument = true;
};

struct CommandInfo
{
    std::string name;
    std::unordered_map<std::string, OptionInfo> options_info;
    CommandType type = CommandType::Interface;
    bool takes_path = false;
    int min_options = 0;
};

struct ParsedCommand
{
    struct Option
    {
        std::string name;
        std::string argument;
    };

    std::string project_path;
    std::string command_name;
    CommandType command_type;
    std::string debug_mode;
    std::vector<Option> options;
};

struct HelpInfo
{
    std::string usage;
    std::string description;
    std::vector<std::pair<std::string, std::string>> options;
};

static const std::unordered_map<std::string, HelpInfo> CommandHelpInfo = {
    { "create", {
        "picsfml create --name <name> [options]",
        "Creates a new SFML project.",
        {
            { "--name <name>",            "Project name (required)." },
            { "--output <name>",          "Output binary name." },
            { "--project_path <path>",    "Directory to create the project in." },
            { "--compiler_path <path>",   "Path to the compiler directory." },
            { "--sfml_path <path>",       "Path to the SFML directory." },
            { "--sfml_version <ver>",     "SFML version to target (e.g. 2.6.2, 3.0.0)." },
            { "--vscode",                 "Generate VSCode project files." },
        }
    }},
    { "build", {
        "picsfml build <project_path> [--debug|--release]",
        "Builds an existing project.",
        {
            { "--debug",   "Build in debug mode (default)." },
            { "--release", "Build in release mode." },
        }
    }},
    { "config", {
        "picsfml config <project_path> [options]",
        "Reads or modifies a project's configuration.",
        {
            { "--set field=value",         "Set a config field." },
            { "--set field[i]=value",      "Set the i-th entry of a list field." },
            { "--get field",               "Print a config field." },
            { "--get field[i]",            "Print the i-th entry of a list field." },
            { "--add field=value",         "Add a value to a list field." },
            { "--remove field=value",      "Remove a value from a list field." },
            { "--show",                    "Print the full project configuration." },
        }
    }},
    { "create-config", {
        "picsfml create-config <project_path>",
        "Creates a default configuration file for an existing project.",
        {}
    }},
    { "defaults", {
        "picsfml defaults [options]",
        "Reads or modifies the global project defaults used by 'create'.",
        {
            { "--set field=value", "Set a default field." },
            { "--get field",       "Print a default field." },
            { "--show",            "Print all defaults." },
        }
    }},
    { "settings", {
        "picsfml settings [options]",
        "Reads or modifies global PicSFML settings.",
        {
            { "--set field=value", "Set a setting." },
            { "--get field",       "Print a setting." },
            { "--show",            "Print all settings." },
        }
    }},
    { "version", {
        "picsfml version",
        "Prints version and build information.",
        {}
    }},
    { "help", {
        "picsfml help --command <command>",
        "Prints general help, or detailed help for a specific command.",
        {}
    }},
    { "interface", {
        "picsfml interface",
        "Opens the interface of PicSFML.",
        {}
    }},
};

static const std::unordered_map<std::string, CommandInfo> CommandsInfo = { 
    { "create", {
            "create",
            {
                {
                    "--name", {"--name", ArgType::String, true}
                },
                {
                    "--output", {"--output", ArgType::String, true }
                },
                {
                    "--path", { "--path", ArgType::String, true}},
                {
                    "--compiler_path", {"--compiler_path", ArgType::String, true}
                },
                {
                    "--sfml_path", {"--sfml_path", ArgType::String, true}
                },
                {
                    "--sfml_version", {"--sfml_version", ArgType::String, true}
                },
                {
                    "--vscode", {"--vscode", ArgType::None, false}
                },
                
            },
            CommandType::Create,
            false,
            1
        }
    },
    { "build", {
            "build",
            {
                {
                    "--debug", {"--debug", ArgType::None, false}
                },
                {
                    "--release", {"--release", ArgType::None, false}
                }
            },
            CommandType::Build,
            true,
            0
        }
    },
    { "config", {
            "config",
            {
                {
                    "--set", {"--set", ArgType::String, true}
                },
                {
                    "--get", {"--get", ArgType::String, true}
                },
                {
                    "--remove", {"--remove", ArgType::String, true}
                },
                {
                    "--add", {"--add", ArgType::String, true}
                },
                {
                    "--show", {"--show", ArgType::None, false}
                }
            },
            CommandType::Config,
            true,
            1
        }
    },
    { "create-config", { 
            "create-config",
            {},
            CommandType::CreateConfig,
            true,
            0
        }
    },
    { "defaults", {
            "defaults",
            {
                {
                    "--set", {"--set", ArgType::String, true}
                },
                {
                    "--get", {"--get", ArgType::String, true}
                },
                {
                    "--show", {"--show", ArgType::None, false}
                }
            },
            CommandType::Defaults,
            false,
            1
        }
    },
    { "settings", {
            "settings",
            {
                {
                    "--set", {"--set", ArgType::String, true}
                },
                {
                    "--get", {"--get", ArgType::String, true}
                },
                {
                    "--show", {"--show", ArgType::None, false}
                }
            },
            CommandType::Settings,
            false,
            1
        }
    },
    { "version", { 
            "version",
            {},
            CommandType::Version,
            false,
            0
        }
    },
    { "help", { 
            "help",
            {
                {"--command", {"--command", ArgType::String, true}}
            },
            CommandType::Help,
            false,
            0
        }
    },
    { "interface",{
            "interface",
            {

            }
        }
    }
};

static const std::unordered_map<std::string, OptionInfo> OtherOptionsInfo = {
    {
        "--verbose", {"--verbose", ArgType::None, false}
    },
    {
        "--quite", {"--quite", ArgType::None, false}
    },
    {
        "--normal", {"--normal", ArgType::None, false}
    }
};

std::optional<ParsedCommand> ParseCommand(int argc, char *argv[])
{
    if(argc < 2) return std::nullopt;

    ParsedCommand parsed_commnad;

    int index = 1;

    std::string command_arg(argv[index++]); 

    auto commnad_info_it = CommandsInfo.find(command_arg);

    if(commnad_info_it == CommandsInfo.end())
    {
        std::cout << "Commnad '" << command_arg << "' does not exist in the picsfml workflow\n";
        return std::nullopt;
    }

    parsed_commnad.command_name = command_arg;

    CommandInfo command_info = commnad_info_it->second;

    parsed_commnad.command_type = command_info.type;

    if(index >= argc)
    {
        if(command_info.min_options > 0)
        {
            std::cout << "Command '" << command_arg << "' needs options to work, use help for more info\n";
            return std::nullopt;
        }

        return parsed_commnad;
    }

    if(command_info.takes_path)
    {
        std::string path_arg = std::filesystem::absolute(std::string(argv[index++])).make_preferred().lexically_normal().string();
        parsed_commnad.project_path = path_arg;
    }

    while (index < argc)
    {
        std::string option_arg(argv[index++]);

        if(option_arg.find("--") == std::string::npos)
        {
            std::cout << "Argument '" << option_arg << "' is not an option\n";
            continue;
        }

        auto other_option_info_it = OtherOptionsInfo.find(option_arg);

        if(other_option_info_it != OtherOptionsInfo.end())
        {
            OptionInfo other_option_info = other_option_info_it->second;

            if(other_option_info.name == "--verbose" || other_option_info.name == "--quite" || other_option_info.name == "--normal")
            {
                parsed_commnad.debug_mode = other_option_info.name.substr(2, other_option_info.name.size()); 
                continue;
            }
            
            ParsedCommand::Option other_option;

            other_option.name = other_option_info.name.substr(2, other_option_info.name.size());

            if(other_option_info.takes_argument)
            {
                std::string argument(argv[index]);

                if(argument.find("--") != std::string::npos)
                {
                    std::cout << "Argument '" << argument << "looks like an option\n";
                    continue;
                }

                other_option.argument = argument;
                index++;
            }

            parsed_commnad.options.push_back(other_option);

            continue;
        }

        auto commnad_option_info_it = command_info.options_info.find(option_arg);

        if(commnad_option_info_it == command_info.options_info.end())
        {
            std::cout << "Argument '" << option_arg << "' is not a '" << command_arg << "' option\n";
            continue;
        }

        OptionInfo command_option_info = commnad_option_info_it->second;
        
        ParsedCommand::Option commnad_option;

        commnad_option.name = command_option_info.name.substr(2, command_option_info.name.size());

        if(command_option_info.takes_argument)
        {
            std::string argument(argv[index]);

            if(argument.find("--") != std::string::npos)
            {
                std::cout << "Argument '" << argument << "look like an option\n";
                continue;
            }

            commnad_option.argument = argument;
            index++;
        }
        
        parsed_commnad.options.push_back(commnad_option);
    }

    return parsed_commnad;
}

bool ParseListArgument(const std::string &argument, std::string &field, std::string &value, int &index)
{
    size_t open = argument.find('[');
    size_t close = argument.find(']');

    if(open == std::string::npos || close == std::string::npos || close < open)
        return false;

    std::string index_str = argument.substr(open + 1, close - open - 1);

    field = argument.substr(0, open);

    size_t eq = argument.find('=', close);
    value = (eq != std::string::npos) ? argument.substr(eq + 1) : "";

    try
    {
        index = std::stoi(index_str);
    }
    catch(...)
    {
        return false;
    }

    return true;
}

void ParseKeyValueArgument(const std::string &argument, std::string &field, std::string &value)
{
    size_t eq = argument.find('=');
    field = argument.substr(0, eq);
    value = (eq != std::string::npos) ? argument.substr(eq + 1) : "";
}