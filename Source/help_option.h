void HelpOption()
{
    std::cout << "Help - PicSFML SFML Project Creator & Builder\n";
    std::cout << "--------------------------------------\n";

    std::cout << "Usage:\n";
    std::cout << "picsfml [option] <path> [argumets]\n";
    std::cout << '\n';
    
    std::cout << "<path>            The path to your project <string> ('.' for current directory)\n";
    std::cout << '\n';

    std::cout << "Options:\n";
    std::cout << "  -b, --build     Build Project\n";
    std::cout << "  Arguments:\n";
    std::cout << "      -r          Release build\n";
    std::cout << "      -d          Debug build\n";
    std::cout << '\n';

    std::cout << "  -c, --create    Create project\n";
    std::cout << "  Arguments:\n";
    std::cout << "      -n          Project name <string> (default PicSFMLProject)\n";
    std::cout << "      -o          Output name <string> (default main)\n";
    std::cout << "      -m          Main file name <string> (default main.cpp)\n";
    std::cout << "      -g          GCC path <string> (path to mingw(32|64) directory)\n";
    std::cout << "      -s          SFML path <string> (path to SFML-x.x.x directory)\n";
    std::cout << "      -sv         SFML version <string> (default 3.0.0) {versions suported: 2.6.2, 3.0.0}\n";
    std::cout << "      -vs         Use VSCode properties file (default false)\n";
    std::cout << "      --audio     Use SFML audio (default false)\n";
    std::cout << "      --network   Use SFML network (default false)\n";
    std::cout << '\n';
    
    std::cout << "  -h, --help        Show help\n";
    std::cout << "  -v, --version     Show version\n";
}