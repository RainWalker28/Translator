#include "parser.hpp"

#include <cstring>
#include <string>
#include <deque>

terminal_t::terminal_t(const std::string& name, size_t row, size_t column)
    :name(name), row(row), column(column)
    {}

terminal_t& terminal_t::operator=(const terminal_t& other) {
    if (&other == this)
        return *this;
    name = other.name;
    column = other.column;
    row = other.row;
    subTerminals = other.subTerminals;
    return *this;
}

void terminal_t::addSubterm(const terminal_t& newSubterm) {
    subTerminals.push_back(newSubterm);
}

Parser::Parser(const std::string& path, const std::vector<Token>& fileTokens, 
                std::deque<Token> ConstantsTable,
                std::deque<Token> UserIdentifiersTable) 
    :m_FileTokens(fileTokens), m_Constants(ConstantsTable), m_UserIdent(UserIdentifiersTable), 
    m_SyntaxTree("<signal-program>"), path(path)
{
    m_CurrentToken = m_FileTokens.begin();
}

void Parser::doParsing() {
    if (m_CurrentToken == m_FileTokens.end()) {
        return;
    }
   
    signal_program();
    
}

void Parser::printLog() {
    
    out.open(path + "generated.txt");
    if (!out.is_open()) {
        throw ("Parser: Cannot open output file");
        return;
    }
    ShowTabTokens(m_tokens);
    OutputString("\n\n");
    
    outputTree(m_SyntaxTree, "");
    
    OutputString("\nError table:\n");
    ShowTabError(m_Errors);
    OutputString("\nUser Identificator Table: \nName    Code\n");
    ShowTabIdent(m_UserIdent);
    OutputString("\nConstants Table: \nName    Code\n");
    ShowTabConst(m_Constants);
    OutputString("\n\n");
    
    ShowTabError(m_Code);
    CloseFile();

}

terminal_t Parser::getTree() const {
    return m_SyntaxTree;
}

bool Parser::signal_program() {
    m_SyntaxTree.addSubterm(terminal_t("<program>"));
    if (!program(m_SyntaxTree.subTerminals.back())) {
            return false;
    }
    return true;

}

bool Parser::program(terminal_t& terminal) {

    if (m_CurrentToken->name == "PROGRAM") {
        terminal.addSubterm(terminal_t(std::to_string(401) + " PROGRAM", m_CurrentToken->line, m_CurrentToken->column));
        terminal.addSubterm(terminal_t("<procedure-identifier>"));
        
        if (!procedure_identifier(terminal.subTerminals.back())) {
            return false;
        }
        if (!readNextToken()) {
            printEOFError("';'");
            return false;
        }
        if (m_CurrentToken->name == ";") {
            terminal.addSubterm(terminal_t(std::to_string((int)';') + " ;"));
        } else {
            printExpectedError("';'");
            return false;
        }
        terminal.addSubterm(terminal_t("<block>"));
        
        if (!block(terminal.subTerminals.back())) {
            return false;
        }
        if (!readNextToken()) {
            printEOFError("'.'");
            return false;
        }
        if (m_CurrentToken->name == ".") {
            terminal.addSubterm(terminal_t(std::to_string((int)'.') + " ."));
        } else {
            printExpectedError("'.'");
            return false;
        }
    }
    else {
        printExpectedError("'PROGRAM'");
        return false;
    }
    return true;
}

bool Parser::block(terminal_t& terminal) {
    terminal.addSubterm(terminal_t("<declarations>"));
    
    if (!declarations(terminal.subTerminals.back())) {
        return false;
    }

    if (m_CurrentToken->name == "BEGIN") {
        terminal.addSubterm(terminal_t(std::to_string(402) + " BEGIN", m_CurrentToken->line, m_CurrentToken->column));
    } else {
        printExpectedError("'BEGIN'");
        return false;
    }

    terminal.addSubterm(terminal_t("<statement_list>"));
    
    if (!statement_list(terminal.subTerminals.back())) {
        return false;
    }
    if (m_CurrentToken->name == "END") {
        terminal.addSubterm(terminal_t(std::to_string(403) + " END", m_CurrentToken->line, m_CurrentToken->column));
    } else {
        printExpectedError("'END'");
        return false;
    }
    return true;
}

bool Parser::declarations(terminal_t& terminal) {
    terminal.addSubterm(terminal_t("<constant-declarations>"));
    
    if (!const_declarations(terminal.subTerminals.back())) {
        return false;
    }
    return true;
}

bool Parser::const_declarations(terminal_t& terminal) {
    
    if (!readNextToken()) {
        printEOFError("'CONST'");
        return false;
    }
    if (m_CurrentToken->name == "CONST") {
        terminal.addSubterm(terminal_t(std::to_string(404) + " CONST", m_CurrentToken->line, m_CurrentToken->column));
        terminal.addSubterm(terminal_t("<constant-declarations-list>"));
        
        if (!const_declarations_list(terminal.subTerminals.back())) {
            return false;
        }
    } else {
        terminal.addSubterm(terminal_t("<empty>"));
    }
    return true;
}

bool Parser::const_declarations_list(terminal_t& terminal) {
    if (!readNextToken()) {
        printEOFError("<identifier>");
        return false;
    }
    if (isUserIdentifier(m_CurrentToken->name) != 0) {
        m_CurrentToken--;
        terminal.addSubterm(terminal_t("<constant-declaration>"));
        
        if (!const_declaration(terminal.subTerminals.back())) {
            return false;
        }
        terminal.addSubterm(terminal_t("<constant-declarations-list>"));
        
        if (!const_declarations_list(terminal.subTerminals.back())) {
            return false;
        }
    } else {
        terminal.addSubterm(terminal_t("<empty>"));
    }
    return true;
}

bool Parser::const_declaration(terminal_t& terminal) {
    terminal.addSubterm(terminal_t("<constant-identifier>"));
    
    if (!constant_identifier(terminal.subTerminals.back())) {
        //printExpectedError("'<identifier>'");
        return false;
    }
    if (!readNextToken()) {
        printEOFError("'='");
        return false;
    }
    if (m_CurrentToken->name == "=") {
        terminal.addSubterm(terminal_t(std::to_string((int)'=') + " ="));
    } else {
        printExpectedError("'='");
        return false;
    }
    terminal.addSubterm(terminal_t("<constant>"));
    
    if (!constant(terminal.subTerminals.back())) {
        return false;
    }
    if (!readNextToken()) {
        printEOFError("';'");
        return false;
    }
    if (m_CurrentToken->name == ";") {
        terminal.addSubterm(terminal_t(std::to_string((int)';') + " ;"));
    } else {
        printExpectedError("';'");
        return false;
    }
    return true;
}

bool Parser::statement_list(terminal_t& terminal) {
    if (!readNextToken()) {
        terminal.addSubterm(terminal_t("<empty>"));
        printEOFError("END");
        return false;
    }
    if (isUserIdentifier(m_CurrentToken->name) != 0 ) {
        terminal.addSubterm(terminal_t("<statement>"));
        m_CurrentToken--;
        
        if (!statement(terminal.subTerminals.back())) {
            return false;
        }
        terminal.addSubterm(terminal_t("<statement_list>"));
        
        if (!statement_list(terminal.subTerminals.back())) {
            return false;
        }

    } else {
        terminal.addSubterm(terminal_t("<empty>"));
    }
    return true;
}

bool Parser::statement(terminal_t& terminal) {
    terminal.addSubterm(terminal_t("<variable-identifier>"));
    
    if (!variable_identifier(terminal.subTerminals.back())) {
        return false;
    }
    if (!readNextToken()) {
        printEOFError("':='");
        return false;
    }
    terminal.addSubterm(terminal_t(std::to_string(301) + " :="));
    if (m_CurrentToken->name != ":=") {
        printExpectedError("':='");
        return false;
    }

    
    terminal.addSubterm(terminal_t("<constant>"));
    
    if (!constant(terminal.subTerminals.back())) {
        return false;
    }
    if (!readNextToken()) {
        printEOFError("';'");
        return false;
    }
    if (m_CurrentToken->name == ";") {
        terminal.addSubterm(terminal_t(std::to_string((int)';') + " ;"));
    } else {
        printExpectedError("';'");
        return false;
    }
    return true;

}



bool Parser::constant(terminal_t& terminal) {
    terminal.addSubterm(terminal_t("<unsigned-integer>"));
    
    if (!unsigned_integer(terminal.subTerminals.back())) {
        return false;
    }
    return true;
}

bool Parser::variable_identifier(terminal_t& terminal) {
    terminal.addSubterm(terminal_t("<identifier>"));
    
    if (!identifier(terminal.subTerminals.back())) {
        return false;
    }
    return true;
}

bool Parser::constant_identifier(terminal_t& terminal) {
    terminal.addSubterm(terminal_t("<identifier>"));
    
    if (!identifier(terminal.subTerminals.back())) {
        return false;
    }
    return true;
}

bool Parser::procedure_identifier(terminal_t& terminal) {
    terminal.addSubterm(terminal_t("<identifier>"));
    
    if (!identifier(terminal.subTerminals.back())) {
        return false;
    }
    return true;
    
}

bool Parser::identifier(terminal_t& terminal) {
    if (!readNextToken()) {
        printEOFError("<identifier>");
        return false;
    }
    size_t id = isUserIdentifier(m_CurrentToken->name);
    if (id) {
        terminal.addSubterm(terminal_t(std::to_string(id) + " " + m_CurrentToken->name, m_CurrentToken->line, m_CurrentToken->column));
        return true;
    }
    else {
         printExpectedError("<identifier>");
         return false;
    }
    return true;
}

bool Parser::unsigned_integer(terminal_t& terminal) {
    if (!readNextToken()) {
        printEOFError("<unsigned-integer>");
        return false;
    }
    size_t id = isUnsignedInteger(m_CurrentToken->name);
    if (id) {
        terminal.addSubterm(terminal_t(std::to_string(id) + " " + m_CurrentToken->name, m_CurrentToken->line, m_CurrentToken->column));
        return true;
    }
    else {
         printExpectedError("<unsigned-integer>");
         return false;
    }
    return true;
}

bool Parser::empty(terminal_t& terminal) {
    return true;

}

size_t Parser::isUserIdentifier(const std::string& word) const {
    for (auto& i : m_UserIdent) {
        if (i.name == word) {
            return i.code;
        }
    }
    return 0;
}

size_t Parser::isUnsignedInteger(const std::string& value) const {
        for (auto& i : m_Constants) {
        if (i.name == value) {
            return i.code;
        }
    }
    return 0;
}

bool Parser::readNextToken() {
    m_CurrentToken++;
    return m_CurrentToken != m_FileTokens.end();
}

void Parser::printEOFError(const std::string& expected) {
    
    auto tmp = m_CurrentToken - 1;
    m_Errors << "Parser: Error (line" << tmp->line << ", column "  << tmp->column + tmp->name.length() <<  "): Expected " << expected << " but 'EOF' found\n";
        
}

void Parser::printExpectedError(const std::string& expected) {
    
    m_Errors << "Parser: Error (line" << m_CurrentToken->line << ", column " << m_CurrentToken->column <<  "): Expected " << expected  <<  "' but '" << m_CurrentToken->name << "' found\n";
    
            
}