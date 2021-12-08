#include "generator.hpp"

#include <string>

Generator::Generator(const std::string& path, const terminal_t& tree, const std::vector<Token>& tokens) 
    :m_InputTree(tree), path(path)
{
}

void Generator::generate_code() {
    if (m_InputTree.name.empty()) {
        return;
    }
    // set tree's head to <program>
    terminal_t head = m_InputTree.subTerminals.front();
    m_Generated.push_back("\nCODE SEGMENT");
    m_Generated.push_back("ASSUME CS:CODE, DS:DATA");
    m_Generated.push_back(get_identifier(head.subTerminals[1]) + ":");
    m_ProgramName = get_identifier(head.subTerminals[1]);
    // set tree's head to <block>
    head = head.subTerminals[3];

    m_Generated.push_front(declarations(head.subTerminals.front()));

    m_Generated.push_back("push ebp"); 
    m_Generated.push_back("mov ebp, esp");

    // set head to 1st <statement-list>
    head = head.subTerminals[2];    

    m_Generated.push_back(statement_list(head));

    m_Generated.push_back("pop ebp");
    m_Generated.push_back("ret");
    m_Generated.push_back("CODE ENDS");

    

    OutputString("\n");
    for (const std::string& str : m_Generated) {
        OutputString(str + "\n");
        m_Code << str << "\n" ;
    }
 
}



std::string Generator::declarations(terminal_t term) {

    std::string result = "DATA SEGMENT\n";

    // set to <constant-declarations-list>
    term = term.subTerminals.front().subTerminals.back();

    while (term.subTerminals.back().name != "<empty>") {
        auto newDeclaration = get_const_declaration(term.subTerminals.front());
        auto tmp = get_variable_terminal(term.subTerminals.front());
        if (!exists(m_DeclaredConstants, newDeclaration)) {
            if (newDeclaration.first != m_ProgramName)
            {
                m_DeclaredConstants.push_back(newDeclaration);
            }
            else {
                m_Errors << "Code generator: Error (line" << tmp.row << ", column " << tmp.column <<  "): " << std::string(newDeclaration.first + " already exists") << "\n";
            }
        } else {
            m_Errors << "Code generator: Error (line" << tmp.row << ", column " << tmp.column <<  "): " << std::string(newDeclaration.first + " already exists") << "\n";
        }
        // set to next <constant-declarations-list>
        term = term.subTerminals[1];
    }
 
    for (const std::pair<std::string, int>& constant : m_DeclaredConstants) {
        result += constant.first + " DWORD " + std::to_string(constant.second) + '\n';
    }

    result += "DATA ENDS";
    return result;
}

std::string Generator::statement_list(terminal_t term) {
    std::string result;
    if (term.subTerminals.front().name == "<empty>") {
        result += "nop\n";
        return result;
    }
    result += statement(term.subTerminals.front());
    result += statement_list(term.subTerminals.back());
    return result;
}

std::string Generator::statement(terminal_t term) {
    std::string result;

    
    auto newStatement = get_const_declaration(term);
    auto tmp = get_variable_terminal(term);

    if (newStatement.first == m_ProgramName) {
        m_Errors << "Code generator: Error (line" << tmp.row <<", column "  << tmp.column << "): " << std::string(newStatement.first + " is Program Name") << "\n";
        return result;
    }
    if ((exists(m_DeclaredConstants, newStatement))) {
        m_Errors << "Code generator: Error (line"  << tmp.row << ", column " << tmp.column << "): " << std::string(newStatement.first + " is constant") << "\n";
        return result;
    }
    result = "mov " + newStatement.first + "," + std::to_string(newStatement.second) + '\n';
    
    return result;
}


std::string Generator::get_identifier(terminal_t term) {
    std::string identifier;
    term = term.subTerminals.front();
    if (term.name == "<identifier>") {
        term = term.subTerminals.front();
    }
    return get_last_word(term.name);
}

int Generator::get_constant(terminal_t term) {
    return std::atoi(get_last_word(term.subTerminals.front().subTerminals.front().name).c_str());
}

std::pair<std::string, int> Generator::get_const_declaration(terminal_t term) {
    std::string identifier = get_identifier(term.subTerminals.front());
    int value = get_constant(term.subTerminals[2]);
    return std::make_pair(identifier, value);
}
//new
std::pair<std::string, int> Generator::get_statement(terminal_t term) {
    std::string identifier = get_identifier(term.subTerminals.front());
    int value = get_constant(term.subTerminals[2]);
    return std::make_pair(identifier, value);
}

std::string Generator::get_last_word( std::string str) {
    while( !str.empty() && std::isspace( str.back() ) ) str.pop_back() ;
    const auto pos = str.find_last_of( " \t\n" ) ;
    return pos == std::string::npos ? str : str.substr(pos+1) ;
}

bool Generator::exists(const std::deque<std::pair<std::string, int>>& array, const std::pair<std::string,int>& value) {
    for (std::deque<std::pair<std::string, int>>::const_iterator it = array.begin(); it != array.end(); ++it) {
        if (it->first == value.first) {
            return true;
        }
    }
    return false;
}

std::string Generator::add_instruction(terminal_t term) {
    return get_last_word(term.subTerminals.front().name);
}

terminal_t Generator::get_variable_terminal(terminal_t term) {
    return term.subTerminals.front().subTerminals.front().subTerminals.front();
}
