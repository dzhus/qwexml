#ifndef QWE_XMLPARSE_H
#define QWE_XMLPARSE_H
#include "qwexml.hpp"
#include <string>
#include <iostream>
#include <stdlib.h>    

namespace qwe {

    enum token_type {NONE, TAG, SPACE, TEXT};

    enum error_type {UNKNOWN_TOKEN, TAG_ERROR, UNBALANCED_TAG, UNEXPECTED_CLOSE};

    /**
     * Simple error handler.
     */
    void error(error_type n);
    
    /**
     * Token class.
     *
     * Tokens work with input streams, consuming character data from
     * them. A high level parser must parse character stream of known
     * tokens using the following policy:
     *
     * - choose token to use by calling Token::can_eat() method of each
     *   known token with input stream;
     *
     * - feed input stream to chosen token using Token::feed() or input
     *   operator;
     *
     * - when feeding successfully returns, finished token may be
     *   added to a list of parsed tokens.
     *
     * Token::can_eat() and Token::feed() implementations must
     * guarantee that feed() successfully returns only if no errors
     * occured during reading token.
     *
     * We do NOT perform full readback procedure in case of inapproprate
     * input, so while reading a token is in progress all read errors must
     * be considered fatal. Error handling must be implemented in the
     * Token::feed method.
     */
    class Token {
    protected:
        /**
         * Raw token contents as read from input stream.
         */
        std::string contents;
    
        token_type type;
    public:
        /**
         * Prepare to consume next portion of character data.
         */
        virtual void flush(void);

        std::string get_contents(void);

        virtual token_type get_type(void);
        
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
        virtual bool can_eat(std::istream &in) = 0;
    
        /**
         * Read tag from input stream.
         */
        virtual bool feed(std::istream &in) = 0;

        /**
         * Feeds input stream to token.
         */
        friend std::istream& operator >>(std::istream &in, Token &t);

        virtual Token* _copy(void) = 0;
    };
    
    /**
     * Token class for XML tags.
     */
    class TagToken : public Token {
    private:
        /**
         * Possible states of FA used to read a tag from stream.
         */
        enum state {START, OPEN, SLASH, NAME, CLOSE_NAME, ESPC, EMPTY, END};

        /**
         * Current state of tag-reading FA.
         */
        state current_state;

        /**
         * Tag name.
         */
        std::string name;

        /**
         * True if tag is closing.
         */
        bool closing;

        /**
         * True if tag is empty.
         */
        bool empty;

    public:
        void flush(void);

        TagToken(void);
            
        TagToken(TagToken &t);
        
        TagToken* _copy(void);
    
        std::string get_name(void);

        bool is_closing(void);

        bool is_empty(void);

        /**
         * Reads one tag from stream and sets tag properties.
         */
        bool feed(std::istream &in);
        
        /**
         * Returns true if stream contains a tag.
         */
        bool can_eat(std::istream &in);
    };

    /**
     * Functional class for determining space between XML tags.
     */
    class isxmlspace {
    public:
        bool operator () (char c);
    };

    /**
     * Functional class for determining text node contents.
     */
    class isxmltext {
    public:
        bool operator () (char c);
    };

    /**
     * Template for token classes which infinitely read character data for
     * which F holds.
     *
     * @param F Functional object for testing character data.
     */
    template <class F, token_type T>
    class SimpleToken : public Token {
    public:
        SimpleToken(void)
        {
            type = T;
            flush();
        }
    
        SimpleToken(SimpleToken &t)
        {
            type = T;
            flush();
            contents = t.contents;
        }
    
        SimpleToken* _copy(void)
        {
            return new SimpleToken(*this);
        }

        bool can_eat(std::istream &in)
        {
            return (F()(in.peek()));
        }
    
        /**
         * Read characters while filtering function holds.
         */
        bool feed(std::istream &in)
        {
            char c;
            while ((c = in.get()))
            {
                if (F()(c))
                    contents += c;
                else
                {
                    in.putback(c);
                    return true;
                }
            }
            return false;
        }
    };

    typedef SimpleToken<isxmltext, TEXT> TextToken;
    typedef SimpleToken<isxmlspace, SPACE> SpaceToken;

    typedef List <Token> TokenList;

    class XmlLexer {
    private:
        /**
         * List of complete read tokens.
         */
        TokenList *tokens;

        /**
         * List of known token types.
         *
         * Used as workers to read characters. Flushed after every
         * complete token successfully read.
         */
        TokenList *known;

        /**
         * Token currently being read.
         */
        Token* current;

        /**
         * Choose known token to read next stream data.
         *
         * In case no known token can be read, error() is called.
         * 
         * @return Pointer to appropriate Token.
         */
        Token* choose_token(std::istream &in);
        
        friend class XmlParser;
    public:
        /**
         * Construct new lexer object using a list of tokens.
         */
        XmlLexer(TokenList *l);

        /**
         * Choose next token and completely read it.
         */
        friend std::istream& operator >>(std::istream &in, XmlLexer &l);

        /**
         * Iterator for the list of read tokens.
         */
        TokenList::StlIterator begin(void);
        TokenList::StlIterator end(void);
    };

    class XmlParser
    {
    private:
        XmlLexer *lexer;

        /**
         * Iterators pointing to beginning and end of tokens processed
         * so far.
         */
        TokenList::StlIterator begin, end;

        /**
         * Parsed XML tree.
         */
        ElementNode *root;

        /**
         * True if some tokens have been parsed.
         */
        bool started;

    public:
        XmlParser(void);

        /**
         * Reads XML data from input stream and builds
         * XmlParser::root.
         */
        friend std::istream& operator >>(std::istream &in, XmlParser &p);
        
        /**
         * Iterator for a list of tokens read by underlying lexer.
         */
        TokenList::StlIterator tokens_begin(void);
        TokenList::StlIterator tokens_end(void);
    };
}
#endif
