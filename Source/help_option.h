void HelpOption()
{
    std::cout << "Help - PicSFML SFML Project Creator & Builder\n";
    std::cout << "--------------------------------------\n";

    std::cout << "Usage:\n";
    std::cout << "picsfml [option] <path> [argumets] (values) ...\n";
    std::cout << '\n';
    
    std::cout << "<path>                                The path to your project <string> ('.' for current directory)\n";
    std::cout << '\n';

    std::cout << "Options:\n";
    std::cout << "  -b, --build     Build Project\n";
    std::cout << "  Arguments:\n";
    std::cout << "      -r, --release                   Release build\n";
    std::cout << "      -d, --debug                     Debug build\n";
    std::cout << '\n';

    std::cout << "  -c, --create    Create project\n";
    std::cout << "  Arguments:\n";
    std::cout << "      -n,  --name <string>            Project name (default PicSFMLProject)\n";
    std::cout << "      -o,  --output <string>          Output name  (default main)\n";
    std::cout << "      -m,  --main <string>            Main file name (default main.cpp)\n";
    std::cout << "      -g,  --gcc <string>             GCC path (path to mingw(32|64) directory)\n";
    std::cout << "      -s,  --sfml <string>            SFML path (path to SFML-x.x.x directory)\n";
    std::cout << "      -sv, --sfml_version <string>    SFML version (default 3.0.0) {versions suported: 2.6.2, 3.0.0}\n";
    std::cout << "      --vscode                        Use VSCode properties file (default false)\n";
    std::cout << "      --audio                         Use SFML audio (default false)\n";
    std::cout << "      --network                       Use SFML network (default false)\n";
    std::cout << '\n';

    std::cout << "  -s, --set       Modify '.picsfml_config' file\n";
    std::cout << "  Arguments:\n";
    std::cout << "      -n,  --name\n";
    std::cout << "              --set <string>          Set project name\n";
    std::cout << "      -o,  --output\n";   
    std::cout << "              --set <string>          Set output executable name\n";
    std::cout << "      -m,  --main\n"; 
    std::cout << "              --set <string>          Set main source file\n";
    std::cout << "      -g,  --gcc\n";  
    std::cout << "              --set <string>          Set GCC/MinGW path\n";
    std::cout << "      -s,  --sfml\n"; 
    std::cout << "              --set <string>          Set SFML path\n";
    std::cout << "      -sv, --sfml_version\n"; 
    std::cout << "              --set <string>          Set SFML version\n";
    std::cout << "      -av, --application_version\n";
    std::cout << "              --set <string>          Set application version\n";
    std::cout << "      -l,  --library\n";  
    std::cout << "              --add <string>          Add library\n";
    std::cout << "              --remove <string>       Remove library\n";
    std::cout << "              --remove --back         Remove last library\n";
    std::cout << "              --remove --front        Remove first library\n";
    std::cout << "      -i,  --include\n";  
    std::cout << "              --add <string>          Add include path\n";
    std::cout << "              --remove <string>       Remove include path\n";
    std::cout << "              --remove --back         Remove last include\n";
    std::cout << "              --remove --front        Remove first include\n";
    std::cout << "      -b,  --binary\n";   
    std::cout << "              --add <string>          Add binary/DLL\n";
    std::cout << "              --remove <string>       Remove binary/DLL\n";
    std::cout << "              --remove --back         Remove last binary\n";
    std::cout << "              --remove --front        Remove first binary\n";
    std::cout << "      -df, --debug_flags\n";  
    std::cout << "              --set <string>          Set debug build flags\n";
    std::cout << "              --clear                 Clear debug flags\n";
    std::cout << "      -rf, --release_flags\n";    
    std::cout << "              --set <string>          Set release build flags\n";
    std::cout << "              --clear                 Clear release flags\n";
    std::cout << "      --vscode\n";    
    std::cout << "              --true                  Enable VSCode configuration\n";
    std::cout << "              --false                 Disable VSCode configuration\n";
    std::cout << "      --audio\n"; 
    std::cout << "              --true                  Enable SFML audio module\n";
    std::cout << "              --false                 Disable SFML audio module\n";
    std::cout << "      --network\n";   
    std::cout << "              --true                  Enable SFML network module\n";
    std::cout << "              --false                 Disable SFML network module\n";
    std::cout << '\n';
    
    std::cout << "  -l, --list       Show '.picsfml_config' information\n";
    std::cout << "  Arguments:\n";
    std::cout << "      --all                           Show all the information\n";
    std::cout << "      -n,  --name                     Show the project name\n";
    std::cout << "      -o,  --output                   Show the project output\n";   
    std::cout << "      -m,  --main                     Show the main file\n"; 
    std::cout << "      -g,  --gcc                      Show the gcc path\n";  
    std::cout << "      -s,  --sfml                     Show the sfml path\n"; 
    std::cout << "      -sv, --sfml_version             Show the sfml version\n"; 
    std::cout << "      -av, --application_version      Show the application version\n";
    std::cout << "      -l,  --library                  Show the project libraries\n";  
    std::cout << "      -i,  --include                  Show the project include\n";  
    std::cout << "      -b,  --binary                   Show the project binaries\n";   
    std::cout << "      -df, --debug_flags              Show debug build flags\n";  
    std::cout << "      -rf, --release_flags            Show release build flags\n";    
    std::cout << "      --vscode                        Show using VSCode check\n";    
    std::cout << "      --audio                         Show using audio check\n"; 
    std::cout << "      --network                       Show using network check\n";   
    std::cout << '\n';

    std::cout << "  -h, --help                          Show help\n";
    std::cout << "  -v, --version                       Show version\n";
    std::cout << '\n';

    std::cout << "Examples:\n";
    std::cout << "  picsfml --create <path> --gcc \"C:/mingw64\" --sfml \"C:SFML-3.0.0\" --vscode\n";
    std::cout << "  picsfml --build <path> --debug\n";
    std::cout << "  picsfml --set <path> --libaray --remove --front\n";
    std::cout << "  picsfml --list <path> --all\n";
}