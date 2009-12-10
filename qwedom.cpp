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
 * XML element with siblings and attributes.
 */
class QweElementNode : public QweXmlNode {

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
    
    /**
     * Add new attribute to element.
     */
    void add_attribute(string name, string value)
    {
        return attributes->append_item(new QweAttrNode(name, value));
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
    QweTextNode *t = new QweTextNode("Text inside the tag");
    root->add_child(t);
    root->add_child(s);
    root->add_child(t);
    root->add_attribute("key", "value");
    s->add_child(t);
    s->add_attribute("key2", "value");
    return 0;
}
