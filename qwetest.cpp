#include <stdio.h>
#include <iostream>
#include <algorithm>
#include "qwexml.hpp"

using namespace qwe;

int main()
{
    /// Building tree from C++
    ElementNode *root1 = new ElementNode("root");
    ElementNode *root2 = new ElementNode("root2");
    ElementNode *tag = new ElementNode("tag");
    TextNode *text = new TextNode("my text");

    tag->add_child(text);
    tag->add_attribute("key", "value");

    std::string s[4] = {"foo", "bar", "baz", "quux"};
    for (int i = 0; i < 4; i++)
    {
        root1->add_child(tag);
        ((ElementNode *)(root1->last_child()))->first_attribute()->set_value(s[i]);
        root2->add_child(tag);
    }

    std::cout << "Root:" << std::endl;
    std::cout << root1->get_printable();

    /// Manually traversing node children
    std::cout << std::endl << "Reverse:" << std::endl;
    NodeList::StlIterator Iter;
    for (Iter = root1->children_rbegin();
         Iter != root1->children_rend();
         Iter--)
    {
        std::cout << (*Iter)->get_printable() << std::endl;
    }
    std::cout << std::endl;
    
    /// Using STL algorithms with node children iterators
    if (std::equal(root1->children_begin(), root1->children_end(), root1->children_begin()))
        std::cout << "std::equal test #1 passed" << std::endl;
    if (!std::equal(root1->children_begin(), root1->children_end(), root2->children_begin()))
        std::cout << "std::equal test #2 passed" << std::endl;
    return 0;
}
