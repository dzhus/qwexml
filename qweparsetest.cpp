#include <iostream>
#include <sstream>
#include "qweparse.hpp"

using namespace qwe;

std::string finished_string(XmlParser *p)
{
    return ((p->is_finished()) ? "FINISHED" : "UNFINISHED");
}

/**
 * Read XML from standard input and print it back to standard output.
 */
int main()
{
    /// @todo Find out why paring fails with smaller values
    const int buf_size = 128;

    XmlParser *p = new XmlParser();
    char buffer[buf_size];
    std::istringstream *is;

    while (std::cin.getline(buffer, buf_size))
    {
        is = new std::istringstream();
        is->str(buffer);
        *is >> *p;

        if (p->top())
        {
            std::cout << ":: " << finished_string(p) << ": ";
            std::cout << p->top()->get_printable() << std::endl;
        }
    }
    return 0;
}
