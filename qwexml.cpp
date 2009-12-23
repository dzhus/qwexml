#include "qwexml.hpp"

namespace qwe {
    XmlNode::XmlNode(void)
    {
        parent = 0;
    }

    XmlNode* XmlNode::get_parent(void)
    {
        return parent;
    }

    TextNode::TextNode(std::string s)
        :str(s)
    {}
    
    TextNode::TextNode(TextNode &n)
    {
        str = n.str;
    }

    std::string TextNode::get_contents(void)
    {
        return str;
    }
    
    void TextNode::set_contents(std::string s)
    {
        str = s;
    }

    std::string TextNode::get_printable(void)
    {
        return get_contents();
    }

    TextNode* TextNode::_copy(void)
    {
        return new TextNode(*this);
    }

    ElementNode::AttrNode::AttrNode(std::string n, std::string v)
        :name(n), value(v)
    {}

    ElementNode::AttrNode::AttrNode(AttrNode &n)
    {
        name = n.name;
        value = n.value;
    }

    ElementNode::AttrNode* ElementNode::AttrNode::_copy(void)
    {
        return new AttrNode(*this);
    }

    std::string ElementNode::AttrNode::get_name(void)
    {
        return name;
    }

    std::string ElementNode::AttrNode::get_value(void)
    {
        return value;
    }

    void ElementNode::AttrNode::set_value(std::string &v)
    {
        value = v;
    }

    ElementNode::ElementNode(void)
    {
        children = new NodeList();
        attributes = new AttrList();
    }
    
    ElementNode::ElementNode(std::string s)
        :name(s)
    {
        children = new NodeList();
        attributes = new AttrList();
    }

    /**
     * Deep copy of existing element node.
     */
    ElementNode::ElementNode(ElementNode &n)
    {
        name = n.name;
        children = new NodeList(*n.children);
        attributes = new AttrList(*n.attributes);
    }

    ElementNode* ElementNode::_copy(void)
    {
        return new ElementNode(*this);
    }

    /**
     * Add new attribute to element.
     */
    void ElementNode::add_attribute(std::string name, std::string value)
    {
        /// @note
        /// Double memory allocation, because list copies all
        /// data!
        attributes->push_item(new AttrNode(name, value));
    }

    void ElementNode::add_attribute(AttrNode *n)
    {
        attributes->push_item(n);
    }

    /**
     * Add new child element node to element.
     */
    void ElementNode::add_child(ElementNode *n)
    {
        children->push_item(n);
        ((ElementNode *)(last_child()))->parent = this;
    }

    /**
     * Add new child text node to element.
     */
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

    /**
     * Return plain name of element.
     */
    std::string ElementNode::get_name(void)
    {
        return name;
    }

    void ElementNode::set_name(std::string s)
    {
        name = s;
    }

    /**
     * std::string of element node with all attributes and children.
     */
    std::string ElementNode::get_printable(void)
    {
        std::string s = std::string();
        s += "<" + get_name();
        if (has_attributes())
        {
            s += " ";
            AttrList::StlIterator i = attributes_begin(), e = attributes_end();
            bool first = true;
            while (i != e)
            {
                if (!first)
                    s += " ";
                first = false;
                s += (*i)->get_name();
                s += "=\"" + (*i)->get_value() +"\"";
                i++;
            }
        }
        s += ">";
        if (has_children())
        {
            NodeList::StlIterator i = children_begin(), e = children_end();
            while (i != e)
            {
                s += (*i)->get_printable();
                i++;
            }
        }
        s+= "</" + get_name() + ">";
        return s;
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

    ElementNode::AttrList::StlIterator ElementNode::attributes_begin(void)
    {
        return attributes->begin();
    }

    ElementNode::AttrList::StlIterator ElementNode::attributes_end(void)
    {
        return attributes->end();
    }

    /**
     * Convinience accessor.
     */
    XmlNode* ElementNode::first_child(void)
    {
        return children->first_item();
    }

    XmlNode* ElementNode::last_child(void)
    {
        return children->last_item();
    }

    ElementNode::AttrNode* ElementNode::first_attribute(void)
    {
        return attributes->first_item();
    }
}
