#include <string>
#include <iostream>
#include <stdlib.h>
#include "qweparse.hpp"

namespace qwe {

    /**
     * Produce error message and exit with error code.
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

    void Token::flush(void)
    {
        contents = "";
    }

    std::string Token::get_contents(void)
    {
        return contents;
    }

    token_type Token::get_type(void)
    {
        return type;
    }

    /**
     * Wrap Token::feed() for use with input operator.
     */
    std::istream& operator >>(std::istream &in, Token &t)
    {
        t.feed(in);
        return in;
    }

    void TagToken::flush(void)
    {
        Token::flush();
        current_state = START;
        name = "";
        closing = false;
        empty = false;
    }

    TagToken::TagToken(void)
    {
        type = TAG;
        flush();
    }
    
    TagToken::TagToken(TagToken &t)
    {
        type = TAG;
        flush();
        contents = t.contents;
        name = t.name;
        closing = t.closing;
        empty = t.empty;
    }

    TagToken* TagToken::_copy(void)
    {
        return new TagToken(*this);
    }
    
    std::string TagToken::get_name(void)
    {
        return name;
    }

    bool TagToken::is_closing(void)
    {
        return closing;
    }

    bool TagToken::is_empty(void)
    {
        return empty;
    }

    bool TagToken::can_eat(std::istream &in)
    {
        return ('<' == in.peek());
    }

    /**
     * Read next opening or closing tag from stream, setting the
     * following fields of TagToken object to appropriate values
     * describing properties of read tag:
     *
     * - TagToken::contents: character data read from stream (like @c
     *   &lt;sometag>);
     *
     * - TagToken::name: tag name (like @c sometag);
     *
     * - TagToken::closing;
     *
     * - TagToken::empty.
     *
     * In case of read errors, error() is called with TAG_ERROR code.
     *
     * Tags are read using finite automata according to the following
     * grammar:
     *
     @verbatim
     STag ::= '<' Name (S Attribute)* S? '>'
     EmptyElemTag ::= '<' Name (S Attribute)* S? '/>'
     ETag ::= '</' Name S? '>'
@endverbatim

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
     *
     * @see TagToken::state
     * @see http://www.w3.org/TR/REC-xml/
     */
    bool TagToken::feed(std::istream &in)
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
     * Return true if character is space between XML nodes.
     */
    bool isxmlspace::operator () (char c)
    {
        return isspace(c);
    }

    /**
     * Return true if character may belong to text node.
     */
    bool isxmltext::operator () (char c)
    {
        return ((isgraph(c) || isspace(c)) &&           \
                !((c == '<') || (c == '&')));
    }

    XmlLexer::XmlLexer(TokenList *l)
        :current(0)
    {
        tokens = new TokenList();
        known = new TokenList(*l);
    }

    /**
     * Call Token::can_eat() for each known lexer token and pick the
     * first one which returns true. Tokens are tried in the same
     * order as in the list which was used to construct lexer.
     */
    Token* XmlLexer::choose_token(std::istream &in)
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

    /**
     * Read one token from input stream and add it to XmlLexer::tokens
     * list.
     */
    std::istream& operator >>(std::istream &in, XmlLexer &l)
    {
        /// Iterators over the list of known tokens
        TokenList::StlIterator i, end;
        end = l.known->end();
        while (in.peek() != -1)
        {
            /// Choose and read one full token
            l.current = l.choose_token(in);
            in >> *(l.current);
            
            /// Store copy of fully read token
            l.tokens->push_item(l.current);

            /// Flush worker token
            l.current->flush();
            l.current = 0;
        }
        return in;
    }

    void XmlLexer::flush(void)
    {
        tokens->clear();        
    }

    TokenList::StlIterator XmlLexer::begin(void)
    {
        return tokens->begin();
    }
    
    TokenList::StlIterator XmlLexer::end(void)
    {
        return tokens->end();
    }

    XmlParser::XmlParser(void)
    {
        /// Setup lexer
        qwe::TokenList *xml_tokens = new qwe::TokenList();
        xml_tokens->push_item(new qwe::TagToken());
        xml_tokens->push_item(new qwe::SpaceToken());
        xml_tokens->push_item(new qwe::TextToken());
        lexer = new XmlLexer(xml_tokens);

        stack = new List<TagToken>;
        current_node = root = new ElementNode("T");
    }

    std::istream& operator >>(std::istream &in, XmlParser &p)
    {            
        TokenList::StlIterator begin, end;

        // Temporary tokens
        Token *current;
        TagToken *current_tag;
        TextToken *current_text;
        SpaceToken *current_space;

        /// Forget tokens read during last feeding and consume new
        /// portion
        p.lexer->flush();
        in >> *(p.lexer);

        begin = p.lexer->begin();
        end = p.lexer->end();

        while (begin != end)
        {
            current = *(begin);
            switch (current->get_type())
            {
            case TAG:
                current_tag = (TagToken *)(current);
                /// Closing tag must occur only if opening tag with
                /// the same name is on the top of XmlParser::stack.
                if (current_tag->is_closing())
                {
                    if (p.stack->is_empty())
                        error(UNEXPECTED_CLOSE);
                    else if ((current_tag->get_name()) ==       \
                             p.stack->last_item()->get_name())
                    {
                        p.stack->pop_item();
                        p.current_node = (ElementNode *)(p.current_node->get_parent());
                    }
                    else
                        error(UNBALANCED_TAG);
                }
                else
                {
                    p.current_node->add_child(new ElementNode(current_tag->get_name()));

                    /// Empty tags are not pushed to stack because
                    /// they don't need to be closed
                    if (!current_tag->is_empty())
                    {
                        p.stack->push_item(current_tag);
                        p.current_node = (ElementNode *)(p.current_node->last_child());
                    }
                }
                break;

            case TEXT:
                current_text = (TextToken *)(current);
                p.current_node->add_child(new TextNode(current_text->get_contents()));
                break;
            }
            begin++;
        }
        return in;
    }

    /**
     * If stack is not empty, parser expects more closing tags to
     * come.
     */
    bool XmlParser::is_finished(void)
    {
        return stack->is_empty();
    }

    NodeList::StlIterator XmlParser::top_begin(void)
    {
        return root->children_begin();
    }
    
    NodeList::StlIterator XmlParser::top_end(void)
    {
        return root->children_end();
    }

    XmlNode* XmlParser::top(void)
    {
        return root->first_child();
    }
}
