#include "common.hpp"
#include "out.hpp"
#include "parser.hpp"

#include <sstream>
#include <utility>
#include <deque>
#include <vector>
#include <iterator>

class Generator {
public:
    Generator(const std::string& path, const terminal_t& tree, const std::vector<Token>& tokens);

    void generate_code();

private:
    std::string                 declarations(terminal_t term);
    std::string                 statement_list(terminal_t term);
    std::string                 statement(terminal_t term);
    std::string                 get_identifier(terminal_t term);
    int                         get_constant(terminal_t term);
    std::pair<std::string, int> get_const_declaration(terminal_t term);
    std::pair<std::string, int> get_statement(terminal_t term);
    std::string                 get_last_word(const std::string str);
    bool                        exists(const std::deque<std::pair<std::string, int>>& array, const std::pair<std::string,int>& value);

    std::string                 add_instruction(terminal_t term);
    terminal_t                  get_variable_terminal(terminal_t);



private:

    std::deque<std::pair<std::string, int>>         m_DeclaredConstants;
    std::deque<std::pair<std::string, int>>         m_Statement;
    std::string                                     m_ProgramName;
    terminal_t                                      m_InputTree;
    std::deque<std::string>                         m_Generated;
    std::string                                     path;  
    
    std::vector<std::string>                         m_CaseLabels;
};