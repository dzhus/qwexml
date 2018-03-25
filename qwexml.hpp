#ifndef QWE_XML_H
#define QWE_XML_H
#ifdef QWE_USE_STL
#include <iterator>
#endif
#include "qwelist.hpp"
#include "qwestring.hpp"

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
    private:
        /**
         * Pointer to parent node.
         */
        XmlNode* parent;

    public:
        XmlNode(void);
        virtual ~XmlNode(void);

        XmlNode* get_parent(void);

        virtual String& get_printable(void) = 0;

        friend class TextNode;
        friend class ElementNode;
    };
    typedef List <XmlNode *> NodeList;


    /**
     * Node containing only text.
     */
    class TextNode : public XmlNode {
    private:
        String str;

    public:
        /**
         * Constructs TextNode object with given contents.
         */
        TextNode(String &s);

        /**
         * Returns raw contents of text node.
         */
        String get_contents(void);

        void set_contents(String &s);

        /**
         * Returns printable representation of text node contents.
         */
        String& get_printable(void);
    };

    /**
     * Element attribute class.
     */
    class AttrNode {
    private:
        String name;
        String value;
    public:
        AttrNode(String &n, String &v);

        String& get_name(void);

        String& get_value(void);

        void set_value(String &v);
    };
    typedef List <AttrNode *> AttrList;

    /**
     * Element node with attributes and children.
     */
    class ElementNode : public XmlNode {
    private:
        /**
         * Name of XML element.
         */
        String name;

        NodeList *children;
        AttrList *attributes;

    public:
        ElementNode(void);

        ~ElementNode(void);

        ElementNode(String &s);

        /**
         * Adds new attribute to element provided its key and value.
         */
        void add_attribute(String &name, String &value);

        /**
         * Adds new attribute using a pointer to existing AttrNode object.
         */
        void add_attribute(AttrNode *n);

        /**
         * Adds new child element node to element.
         */
        void add_child(ElementNode *n);

        /**
         * Adds new child text node to element.
         */
        void add_child(TextNode *n);

        bool has_children(void);

        bool has_attributes(void);

        /**
         * Returns plain name of element.
         */
        String& get_name(void);

        void set_name(String &s);

        /**
         * Returns printable representation of element node with all
         * attributes and children.
         */
        String& get_printable(void);

        /**
         * Iterators for children
         *
         * @todo Boilerplate code.
         */
        NodeList::StlIterator children_begin(void);

        NodeList::StlIterator children_end(void);

        NodeList::StlIterator children_rbegin(void);

        NodeList::StlIterator children_rend(void);

        AttrList::StlIterator attributes_begin(void);

        AttrList::StlIterator attributes_end(void);

        /**
         * Convinience accessors.
         *
         * Element children are text nodes and/or other elements.
         */
        XmlNode* first_child(void);
        XmlNode* last_child(void);
        AttrNode* first_attribute(void);
    };
}
#endif
