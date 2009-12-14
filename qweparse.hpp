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
    void error(error_type n)
    {
        switch (n)
        {
        case TAG_ERROR:
            std::cout << "Error while reading tag" << std::endl;
            exit(TAG_ERROR);
        case UNKNOWN_TOKEN:
            std::cout << "Could not choose appropriate token" << std::endl;
            exit(UNKNOWN_TOKEN);
        case UNBALANCED_TAG:
            std::cout << "Unbalanced opening and closing tags" << std::endl;
            exit(UNBALANCED_TAG);
        case UNEXPECTED_CLOSE:
            std::cout << "Unexpected closing tag" << std::endl;
            exit(UNEXPECTED_CLOSE);
        }
    }

    /**
     * Token class.
     *
     * Tokens work with input streams, consuming character data from
     * them. A high level parser must parse character stream
     * consisting of known tokens using the following policy:
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
         * Clear contents and prepare to consume next portion of character
         * data.
         */
        virtual void flush(void)
        {
            contents = "";
        }

        std::string get_contents(void)
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
        virtual bool can_eat(std::istream &in) = 0;
    
        /**
         * Try to add more character contents for token.
         */
        virtual bool feed(std::istream &in) = 0;

        friend std::istream& operator >>(std::istream &in, Token &t)
        {
            t.feed(in);
            return in;
        }

        virtual Token* _copy(void) = 0;
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
    class TagToken : public Token {
    private:
        enum state {START, OPEN, SLASH, NAME, CLOSE_NAME, ESPC, EMPTY, END};

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
        void flush(void)
        {
            Token::flush();
            current_state = START;
            name = "";
            closing = false;
            empty = false;
        }

        TagToken(void)
        {
            type = TAG;
            flush();
        }
    
        TagToken(TagToken &t)
        {
            type = TAG;
            flush();
            contents = t.contents;
            name = t.name;
            closing = t.closing;
            empty = t.empty;
        }

        TagToken* _copy(void)
        {
            return new TagToken(*this);
        }
    
        std::string get_name(void)
        {
            return name;
        }

        bool is_closing(void)
        {
            return closing;
        }

        bool is_empty(void)
        {
            return empty;
        }

        /**
         * Read one langle, then possibly one slash, then at least one
         * alpha symbol, then rangle.
         *
         * TagToken::name is set to tag name, TagToken::closing is
         * set to true if closing tag was read.
         */
        bool feed(std::istream &in)
        {
            char c;
            while ((c = in.get()))
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
                case END:
                    in.putback(c);
                    return true;
                }
            
                if (accepted)
                    contents += c;
                else
                    error(TAG_ERROR);
            }
            return false;
        }

        /**
         * Decide to eat tag if stream starts with langle.
         */
        bool can_eat(std::istream &in)
        {
            return ('<' == in.peek());
        }
    };

    class isxmlspace {
    public:
        bool operator () (char c)
        {
            return isspace(c);
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
        Token* choose_token(std::istream &in)
        {
            char c;
            TokenList::StlIterator i, end;
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
            error(UNKNOWN_TOKEN);
            return false;
        }

        friend class XmlParser;
    public:
        XmlLexer(TokenList *l)
            :current(0)
        {
            tokens = new TokenList();
            known = new TokenList(*l);
        }

        /**
         * Try each known token, completely read it.
         */
        friend std::istream& operator >>(std::istream &in, XmlLexer &l)
        {
            /// Iterators over the list of known tokens
            TokenList::StlIterator i, end;
            end = l.known->end();
            while (in.peek() != -1)
            {
                l.current = l.choose_token(in);

                in >> *(l.current);
            
                /// Store copy of fully read token
                l.tokens->push_item(l.current);
                /// Flush worker
                l.current->flush();
                l.current = 0;
            }
            return in;
        }
        
        TokenList::StlIterator begin(void)
        {
            return tokens->begin();
        }

        TokenList::StlIterator end(void)
        {
            return tokens->end();
        }

        TokenList::StlIterator rbegin(void)
        {
            return tokens->rbegin();
        }

        TokenList::StlIterator rend(void)
        {
            return tokens->rend();
        }
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
        XmlParser(void)
        {
            qwe::TokenList *xml_tokens = new qwe::TokenList();
            xml_tokens->push_item(new qwe::TagToken());
            xml_tokens->push_item(new qwe::SpaceToken());
            xml_tokens->push_item(new qwe::TextToken());
            
            lexer = new XmlLexer(xml_tokens);
        }

        friend std::istream& operator >>(std::istream &in, XmlParser &p)
        {            
            TokenList stack;
            ElementNode *current_node;

            // Temporary tokens
            Token *current;
            TagToken *current_tag;
            TextToken *current_text;
            SpaceToken *current_space;

            in >> *(p.lexer);
            
            if (!p.started)
            {
                p.begin = p.lexer->begin();
                p.started = true;
            }

            /// @todo Fix this
            current_node = p.root = new ElementNode("XML");

            p.end = p.lexer->end();

            while (p.begin != p.end)
            {
                current = *(p.begin);
                switch (current->get_type())
                {
                case TAG:
                    current_tag = (TagToken *)(current);
                    if (current_tag->is_closing())
                    {
                        if (stack.is_empty())
                            error(UNEXPECTED_CLOSE);
                        else if ((current_tag->get_name()) ==            \
                              ((TagToken *)(stack.last_item()))->get_name())
                        {
                            // Tag successfully closed
                            stack.pop_item();
                            current_node = (ElementNode *)(current_node->get_parent());
                        }
                        else
                            error(UNBALANCED_TAG);
                    }
                    else
                    {
                        current_node->add_child(new ElementNode(current_tag->get_name()));

                        // Expect inner contents for non-empty tags
                        if (!current_tag->is_empty())
                        {
                            stack.push_item(current_tag);
                            current_node = (ElementNode *)(current_node->last_child());
                        }
                    }
                    break;

                case TEXT:
                    current_text = (TextToken *)(current);
                    current_node->add_child(new TextNode(current_text->get_contents()));
                    break;
                }
                p.begin++;
            }
            std::cout << p.root->get_printable();
            return in;
        }

        TokenList::StlIterator tokens_begin(void)
        {
            return lexer->tokens->begin();
        }

        TokenList::StlIterator tokens_end(void)
        {
            return lexer->tokens->end();
        }
    };
}
#endif
