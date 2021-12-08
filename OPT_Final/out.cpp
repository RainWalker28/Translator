//
//  out.cpp
//  lab1OPT
//
//  Created by Michael on 21.05.2021.
//  Copyright © 2021 Michael. All rights reserved.
//

#include "out.hpp"
#include "lexer.hpp"
#include <iomanip>


void ShowTabConst(std::deque<Token>& m_Constants){
    if(m_Constants.empty()){
       
        out << "Table of Constants is empty\n";
        return;
    }
    std::deque<Token>::iterator it;
    for (it = m_Constants.begin(); it != m_Constants.end(); ++it){
       
        out << it->name << "---" << it->code << std::endl;
    }
}


void ShowTabIdent(std::deque<Token>& m_UserIdent){
    if(m_UserIdent.empty()){
       
        out << "Table of Identifiers is empty\n";
        return;
    }
    std::deque<Token>::iterator it;
    for (it = m_UserIdent.begin(); it != m_UserIdent.end(); it++){
       
        out << it->name << "---" << it->code << std::endl;
    }
}


void ShowTabError(std::stringstream& m_Errors) {

   out << m_Errors.str()<< std::endl;
}

void ShowTabTokens(std::vector<Token>& m_tokens) {
    if (m_tokens.empty()) {
        
        out << "No tokens\n";
        return;
    }
    std::vector<Token>::const_iterator it;
    for (it = m_tokens.begin(); it != m_tokens.end(); it++) {
        
         out << std::left << std::setw(6) << it->line
              << std::left << std::setw(8) << it->column
              << std::left << std::setw(8) << it->code
              << std::left << std::setw(6) << it->name << '\n';
    }
}

void OutputString(const std::string&str) {
    if (!out.is_open()) {
        return;
    }

    out << str;
}

void CloseFile() {
    out.close();
}




bool outPutIsOpen() {
    return out.is_open();
}


void addError(std::string error) {
    m_Errors << error << std::endl;
}


std::vector<Token> getTokens() { 
    return m_tokens;
}

void outputTree(const terminal_t& tree, const std::string& point )  {
    out << point << tree.name << std::endl;
    //std::cout << point << tree.name << std::endl;
    std::string tmp = point;
    for (const terminal_t& node : tree.subTerminals) {
        outputTree(node, tmp + "..");
    }
}

