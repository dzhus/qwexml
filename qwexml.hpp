#ifndef QWE_XML_H
#define QWE_XML_H
#ifdef QWE_USE_STL
#include <iterator>
#endif

/**
 * @todo Use homebrew string implementation.
 * 
 * @todo Fix memory leaks.
 */
#include <string>

using namespace std;

/**
 * Heterogeneous list.
 *
 * List elements are copied as they're added to the list. List
 * elements must support virual _copy() method which returns a pointer
 * to the copied object.
 * 
 * @param T Base class for element stored in the list.
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
            :next(0), prev(0)
        {
            data = d->_copy();
        }
    };

    Node *head, *tail, *head_sentinel, *tail_sentinel;

public:
    /**
     * STL-style bidirectional iterator for QweList.
     *
     * @todo Output iteration
     */
    class StlIterator {
    private:
        /**
         * List we iterate over.
         */
        QweList<Data> *list;

        /**
         * Current list position.
         */
        Node *position;

    public:
#ifdef QWE_USE_STL
        /**
         * Standard traits.
         */
        typedef bidirectional_iterator_tag iterator_category;
        typedef Data value_type;
        typedef ptrdiff_t difference_type;
        typedef Data* pointer;
        typedef Data& reference;
#endif

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

    QweList(void)
        :head(0), tail(0)
    {
        /// Each list must have unique sentinels
        head_sentinel = new Node();
        tail_sentinel = new Node();
    }

    QweList(QweList &l)
    {
        head_sentinel = new Node();
        tail_sentinel = new Node();

        StlIterator i = l.begin(), end = l.end();
        while (i != end)
        {
            append_item(*i);
            i++;
        }
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

    StlIterator begin(void)
    {
        /// @internal When list is empty, immediately point to tail
        /// sentinel, so that @code{begin() == end()}
        return StlIterator(this, head ? head : tail_sentinel);
    }

    StlIterator rbegin(void)
    {
        return StlIterator(this, tail ? tail : head_sentinel);
    }

    StlIterator end(void)
    {
        return StlIterator(this, this->tail_sentinel);
    }

    StlIterator rend(void)
    {
        return StlIterator(this, this->head_sentinel);
    }
};

/**
 * Node of XML document, either text or element.
 *
 * @todo Decouple get_printable(). Implement iterators for traversing
 * the whole tree (depth-first).
 */
class QweXmlNode {
public:
    virtual string get_printable(void) = 0;

    /**
     * Return a pointer to copy of current object.
     *
     * We need this because copy constructors can't be declared as
     * virtual.
     */
    virtual QweXmlNode* _copy(void) = 0;
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

    QweTextNode(QweTextNode &n)
    {
        str = n.str;
    }

    /**
     * Raw contents of text node.
     */
    string get_contents(void)
    {
        return str;
    }

    void set_contents(string s)
    {
        str = s;
    }

    string get_printable(void)
    {
        return get_contents();
    }

    QweTextNode* _copy(void)
    {
        return new QweTextNode(*this);
    }
};

/**
 * Element node with attributes and children.
 *
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

        QweAttrNode(QweAttrNode &n)
        {
            name = n.name;
            value = n.value;
        }

        QweAttrNode* _copy(void)
        {
            return new QweAttrNode(*this);
        }

        string get_name(void)
        {
            return name;
        }

        string get_value(void)
        {
            return value;
        }
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
     * Deep copy of existing element node.
     */
    QweElementNode(QweElementNode &n)
    {
        name = n.name;
        children = new QweNodeList(*n.children);
        attributes = new QweAttrList(*n.attributes);
    }

    /**
     * Add new attribute to element.
     */
    void add_attribute(string name, string value)
    {
        /// @note
        /// Double memory allocation, because list copies all
        /// data!
        attributes->append_item(new QweAttrNode(name, value));
    }

    void add_attribute(QweAttrNode *n)
    {
        attributes->append_item(n);
    }
    
    /**
     * Add new child element node to element.
     */
    void add_child(QweElementNode *n)
    {
        children->append_item(n);
    }

    /**
     * Add new child text node to element.
     */
    void add_child(QweTextNode *n)
    {
        children->append_item(n);
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
    string get_name(void)
    {
        return name;
    }

    void set_name(string s)
    {
        name = s;
    }
    
    /**
     * String of element node with all attributes and children.
     */
    string get_printable(void)
    {
        string s = string();
        s += "<" + get_name();
        if (has_attributes())
        {
            s += " ";
            QweAttrList::StlIterator i = attributes_begin(), e = attributes_end();
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
            QweNodeList::StlIterator i = children_begin(), e = children_end();
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

    QweAttrList::StlIterator attributes_begin(void)
    {
        return attributes->begin();
    }

    QweAttrList::StlIterator attributes_end(void)
    {
        return attributes->end();
    }

    QweElementNode* _copy(void)
    {
        return new QweElementNode(*this);
    }
};
#endif
