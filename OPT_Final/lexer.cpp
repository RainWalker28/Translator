//
//  lexer.cpp
//  lab1OPT
//
//  Created by Michael on 19.05.2021.
//  Copyright © 2021 Michael. All rights reserved.
//

#include "lexer.hpp"
#include "out.hpp"

TokenAnalyzer::TokenAnalyzer(std::string path)
    :path(path)
 {
    
    m_InputFile.open(path + "input.sig");
    if (!m_InputFile.is_open()){
        std::cout<< "Error while opening file " << path + "input.sig\n";
        exit(1);
    }
    
    for (size_t i = 0; i < 128; i++){
        if (((i >= 8) && (i <= 13)) || i == 32) {
            m_Attributes[i] = WHITESPACE;
        } else if ((i >= '0') && (i <= '9')) {
            m_Attributes[i] = DIGIT;
        } else if ((i >= 'A') && (i <= 'Z')) {
            m_Attributes[i] = LETTER;
        } else if ((i == ';') || (i == '.') || (i == '=')) {
            m_Attributes[i] = DM1;
        } else if (i == '-') {
            m_Attributes[i] = DM2;
        } else if (i == ':') {
            m_Attributes[i] = DM3;
        } else if (i == '(') {
            m_Attributes[i] = COM;
        } else {
            m_Attributes[i] = ERR;
        }
    }
}


void TokenAnalyzer::parseFile() {
    
     m_CurrentChar = readChar();
     while (!m_InputFile.eof()) {
          switch (m_Attributes[m_CurrentChar]) {
          case WHITESPACE:
               whitespace();
               break;
          case DIGIT:
               digit();
               break;
          case LETTER:
               letter();
               break;
          case DM1:
               dm1();
               break;
          case DM2:
               dm2();
               break;
          case DM3:
               dm3();
               break;
          case COM:
               com();
               break;
          case ERR:
               err();
               break;
          }
    }

}

char TokenAnalyzer::readChar() {
    char tmp = m_InputFile.get();
    //++m_CurrentX;
    return tmp;
}

void TokenAnalyzer::whitespace() {
    /*m_CurrentChar = readChar();
    ++m_CurrentX;*/
     
    while (m_Attributes[m_CurrentChar] == WHITESPACE && !m_InputFile.eof()) {
        if (m_CurrentChar == '\n') {
            m_CurrentToken.line++;
            m_CurrentX = 0;
        }
        m_CurrentChar = readChar();
        ++m_CurrentX;
    }
}

void TokenAnalyzer::digit(bool toClear) {
     if (toClear) {
        m_CurrentToken.clear();
        m_CurrentToken.column = m_CurrentX;
    }
    
    while (isDigit(m_CurrentChar)) {
        m_CurrentToken.name += m_CurrentChar;
        m_CurrentChar = readChar();
        ++m_CurrentX;

    }
    

    if((m_CurrentToken.code = exists(m_Constants, m_CurrentToken)) == 0) {
        //if constant doesn't exist
        if (m_Constants.size() == 0) {
            m_CurrentToken.code = CONST_START_VALUE;
        }
        else {
            m_CurrentToken.code = CONST_START_VALUE + m_Constants.size();
        }
         m_Constants.push_back(m_CurrentToken);
    }
   
    m_tokens.push_back(m_CurrentToken);
}

void TokenAnalyzer::letter() {
    m_CurrentToken.clear();
    m_CurrentToken.column = m_CurrentX;
    while ((isLetter(m_CurrentChar)) || (isDigit(m_CurrentChar))){
        m_CurrentToken.name += m_CurrentChar;
        ++m_CurrentX;
        m_CurrentChar = readChar();
    }

    if((m_CurrentToken.code = exists(m_Reserved, m_CurrentToken)) == 0){
        
    
        if((m_CurrentToken.code = exists(m_UserIdent, m_CurrentToken)) == 0) {
               //if constant doesn't exist
               if (m_UserIdent.size() == 0) {
                   m_CurrentToken.code = USERIDENT_START_VALUE;
               }
               else {
                   m_CurrentToken.code = USERIDENT_START_VALUE + m_UserIdent.size();
               }
               m_UserIdent.push_back(m_CurrentToken);
           }
        
        }
    m_tokens.push_back(m_CurrentToken);
}


void TokenAnalyzer::dm1() {
    m_CurrentToken.clear();
    m_CurrentToken.column = m_CurrentX;
    m_CurrentToken.name = m_CurrentChar;
    m_CurrentToken.code = m_CurrentChar;
    m_tokens.push_back(m_CurrentToken);
    ++m_CurrentX;
    m_CurrentChar = readChar();
}

void TokenAnalyzer::dm2() {
    m_CurrentToken.clear();
    m_CurrentToken.column = m_CurrentX;
    m_CurrentToken.name += m_CurrentChar;
    m_CurrentChar = readChar();
    if (isDigit(m_CurrentChar)) {
        digit(false);
    }
    else {
        ++m_CurrentX;
        m_Errors << "Lexer: Error (line " << m_CurrentToken.line << ", column " << m_CurrentX
        << "): Expected digit but '"
        << m_CurrentChar << "' detected\n";
        return;
    }
}

void TokenAnalyzer::dm3() {
    m_CurrentToken.clear();
    m_CurrentToken.column = m_CurrentX;
    m_CurrentToken.name += m_CurrentChar;
    m_CurrentChar = readChar();
    m_CurrentToken.name += m_CurrentChar;
    if(m_CurrentChar != '='){
        ++m_CurrentX;
        m_Errors << "Lexer: Error (line " << m_CurrentToken.line << ", column " << m_CurrentX
        << "): Expected '=' but '"
        << m_CurrentChar << "' found\n";
        return;
    }
    else {
        
        m_CurrentToken.code = ROZDILN_START_VALUE ;
           
        m_tokens.push_back(m_CurrentToken);
        m_CurrentChar = readChar();
        ++m_CurrentX;

    }
    
}

void TokenAnalyzer::com() {
     size_t col = m_CurrentX;
     size_t row = m_CurrentToken.line;
    m_CurrentToken.column = m_CurrentX;
    m_CurrentChar = readChar();
    bool isCOM = false;
    if(m_CurrentChar != '*'){
        m_Errors << "Lexer: Error (line " << m_CurrentToken.line << ", column " << m_CurrentX
        << "): Illegal symbol '(' detected\n";
        return;
    }
    //begin of commentar
    
    while(!m_InputFile.eof()){
        m_CurrentChar = readChar();
        ++m_CurrentX;
        if (m_CurrentChar == '\n'){
            m_CurrentToken.line++;
            m_CurrentX = 0;
            continue;
        }
        if (m_CurrentChar == '*') {
            isCOM = true;
            continue;
        }
        if(m_CurrentChar == ')' && isCOM) {
            m_CurrentChar = readChar();
            ++m_CurrentX;
            break;
        }
        if(m_InputFile.eof()){
            m_Errors << "Lexer: Error (line " << row << ", column " << col
            << "): unclosed commentar\n";
            break;
        }
        isCOM = false;
    }
}

void TokenAnalyzer::err() {
    std::string err ("Illegal symbol '");
    err += m_CurrentChar;
    err += "' detected";
    m_Errors << "Lexer: Error (line " << m_CurrentToken.line << ", column " << m_CurrentX
               << "): "<< err << std::endl;
    m_CurrentChar = readChar();
    ++m_CurrentX;
}


bool TokenAnalyzer::isWhitespace(char ch) const {
    return m_Attributes[ch] == WHITESPACE;
}

bool TokenAnalyzer::isDigit(char ch) const {
    return m_Attributes[ch] == DIGIT;
}

bool TokenAnalyzer::isLetter(char ch) const {
    return m_Attributes[ch] == LETTER;
}

bool TokenAnalyzer::isDM1(char ch) const {
    return m_Attributes[ch] == DM1;
}

bool TokenAnalyzer::isDM2(char ch) const {
    return m_Attributes[ch] == DM2;
}

bool TokenAnalyzer::isDM3(char ch) const {
    return m_Attributes[ch] == DM3;
}

bool TokenAnalyzer::isCom(char ch) const {
    return m_Attributes[ch] == COM;
}


size_t TokenAnalyzer::exists(const std::deque<Token>& table, const Token& token) const {
    for (const Token& i : table) {
        if (i.name == token.name) {
            return i.code;
        }
    }
    return 0;
}


void TokenAnalyzer::printLog() {
    out.open(path + "generated.txt");
    if (!out.is_open()){
        out.close();
        throw("cannot open output file\n");
    }
    ShowTabTokens(m_tokens);
    OutputString("\nError table:\n");
    ShowTabError(m_Errors);
    OutputString("\nUser Identificator Table: \nName    Code\n");
    ShowTabIdent(m_UserIdent);
    OutputString("\nConstants Table: \nName    Code\n");
    ShowTabConst(m_Constants);
    CloseFile();
}



std::deque<Token>  TokenAnalyzer::getUserIdentifiersTable() const {
    return m_UserIdent;
}

std::deque<Token>  TokenAnalyzer::getConstantsTable() const {
    return m_Constants;
}

std::vector<Token>  TokenAnalyzer::getTokens() const { 
    return ::getTokens();
}