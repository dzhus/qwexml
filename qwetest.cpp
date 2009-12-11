#include <iostream>
#include "qwexml.hpp"

int main()
{
    QweElementNode *root = new QweElementNode("root");
    QweElementNode *tag;
    
    string s;
    int i;
    for (i = 0; i < 5; i++)
    {
        root->add_child(new QweElementNode("tag" + string(s)));
        s += "1";
    }

    QweNodeList::StlIterator Iter;

    cout << "Forward:" << endl;
    for (Iter = root->get_children.begin();
         Iter != root->children_end();
         Iter++)
    {
        cout << (*Iter)->get_printable() << endl;
    }

    cout << "Reverse:" << endl;
    for (Iter = root->children_rbegin();
         Iter != root->children_rend();
         Iter--)
    {
        cout << (*Iter)->get_printable() << endl;
    }

    return 0;
}
