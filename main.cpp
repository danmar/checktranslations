
#include <iostream>
#include <fstream>
#include <sstream>

static std::string readfile(const char filename[])
{
    std::ifstream f(filename);
    if (!f.is_open()) {
        std::cerr << "error: failed to open '" << filename << "'\n";
        return "";
    }

    std::ostringstream ret;

    enum {tag,content} state = content;

    for (char c = f.get(); f.good(); c = f.get()) {
        if (c == '<')
            state = tag;
        if (c == '\n' || state == tag)
            ret << c;
        if (c == '>')
            state = content;
    }

    return ret.str();
}

int main(int argc, char **argv)
{
    std::cout << "xmldocdiff\n";
    if (argc != 3) {
        std::cout << "syntax: xmldocdiff file1.xml file2.xml\n";
        return 1;
    }

    const std::string data1(readfile(argv[1]));
    const std::string data2(readfile(argv[2]));

    if (data1 == data2)
        std::cout << "match\n";
    else
        std::cout << "diff\n";

    return (data1 == data2) ? 0 : 1;
}
