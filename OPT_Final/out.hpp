//
//  out.hpp
//  lab1OPT
//
//  Created by Michael on 21.05.2021.
//  Copyright © 2021 Michael. All rights reserved.
//

#pragma once
#include <iomanip>
#include <fstream>
#include <deque>
#include <iterator>
#include <string>
#include "lexer.hpp"
#include "common.hpp"


void ShowTabIdent(std::deque<Token>& m_UserIdent);
void ShowTabConst(std::deque<Token>& m_Constants);

void ShowTabTokens(std::vector<Token>& m_tokens);
void ShowTabError(std::stringstream& m_Errors);

void OutputString(const std::string&str);
void CloseFile();



bool outPutIsOpen();
void addError(std::string error);
void outputTree(const terminal_t& tree, const std::string& point);
std::vector<Token>   getTokens();

