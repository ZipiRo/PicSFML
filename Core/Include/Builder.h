namespace PicSFML
{
    enum BuildType 
    {
        Release,
        Debug
    };
    
    ToolResult Build(const ToolContext &context, BuildType type);    
}