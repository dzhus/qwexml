#ifndef QWE_LIST_H
#define QWE_LIST_H
#ifdef QWE_USE_STL
#include <iterator>
#endif
#include <string>

/**
 * @todo Use homebrew string implementation.
 * 
 * @todo Fix memory leaks.
 */

namespace qwe {
    
    /**
     * Heterogeneous list.
     *
     * List elements are copied as they're added to the list. List
     * elements must support virual _copy() method which returns a pointer
     * to the copied object.
     *
     * @param T Base class for element stored in the list.
     *
     * @internal We need _copy because virtual copy constructors aren't
     * supported.
     */
    template <class T>
    class List {
    private:
        typedef T Data;

        class ListItem {
        public:
            ListItem *next, *prev;

            /**
             * Pointer to item data.
             *
             * @remark Rationale behind storing pointers is our wish to
             * support STL-style external iterators for lists of objects
             * which are inherited from single base class.
             */
            Data *data;

            ListItem(void)
                :next(0), prev(0), data(0)
            {}

            ListItem(Data *d)
                :next(0), prev(0)
            {
                data = d->_copy();
            }
        };

        ListItem *head, *tail, *head_sentinel, *tail_sentinel;

        void _init_sentinels(void)
        {
            /// Each list must have unique sentinels
            head_sentinel = new ListItem();
            tail_sentinel = new ListItem();
        }
    public:
        /**
         * STL-style bidirectional iterator for List.
         *
         * @todo Output iteration
         */
        class StlIterator {
        private:
            /**
             * List we iterate over.
             */
            List<Data> *list;

            /**
             * Current list position.
             */
            ListItem *position;

        public:
    #ifdef QWE_USE_STL
            /**
             * Standard traits.
             */
            typedef std::bidirectional_iterator_tag iterator_category;
            typedef Data value_type;
            typedef ptrdiff_t difference_type;
            typedef Data* pointer;
            typedef Data& reference;
    #endif

            StlIterator(void)
                :list(0), position(0)
            {}

            StlIterator(List<T>* l, ListItem* p)
                :list(l), position(p)
            {}


            StlIterator& operator ++(void)
            {
                return position = position->next;
            }

            StlIterator& operator ++(int)
            {
                position = position->next;
                return *this;
            }

            StlIterator& operator --(void)
            {
                position = position->prev;
                return *this;
            }

            StlIterator& operator --(int)
            {
                position = position->prev;
                return *this;
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

            /**
             * Return pointer to iterator's current item.
             */
            Data* operator *(void)
            {
                return position->data;
            }
        };

        List(void)
            :head(0), tail(0)
        {
            _init_sentinels();
        }

        List(List &l)
        {
            _init_sentinels();

            StlIterator i = l.begin(), end = l.end();
            while (i != end)
            {
                push_item(*i);
                i++;
            }
        }

        /**
         * Append new item to the end of list.
         */
        void push_item(Data *d)
        {
            ListItem *n = new ListItem(d);
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

        /**
         * @internal When list is empty, immediately point to tail
         * sentinel, so that <pre>begin() == end()</pre> holds.
         */
        StlIterator begin(void)
        {
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
}
#endif
