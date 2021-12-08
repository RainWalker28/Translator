#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <map>
#include <string>
#include <deque>

#include "common.hpp"
#include "out.hpp"


class Parser{
public:
    Parser(const std::string& path, const std::vector<Token>& fileTokens, std::deque<Token> ConstantsTable,
                 std::deque<Token> UserIdentifiersTable) ;
    void doParsing();
    void printLog();

    terminal_t getTree() const;

private:

    // functions to handle non-terminals
    bool signal_program();
    bool program(terminal_t&);
    bool block(terminal_t&);
    bool declarations(terminal_t&);
    bool const_declarations_list(terminal_t&);
    bool const_declarations(terminal_t&);
    bool const_declaration(terminal_t&);
    bool statement_list(terminal_t&);
    bool statement(terminal_t&);
    bool constant(terminal_t&);
    bool variable_identifier(terminal_t&);;
    bool constant_identifier(terminal_t&);
    bool procedure_identifier(terminal_t&);
    bool identifier(terminal_t&);
    bool unsigned_integer(terminal_t&);
    bool empty(terminal_t&);

    size_t isUserIdentifier(const std::string& word) const;
    size_t isUnsignedInteger(const std::string& word) const;
    size_t exists(const std::deque<Token>& table, const Token& token) const;
        //true is reading successful 
    bool readNextToken();
    void printEOFError(const std::string& expected);
    void printExpectedError(const std::string& expected);

private:

    std::vector<Token>            m_FileTokens;
    std::vector<Token>::iterator               m_CurrentToken;
    std::deque<Token>   m_Constants;
    std::deque<Token>   m_UserIdent;
    terminal_t                      m_SyntaxTree;

    //bool                            m_ErrorFound;
    bool                            m_IsEmpty;
    
    std::string                     path;                          
};

#endif // PARSER_H