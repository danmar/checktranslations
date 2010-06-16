
#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>

static std::string readfile(const char filename[], const bool dots)
{
    std::ifstream f(filename);
    if (!f.is_open()) {
        std::cerr << "error: failed to open '" << filename << "'\n";
        return "";
    }

    std::ostringstream ret;

    enum {tagname, tagdata, content} state = content;

    for (char c = f.get(); f.good(); c = f.get()) {
        // update state..
        if (c == '<') {
            state = tagname;
        } else if (c == ' ' && state == tagname) {
            state = tagdata;
        } else if (c == '>' && (state == tagname || state == tagdata)) {
            state = content;
        }

        // filtering input data..
        if (state == tagname) {
            ret << c;
        } else if (state == content) {
            if (c == '\n' ||
                    (dots && c == '.') ||
                    c == '!' ||
                    c == '?' ||
                    c == '\"' ||
                    c == '>' ||
                    std::isdigit(c&0xff)) {
                ret << c;
            }
        }
    }

    return ret.str();
}

int main(int argc, char** argv)
{
    std::cout << "xmldocdiff\n";

    std::string file1, file2;
    bool dots = false;

    if (argc == 3) {
        file1 = argv[1];
        file2 = argv[2];
    } else if (argc == 4 && strcmp(argv[1],"--dots")==0) {
        file1 = argv[2];
        file2 = argv[3];
    } else {
        std::cout << "syntax: xmldocdiff [--dots] file1.xml file2.xml\n";
        return 1;
    }

    const std::string data1(readfile(argv[1], dots));
    const std::string data2(readfile(argv[2], dots));

    /*
        std::ofstream f1("dbg1.txt");
        f1 << data1;
        std::ofstream f2("dbg2.txt");
        f2 << data2;
    */

    // match data1 and data2, ignoring newlines
    unsigned int line1 = 1, line2 = 1;
    std::string::size_type pos1 = 0, pos2 = 0;
    while (pos1 < data1.size() || pos2 < data2.size()) {
        unsigned char c1 = 0;
        while (pos1 < data1.size() && c1 == 0) {
            c1 = data1[pos1++];
            if (c1 == '\n') {
                line1++;
                c1 = 0;
            }
        }

        unsigned char c2 = 0;
        while (pos2 < data2.size() && c2 == 0) {
            c2 = data2[pos2++];
            if (c2 == '\n') {
                line2++;
                c2 = 0;
            }
        }

        if (c1 != c2) {
            std::cerr << "mismatch.\n"
                      << "  line1=" << line1 << " line2=" << line2 << "\n"
                      << "  data1=" << (char)c1 << " data2=" << (char)c2 << "\n";
            return 1;
        }
    }

    return 0;
}
