namespace PicSFML
{
    class Version
    {
    private:
        std::vector<int> version;

    public:
        Version() = default;
        Version(const std::string sversion);

        int operator[](size_t i) const;

        void ParseString(const std::string &sversion);

        std::string AsString(char delimitator) const;

        int AsInt() const;
    };
}