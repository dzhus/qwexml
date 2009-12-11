#ifndef QWE_XML_H
#define QWE_XML_H
#ifdef QWE_USE_STL
#include <iterator>
#endif

#include <string>

using namespace std;

/**
 * Heterogeneous list
 * 
 * @param Data Base class for element stored in the list.
 */
template <typename T>
class QweList {
private:
    typedef T Data;

    class Node {
    public:
        Data *data;
        Node *next, *prev;

        Node(void)
            :next(0), prev(0), data(0)
        {}

        Node(Data *d)
            :next(0), prev(0), data(d)
        {}
    };

    Node *head, *tail, *head_sentinel, *tail_sentinel;

public:
#ifdef QWE_USE_STL
    class StlIterator {
    public:
        /**
         * Standard traits.
         */
        typedef input_iterator_tag iterator_category;
        typedef Data value_type;
        typedef ptrdiff_t difference_type;
        typedef Data* pointer;
        typedef Data& reference;

        /**
         * List we iterate over.
         */
        QweList<Data> *list;

        /**
         * Current list position.
         */
        Node *position;

        StlIterator(void)
            :list(0), position(0)
        {}

        StlIterator(QweList<T>* l, Node* p)
            :list(l), position(p)
        {}

        
        StlIterator& operator ++(void)
        {
            position = position->next;
        }

        StlIterator& operator ++(int)
        {
            position = position->next;
        }

        StlIterator& operator --(void)
        {
            position = position->prev;
        }

        StlIterator& operator --(int)
        {
            position = position->prev;
        }

        /**
         * Compare two iterators by comparing underlying lists and
         * positions.
         */
        bool operator==(StlIterator iter)
        {
            return (list == iter.list && position == iter.position);
        }

        bool operator !=(StlIterator iter)
        {
            return (list != iter.list || position != iter.position);
        }

        StlIterator& operator =(StlIterator iter)
        {
            list = iter.list;
            position = iter.position;
            return *this;
        }

        Data* operator *(void)
        {
            return position->data;
        }
    };
#endif

    QweList(void)
        :head(0), tail(0)
    {
        /// Each list must have unique sentinels
        head_sentinel = new Node();
        tail_sentinel = new Node();
    }
    
    /**
     * Append new item to the end of list.
     */
    void append_item(Data *d)
    {
        Node *n = new Node(d);
        if (!head)
        {
            head = tail = n;
        }
        else
        {
            tail->next = n;
            n->prev = tail;
            tail = n;
        }
        tail->next = tail_sentinel;
        head->prev = head_sentinel;
    }
    
    /**
     * Return true if list is empty.
     */
    bool is_empty(void)
    {
        return (head == 0);
    }

#ifdef QWE_USE_STL
    StlIterator begin(void)
    {
        return StlIterator(this, this->head);
    }

    StlIterator rbegin(void)
    {
        return StlIterator(this, this->tail);
    }

    StlIterator end(void)
    {
        return StlIterator(this, this->tail_sentinel);
    }

    StlIterator rend(void)
    {
        return StlIterator(this, this->head_sentinel);
    }
#endif
};

/**
 * Node of XML document, either text or element.
 */
class QweXmlNode {
public:
    virtual string get_printable(void) = 0;
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

    /**
     * Raw contents of text node.
     */
    string get_contents(void)
    {
        return str;
    }

    string get_printable(void)
    {
        return get_contents();
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
    
    bool has_children(void)
    {
        return !(children->is_empty());
    }

    /**
     * Return plain name of element.
     */
    string get_name(void)
    {
        return name;
    }

    string get_printable(void)
    {
        return get_name();
    }

#ifdef QWE_USE_STL
    /**
     * STL iterators for children
     *
     * @todo Boilerplate code.
     */
    QweNodeList::StlIterator children_begin(void)
    {
        return children->begin();
    }

    QweNodeList::StlIterator children_end(void)
    {
        return children->end();
    }

    QweNodeList::StlIterator children_rbegin(void)
    {
        return children->rbegin();
    }

    QweNodeList::StlIterator children_rend(void)
    {
        return children->rend();
    }
#endif
};
#endif
