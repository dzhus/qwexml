#include <iostream>
#include "qweparse.hpp"

using namespace qwe;

/**
 * Read XML from standard input and print it back to standard output.
 */
int main()
{
    XmlParser *p = new XmlParser();
    std::cin >> *p;

    TokenList::StlIterator i = p->tokens_begin(), end = p->tokens_end();

    while (i != end)
    {
        std::cout << (*i)->get_contents();
        i++;
    }
    return 0;
}

