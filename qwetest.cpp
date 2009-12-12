#include <iostream>
#include <algorithm>
#include "qwexml.hpp"

int main()
{
    QweElementNode *root1 = new QweElementNode("root1");
    QweElementNode *root2 = new QweElementNode("root2");
    QweElementNode *tag = new QweElementNode("tag");
    QweTextNode *text = new QweTextNode("my text");

    string s;
    int i;
    for (i = 0; i < 5; i++)
    {
        s += "1";
        root1->add_child(text);
        root1->add_child(new QweTextNode(s));
        root2->add_child(text);
    }

    text->set_contents("changed text");
    cout << "Current text contents: " << text->get_contents();
    QweNodeList::StlIterator Iter;

    cout << endl << "Previously populated root:" << endl;
    for (Iter = root1->children_begin();
         Iter != root1->children_end();
         Iter++)
    {
        cout << (*Iter)->get_printable() << endl;
    }

    cout << endl << "Reverse:" << endl;
    for (Iter = root1->children_rbegin();
         Iter != root1->children_rend();
         Iter--)
    {
        cout << (*Iter)->get_printable() << endl;
    }
    cout << endl;
    if (std::equal(root1->children_begin(), root1->children_end(), root1->children_begin()))
        cout << "std::equal test #1 passed" << endl;
    if (!std::equal(root1->children_begin(), root1->children_end(), root2->children_begin()))
        cout << "std::equal test #2 passed" << endl;
    return 0;
}
