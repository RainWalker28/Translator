//
//  lexer.hpp
//  lab1OPT
//
//  Created by Michael on 19.05.2021.
//  Copyright © 2021 Michael. All rights reserved.
//


#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <deque>
#include <array>
#include <sstream>
#include <vector>
#include <iomanip>

#include "common.hpp"



#define CONST_START_VALUE       501
#define USERIDENT_START_VALUE   1001
#define ROZDILN_START_VALUE     301

#define WHITESPACE  0
#define DIGIT       1
#define LETTER      2
#define DM1         3 // ; . =
#define DM2         4 // -
#define DM3         5 // :
#define COM         6 // (
#define ERR         7 // rest symbols


class TokenAnalyzer {
public:
    TokenAnalyzer(std::string path);

    void parseFile();
    void printLog();
    std::vector<Token>   getTokens() const;
    std::deque<Token>   getConstantsTable() const;
    std::deque<Token>   getUserIdentifiersTable() const;
private:
    char readChar();
    
    void whitespace();
    void digit(bool toClear = true);
    void letter();
    void dm1();
    void dm2();
    void dm3();
    void com();
    void err();
    
    bool isWhitespace(char ch) const;
    bool isDigit(char ch) const;
    bool isLetter(char ch) const;
    bool isDM1(char ch) const;
    bool isDM2(char ch) const;
    bool isDM3(char ch) const;
    bool isCom(char ch) const;
    
    size_t exists(const std::deque<Token>& table, const Token& token) const;
    
private:
    std::array<unsigned char, 128> m_Attributes;
    std::string path;
    //array of read tokens
    std::deque<Token> m_Tokens;
    
    //array of 'rozdilnik(:=)'
    std::deque<Token>   m_Rozdilnik;
    //array of reserved keywords
const std::deque<Token> m_Reserved = {
    Token{401, 0, 0, "PROGRAM"},
    Token{402, 0, 0, "BEGIN"},
    Token{403, 0, 0, "END"},
    Token{404, 0, 0, "CONST"}
};





//array of user identifiers
    std::deque<Token>   m_UserIdent;
//array of constants
    std::deque<Token>   m_Constants;




    
    std::ifstream       m_InputFile;

    Token               m_CurrentToken;
    size_t              m_CurrentX = 1;
    char                m_CurrentChar;
    
};
