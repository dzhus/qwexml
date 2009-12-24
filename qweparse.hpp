#ifndef QWE_XMLPARSE_H
#define QWE_XMLPARSE_H
#include "qwexml.hpp"
#include <string>
#include <iostream>
#include <stdlib.h>    

namespace qwe {

    enum token_type {NONE, TAG, SPACE, TEXT};

    enum error_type {UNKNOWN_TOKEN, TAG_ERROR, UNBALANCED_TAG, UNEXPECTED_CLOSE, MULTI_TOP};

    /**
     * Simple error handler.
     */
    void error(error_type n);
    
    /**
     * Token class.
     *
     * Tokens work with input streams, consuming character data from
     * them. A high level lexer must break character stream of known
     * tokens into lexems using the following policy:
     *
     * - choose token to use by calling Token::can_eat() method of each
     *   known token with input stream;
     *
     * - feed input stream to chosen token using Token::feed() or input
     *   operator;
     *
     * - when feeding successfully returns, if Token::is_finished()
     *   the token may be added to a list of read tokens. In other
     *   case, lexer must expect more content of current token to
     *   come.
     *
     * Token::can_eat() and Token::feed() implementations must
     * guarantee that feed() successfully returns only if no errors
     * occured during reading token.
     *
     * We do NOT perform full readback procedure in case of inapproprate
     * input, so while reading a token is in progress all read errors must
     * be considered fatal. Error handling must be implemented in the
     * Token::feed method.
     *
     * @see XmlLexer
     */
    class Token {
    protected:
        /**
         * Raw token contents as read from input stream.
         */
        std::string contents;
    
        token_type type;

        /**
         * True if token was completely read.
         */
        bool finished;
    public:
        /**
         * Prepares token to consume next portion of character data.
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
         *
         * Must set Token::finished to true if read was complete.
         * Token must properly preserve its inner state in case of EOF
         * occuring while reading is in progress.
         */
        virtual bool feed(std::istream &in) = 0;

        /**
         * Feeds input stream to token.
         */
        friend std::istream& operator >>(std::istream &in, Token &t);

        bool is_finished(void);

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
                    /// @internal When EOF occurs, SimpleToken is
                    /// ended. Thus text and space nodes are read in
                    /// portions.
                    in.putback(c);
                    finished = true;
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
         * Constructs new lexer object using a list of tokens.
         */
        XmlLexer(TokenList *l);

        /**
         * Choose next token and completely read it.
         */
        friend std::istream& operator >>(std::istream &in, XmlLexer &l);

        /**
         * Clears list of read tokens.
         */
        void flush(void);

        /**
         * Iterator for the list of read tokens.
         */
        TokenList::StlIterator begin(void);
        TokenList::StlIterator end(void);
    };

   /**
    * XML parser class.
    *
    * Utilizes XmlLexer to parse input stream into a list of tokens
    * which are translated into a tree of ElementNode objects.
    *
    * Portions of XML data are fed to parser using input operator.
    * XmlParser::is_finished() method is used to check if parsing is
    * complete.
    */
    class XmlParser
    {
    private:
        XmlLexer *lexer;

        /**
         * Root element contains just top-level element of the XML
         * document.
         *
         * @see XmlParser::top()
         */
        ElementNode *root;

        /**
         * Stack of open tag elements.
         *
         * @todo Balancing is checked using tag names, so a list of
         * strings will be sufficient here.
         */
        List<TagToken> *stack;

        /**
         * XML element currently being read.
         */
        ElementNode *current_node;
    public:
        XmlParser(void);
        
        /**
         * Reads a portion of XML data from input stream and updates
         * XmlParser::root.
         *
         * New elements are added into the tree as opening tags occur
         * in the input stream.
         *
         * @see XmlParser::is_finished()
         */
        friend std::istream& operator >>(std::istream &in, XmlParser &p);
        
        /**
         * Checks if parsing is complete.
         *
         * @return True if current top-level element has been
         * completely read from opening to closing tag
         */
        bool is_finished(void);

        /**
         * First top-level element.
         */
        XmlNode* top(void);
    };
}
#endif
