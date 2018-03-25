#include "qwexml.hpp"

namespace qwe {
    XmlNode::XmlNode(void)
    {
        parent = 0;
    }

    XmlNode::~XmlNode(void)
    {}

    XmlNode* XmlNode::get_parent(void)
    {
        return parent;
    }

    TextNode::TextNode(String &s)
        :str(s)
    {}

    String TextNode::get_contents(void)
    {
        return str;
    }

    void TextNode::set_contents(String &s)
    {
        str = s;
    }

    String& TextNode::get_printable(void)
    {
        return str;
    }

    AttrNode::AttrNode(String &n, String &v)
        :name(n), value(v)
    {}

    String& AttrNode::get_name(void)
    {
        return name;
    }

    String& AttrNode::get_value(void)
    {
        return value;
    }

    void AttrNode::set_value(String &v)
    {
        value = v;
    }

    ElementNode::ElementNode(void)
    {
        children = new NodeList();
        attributes = new AttrList();
    }

    ElementNode::ElementNode(String &s)
        :name(s)
    {
        children = new NodeList();
        attributes = new AttrList();
    }

    ElementNode::~ElementNode(void)
    {
        delete children;
        delete attributes;
    }

    void ElementNode::add_attribute(String &name, String &value)
    {
        attributes->push_item(new AttrNode(name, value));
    }

    void ElementNode::add_attribute(AttrNode *n)
    {
        attributes->push_item(n);
    }

    void ElementNode::add_child(ElementNode *n)
    {
        children->push_item(n);
        ((ElementNode *)(last_child()))->parent = this;
    }

    void ElementNode::add_child(TextNode *n)
    {
        children->push_item(n);
        ((TextNode *)(last_child()))->parent = this;
    }

    bool ElementNode::has_children(void)
    {
        return !(children->is_empty());
    }

    bool ElementNode::has_attributes(void)
    {
        return !(attributes->is_empty());
    }

    String& ElementNode::get_name(void)
    {
        return name;
    }

    void ElementNode::set_name(String &s)
    {
        name = String(s);
    }

    /**
     * Return printable string of element tag with attributes, then
     * recursively traverse all children and add their printable
     * strings.
     */
    String& ElementNode::get_printable(void)
    {
        /// Allocate on heap so we can return a reference.
        String *s = new String();
        /// Opening tag
        *s += "<";
        *s += get_name();

        /// Attributes
        if (has_attributes())
        {
            *s += " ";
            AttrList::StlIterator i = attributes_begin(), e = attributes_end();
            bool first = true;
            while (i != e)
            {
                if (!first)
                    *s += " ";
                first = false;
                *s += (*i)->get_name();
                *s += "=\"";
                *s += (*i)->get_value();
                *s += "\"";
                i++;
            }
        }
        *s += ">";

        /// Children
        if (has_children())
        {
            NodeList::StlIterator i = children_begin(), e = children_end();
            while (i != e)
            {
                *s += (*i)->get_printable();
                i++;
            }
        }

        /// Closing tag
        *s += "</";
        *s += get_name();
        *s += ">";
        return *s;
    }

    NodeList::StlIterator ElementNode::children_begin(void)
    {
        return children->begin();
    }

    NodeList::StlIterator ElementNode::children_end(void)
    {
        return children->end();
    }

    NodeList::StlIterator ElementNode::children_rbegin(void)
    {
        return children->rbegin();
    }

    NodeList::StlIterator ElementNode::children_rend(void)
    {
        return children->rend();
    }

    AttrList::StlIterator ElementNode::attributes_begin(void)
    {
        return attributes->begin();
    }

    AttrList::StlIterator ElementNode::attributes_end(void)
    {
        return attributes->end();
    }

    XmlNode* ElementNode::first_child(void)
    {
        return children->first_item();
    }

    XmlNode* ElementNode::last_child(void)
    {
        return children->last_item();
    }

    AttrNode* ElementNode::first_attribute(void)
    {
        return attributes->first_item();
    }
}
