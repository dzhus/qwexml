#ifndef QWE_XML_H
#define QWE_XML_H
#ifdef QWE_USE_STL
#include <iterator>
#endif
#include "qwelist.hpp"

/**
 * XML structure classes.
 */

namespace qwe {

    /**
     * Node of XML document, either text or element.
     *
     * @todo Decouple get_printable(). Implement iterators for traversing
     * the whole tree (depth-first).
     */
    class XmlNode {
    public:
        virtual std::string get_printable(void) = 0;

        /**
         * Return a pointer to copy of current object.
         *
         * We need this because copy constructors can't be declared as
         * virtual.
         */
        virtual XmlNode* _copy(void) = 0;
    };
    typedef List <XmlNode> NodeList;


    /**
     * Node containing only text.
     */
    class TextNode : public XmlNode {
    private:
        std::string str;

    public:
        TextNode(std::string s)
            :str(s)
        {}

        TextNode(TextNode &n)
        {
            str = n.str;
        }

        /**
         * Raw contents of text node.
         */
        std::string get_contents(void)
        {
            return str;
        }

        void set_contents(std::string s)
        {
            str = s;
        }

        std::string get_printable(void)
        {
            return get_contents();
        }

        TextNode* _copy(void)
        {
            return new TextNode(*this);
        }
    };

    /**
     * Element node with attributes and children.
     *
     */
    class ElementNode : public XmlNode {
    private:
        class AttrNode {
        private:
            std::string name;
            std::string value;
        public:
            AttrNode(std::string n, std::string v)
                :name(n), value(v)
            {}

            AttrNode(AttrNode &n)
            {
                name = n.name;
                value = n.value;
            }

            AttrNode* _copy(void)
            {
                return new AttrNode(*this);
            }

            std::string get_name(void)
            {
                return name;
            }

            std::string get_value(void)
            {
                return value;
            }
        };
        typedef List <AttrNode> AttrList;

        std::string name;
        NodeList *children;
        AttrList *attributes;

    public:
        ElementNode(void)
        {
            children = new NodeList();
            attributes = new AttrList();
        }

        ElementNode(std::string s)
            :name(s)
        {
            children = new NodeList();
            attributes = new AttrList();
        }

        /**
         * Deep copy of existing element node.
         */
        ElementNode(ElementNode &n)
        {
            name = n.name;
            children = new NodeList(*n.children);
            attributes = new AttrList(*n.attributes);
        }

        /**
         * Add new attribute to element.
         */
        void add_attribute(std::string name, std::string value)
        {
            /// @note
            /// Double memory allocation, because list copies all
            /// data!
            attributes->push_item(new AttrNode(name, value));
        }

        void add_attribute(AttrNode *n)
        {
            attributes->push_item(n);
        }

        /**
         * Add new child element node to element.
         */
        void add_child(ElementNode *n)
        {
            children->push_item(n);
        }

        /**
         * Add new child text node to element.
         */
        void add_child(TextNode *n)
        {
            children->push_item(n);
        }

        bool has_children(void)
        {
            return !(children->is_empty());
        }

        bool has_attributes(void)
        {
            return !(attributes->is_empty());
        }

        /**
         * Return plain name of element.
         */
        std::string get_name(void)
        {
            return name;
        }

        void set_name(std::string s)
        {
            name = s;
        }

        /**
         * std::string of element node with all attributes and children.
         */
        std::string get_printable(void)
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

        /**
         * STL-style iterators for children
         *
         * @todo Boilerplate code.
         */
        NodeList::StlIterator children_begin(void)
        {
            return children->begin();
        }

        NodeList::StlIterator children_end(void)
        {
            return children->end();
        }

        NodeList::StlIterator children_rbegin(void)
        {
            return children->rbegin();
        }

        NodeList::StlIterator children_rend(void)
        {
            return children->rend();
        }

        AttrList::StlIterator attributes_begin(void)
        {
            return attributes->begin();
        }

        AttrList::StlIterator attributes_end(void)
        {
            return attributes->end();
        }

        ElementNode* _copy(void)
        {
            return new ElementNode(*this);
        }
    };
}
#endif
