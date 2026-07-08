namespace PicSFML
{
    struct ToolResult
    {
        bool success;
        std::string log;
        std::vector<std::string> errors;
        double buildTimeS;
    };
    
    void PrintResult(const ToolResult& result);
}
