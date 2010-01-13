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
     * Bidirectional list.
     *
     * List items contain pointers which were pushed to the list.
     *
     * Supports STL-style iteration.
     *
     * @param T Base class for element stored in the list.
     */
    template <class T>
    class List {
    private:
        typedef T Data;

        class ListItem {
        public:
            ListItem *next, *prev;
            Data *data;

            ListItem(void)
                :next(0), prev(0), data(0)
            {}

            ListItem(Data *d)
                :next(0), prev(0)
            {
                data = d;
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
                position = position->next;
                return *this;
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
             *
             * @remark Comparing iterators is not the same as
             * comparing items they point to.
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
            :head(0), tail(0)
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
         * Remove last entry from list.
         *
         * @todo Fix leak
         */
        void pop_item()
        {
            if (tail->prev == head_sentinel)
                head = tail = 0;
            else
            {
                tail->prev->next = tail_sentinel;
                tail = tail->prev;
            }
        }
        
        /**
         * Return true if list is empty.
         */
        bool is_empty(void)
        {
            return (head == 0);
        }

        /**
         * Remove all items from list.
         */
        void clear(void)
        {
            StlIterator begin, end;
            begin = rbegin();
            end = rend();
            while (begin != end)
            {
                pop_item();
                begin--;
            }
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

        Data* first_item(void)
        {
            return *begin();
        }

        Data* last_item(void)
        {
            return *rbegin();
        }

    };
}
#endif
