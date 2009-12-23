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
    private:
        /**
         * Pointer to parent node.
         */
        XmlNode* parent;

    public:
        XmlNode(void);

        XmlNode* get_parent(void);

        virtual std::string get_printable(void) = 0;

        /**
         * Return a pointer to copy of current object.
         *
         * We need this because copy constructors can't be declared as
         * virtual.
         */
        virtual XmlNode* _copy(void) = 0;

        friend class TextNode;
        friend class ElementNode;
    };
    typedef List <XmlNode> NodeList;


    /**
     * Node containing only text.
     */
    class TextNode : public XmlNode {
    private:
        std::string str;

    public:
        /**
         * Constructs TextNode object with given contents.
         */
        TextNode(std::string s);

        TextNode(TextNode &n);

        TextNode* _copy(void);
        
        /**
         * Returns raw contents of text node.
         */
        std::string get_contents(void);

        void set_contents(std::string s);

        /**
         * Returns printable representation of text node contents.
         */
        std::string get_printable(void);
    };

    /**
     * Element attribute class.
     */
    class AttrNode {
    private:
        std::string name;
        std::string value;
    public:
        AttrNode(std::string n, std::string v);
        
        AttrNode(AttrNode &n);
        
        AttrNode* _copy(void);
        
        std::string get_name(void);
        
        std::string get_value(void);
        
        void set_value(std::string &v);
    };
    typedef List <AttrNode> AttrList;

    /**
     * Element node with attributes and children.
     */
    class ElementNode : public XmlNode {
    private:
        /**
         * Name of XML element.
         */
        std::string name;
        
        NodeList *children;
        AttrList *attributes;

    public:
        ElementNode(void);
        
        ElementNode(std::string s);
        
        /**
         * Performs a deep copy of existing element node.
         */
        ElementNode(ElementNode &n);

        ElementNode* _copy(void);

        /**
         * Adds new attribute to element provided its key and value.
         */
        void add_attribute(std::string name, std::string value);

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
        std::string get_name(void);
        
        void set_name(std::string s);
        
        /**
         * Returns printable representation of element node with all
         * attributes and children.
         */
        std::string get_printable(void);
        
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
