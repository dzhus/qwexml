#include "qwexml.hpp"
#include <string>
#include <iostream>
#include <stdlib.h>
#include <cstdio>

enum token_type {NONE, TAG, SPACE, TEXT};

enum error_type {UNKNOWN_TOKEN, TAG_ERROR};

/**
 * Simple error handler.
 */
void qwe_error(error_type n)
{
    switch (n)
    {
    case TAG_ERROR:
        cout << "Error while reading tag" << endl;
        exit(TAG_ERROR);
    case UNKNOWN_TOKEN:
        cout << "Could not choose appropriate token" << endl;
        exit(UNKNOWN_TOKEN);
    }
}

/**
 * Token class.
 *
 * Tokens work with input streams, consuming character data from them.
 * A high level parser must parse character stream consisting of known
 * tokens using the following policy:
 *
 * - choose token to use by calling QweToken::can_eat() method of each
 *   known token with input stream;
 *
 * - feed input stream to chosen token using QweToken::feed() or input
 *   operator;
 *
 * - when feeding returns, finished token may be added to a list of
 *   parsed tokens.
 *
 * QweToken::can_eat() and QweToken::feed() implementations must
 * guarantee that feed() successfully returns only if no errors
 * occured during reading token.
 *
 * We do NOT perform full readback procedure in case of inapproprate
 * input, so while reading a token is in progress all read errors must
 * be considered fatal. Error handling must be implemented in the
 * QweToken::feed method.
 */
class QweToken {
protected:
    /**
     * Raw token contents as read from input stream.
     */
    string contents;
    
    token_type type;
public:
    /**
     * Clear contents and prepare to consume next portion of character
     * data.
     */
    virtual void flush(void)
    {
        contents = "";
    }

    string get_contents(void)
    {
        return contents;
    }

    virtual token_type get_type(void)
    {
        return type;
    }

    /**
     * Check upcoming content in the input stream.
     * 
     * General rule for classes implementing this method is to try as
     * little lookahead as possible.
     *
     * @warning Input stream must be left unmodified.
     *
     * @return True if parser should try feeding this token.
     */
    virtual bool can_eat(istream &in) = 0;
    
    /**
     * Try to add more character contents for token.
     */
    virtual void feed(istream &in) = 0;

    friend istream& operator >>(istream &in, QweToken &t)
    {
        t.feed(in);
        return in;
    }

    virtual QweToken* _copy(void) = 0;
};

/**
 * Token class for XML tags of the following structure:
 *
 @dot
 digraph tag {
 node [shape=rectangle, fontname="sans-serif", fontsize=14];
 edge [fontname="sans-serif", fontsize=10];
 START [shape="oval"];
 START -> OPEN [label="<"];
 OPEN -> SLASH [label="/"];
 OPEN -> NAME [label="[[:alnum:]]"];
 SLASH -> CLOSE_NAME [label="[[:alnum:]]"];
 CLOSE_NAME -> CLOSE_NAME [label="[[:alnum:]]"];
 NAME -> NAME [label="[[:alnum:]]"];
 CLOSE_NAME -> END [label=">"];
 NAME -> END [label=">"];
 NAME -> ESPC [label="[[:space:]]"];
 ESPC -> END [label=">"];
 ESPC -> EMPTY [label="/"];
 EMPTY -> END [label=">"];
 CLOSE_NAME -> ESPC [label="[[:space:]]"];
 END [shape="oval"];
 }
 @enddot

 * @see http://www.w3.org/TR/REC-xml/
 *
 * @verbatim
 * STag ::= '<' Name (S Attribute)* S? '>'
 * EmptyElemTag ::= '<' Name (S Attribute)* S? '/>'
 * ETag ::= '</' Name S? '>'
 * @endverbatim
 */
class QweTagToken : public QweToken {
private:
    enum state {START, OPEN, SLASH, NAME, CLOSE_NAME, ESPC, EMPTY, END};

    state current_state;

    /**
     * Tag name.
     */
    string name;

    /**
     * True if tag is closing.
     */
    bool closing;

    /**
     * True if tag is empty.
     */
    bool empty;

public:
    void flush(void)
    {
        QweToken::flush();
        current_state = START;
        name = "";
        closing = false;
        empty = false;
    }

    QweTagToken(void)
    {
        type = TAG;
        flush();
    }
    
    QweTagToken(QweTagToken &t)
    {
        type = TAG;
        flush();
        contents = t.contents;
        name = t.name;
        closing = t.closing;
        empty = t.empty;
    }

    QweTagToken* _copy(void)
    {
        return new QweTagToken(*this);
    }
    
    /**
     * Read one langle, then possibly one slash, then at least one
     * alpha symbol, then rangle.
     *
     * QweTagToken::name is set to tag name, QweTagToken::closing is
     * set to true if closing tag was read.
     */
    void feed(istream &in)
    {
        char c;
        while (c = in.get())
        {
            bool accepted = true;

            switch (current_state)
            {
            case START:
                if (c == '<')
                    current_state = OPEN;
                else
                    accepted = false;
                break;
            case OPEN:
                if (isalpha(c))
                {
                    name += c;
                    current_state = NAME;
                }
                else if (c == '/')
                {
                    current_state = SLASH;
                    closing = true;
                }
                else
                    accepted = false;
                break;
            case SLASH:
                if (isalpha(c))
                {
                    name += c;
                    current_state = CLOSE_NAME;
                }
                else
                    accepted = false;
                break;
            case CLOSE_NAME:
                if (isalpha(c))
                    name += c;
                else if (c == '>')
                    current_state = END;
                else if (isspace(c))
                    current_state = ESPC;
                else
                    accepted = false;
                break;
            case NAME:
                if (isalpha(c))
                    name += c;
                else if (c == '>')
                    current_state = END; 
                else if (isspace(c))
                    current_state = ESPC;
                else
                    accepted = false;
                break;
            case ESPC:
                if (c == '>')
                    current_state = END;
                else if (c == '/')
                {
                    current_state = EMPTY;
                    empty = true;
                }
                else
                    accepted = false;
                break;
            case EMPTY:
                if (c == '>')
                    current_state = END;
                else
                    accepted = false;
                break;
            }
            
            if (accepted)
            {
                contents += c;

                if (current_state == END)
                    break;
            }
            else
            {
                if (current_state == END)
                {
                    in.putback(c);
                    break;
                }
                else
                    qwe_error(TAG_ERROR);
            }
        }
    }

    /**
     * Decide to eat tag if stream starts with langle.
     */
    bool can_eat(istream &in)
    {
        return ('<' == in.peek());
    }
};

class isxmlspace {
public:
    bool operator () (char c)
    {
        return isalpha(c);
    }
};

class isxmltext {
public:
    bool operator () (char c)
    {
        return ((isgraph(c) || isspace(c)) &&   \
                !((c == '<') || (c == '&')));
    }
};

/**
 * Template for token classes which infinitely read character data for
 * which F holds.
 *
 * @param F Functional object for testing character data.
 */
template <class F>
class QweSimpleToken : public QweToken {
public:
    QweSimpleToken(void)
    {
        type = SPACE;
        flush();
    }
    
    QweSimpleToken(QweSimpleToken &t)
    {
        type = SPACE;
        flush();
        contents = t.contents;
    }
    
    QweSimpleToken* _copy(void)
    {
        return new QweSimpleToken(*this);
    }

    bool can_eat(istream &in)
    {
        return (F()(in.peek()));
    }
    
    /**
     * Read characters while filtering function holds.
     */
    void feed(istream &in)
    {
        char c;
        while (c = in.get())
        {
            if (F()(c))
                contents += c;
            else
            {
                in.putback(c);
                break;
            }
        }
    }
};

typedef QweSimpleToken<isxmltext> QweTextToken;
typedef QweSimpleToken<isxmlspace> QweSpaceToken;

typedef QweList <QweToken> QweTokenList;
//typedef QweList<token_type> QweTokenTypeList;

class QweXmlLexer {
private:
    /**
     * List of complete read tokens.
     */
    QweTokenList *tokens;

    /**
     * List of known token types.
     *
     * Used as workers to read characters. Flushed after every
     * complete token successfully read.
     */
    QweTokenList *known;

    /**
     * Token currently being read.
     */
    QweToken* current;

    /**
     * Choose known token to read next stream data.
     *
     * In case no known token can be read, qwe_error() is called.
     * 
     * @return Pointer to appropriate QweToken.
     */
    QweToken* choose_token(istream &in)
    {
        char c;
        QweTokenList::StlIterator i, end;
        i = known->begin();
        end = known->end();
        while (i != end)
        {
            if ((*i)->can_eat(in))
                return *i;
            else
                i++;
        }
        c = in.peek();
        qwe_error(UNKNOWN_TOKEN);
    }

public:
    QweXmlLexer(QweTokenList *l)
        :current(0)
    {
        tokens = new QweTokenList();
        known = new QweTokenList(*l);
    }

    /**
     * Try each known token, completely read it.
     */
    friend istream& operator >>(istream &in, QweXmlLexer &l)
    {
        char c;

        /// Iterators over the list of known tokens
        QweTokenList::StlIterator i, end;
        end = l.known->end();
        while (in.peek() != -1)
        {
            l.current = l.choose_token(in);

            cin >> *(l.current);
            
            /// Store copy of fully read token
            l.tokens->push_item(l.current);
            /// Flush worker
            l.current->flush();
            l.current = 0;
        }
        return in;
    }

    QweTokenList::StlIterator begin(void)
    {
        return tokens->begin();
    }

    QweTokenList::StlIterator end(void)
    {
        return tokens->end();
    }
};

/**
 * Read XML from standard input and print it back to standard output.
 */
int main()
{
    QweTokenList *xml_tokens = new QweTokenList();
    xml_tokens->push_item(new QweTagToken());
    xml_tokens->push_item(new QweSpaceToken());
    xml_tokens->push_item(new QweTextToken());

    QweXmlLexer *l = new QweXmlLexer(xml_tokens);
    cin >> *l;
    QweTokenList::StlIterator i = l->begin(), end = l->end();
    while (i != end)
    {
        cout << (*i)->get_contents();
        i++;
    }
    return 0;
}
