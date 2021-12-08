//
//  main.cpp
//  lab1OPT
//
//  Created by Michael on 19.05.2021.
//  Copyright © 2021 Michael. All rights reserved.
//

#include <iostream>
#include "lexer.hpp"
#include "parser.hpp"
#include "generator.hpp"
std::stringstream   m_Errors;
std::stringstream   m_Code;

std::ofstream       out;
//output
std::vector<Token>   m_tokens;


int main(int argc, char** argv) {
   if (argc !=2 ){
        std::cout << "Invalid arguments!\n";
        return 1;
    }
    TokenAnalyzer Analyzer(argv[1]);
    Analyzer.parseFile();
    Analyzer.printLog();

    Parser parser(argv[1], Analyzer.getTokens(), Analyzer.getConstantsTable(), Analyzer.getUserIdentifiersTable());
    parser.doParsing();
    
    Generator generator(argv[1], parser.getTree(), Analyzer.getTokens());
    generator.generate_code();

    parser.printLog();

    return 0;
}
