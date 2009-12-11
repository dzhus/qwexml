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
        Node(Data *d);
    };
private:
    Node *head, *tail;
public:
    QweList(void);
    void append_item(Data *d);
    bool is_empty(void);
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
    QweTextNode(string s);
    string get_contents(void);
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
        QweAttrNode(string n, string v);
    };
    typedef QweList <QweAttrNode> QweAttrList;
    
private:
    string name;
    QweNodeList *children;
    QweAttrList *attributes;
    
public:
    QweElementNode(string s);
    void add_attribute(string name, string value);
    void add_child(QweXmlNode *n);
    string get_name(void);
};
#endif
