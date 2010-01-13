#include "qwestring.hpp"

namespace qwe {
    LString::LString(void)
    {
        chars = new CharList();
    }

    LString::LString(const char *c)
    {
        chars = new CharList();
        while (*c != 0)
            chars->push_item(*c++);
    }

    LString::LString(LString &s)
    {
        chars = new CharList(*s.chars);
    }

    void LString::append(const char *c)
    {
        while (*c != 0)
            chars->push_item(*c++);
    }

    void LString::append(char c)
    {
        chars->push_item(c);
    }

    void LString::send(std::ostream &o)
    {
        CharList::StlIterator i, end;
        i = chars->begin();
        end = chars->end();
        while (i != end)
        {
            o.put(*i);
            i++;
        }
    }

    std::ostream& operator <<(std::ostream &o, LString &s)
    {
        s.send(o);
        return o;
    }

    LString& operator +=(LString &s, const char *c)
    {
        s.append(c);
        return s;
    }

    LString& operator +=(LString &s, char c)
    {
        s.append(c);
        return s;
    }

    LString& operator +=(LString &s1, LString &s2)
    {
        CharList::StlIterator i = s2.chars->begin(), end = s2.chars->end();
        while (i != end)
        {
            s1.append(*i);
            i++;
        }
        return s1;
    }

    bool operator ==(LString &s1, LString &s2)
    {
        CharList::StlIterator i1, i2, end1, end2; 
        i1 = s1.chars->begin();
        end1 = s1.chars->end();
        i2 = s2.chars->begin();
        end2 = s2.chars->end();
        
        if (s1.chars->get_length() == s2.chars->get_length())
        {
            while (i1 != end1)
            {
                if (*i1 != *i2)
                    return false;
                i1++, i2++;
            }
            return true;
        }
        else
            return false;
    }
}
