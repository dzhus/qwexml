#include <string>

/**
 * This module provides QweList container class which allows to store
 * data of different types.
 */

/**
 * Heterogeneous list
 */
template <class Data>
class QweList {
    class Node {
    public:
        Data *data;
        Node *next, *prev;
        
        Node(Data *d)
        {
            next = 0;
            prev = 0;
            data = d;
        }
    };
protected:
    Node *head, *tail;
    std::string sep;
public:
    QweList(std::string s="")
    {
        head = 0;
        tail = 0;
        sep = s;
    }

    void append_item(Data *d)
    {
        Node *n = new Node(d);
        if (!head)
            head = tail = n;
        else
        {
            n->prev = tail;
            tail->next = n;
            tail = n;
        }
    }

    bool is_empty(void)
    {
        return (head == 0);
    }


    std::string get_printable(void)
    {
        std::string out;
        bool first = true;
        Node *c = head;
        
        while (c)
        { 
            if (first)
                first = false;
            else
                out.append(sep);
            out.append(c->data->get_printable());
            c = c->next;
        }
        return out;
    }
};

