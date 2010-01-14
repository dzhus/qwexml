#ifndef QWE_STRING_H
#define QWE_STRING_H
#include <iostream>
#include "qwelist.hpp"

/**
 * String.
 */

namespace qwe {
    typedef List <char> CharList;

    class LString {
    private:
        /**
         * Characters.
         */
        CharList *chars;

    public:
        LString(void);

        LString(const char *c);

        LString(LString &s);

        ~LString(void);
        
        /**
         * Appends character contents to string.
         */
        void append(const char *c);

        void append(char c);
        
        /**
         * Sends string to output stream.
         */
        void send(std::ostream &o);

        friend LString& operator +=(LString &s1, LString &s2);
        friend bool operator ==(LString &s1, LString &s2);
    };

    std::ostream& operator <<(std::ostream &o, LString &s);
    LString& operator +=(LString &s, const char *c);
    LString& operator +=(LString &s, char c);

    typedef LString String;
}
#endif
