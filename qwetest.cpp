#include "qwexml.hpp"

int main()
{
    QweElementNode *root = new QweElementNode("root");
    QweElementNode *s = new QweElementNode("s");
    QweTextNode *t = new QweTextNode("Text inside the tag");
    root->add_child(t);
    root->add_child(s);
    root->add_child(t);
    root->add_attribute("key", "value");
    s->add_child(t);
    s->add_attribute("key2", "value");
    return 0;
}
