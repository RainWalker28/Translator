//
//  common.h
//  lab1OPT
//
//  Created by Michael on 22.05.2021.
//  Copyright © 2021 Michael. All rights reserved.
//

#pragma once
#include <fstream>
#include <deque>
#include <sstream>
#include <vector>
#include <cstring>

struct Token {
    int code;
    int line = 1;
    int column = 1;
    
    std::string name;
    
    void clear() {
        name.clear();
    }
};


struct terminal_t{
    terminal_t() = default;
    terminal_t(const std::string& name) : name(name) {}
    terminal_t(const std::string& name, size_t row, size_t col);

    terminal_t& operator=(const terminal_t& other);

    void addSubterm(const terminal_t& );
    std::string name;
    size_t row = 0, column = 0;
    std::vector<terminal_t> subTerminals;

    
};


    extern std::stringstream   m_Errors;
    extern std::stringstream   m_Code;

    extern std::ofstream       out;
    //output
    extern std::vector<Token>   m_tokens;
