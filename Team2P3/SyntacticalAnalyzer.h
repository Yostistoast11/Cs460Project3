#ifndef SYN_H
#define SYN_H

#include <iostream>
#include <fstream>
#include "LexicalAnalyzer.h"
#include "Object.h"
#include "CodeGenerator.h"

using namespace std;

class SyntacticalAnalyzer 
{
    public:
	SyntacticalAnalyzer (char * filename);
	~SyntacticalAnalyzer ();
	int program();
        int more_defines();
        int define();
        int stmt_list(string op);
        int stmt(string op);
        int literal();
        int more_tokens(string space);
        int quoted_lit();
        int param_list(int count);
        int else_part();
        int stmt_pair();
        int stmt_pair_body(string op);
        int action();
        int any_other_token();
	void ReportError (const string & msg);
    private:
	int errors;
	LexicalAnalyzer * lex;
	CodeGen * gen;
	token_type token;
	ifstream input;
	ofstream listingFile;
	ofstream ruleFile;
	ofstream debugFile;
	string currentLine;

};
	
#endif
