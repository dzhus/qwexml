#ifndef QWE_XMLPARSE_H
#define QWE_XMLPARSE_H
#include "qwexml.hpp"
#include <iostream>
#include <stdlib.h>    

namespace qwe {

    enum token_type {NONE, TAG, SPACE, TEXT, PI};

    enum error_type {UNKNOWN_TOKEN, TAG_ERROR, PI_ERROR,
                     UNBALANCED_TAG, UNEXPECTED_CLOSE, MULTI_TOP};

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
        String contents;
    
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

        String& get_contents(void);

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
         *
         * Implementations must also add read contents to
         * Token::contents.
         */
        virtual bool feed(std::istream &in) = 0;

        bool is_finished(void);

        virtual Token* copy(void) = 0;
    };

    /**
     * Returns true if character may be used in a tag name.
     */
    bool is_tagname(char c);

    /**
     * Returns true if character may be used in an attribute key name.
     */
    bool is_attkey(char c);

    /**
     * Returns true if character may be used in an attribute value.
     */
    bool is_attval(char c);


    /**
     * Returns true if character may be used in text nodes.
     */
    bool is_xmltext(char c);


    /**
     * Returns true if character may be used inside a processing
     * instruction.
     */
    bool is_picontent(char c);
    
    /**
     * Token class for XML tags.
     */
    class TagToken : public Token {
    private:
        /**
         * Possible states of FA used to read a tag from stream.
         */
        enum state {START, OPEN, CLOSE_SLASH, NAME, CLOSE_NAME, 
                    ESPC, CESPC,
                    KEY, EQUAL, VALUE, END_V,
                    EMPTY, END};

        /**
         * Current state of tag-reading FA.
         */
        state current_state;

         /**
         * True if tag is closing.
         */
        bool closing;

        /**
         * True if tag is empty.
         */
        bool empty;

        /**
         * Empty element node object for this token.
         */
        ElementNode* element;

        /**
         * Add one character to element name.
         */
        void add_to_name(char c);
    
        /**
         * Key of attribute currently being read.
         */
        String current_key;

        /**
         * Value of attribute currently being read.
         */
        String current_value;

    public:
        void flush(void);

        TagToken(void);
            
        TagToken(TagToken &t);
        
        TagToken* copy(void);
    
        ElementNode* get_element(void);

        bool is_closing(void);

        bool is_empty(void);

        /**
         * Reads tag from stream and sets TagToken::element field.
         */
        bool feed(std::istream &in);
        
        /**
         * Returns true if stream contains a tag.
         */
        bool can_eat(std::istream &in);
    };
    
    /**
     * Processing instruction token.
     */
    class PiToken : public Token {
    private:
        /**
         * Possible states of FA used to read a PI from stream.
         */
        enum state {START, OPEN, CONTENTS, CLOSE, END};

        /**
         * Current state of PI-reading FA.
         */
        state current_state;

    public:
        void flush(void);

        PiToken(void);

        PiToken(PiToken &t);

        PiToken* copy(void);

        /**
         * Returns true if stream contains processing instruction.
         */
        bool can_eat(std::istream &in);
        
        /**
         * Reads processing instruction from stream.
         */     
        bool feed(std::istream &in);
    };

    /**
     * Functional class for determining space between XML tags.
     */
    class Fis_xmlspace {
    public:
        bool operator () (char c);
    };

    /**
     * Functional class for determining text node contents.
     */
    class Fis_xmltext {
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
    
        SimpleToken* copy(void)
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

    typedef SimpleToken<Fis_xmltext, TEXT> TextToken;
    typedef SimpleToken<Fis_xmlspace, SPACE> SpaceToken;

    typedef List <Token *> TokenList;

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

        ~XmlLexer(void);

        /**
         * Choose next token and completely read it.
         */
        bool feed(std::istream &in);

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
        List <TagToken *> *stack;

        /**
         * XML element currently being read.
         */
        ElementNode *current_node;
    public:
        XmlParser(void);

        ~XmlParser(void);
        
        /**
         * Reads a portion of XML data from input stream and updates
         * XmlParser::root.
         *
         * New elements are added into the tree as opening tags occur
         * in the input stream.
         *
         * @see XmlParser::is_finished()
         */
        bool feed(std::istream &in);

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

    /**
     * Wrappers for feed methods.
     */
    std::istream& operator >>(std::istream &in, Token &t);
    std::istream& operator >>(std::istream &in, XmlLexer &l);
    std::istream& operator >>(std::istream &in, XmlParser &p);
}
#endif
