#include <iostream>
#include <algorithm>
#include "qwexml.hpp"

using namespace qwe;

int main()
{
    ElementNode *root1 = new ElementNode("root1");
    ElementNode *root2 = new ElementNode("root2");
    ElementNode *tag = new ElementNode("tag");
    tag->add_child(new TextNode("text inside the tag"));
    tag->add_attribute("key", "value");
    TextNode *text = new TextNode("my text");

    std::string s;
    int i;
    for (i = 0; i < 5; i++)
    {
        s += "1";
        root1->add_child(text);
        root1->add_child(new TextNode(s));
        root1->add_child(new ElementNode(s));
        root1->add_child(tag);
        root2->add_child(text);
    }

    text->set_contents("changed text");
    std::cout << "Current text contents: " << text->get_contents();
    NodeList::StlIterator Iter;

    std::cout << std::endl << "Previously populated root:" << std::endl;
    std::cout << root1->get_printable();

    ((ElementNode *)(*(root1->children_rbegin())))->set_name("end_tag");
    std::cout << std::endl << "Updated root:" << std::endl;
    std::cout << root1->get_printable();

    std::cout << std::endl << "Reverse:" << std::endl;
    for (Iter = root1->children_rbegin();
         Iter != root1->children_rend();
         Iter--)
    {
        std::cout << (*Iter)->get_printable() << std::endl;
    }
    std::cout << std::endl;
    if (std::equal(root1->children_begin(), root1->children_end(), root1->children_begin()))
        std::cout << "std::equal test #1 passed" << std::endl;
    if (!std::equal(root1->children_begin(), root1->children_end(), root2->children_begin()))
        std::cout << "std::equal test #2 passed" << std::endl;
    return 0;
}
