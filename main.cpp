#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <unordered_map>

#include <PicSFML.h>

namespace Win32
{
    #include <windows.h>

    std::string GetToolPath()
    {
        char buffer[256];
        GetModuleFileNameA(NULL, buffer, 256);
        return std::filesystem::path(buffer).parent_path().string();
    }
}

enum CommandType
{
    None,
    C_Interface,
    C_Build,
    C_Create,
    C_Config,
    C_Defaults,
    C_Version,
    C_Help
};

struct ParsedCommand
{
    CommandType type;
    std::string project_path;
    std::vector<std::string> arguments;
};

struct CommandNode
{
    std::string name;
    std::string description;

    bool takes_path = false;
    bool takes_value = false;

    std::unordered_map<std::string, CommandNode> children;
    CommandType type = CommandType::None;

    void Print(int depth = 0) const 
    {
        std::string indent(depth * 4, ' ');

        std::cout << indent << '|' << name;

        if (takes_path)
            std::cout << " <path>";

        if (takes_value)
            std::cout << " <value>";

        std::cout << '\n';

        std::cout << indent << "    " << description << "\n";

        for (const auto& [_, child] : children)
            child.Print(depth + 1);
    }
};

auto AddGetSet(CommandNode &node)
{
    node.children["get"] = {
        "get",
        "Get the current value"
    };

    node.children["set"] = {
        "set",
        "Set the value",
        false,
        true
    };
}

auto AddList(CommandNode &node)
{
    node.children["add"] = {
        "add",
        "Add a value",
        false,
        true
    };

    node.children["remove"] = {
        "remove",
        "Remove a value",
        false,
        true
    };

    node.children["list"] = {
        "list",
        "List all values"
    };
}

std::unordered_map<std::string, CommandNode> GetCommandsInfo()
{
    std::unordered_map<std::string, CommandNode> commands;
    
    /// Interface Command
    commands["interface"] = {
        "interface",
        "Open PicSFML Interface",
        false,
        false,
        {},
        CommandType::C_Interface
    };

    /// Build Command
    commands["build"] = {
        "build",
        "Build your picsfml project",
        true,
        false,
        {},
        CommandType::C_Build
    };

    auto& build_command = commands["build"].children;

    build_command["release"] = {
        "release",
        "Build Release Version"
    };
    
    build_command["debug"] = {
        "debug",
        "Build Debug Version"
    };

    /// Config Command
    commands["config"] = {
        "config",
        "Configure Picsfml Project Values",
        true,
        false,
        {},
        CommandType::C_Config
    };

    commands["config"].children["list"] = {
        "list",
        "List Configuration Values"
    };

    auto& config_command = commands["config"].children;

    AddGetSet(config_command["name"] = {
        "name",
        "Project Name"
    });

    AddGetSet(config_command["compiler"] = {
        "compiler",
        "Compiler Directory Path"
    });

    config_command["compiler"].children["set"].description += " ('default' for tool default compiler directory path)";

    AddGetSet(config_command["sfml"] = {
        "sfml",
        "SFML Directory Path\n"
    });
    
    config_command["sfml"].children["set"].description += " ('default' for tool default sfml directory path)";


    AddGetSet(config_command["output"] = {
        "output",
        "Output Directory\n"
    });

    config_command["output"].children["set"].description += " ('default' for tool default output)";

    AddGetSet(config_command["main"] = {
        "main",
        "Main Source File\n"
    });

    config_command["main"].children["set"].description += " ('default' for tool default main)";

    AddGetSet(config_command["sfml_version"] = {
        "sfml_version",
        "SFML Version\n"
    });

    config_command["sfml_version"].children["set"].description += " ('default' for tool default sfml_version)";

    AddGetSet(config_command["app_version"] = {
        "app_version",
        "Application Version"
    });

    AddList(config_command["source"] = {
        "source",
        "Source Files"
    });

    AddList(config_command["include"] = {
        "include",
        "Include Files"
    });

    AddList(config_command["library"] = {
        "library",
        "Library Files"
    });

    AddList(config_command["binary"] = {
        "binary",
        "Binary Files"
    });

    config_command["flags"] = {
        "flags",
        "Compiler Flags"
    };

    AddGetSet(config_command["flags"].children["debug"] = {
        "debug",
        "Debug Build Compiler Flags"
    });

    AddGetSet(config_command["flags"].children["release"] = {
        "release",
        "Release Build Compiler Flags"
    });

    AddGetSet(config_command["vscode"] = {
        "vscode",
        "VSCode Properties File"
    });

    AddGetSet(config_command["audio"] = {
        "audio",
        "Enable Audio Module"
    });

    AddGetSet(config_command["network"] = {
        "network",
        "Enable Network Module"
    });

    /// Defaults Command
    commands["defaults"] = {
        "defaults",
        "Configure Tool Default Values",
        false,
        false,
        {},
        CommandType::C_Defaults
    };

    auto& defaults_command = commands["defaults"].children;

    defaults_command["list"] = {
        "list",
        "List Default Values"
    };
    
    AddGetSet(defaults_command["compiler"] = {
        "compiler",
        "Default Compiler Directory Path"
    });

    AddGetSet(defaults_command["sfml"] = {
        "sfml",
        "Default SFML Directory Path"
    });

    AddGetSet(defaults_command["output"] = {
        "output",
        "Default Output Name"
    });

    AddGetSet(defaults_command["main"] = {
        "main",
        "Default Main Source File"
    });

    AddGetSet(defaults_command["sfml_version"] = {
        "sfml_version",
        "Default SFML Version"
    });

    AddGetSet(defaults_command["projects_directory"] = {
        "projects_directory",
        "Default Projects Directory"
    });

    defaults_command["flags"] = {
        "flags",
        "Default Compiler Flags"
    };

    AddGetSet(defaults_command["flags"].children["debug"] = {
        "debug",
        "Default Debug Build Flags"
    });

    config_command["flags"].children["debug"].children["set"].description += " ('default' for tool default debug build flags)";


    AddGetSet(defaults_command["flags"].children["release"] = {
        "release",
        "Default Release Build Flags"
    });

    config_command["flags"].children["release"].children["set"].description += " ('default' for tool default release build flags)";

    /// Create Command
    commands["create"] = {
        "create",
        "Create PicSFML Project",
        true,
        true,
        {},
        CommandType::C_Create
    };
    
    /// Version Command
    commands["version"] = {
        "version",
        "Display PicSFML Version",
        false,
        false,
        {},
        CommandType::C_Version
    };


    /// Help Command
    commands["help"] = {
        "help",
        "Display PicSFML Manual",
        false,
        false,
        {},
        CommandType::C_Help
    };

    auto& help_command = commands["help"].children;

    help_command["interface"] = {
        "interface",
        "Show 'interface' Command Manual",
    };

    help_command["build"] = {
        "build",
        "Show 'build' Command Manual",
    };
    
    help_command["create"] = {
        "create",
        "Show 'create' Command Manual",
    };
    
    help_command["config"] = {
        "config",
        "Show 'config' Command Manual",
    };

    help_command["defaults"] = {
        "defaults",
        "Show 'defaults' Command Manual",
    };
    
    help_command["version"] = {
        "version",
        "Show 'version' Command Manual",
    };

    help_command["help"] = {
        "help",
        "Show 'help' Command Manual",
    };

    return commands;
}

void PrintCommands(const std::unordered_map<std::string, CommandNode> &commands)
{
    std::cout << "Usage:\n";
    std::cout << "    picsfml <command> <path> [arguments]\n\n";

    std::cout << "Commands:\n\n";

    for (const auto& [_, command] : commands)
    {
        std::cout << "  |" << command.name;

        std::cout << '\n'; 
        std::cout << "      " << command.description << "\n\n";
    }

    std::cout << "Examples:\n";
    std::cout << "    picsfml interface\n";
    std::cout << "    picsfml build <path> [relase | debug]\n";
    std::cout << "    picsfml create <path | default> <project_name>\n";
    std::cout << "    picsfml config <path> [arguments]\n";
    std::cout << "    picsfml defaults <path> [argumesnts]\n";
    std::cout << "    picsfml help [interface | build | create | config | defaults | version | help]\n";
    std::cout << "    picsfml version\n";
}

bool ParseCommand(ParsedCommand &parsed_command, int argc, char *argv[])
{
    const auto &commands_info = GetCommandsInfo();
    parsed_command = ParsedCommand();

    if(argc < 2)
    {
        parsed_command.type = CommandType::C_Interface;
        return true;
    }

    int index = 1;

    std::string command(argv[index++]);

    auto command_info_it = commands_info.find(command); 

    if(command_info_it == commands_info.end())
    {
        std::cout << "Command '" << command << "' does not exist in the registry\n";
        return false;
    }

    const CommandNode &command_info = command_info_it->second;

    parsed_command.type = command_info.type;

    if(index >= argc)
        return true;

    if(command_info.takes_path)
    {
        std::string project_path(argv[index++]);

        if(project_path != "default")
        {
            if(project_path.empty() || !std::filesystem::exists(project_path))
            {
                std::cout << "Command '" << command << "' requires an existent path to a directorty\n";
                return false;
            }
    
            parsed_command.project_path = std::filesystem::absolute(project_path).lexically_normal().string();
        }
        else parsed_command.project_path = project_path;
    }

    const CommandNode *current = &command_info; 
    bool value_consumed = false;

    while (index < argc)
    {
        std::string token(argv[index]);

        auto it = current->children.find(token);

        if (it != current->children.end())
        {
            parsed_command.arguments.push_back(token);

            current = &it->second;
            ++index;
            continue;
        }

        if (current->takes_value)
        {
            parsed_command.arguments.push_back(token);
            value_consumed = true;
            ++index;

            if (index < argc)
            {
                std::cout << "Unexpected argument '" << argv[index] << "' after value for '" << current->name << "'.\n";
                return false;
            }

            break;
        }

        if (!current->children.empty())
        {
            std::cout << "'" << token << "' is not a valid subcommand of '" << current->name << "'.\n";
            return false;
        }

        std::cout << "Unexpected argument '" << token << "' after '" << current->name << "'.\n";
        return false;
    }

    if (!current->children.empty())
    {
        std::cout << "Command '" << current->name << "' requires a subcommand.\n";
        return false;
    }

    else if (current->takes_value && !value_consumed)
    {
        std::cout << "Command '" << current->name << "' requires a value.\n";
        return false;
    }

    return true;
}

bool InterfaceCommand(const ParsedCommand &parsed_command)
{
    std::cout << "The PicSFML interface it is stil in development\n";

    return true;
}

PicSFML::ToolResult BuildCommand(const ParsedCommand &parsed_command)
{
    PicSFML::ToolContext context;
    PicSFML::BuildType type;

    context.project_path = parsed_command.project_path;
    context.tool_path = Win32::GetToolPath();

    LoadProjectConfig(context.config, context.project_path + "/" + PicSFML::PicConfigFileName);

    const auto &args = parsed_command.arguments;

    if(args[0] == "release") type = PicSFML::BuildType::Release;
    else if(args[0] == "debug") type = PicSFML::BuildType::Debug;

    std::cout << "Building " << context.config.name << '\n';
    std::cout << "Version " << context.config.app_version.AsString('.') << '\n';
    std::cout << "Type " << (type == PicSFML::BuildType::Release ? "Release" : "Debug") << "\n\n"; 

    return Build(context, type);
}

PicSFML::ToolResult CreateCommand(const ParsedCommand &parsed_command)
{
    PicSFML::ToolContext context;
    PicSFML::ToolResult result;
    PicSFML::ToolDefaults tool_defaults;

    std::string tool_path = Win32::GetToolPath();
    std::string tool_defaults_path = tool_path + "/" + PicSFML::ToolDefaultsFileName;
    
    if(!PicSFML::LoadToolDefaults(tool_defaults, tool_defaults_path))
        result.errors.push_back("There is no '" + PicSFML::ToolDefaultsFileName + "' file");

    if(parsed_command.project_path == "default") context.project_path = tool_defaults.projects_directory;
        else context.project_path = parsed_command.project_path;

    context.tool_path = tool_path;

    context.config.compiler = tool_defaults.compiler;
    context.config.sfml = tool_defaults.sfml;
    context.config.output = tool_defaults.output;
    context.config.main = tool_defaults.main;
    context.config.sfml = tool_defaults.sfml;
    context.config.sfml_version = tool_defaults.sfml_version;
    context.config.flags[PicSFML::BuildType::Debug] = tool_defaults.flags[PicSFML::BuildType::Debug];
    context.config.flags[PicSFML::BuildType::Release] = tool_defaults.flags[PicSFML::BuildType::Release];
    const auto &args = parsed_command.arguments;

    context.config.name = args[0];

    std::cout << "Creating PicSFML Project: " << args[0] << '\n';

    result = PicSFML::Create(context);

    return result;
}

PicSFML::ToolResult ConfigCommand(const ParsedCommand &parsed_command)
{
    PicSFML::ToolResult result;
    PicSFML::ToolDefaults tool_defaults;
    PicSFML::ProjectConfig config;

    std::string tool_path = Win32::GetToolPath();
    std::string tool_defaults_path = tool_path + "/" + PicSFML::ToolDefaultsFileName;
    std::string project_config_path = parsed_command.project_path + "/" + PicSFML::PicConfigFileName;

    if(!PicSFML::LoadToolDefaults(tool_defaults, tool_defaults_path))
        result.errors.push_back("There is no '" + PicSFML::ToolDefaultsFileName + "' file");

    PicSFML::LoadProjectConfig(config, project_config_path);

    const auto &option = parsed_command.arguments[0];

    if(option == "list")
        return PicSFML::ListConfigValues(config);

    const auto &subcommand = parsed_command.arguments[1];
    const auto &value = parsed_command.arguments[2];
    
    if(option == "name")
    {
        if(subcommand == "set") result = PicSFML::SetString(config.name, value);
        else if(subcommand == "get") return PicSFML::GetString(config.name);
    }
    else if(option == "compiler")
    {
        if(subcommand == "set") 
        {
            if(value == "default") result = PicSFML::SetString(config.compiler, tool_defaults.compiler);
            else result = PicSFML::SetString(config.compiler, value);
        }
        else if(subcommand == "get") return PicSFML::GetString(config.compiler);
    }
    else if(option == "sfml")
    {
        if(subcommand == "set") 
        {
            if(value == "default") result = PicSFML::SetString(config.sfml, tool_defaults.sfml);
            else result = PicSFML::SetString(config.sfml, value);
        }
        else if(subcommand == "get") return PicSFML::GetString(config.sfml);
    }
    else if(option == "output")
    {
        if(subcommand == "set") 
        {
            if(value == "default") result = PicSFML::SetString(config.output, tool_defaults.output);
            else result = PicSFML::SetString(config.output, value);
        }
        else if(subcommand == "get") return PicSFML::GetString(config.output);
    }
    else if(option == "main")
    {
        if(subcommand == "set") 
        {
            if(value == "default") result = PicSFML::SetString(config.main, tool_defaults.main);
            else result = PicSFML::SetString(config.main, value);
        }
        else if(subcommand == "get") return PicSFML::GetString(config.main);
    }
    else if(option == "sfml_version")
    {
        if(subcommand == "set") 
        {
            if(value == "default") result = PicSFML::SetVersion(config.sfml_version, PicSFML::Version(tool_defaults.sfml_version));
            else result = PicSFML::SetVersion(config.sfml_version, PicSFML::Version(value));
        }
        else if(subcommand == "get") return PicSFML::GetVersion(config.sfml_version);
    }
    else if(option == "app_version")
    {
        if(subcommand == "set") result = PicSFML::SetVersion(config.app_version, PicSFML::Version(value));
        else if(subcommand == "get") return PicSFML::GetVersion(config.app_version);
    }
    else if(option == "vscode")
    {
        if(subcommand == "set") result = PicSFML::SetBool(config.vscode, (value == "true" ? true : false));
        else if(subcommand == "get") return PicSFML::GetBool(config.vscode);
    }
    else if(option == "audio")
    {
        if(subcommand == "set") result = PicSFML::SetBool(config.audio, (value == "true" ? true : false));
        else if(subcommand == "get") return PicSFML::GetBool(config.audio);
    }
    else if(option == "network")
    {
        if(subcommand == "set") result = PicSFML::SetBool(config.network, (value == "true" ? true : false));
        else if(subcommand == "get") return PicSFML::GetBool(config.network);
    }
    else if(option == "flags")
    {
        const auto &flag_type = parsed_command.arguments[1];
        const auto &subcommand = parsed_command.arguments[2];
        const auto &value = parsed_command.arguments[3];

        if(flag_type == "debug")
        {
            if(subcommand == "set") 
            {
                if(value == "default") result = PicSFML::SetString(config.flags[PicSFML::BuildType::Debug], tool_defaults.flags[PicSFML::BuildType::Debug]);
                else result = PicSFML::SetString(config.flags[PicSFML::BuildType::Debug], value);   
            }
            else if(subcommand == "get") return PicSFML::GetString(config.flags[PicSFML::BuildType::Debug]);
        }
        else if(flag_type == "release")
        {
            if(subcommand == "set") 
            {
                if(value == "default") result = PicSFML::SetString(config.flags[PicSFML::BuildType::Release], tool_defaults.flags[PicSFML::BuildType::Release]);
                else result = PicSFML::SetString(config.flags[PicSFML::BuildType::Release], value);   
            }
            else if(subcommand == "get") return PicSFML::GetString(config.flags[PicSFML::BuildType::Release]);
        } 
    }
    else if(option == "source")
    {
        if(subcommand == "add") result = PicSFML::AddToList(config.source, value);
        else if(subcommand == "remove") result = PicSFML::RemoveFromList(config.source, value);
        else if(subcommand == "list") return PicSFML::List(config.source); 
    }
    else if(option == "library")
    {
        if(subcommand == "add") result = PicSFML::AddToList(config.library, value);
        else if(subcommand == "remove") result = PicSFML::RemoveFromList(config.library, value);
        else if(subcommand == "list") return PicSFML::List(config.library); 
    }
    else if(option == "include")
    {
        if(subcommand == "add") result = PicSFML::AddToList(config.include, value);
        else if(subcommand == "remove") result = PicSFML::RemoveFromList(config.include, value);
        else if(subcommand == "list") return PicSFML::List(config.include); 
    }
    else if(option == "binary")
    {
        if(subcommand == "add") result = PicSFML::AddToList(config.binary, value);
        else if(subcommand == "remove") result = PicSFML::RemoveFromList(config.binary, value);
        else if(subcommand == "list") return PicSFML::List(config.binary); 
    }
    else 
    {
        return {
            false, 
            "",
            { "Unkown config option '" + option + "'" },
            0
        };
    }

    PicSFML::SaveProjectConfig(config, project_config_path);

    if(config.vscode) 
        PicSFML::CreateVSCodeProperties(config, tool_path, parsed_command.project_path);

    return result;
}

PicSFML::ToolResult DefaultsCommand(const ParsedCommand &parsed_command)
{
    PicSFML::ToolDefaults tool_defaults;
    PicSFML::ToolResult result;

    std::string tool_defaults_path = Win32::GetToolPath() + "/" + PicSFML::ToolDefaultsFileName;

    PicSFML::LoadToolDefaults(tool_defaults, tool_defaults_path);

    const auto &option = parsed_command.arguments[0];

    if(option == "list")
        return PicSFML::ListDefaultValues(tool_defaults);

    const auto &subcommand = parsed_command.arguments[1];
    const auto &value = parsed_command.arguments[2];

    if(option == "compiler")
    {
        if(subcommand == "set") result = PicSFML::SetString(tool_defaults.compiler, value);
        else if(subcommand == "get") return PicSFML::GetString(tool_defaults.compiler);
    }
    else if(option == "sfml")
    {
        if(subcommand == "set") result = PicSFML::SetString(tool_defaults.sfml, value);
        else if(subcommand == "get") return PicSFML::GetString(tool_defaults.sfml);
    }
    else if(option == "sfml_version")
    {
        if(subcommand == "set") result = PicSFML::SetVersion(tool_defaults.sfml_version, PicSFML::Version(value));
        else if(subcommand == "get") return PicSFML::GetVersion(tool_defaults.sfml_version);
    }
    else if(option == "flags")
    {
        const auto &flag_type = parsed_command.arguments[1];
        const auto &subcommand = parsed_command.arguments[2];
        const auto &value = parsed_command.arguments[3];

        if(flag_type == "debug")
        {
            if(subcommand == "set") result = PicSFML::SetString(tool_defaults.flags[PicSFML::BuildType::Debug], value);
            else if(subcommand == "get") return PicSFML::GetString(tool_defaults.flags[PicSFML::BuildType::Debug]);
        }
        else if(flag_type == "release")
        {
            if(subcommand == "set") result = PicSFML::SetString(tool_defaults.flags[PicSFML::BuildType::Release], value);
            else if(subcommand == "get") return PicSFML::GetString(tool_defaults.flags[PicSFML::BuildType::Release]);
        } 
    }
    else if(option == "output")
    {
        if(subcommand == "set") result = PicSFML::SetString(tool_defaults.output, value);
        else if(subcommand == "get") return PicSFML::GetString(tool_defaults.output);
    }
    else if(option == "main")
    {
        if(subcommand == "set") result = PicSFML::SetString(tool_defaults.main, value);
        else if(subcommand == "get") return PicSFML::GetString(tool_defaults.main);
    }
    else if(option == "projects_directory")
    {
        if(subcommand == "set") result = PicSFML::SetString(tool_defaults.projects_directory, std::filesystem::absolute(value).lexically_normal().string());
        else if(subcommand == "get") return PicSFML::GetString(tool_defaults.projects_directory);
    }

    PicSFML::SaveToolDefaults(tool_defaults, tool_defaults_path);

    return result;
}

bool VersionCommand(const ParsedCommand &parsed_command)
{
    std::cout << "PicSFML SFML Project Creator & Builder, by ZipiRo.\n";
    std::cout << "Created to simplify SFML project creation, and makeing it easier with including code\n";
    std::cout << "and linking libraryes for any other things you want to add to your project.\n";
    std::cout << "Surely works with SFML-2.6.2, SFML-3.0.0, more in the future.\n";
    std::cout << "Created on 05/05/2026 and Last Updated 07/11/2026.\n";
    std::cout << "PicSFML Version " << PicSFML::PicSFMLVersion.AsString('.') << "\n";

    return true;
}

bool HelpCommand(const ParsedCommand &parsed_command)
{
    const auto& commands = GetCommandsInfo();

    if (parsed_command.arguments.empty())
    {
        PrintCommands(commands);
        return true;
    }

    auto it = commands.find(parsed_command.arguments[0]);

    if (it == commands.end())
    {
        std::cout << "Unknown command '" + parsed_command.arguments[0] + "'."; 
        return false;
    }

    PicSFML::ToolResult result;

    result.success = true;

    std::cout << it->second.name << "\n";
    std::cout << std::string(9 + it->second.name.size(), '-') << "\n\n";

    it->second.Print();

    return true;
}

bool ExecuteCommand(const ParsedCommand &parsed_command)
{
    PicSFML::ToolResult result;

    std::string tool_defaults_path = Win32::GetToolPath() + "/" + PicSFML::ToolDefaultsFileName;

    if(!std::filesystem::exists(tool_defaults_path))
        PicSFML::SaveToolDefaults(PicSFML::ToolDefaults(), tool_defaults_path);

    std::cout << "PicSFML - SFML Project Creator & Builder\n";
    std::cout << "=======================================\n\n";

    switch (parsed_command.type)
    {
    case CommandType::C_Interface:
        return InterfaceCommand(parsed_command);
        break;

    case CommandType::C_Build:
        result = BuildCommand(parsed_command);
        break;

    case CommandType::C_Create:
        result = CreateCommand(parsed_command);
        break;

    case CommandType::C_Config:
        result = ConfigCommand(parsed_command);
        break;

    case CommandType::C_Defaults:
        result = DefaultsCommand(parsed_command);
        break;

    case CommandType::C_Version:
        return VersionCommand(parsed_command);
        break;

    case CommandType::C_Help:
        return HelpCommand(parsed_command);
        break;

    default:
        return false;
    }

    PrintResult(result);

    return result.success;
}

int main(int argc, char *argv[])
{
    ParsedCommand parsed_command;

    if(!ParseCommand(parsed_command, argc, argv)) return 1;

    if(!ExecuteCommand(parsed_command)) return 1;

    return 0;
}