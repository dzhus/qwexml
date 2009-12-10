/**
 * Heterogeneous list
 * 
 * @param Data Base class for element stored in the list.
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
private:
    Node *head, *tail;
public:
    QweList(void)
    {
        head = 0;
        tail = 0;
    }

    /**
     * Add new item to the end of list.
     */
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

    /**
     * Return true if list is empty.
     */
    bool is_empty(void)
    {
        return (head == 0);
    }
};

