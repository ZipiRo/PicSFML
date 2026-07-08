#include <iostream>
#include <vector>
#include <string>

#include <ToolResult.h>

using namespace PicSFML;

void PicSFML::PrintResult(const ToolResult& result)
{
    if (!result.log.empty())
        std::cout << result.log;

    if (!result.errors.empty())
    {
        std::cout << "\nErrors:\n";

        for (const auto &error : result.errors)
            std::cout << "  - " << error << '\n';
    }

    if (result.success) std::cout << "\nCommand completed in " << result.buildTimeS << "s\n";
    else std::cout << "\nCommand failed in "<< result.buildTimeS << "s\n";
}