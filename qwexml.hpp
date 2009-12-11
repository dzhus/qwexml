#ifndef QWE_XML_H
#define QWE_XML_H
#include <string>

using namespace std;

/**
 * Heterogeneous list
 * 
 * @param Data Base class for element stored in the list.
 */
template <class Data>
class QweList {
    class Node {
    public:
        Data *data;
        Node *next, *prev;
        Node(Data *d)
            :next(0), prev(0), data(d)
        {}
    };
private:
    Node *head, *tail;
public:
    QweList(void)
        :head(0), tail(0)
    {}
    
    void append_item(Data *d)
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
    bool is_empty(void)
    {
        return (head == 0);
    }

};

/**
 * Node of XML document, either text or element.
 */
class QweXmlNode {
};
typedef QweList <QweXmlNode> QweNodeList;


/**
 * Node containing only text.
 */
class QweTextNode : public QweXmlNode {
private:
    string str;
public:
    QweTextNode(string s)
        :str(s)
    {}

    string get_contents(void)
    {
        return this->str;
    }
};

/**
 * Element node with attributes and children.
 */
class QweElementNode : public QweXmlNode {
    class QweAttrNode {
    private:
        string name;
        string value;
    public:
        QweAttrNode(string n, string v)
            :name(n), value(v)
        {}
    };
    typedef QweList <QweAttrNode> QweAttrList;
    
private:
    string name;
    QweNodeList *children;
    QweAttrList *attributes;
    
public:
    QweElementNode(string s)
        :name(s)
    {
        children = new QweNodeList();
        attributes = new QweAttrList();
    }

    /**
     * Add new attribute to element.
     */
    void add_attribute(string name, string value)
    {
        attributes->append_item(new QweAttrNode(name, value));
    }
    
    /**
     * Add new child node to element.
     */
    void add_child(QweXmlNode *n)
    {
        children->append_item(n);
    }
    
    string get_name(void)
    {
        return this->name;
    }
};
#endif
