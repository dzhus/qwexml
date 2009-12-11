#include <string>
#include "qwexml.hpp"

using namespace std;

template <class Data>
QweList<Data>::Node::Node(Data *d)
    :next(0), prev(0), data(d)
{}

template <class Data>
QweList<Data>::QweList(void)
    :head(0), tail(0)
{}
    
template <class Data>
void QweList<Data>::append_item(Data *d)
{
    Node *n = new Node(d);
    if (!head)
        head = tail = n;
    else
    {
        n->prev = tail;
        tail->next = n;
        tail = n;
    }
}

/**
 * Return true if list is empty.
 */
template <class Data>
bool QweList<Data>::is_empty(void)
{
    return (head == 0);
}

QweTextNode::QweTextNode(string s)
    :str(s) 
{}


QweElementNode::QweElementNode(string s)
    :name(s)
{
    children = new QweNodeList();
    attributes = new QweAttrList();
}

QweElementNode::QweAttrNode::QweAttrNode(string n, string v)
    :name(n), value(v)
{}

/**
 * Add new attribute to element.
 */
void QweElementNode::add_attribute(string name, string value)
{
    attributes->append_item(new QweAttrNode(name, value));
}

void QweElementNode::add_child(QweXmlNode *n)
{
    children->append_item(n);
}

