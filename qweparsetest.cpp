#include <iostream>
#include "qweparse.hpp"

using namespace qwe;

/**
 * Read XML from standard input and print it back to standard output.
 */
int main()
{
    qwe::TokenList *xml_tokens = new qwe::TokenList();
    xml_tokens->push_item(new TagToken());
    xml_tokens->push_item(new SpaceToken());
    xml_tokens->push_item(new TextToken());

    XmlLexer *l = new XmlLexer(xml_tokens);
    std::cin >> *l;
    TokenList::StlIterator i = l->begin(), end = l->end();
    while (i != end)
    {
        std::cout << (*i)->get_contents();
        i++;
    }
    return 0;
}

