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
    const int buf_size = 128;
    
    XmlParser *p = new XmlParser();
    char buffer[buf_size];
    std::istringstream *is;

    while (std::cin.getline(buffer, buf_size))
    {
        is = new std::istringstream();
        is->str(buffer);
        *is >> *p;

        if(p->top())
        {
            std::cout << "<< " << finished_string(p) << ": " << p->top()->get_printable();
            std::cout << std::endl;
        }
    }
    return 0;
}

