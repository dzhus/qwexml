#include <qwelist.cpp>
#include <string>
#include <iterator>
#include <iostream>

using namespace std;

/**
 * Node of XML document, either text or element.
 */
class QweXmlNode {
};
typedef QweList <QweXmlNode> QweXmlTree;


/**
 * Node with text.
 */
class QweTextNode : public QweXmlNode {
private:
    std::string str;
public:
    /** Create new text node.
     *
     * @param s String contents of the node.
     */
    QweTextNode(std::string s)
    {
        str = s;
    }
};

/**
 * Attribute list of element node.
 */
class QweAttrNode {
private:
    string name;
    string value;
public:
    QweAttrNode(string n, string v)
    {
        name = n;
        value = v;
    }
};
typedef QweList <QweAttrNode> QweAttrList;

/**
 * XML element with siblings and attributes.
 */
class QweElementNode : public QweXmlNode {
private:
    std::string name;
    QweXmlTree *children;
    QweAttrList *attributes;
public:
    QweElementNode(std::string s)
    {
        name = s;
        children = new QweXmlTree();
        attributes = new QweAttrList();
    }

    
    void add_attribute(QweAttrNode *n)
    {
        return attributes->append_item(n);
    }
    
    void add_child(QweXmlNode *n)
    {
        children->append_item(n);
    }
};

int main()
{
    QweElementNode *root = new QweElementNode("root");
    QweElementNode *s = new QweElementNode("s");
    QweAttrNode *a = new QweAttrNode("key", "value");
    QweAttrNode *a2 = new QweAttrNode("key2", "value2");
    QweTextNode *t = new QweTextNode("Text inside the tag");
    root->add_child(t);
    root->add_child(s);
    root->add_child(t);
    root->add_attribute(a);
    s->add_child(t);
    s->add_attribute(a2);
    return 0;
}
