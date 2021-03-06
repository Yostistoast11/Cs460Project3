//File: SyntacticalAnalyzer.cpp
//Authors:  Yomaskeen
//Date: 11/25/2018
//Discription: This File is the .cpp for the syntatical analyzer class and will work with the lexical analyzer
//implimented in Project1 to check for syntactical correctness of a scheme style program. This will generate
//an out put file of the name of the input file stripped of .ss and added .p2. In this file there will be printed
//a summary of how this program read through the input file and aslo any syntactical errors within the code.
#include <iostream>
#include <iomanip>
#include <fstream>
#include "SyntacticalAnalyzer.h"
#include "CodeGenerator.h"

using namespace std;
int depth = 0;
bool from_else = false;
bool is_main = false;
bool from_action = false;
bool is_param = false;
int tabs =0;

SyntacticalAnalyzer::SyntacticalAnalyzer (char * filename)
{
	lex = new LexicalAnalyzer (filename);
	string filename2 = "Test.cpp";
	gen = new CodeGen (filename);
	token_type t;
	
	int i =0;
	string file = "";
	while( filename[i] != '.'){
		file += filename[i];
		i++;
	}
	token = lex -> GetToken();
	
	ruleFile.open(file + ".p2");

	errors = program();
}

SyntacticalAnalyzer::~SyntacticalAnalyzer ()
{// this is the deconstructor for the syntactical analyzer class
  cout << errors << " Syntactical errors found in input file\n";
  lex->listing << errors << " Syntactical errors found in input file\n";
  lex->debug << errors << " Syntactical errors found in input file\n";
  ruleFile.close();
  delete lex;
  delete gen;
}

int SyntacticalAnalyzer::program(){
  //This is the program function and is also rule one of our language this will check for
  //correct syntax and send the program off to the next rule using recursion.

	ruleFile  << "Entering Program function; current token is: " << lex ->GetTokenName (token) << ", lexeme: " << lex->GetLexeme() <<  endl; 
	int errors = 0;
	ruleFile << "Using Rule 1" << endl;

	if(token != LPAREN_T){
		errors++;
		ReportError(string("expected a LPAREN_T, currently in program, but found  " + lex->GetTokenName(token)));
	}
	token = lex->GetToken();

	errors += define();

	if(token != LPAREN_T){
		errors++;
		ReportError(string("expected a LPAREN_T in program, but found  " + lex->GetTokenName(token)));
	}

	token = lex->GetToken();
	
	errors += more_defines();

	if (token != EOF_T) {
	  errors ++;		
	  ReportError(string("expected a EOF_T in program, but found  " + lex->GetTokenName(token)));
	} // end of if EOF
	ruleFile << "Exiting Program function; current token is: " << lex ->GetTokenName (token) << endl;

	return errors;

}

int SyntacticalAnalyzer::more_defines(){
  //This function more-defines deals with the more_defines rules( 2-3) and will check for syntactical correctness,
  //this function will, depending on the token seen, send the program too define, stmt, or stmt-list for
  //further analyzing.
	ruleFile << "Entering More_Defines function; current token is: " << lex ->GetTokenName(token) << ", lexeme: " << lex->GetLexeme() << endl;

	int errors = 0;
	if(token == IDENT_T){
		ruleFile << "Using Rule 3" << endl;
		//gen->WriteCode(tabs, "Object  " + lex->GetLexeme());
		if (token == EOF_T){
		  errors++;
		  ReportError(string("EOF_T token detected ending function stmt_list"));
		  ruleFile << "Exiting Stmt_List function; current token is: " << lex ->GetTokenName (token) << endl;
		  return errors;
		  
		}
		errors += stmt_list("");
		if(token != RPAREN_T){
		  errors++;
		  ReportError(string("expected a RPAREN_T in m_d, but found  " + lex->GetTokenName(token)));
		}
		token = lex->GetToken();
		ruleFile << "Exiting More_Defines function; current token is: " << lex ->GetTokenName (token) << endl; 
		
		return errors; 
	} 

	ruleFile << "Using Rule 2" << endl;
	if (token != EOF_T)
	  errors += define();
	
	
	if(token != LPAREN_T){
	  errors++;
	  ReportError(string("expected a LPAREN_T in m_d, but found  " + lex->GetTokenName(token)));
	}
	token = lex->GetToken();
	
	if (token != EOF_T)
	  errors+= more_defines();
	ruleFile << "Exiting More_Defines function; current token is: " << lex ->GetTokenName (token) << endl;

	return errors;

}			

int SyntacticalAnalyzer::define(){
  //This function deals with the defines rules(4) and will check for syntactical correctness,
  //this function will, depending on the token seen, send the program to param_list after
  //checking for the presence of some needed tokens for this rule.
  ruleFile << "Entering Define function; current token is: " << lex ->GetTokenName(token) << ", lexeme: " << lex->GetLexeme() << endl;
  int errors = 0;

	ruleFile << "Using  Rule 4" << endl;

	if(token != DEFINE_T){
		errors++;
		ReportError(string("expected a DEFINE_T in define, but found  " + lex->GetTokenName(token)));
	}

	token = lex->GetToken();
	if(token != LPAREN_T){
		errors++;
		ReportError(string("expected a LPAREN_T in define, but found  " + lex->GetTokenName(token)));
	}
	
	token = lex->GetToken();
	if(token != IDENT_T){
		errors++;
		ReportError(string("expected a IDENT_T in define, but found  " + lex->GetTokenName(token)));
	}

	//Caveman's first attempt at coding project3
	if ( lex->GetLexeme() == "main" ){
	  gen->WriteCode (tabs, ("int " + lex->GetLexeme() + "("));
	  is_main = true;
	}
	else
	  gen->WriteCode (tabs, ("Object " + lex->GetLexeme() + "(")); 

	token = lex->GetToken();
	errors += param_list(1);
	if(token != RPAREN_T){
		errors++;
		ReportError(string("expected a RPAREN_T in define, but found  " + lex->GetTokenName(token)));
	}
	
	gen->WriteCode(tabs, "){\n");
	tabs++;
	token = lex->GetToken();
	errors += stmt("");
	errors += stmt_list("");


	if(token != RPAREN_T){
	  errors++;
	  ReportError(string("expected a RPAREN_T in define, but found  " + lex->GetTokenName(token)));
		token = lex->GetToken();
	}
	if(is_main)
	  gen->WriteCode(tabs, "return 0;\n");
	if(!(is_main))
          gen->WriteCode(tabs, "return Object(0);\n");
	//Caveman's second attempt
	tabs--;
	gen->WriteCode(tabs, "}\n"); 
	token = lex->GetToken();
	is_main = false;
	ruleFile << "Exiting Define function; current token is: " << lex ->GetTokenName (token) << endl;


	return errors;

}

int SyntacticalAnalyzer::stmt_list(string op){
  //This function deals with the stmt_list rules(5-6) and will check for syntactical correctness,
  //this function will then, depending on the token seen, send the program to stmt and stmt list
  // or will return after seeing nothing in the file.
  
  ruleFile << "Entering Stmt_List function; current token is: " << lex ->GetTokenName(token) << ", lexeme: " << lex->GetLexeme() << endl;

	int errors = 0;
	if(token == EOF_T){
		errors++;
		ReportError(string("EOF_T token detected ending function stmt_list"));
		ruleFile << "Exiting Stmt_List function; current token is: " << lex ->GetTokenName (token) << endl;
		return errors;

	}
	
	if(token == RPAREN_T ) {
	  ruleFile << "Using Rule 6" << endl;
	  ruleFile << "Exiting Stmt_List function; current token is: " << lex ->GetTokenName (token) << endl;
	  return errors;
	} 

	
	
	errors += stmt("");
	
        if(token == RPAREN_T )
          op = "";

	if (op != "" ){
	  gen->WriteCode(0, " " + op + " ");
	}
	
	
	errors += stmt_list(op);
	
	ruleFile << "Exiting Stmt_List function; current token is: " << lex ->GetTokenName (token) << endl;

	return errors;

}

int SyntacticalAnalyzer::stmt(string op){
  //This function deals with the stmt rules (7-9) and will check for syntactical correctness,
  //this function will then, depending on the token seen, send the program to literal, action, or will return. 
	ruleFile << "Entering Stmt function; current token is: " << lex ->GetTokenName(token) << ", lexeme: " << lex->GetLexeme() << endl;

	int errors = 0;
	/*
	while (token != IDENT_T && token && LPAREN_T && token != NUMLIT_T && token != STRLIT_T && token != SQUOTE_T && token != EOF_T){
	  ReportError(string("LPAREN_T, NUMLIT_T, STRLIT_T, SQUOTE_T, IDENT,_T expected in stmt function, current token is: " + lex->GetTokenName(token)));
	  token=lex->GetToken();
	  errors++;
	}
	*/

	if (op != "" && op!= "-" && op!="%" && op!= "/"){	  
	  gen->WriteCode(0, op);
	  op = "";
	}

	switch(token){
	case IDENT_T:

	  if (lex->GetLexeme() != "main" ){
	    if (depth ==0)
	      if(!is_main){
		gen->WriteCode(tabs, "return ");
		gen->WriteCode(0, lex->GetLexeme());
		gen->WriteCode(0, ";\n");
	      }
	      else
		gen->WriteCode(tabs, lex->GetLexeme());
	    else
	      gen->WriteCode(0, lex->GetLexeme());
	    
	  }
	  
	  if ( op == "-" || op == "%" || op == "/")
	    gen->WriteCode(0, " " + op +" ");
	  ruleFile << "Using Rule 8" << endl;
	  token = lex->GetToken();
	  break;
	  
	  
	case LPAREN_T:
	  ruleFile << "Using Rule 9" << endl;
	  //ruleFile << "stmt function complete. Current token is: " << lex->GetTokenName(token) << endl;
	  token = lex->GetToken();
		    
	  if(depth == 0 && lex->GetLexeme() != "display" && lex->GetLexeme() != "newline" && lex->GetLexeme() != "if"   && lex->GetLexeme() != "cond" && !is_main) {
            gen->WriteCode(tabs, "return ");
	  }
	  
	  depth++;
	  errors += action();
	  depth--;
	  
	  //returned token should be a RPAREN?
	  if(token == RPAREN_T){
	    token=lex->GetToken();
	    
	    if (op != ""){
	      if(depth == 0)
		gen->WriteCode(tabs, op);
	      else
		gen->WriteCode(0, op);
	      op = "";
	    }
	    
	    if(depth == 0){
	      if (!from_else)
		gen->WriteCode(0, ";\n");
	      else
		from_else = false;
	    }
	    if(depth < 0)
	      depth =0;
	    
	  }
	  else{
	    ReportError(string("RPAREN_T expected in stmt function, current token is: " + lex->GetTokenName(token)));
	    token=lex->GetToken();
	  }
	  break;
	  
	  
	case NUMLIT_T: 
	  //ruleFile << "Stmt function complete. Current token is: " << lex->GetTokenName(token) << endl;
	  ruleFile << "Using Rule 7"  << endl;
	  if(depth == 0 && lex->GetLexeme() != "display" && lex->GetLexeme() != "newline" && lex->GetLexeme() != "if"   && lex->GetLexeme() != "cond" && !is_main) {
            gen->WriteCode(tabs, "return ");  
	  }

	  errors += literal();
	  
	  if (op != "" && op != "return "){
	    gen->WriteCode(0, op);
	    op = "";
	  }
	  if (lex->GetLexeme() != "main" ){
            if (depth ==0)
              if(!is_main)
                gen->WriteCode(0, ";\n");
          }

	  break;
	  
	case STRLIT_T:
	  //ruleFile << "stmt function complete. Current token is: " << lex->GetTokenName(token) << endl;
	  ruleFile << "Using Rule 7"  << endl;
	   if (lex->GetLexeme() != "main" ){
	     if (depth ==0)
	       if(!is_main)
		 gen->WriteCode(tabs, "return ");  
	   }

	  errors += literal();
	  if (lex->GetLexeme() != "main" ){
	    if (depth ==0)
	      if(!is_main)
		gen->WriteCode(0, ";\n");
	  }
	  break;
	  
	case SQUOTE_T:
	  //ruleFile << "stmt function complete. Current token is: " << lex->GetTokenName(token) << endl;
	  if (lex->GetLexeme() != "main" ){
	    if (depth ==0)
	      if(!is_main)
		gen->WriteCode(tabs, "return ");
	  }
	  ruleFile << "Using Rule 7"  << endl;
	  errors += literal();
	  if (lex->GetLexeme() != "main" ){
            if (depth ==0)
              if(!is_main)
                gen->WriteCode(0, ";\n");
          }
	  
	  break;
	  
	  
	default: 
	  errors++;
	  ReportError(string("No rule found in stmt function, current token is: " + lex->GetTokenName(token)));
	  token = lex->GetToken();
	  break;
	}

	ruleFile << "Exiting Stmt function; current token is: " << lex ->GetTokenName (token) << endl;
  	return errors;	
}

int SyntacticalAnalyzer::literal(){
  //This function deals with the literal rules (10-12) and will check for syntactical correctness,
  //this function will then, depending on the token seen, send the program to quoted lit or return.
  ruleFile << "Entering Literal function; current token is: " << lex ->GetTokenName(token) << ", lexeme: " << lex->GetLexeme() << endl;
  
  int errors =0;
  
  if (token == NUMLIT_T)
    { 
      ruleFile << "Using Rule 10" << endl;
     
      gen->WriteCode(0,"Object(" + lex->GetLexeme() + ")");

      token = lex->GetToken();
      ruleFile << "Exiting Literal function; current token is: " << lex ->GetTokenName (token) << endl;
      return errors;
    }
  
  
  if (token == STRLIT_T)
    {
     ruleFile << "Using Rule 11" << endl;
     gen->WriteCode(0,"Object(" + lex->GetLexeme() + ")");
     token = lex->GetToken();
     ruleFile << "Exiting Literal function; current token is: " << lex ->GetTokenName (token) << endl;
     return errors;
    }
  
  
  if ( token == SQUOTE_T )
    {
      //should we have (" \" ") instead of getlexeme(), because the lexeme should be a ' 
      ruleFile << "Using Rule 12" << endl;
      //ruleFile << "Literal Function complete. Current token is: " << lex->GetTokenName(token) << endl;
      token = lex->GetToken();
      gen->WriteCode(0,"Object(");
      gen->WriteCode(0, "\""); 
      errors += quoted_lit();
      gen->WriteCode(0, "\"");
      gen->WriteCode(0, ")");
      is_param = true;
      //gen->WriteCode(tabs, " \" ");
    }
  else{
    if ( token != RPAREN_T){
      errors++;
      ReportError(string("expected either NUMLIT_T, STRLIT_T, or SQUOTE_T in literal but got: " + lex->GetTokenName(token)));
    }
  }
  ruleFile << "Exiting Literal function; current token is: " << lex ->GetTokenName (token) << endl;
  return errors;
}

int SyntacticalAnalyzer::more_tokens(string space){
  //This function deals with the more-tokens rules (14-15) and will check for syntactical correctness,
  //this function will then, depending on the token seen, send the program to anyother tokens, more tokens,
  // or will return.
	ruleFile << "Entering More_Tokens function; current token is: " << lex ->GetTokenName(token) << ", lexeme: " << lex->GetLexeme() << endl;	
	int errors = 0;
	
	if (token == EOF_T){
	  errors++;
	  ruleFile << "More_Tokens function complete. Current token is: " << lex->GetTokenName(token) << endl;
	  return errors;  
	}
	
	if (token == RPAREN_T) {
		ruleFile << "Using Rule 15" << endl;
		ruleFile << "Exiting More_Tokens function; current token is: " << lex ->GetTokenName (token) << endl;
		return errors;
	}
	
	ruleFile << "Using Rule 14" << endl;
	if (space == " "){
          gen->WriteCode(0, space);
	  space = "";
	}
	 
	errors += any_other_token();		
	errors += more_tokens(" ");
	ruleFile << "Exiting More_Tokens function; current token is: " << lex ->GetTokenName (token) << endl;

	return errors;
}

int SyntacticalAnalyzer::quoted_lit(){
   //This function deals with the quoted_lit rules (13) and will check for syntactical correctness,
  //this function will then, depending on the token seen, send the program to any other token.
	ruleFile << "Entering Quoted_Lit function; current token is: " << lex ->GetTokenName(token) << ", lexeme: " << lex->GetLexeme() << endl;

	int errors = 0;
	ruleFile << "Using Rule 13" << endl;
	//gen->WriteCode(tabs, "\"");
	errors += any_other_token();
	//gen->WriteCode(tabs, "\"");
	ruleFile << "Exiting Quoted_Lit function; current token is: " << lex ->GetTokenName (token) << endl;

	return errors;
}			

int SyntacticalAnalyzer::param_list(int i){
  //This function deals with the param_list rules (16-17) and will check for syntactical correctness,
  //this function will then, depending on the token seen, send the program to param_list or return.
  ruleFile << "Entering Param_List function; current token is: " << lex ->GetTokenName(token) << ", lexeme: " << lex->GetLexeme() << endl;

	int errors = 0;

	if(token == RPAREN_T) {
		ruleFile << "Using Rule 17" << endl;    
		ruleFile << "Exiting Param_List function; current token is: " << lex ->GetTokenName (token) << endl;
		return errors;
	}
	else if (token == IDENT_T){
	  ruleFile << "Using Rule 16" << endl;
	  if(i ==1)
	    gen->WriteCode(tabs, "Object " + lex->GetLexeme());
	  else
	    gen->WriteCode(tabs, ", Object " + lex->GetLexeme());
	  
	  token = lex->GetToken();
	  errors += param_list(i+1);
	  ruleFile << "Exiting Param_List function; current token is: " << lex ->GetTokenName (token) << endl;
	  return errors;
	}
	else{//we have an error
	  errors++;
		ReportError(string("expected a IDENT_T or RPAREN_T in paramList, but found  " + lex->GetTokenName(token)));
		ruleFile << "Exiting Param_List function; current token is: " << lex ->GetTokenName (token) << endl;
		return errors;
	}
	ruleFile << "Exiting Param_List function; current token is: " << lex ->GetTokenName (token) << endl;
	return errors;
}

int SyntacticalAnalyzer::else_part(){
  //This function deals with the else part rules (18-19) and will check for syntactical correctness,
  //this function will then, depending on the token seen, send the program to stmt ot return.
	ruleFile << "Entering Else_Part function; current token is: " << lex ->GetTokenName(token) << ", lexeme: " << lex->GetLexeme() << endl;

	int errors = 0;
	if (token == EOF_T) {
		errors++;
		ReportError(string("Reached an EOF_T but expected something else in else part"));
		ruleFile << "Exiting Else_Part function; current token is: " << lex ->GetTokenName (token) << endl;
		return errors;
	} // end EOF if 
	if (token == RPAREN_T) {
		ruleFile << "Using Rule 19" << endl;
		ruleFile << "Exiting Else_Part function; current token is: " << lex ->GetTokenName (token) << endl;
		return errors;
	} // end Right paren if

      
	ruleFile << "Using Rule 18" << endl;
	gen->WriteCode(tabs, "else{\n");
	tabs++;
	errors += stmt("");
	tabs--;
	gen->WriteCode(tabs, "}\n");
	//	from_else = true;
	ruleFile << "Exiting Else_Part function; current token is: " << lex ->GetTokenName (token) << endl;

	return errors;
}

int SyntacticalAnalyzer::stmt_pair(){
  //This function deals with the stmt_pair rules (20-21) and will check for syntactical correctness,
  //this function will then, depending on the token seen, send the program to stmt_pair_body or will return.
	ruleFile << "Entering Stmt_Pair function; current token is: " << lex ->GetTokenName(token) << ", lexeme: " << lex->GetLexeme() << endl;

	int errors = 0;
	if(token == RPAREN_T){
	  ruleFile << "Using Rule 21" << endl;
	  ruleFile << "Exiting Stmt_Pair function; current token is: " << lex ->GetTokenName (token) << endl;
	  return errors;

	}
	if(token != LPAREN_T){
	  errors ++;
	  token = lex->GetToken();
        } // end left paren if     
	token = lex->GetToken();
	ruleFile << "Using Rule 20" << endl;
	errors += stmt_pair_body("else if(");
	while(token != RPAREN_T){
		errors++;
		ReportError(string("expected a RPAREN_T in stmtpair, but found  " + lex->GetTokenName(token)));
		token = lex->GetToken();
	} // end right paren if 
	//token = lex->GetToken();
	ruleFile << "Exiting Stmt_Pair function; current token is: " << lex ->GetTokenName (token) << endl;

	return errors;

}

int SyntacticalAnalyzer::stmt_pair_body(string op){
  //This function deals with the stmt_pair_body rules (22-23) and will check for syntactical correctness,
  //this function will then, depending on the token seen, send the program to stmt, stmt, and stmt_par, or
  // just stmt.
	ruleFile << "Entering Stmt_Pair_Body function; current token is: " << lex ->GetTokenName(token) << ", lexeme: " << lex->GetLexeme() << endl;

	int errors = 0;
	if (token == ELSE_T){
		//rule 23
	  ruleFile << "Using Rule 23" << endl;
	  gen->WriteCode(tabs, lex->GetLexeme() + "{ \n");
	  tabs++;
	  token = lex->GetToken();
	  depth--;
	  errors += stmt("");
	  if (token != RPAREN_T){
	    errors++;
	    ReportError(string("expected a RPAREN_T in Stmt_Pair_Body, but found  " + lex->GetTokenName(token)));
	  }
	  else
	    token = lex->GetToken();
	  depth++;
	  from_else= true;
	  tabs--;
	  gen->WriteCode(tabs,  "} \n");
	}

	if(token == NUMLIT_T || token == STRLIT_T || token == SQUOTE_T ||token == IDENT_T || token== LPAREN_T) {
	  //rule 22
	  
	   ruleFile << "Using Rule 22" << endl;
	   gen->WriteCode(tabs, op);
	   errors += stmt("");
	   gen->WriteCode(0, "){ \n");
	   depth--;
	   tabs++;
	   errors += stmt("");
	   depth++;
	   tabs--;
	   gen->WriteCode(tabs, "} \n");
	   if (token != RPAREN_T){
	     errors++;
	     ReportError(string("expected a RPAREN_T in Stmt_Pair_Body, but found  " + lex->GetTokenName(token)));
	   }
	   token = lex->GetToken();
	   
	   errors += stmt_pair();
	}

	ruleFile << "Exiting Stmt_Pair_Body function; current token is: " << lex->GetTokenName(token) << endl;
	
	return errors;
}

int SyntacticalAnalyzer::action(){
  //This function deals with the action rules (24-49) and will check for syntactical correctness,
  //this function will then, depending on the token seen, send the program to stmt, stmt-list, stmt and stmt-list, or return. 
  ruleFile << "Entering Action function; current token is: " << lex ->GetTokenName(token) << ", lexeme: " << lex->GetLexeme() << endl;

	int errors = 0;
	if (token == IF_T)
	{//applying rule 24
		ruleFile << "Using Rule 24" << endl;
		token = lex->GetToken();
	
		gen->WriteCode(tabs, "if(");
		errors += stmt("");
		gen->WriteCode(0, "){\n");
		depth--;
		tabs++;
		errors += stmt("");
		tabs--;
		gen->WriteCode(tabs, "}\n");		
		errors+= else_part();
		from_else = true;
		depth++;
		ruleFile << "Exiting Action function; current token is: " << lex ->GetTokenName (token) << endl;
		return errors;
	}
	if (token == COND_T)
	{//applying rule 25
		ruleFile << "Using Rule 25" << endl;
		token = lex->GetToken();
		
		if (token != LPAREN_T){
		  errors++;
		  ReportError(string("LPAREN_T expected in action, but see " + lex->GetTokenName(token)));
		}
		token = lex->GetToken();
		errors+= stmt_pair_body("if(");
		ruleFile << "Exiting Action function; current token is: " << lex ->GetTokenName (token) << endl;
		return errors;
	}
	if ( token == LISTOP_T){
		//RULE 26
		ruleFile << "Using Rule 26" << endl;
		 if(depth == 0)
                   gen->WriteCode(tabs, "listop(\""+ lex->GetLexeme() );
                else
		   gen->WriteCode(0, "listop(\""+ lex->GetLexeme() );

		token = lex->GetToken();
		errors += stmt("\" , ");
		gen->WriteCode(0, ")");
		ruleFile << "Exiting Action function; current token is: " << lex->GetTokenName(token) << endl;
		return errors;
	}
	if ( token == CONS_T){
		//RULE 27
		ruleFile << "Using Rule 27" << endl;
		token = lex->GetToken();
		errors += stmt("cons(");
	
		errors += stmt(" , ");
		gen->WriteCode(0, ")");
		ruleFile << "Exiting Action function; current token is: " << lex->GetTokenName(token) << endl;
		return errors;
	}
	if ( token == AND_T){
		//RULE 28
		ruleFile << "Using Rule 28" << endl;
		token = lex->GetToken();
		errors += stmt_list("&&");
		ruleFile << "Exiting Action function; current token is: " << lex->GetTokenName(token) << endl;
		return errors;
	}
	if ( token == OR_T){
		//RULE 29
		ruleFile << "Using Rule 29" << endl;
		token = lex->GetToken();
		errors += stmt_list("||");
		ruleFile << "Exiting Action function; current token is: " << lex->GetTokenName(token) << endl;
		return errors;
	}
	if ( token == NOT_T){
		//RULE 30
		ruleFile << "Using Rule 30" << endl;
		token = lex->GetToken();
		errors += stmt("!(");
		gen->WriteCode(0, ")");
		ruleFile << "Exiting Action function; current token is: " << lex->GetTokenName(token) << endl;
		return errors;
	}
	if ( token == NUMBERP_T){
		//RULE 31
		ruleFile << "Using Rule 31" << endl;
		token = lex->GetToken();
		errors += stmt("numberp(");
		gen->WriteCode(0, ")");
		ruleFile << "Exiting Action function; current token is: " << lex->GetTokenName(token) << endl;
		return errors;
	}
	if ( token == LISTP_T){
		//RULE 32
		ruleFile << "Using Rule 32" << endl;
		token = lex->GetToken();
		errors += stmt("listp(");
		gen->WriteCode(0, ")");
		ruleFile << "Exiting Action function; current token is: " << lex->GetTokenName(token) << endl;
		return errors;
	}
	if ( token == ZEROP_T){
		//RULE 33
		ruleFile << "Using Rule 33" << endl;
		token = lex->GetToken();
		errors += stmt("zerop(");
		gen->WriteCode(0, ")");
		ruleFile << "Exiting Action function; current token is: " << lex->GetTokenName(token) << endl;
		return errors;
	}
	if ( token == NULLP_T){
		//RULE 34
		ruleFile << "Using Rule 34" << endl;
		token = lex->GetToken();
		errors += stmt("nullp(");
		gen->WriteCode(0, ")");
		ruleFile << "Exiting Action function; current token is: " << lex->GetTokenName(token) << endl;
		return errors;
	}
	if ( token == STRINGP_T){
		//RULE 35
		ruleFile << "Using Rule 35" << endl;
		token = lex->GetToken();
		errors += stmt("stringp(");
		gen->WriteCode(0, ")");
		ruleFile << "Exiting Action function; current token is: " << lex->GetTokenName(token) << endl;
		return errors;
	}
	if ( token == PLUS_T){
		//RULE 36
		ruleFile << "Using Rule 36" << endl;
		token = lex->GetToken();
		if(depth == 0)
		  gen->WriteCode(tabs, "(");
		else
		   gen->WriteCode(0, "(");
		errors += stmt_list("+");
		gen->WriteCode(0, ")");
		ruleFile << "Exiting Action function; current token is: " << lex->GetTokenName(token) << endl;
		return errors;
	}
	if ( token == MINUS_T){
		//RULE 37
		ruleFile << "Using Rule 37" << endl;
		token = lex->GetToken();
		if(depth == 0)
                  gen->WriteCode(tabs, "(");
                else
		  gen->WriteCode(0, "(");
		errors += stmt("-");
		errors += stmt_list("");
		gen->WriteCode(0, ")");
		ruleFile << "Exiting Action function; current token is: " << lex->GetTokenName(token) << endl;
		return errors;
	}
	if ( token == DIV_T){
		//RULE 38
		ruleFile << "Using Rule 38" << endl;
		token = lex->GetToken();
		if(depth == 0)
		  gen->WriteCode(tabs, "(");
                else
		  gen->WriteCode(0, "(");
		errors += stmt("/");
		errors += stmt_list("");
		gen->WriteCode(0, ")");
		ruleFile << "Exiting Action function; current token is: " << lex->GetTokenName(token) << endl;
		return errors;
	}
	if ( token == MULT_T){
		//RULE 39
		ruleFile << "Using Rule 39" << endl;
		token = lex->GetToken();
		if(depth == 0)
                  gen->WriteCode(tabs, "(");
                else
		  gen->WriteCode(0, "(");
		errors += stmt_list("*");
		gen->WriteCode(0, ")");
		ruleFile << "Exiting Action function; current token is: " << lex->GetTokenName(token) << endl;
		return errors;
	}
	if ( token == MODULO_T){
		//RULE 40
		ruleFile << "Using Rule 40" << endl;
		token = lex->GetToken();
		if(depth == 0)
                  gen->WriteCode(tabs, "(");
                else
		  gen->WriteCode(0, "(");
		errors += stmt("%");
		errors += stmt("");
		gen->WriteCode(0, ")");
		ruleFile << "Exiting Action function; current token is: " << lex->GetTokenName(token) << endl;
		return errors;
	}
	if ( token == ROUND_T){
		//RULE 41
		ruleFile << "Using Rule 41" << endl;
		token = lex->GetToken();
		errors += stmt("round(");
		gen->WriteCode(0, ")");
		ruleFile << "Exiting Action function; current token is: " << lex->GetTokenName(token) << endl;
		return errors;
	}
	if ( token == EQUALTO_T){
		//RULE 42
		ruleFile << "Using Rule 42" << endl;
		token = lex->GetToken();
		errors += stmt_list("==");
		ruleFile << "Exiting Action function; current token is: " << lex->GetTokenName(token) << endl;
		return errors;
	}
	if ( token == GT_T){
	  //RULE 43
	  ruleFile << "Using Rule 43" << endl;
	  token = lex->GetToken();
	  errors += stmt_list(">");
	  ruleFile << "Exiting Action function; current token is: " << lex->GetTokenName(token) << endl;
	  return errors;
	}
	if ( token == LT_T){
		//RULE 44
		ruleFile << "Using Rule 44" << endl;
		token = lex->GetToken();
		errors += stmt_list("<");
		ruleFile << "Exiting Action function; current token is: " << lex->GetTokenName(token) << endl;
		return errors;
	}
	if ( token == GTE_T){
		//RULE 45
		ruleFile << "Using Rule 45" << endl;
		token = lex->GetToken();
		errors += stmt_list(">=");
		ruleFile << "Exiting Action function; current token is: " << lex->GetTokenName(token) << endl;
		return errors;
	}
	if ( token == LTE_T){
		//RULE 46
		ruleFile << "Using Rule 46" << endl;
		token = lex->GetToken();
		errors += stmt_list("<=");
		ruleFile << "Exiting Action function; current token is: " << lex->GetTokenName(token) << endl;
		return errors;
	}
	if ( token == IDENT_T){
		//RULE 47
		ruleFile << "Using Rule 47" << endl;
		from_action = true;
		is_param = false;
		if (depth == 0)
		  gen->WriteCode(tabs, lex->GetLexeme() + "(");
		else
		  gen->WriteCode(0, lex->GetLexeme() + "(");
		token = lex->GetToken();
		errors += stmt_list(",");
		gen->WriteCode(0, ")");
		from_action = false;
		ruleFile << "Exiting Action function; current token is: " << lex->GetTokenName(token) << endl;
		return errors;
	}
	if ( token == DISPLAY_T){
		//RULE 48
		ruleFile << "Using Rule 48" << endl;
		token = lex->GetToken();
		gen->WriteCode(tabs, "cout << ");
		errors += stmt("");
		//gen->WriteCode(tabs, "; \n");
		ruleFile << "Exiting Action function; current token is: " << lex->GetTokenName(token) << endl;
		return errors;
	}
	if ( token == NEWLINE_T){
		//RULE 49
		ruleFile << "Using Rule 49" << endl;
		token = lex->GetToken();
		gen->WriteCode(tabs, "cout << endl");
		ruleFile << "Exiting Action function; current token is: " << lex->GetTokenName(token) << endl;
		return errors;
	}
	//no rule found
	//ruleFile << "error in action" << endl;
	errors++;
	ReportError(string("no rule found in action, curren token is " + lex->GetTokenName(token)));
	token = lex->GetToken();
	ruleFile << "Exiting Action function; current token is: " << lex->GetTokenName(token) << endl;
	return errors;

}

int SyntacticalAnalyzer::any_other_token(){
   //This function deals with the stmt_pair_body rules (22-23) and will check for syntactical correctness,
  //this function will then, depending on the token seen, send the program tomre, tokens, any_other_tokens, or return. 
  int errors = 0;
  string error_type;
  ruleFile << "Entering Any_Other_Token function; current token is: " << lex ->GetTokenName(token) << ", lexeme: " << lex->GetLexeme() << endl;
  string sp = "";
  string rm_quote = "\\";

  switch(token){
  case LPAREN_T:
    ruleFile << "Using Rule 50\n";
    gen->WriteCode(0, lex->GetLexeme());
    token = lex->GetToken();
    errors += more_tokens("");
    if(token != RPAREN_T){
      errors++;
      ReportError(string("Expected RPARENT_T in any_other_token, but found " + lex->GetTokenName(token)));
    }
    gen->WriteCode(0, lex->GetLexeme());
    token = lex->GetToken();
    break;
  case IDENT_T:
    ruleFile << "Using Rule 51\n";
    gen->WriteCode(0, lex->GetLexeme());
    token = lex->GetToken();
    break;
  case NUMLIT_T:
    ruleFile << "Using Rule 52\n";
    gen->WriteCode(0, lex->GetLexeme());
    token = lex->GetToken();
    break;
  case STRLIT_T:
    ruleFile << "Using Rule 53\n";
    //gen->WriteCode(0,"\"");
    sp = lex->GetLexeme();
    for( int i = 0 ; i < sp.size()-1; i++)
      rm_quote+= sp[i];
     rm_quote+= "\\\"";
    gen->WriteCode(0,rm_quote);
    token = lex->GetToken();
    //gen->WriteCode(0,"\"");
    break;
  case CONS_T:
    ruleFile << "Using Rule 54\n";
    gen->WriteCode(0, lex->GetLexeme());
    token = lex->GetToken();
    break;
  case IF_T:
    ruleFile << "Using Rule 55\n";
    gen->WriteCode(0, lex->GetLexeme());
    token = lex->GetToken();
    break;
  case DISPLAY_T:
    ruleFile << "Using Rule 56\n";
    gen->WriteCode(0, lex->GetLexeme());
    token = lex->GetToken();
    break;
  case NEWLINE_T:
    ruleFile << "Using Rule 57\n";
    gen->WriteCode(0, lex->GetLexeme());
    token = lex->GetToken();
    break;
  case LISTOP_T:
    ruleFile << "Using Rule 58\n";
    gen->WriteCode(0, lex->GetLexeme());
    token = lex->GetToken();
    break;
  case AND_T:
    ruleFile << "Using Rule 59\n";
    gen->WriteCode(0, lex->GetLexeme());
    token = lex->GetToken();
    break;
  case OR_T:
    ruleFile << "Using Rule 60\n";
    gen->WriteCode(0, lex->GetLexeme());
    token = lex->GetToken();
    break;
  case NOT_T:
    ruleFile << "Using Rule 61\n";
    gen->WriteCode(0, lex->GetLexeme());
    token = lex->GetToken();
    break;
  case DEFINE_T:
    ruleFile << "Using Rule 62\n";
    gen->WriteCode(0, lex->GetLexeme());
    token = lex->GetToken();
    break;
  case NUMBERP_T:
    ruleFile << "Using Rule 63\n";
    gen->WriteCode(0, lex->GetLexeme());
    token = lex->GetToken();
    break;
  case LISTP_T:
    ruleFile << "Using Rule 64\n";
    gen->WriteCode(0, lex->GetLexeme());
    token = lex->GetToken();
    break;
  case ZEROP_T:
    ruleFile << "Using Rule 65\n";
    gen->WriteCode(0, lex->GetLexeme());
    token = lex->GetToken();
    break;
  case NULLP_T:
    ruleFile << "Using Rule 66\n";
    gen->WriteCode(0, lex->GetLexeme());
    token = lex->GetToken();
    break;
  case STRINGP_T:
    ruleFile << "Using Rule 67\n";
    gen->WriteCode(0, lex->GetLexeme());
    token = lex->GetToken();
    break;
  case PLUS_T:
    ruleFile << "Using Rule 68\n";
    gen->WriteCode(0, lex->GetLexeme());
    token = lex->GetToken();
    break;
  case MINUS_T:
    ruleFile << "Using Rule 69\n";
    gen->WriteCode(0, lex->GetLexeme());
    token = lex->GetToken();
    break;
  case DIV_T:
    ruleFile << "Using Rule 70\n";
    gen->WriteCode(0, lex->GetLexeme());
    token = lex->GetToken();
    break;
  case MULT_T:
    ruleFile << "Using Rule 71\n";
    gen->WriteCode(0, lex->GetLexeme());
    token = lex->GetToken();
    break;
  case MODULO_T:
    ruleFile << "Using Rule 72\n";
    gen->WriteCode(0, lex->GetLexeme());
    token = lex->GetToken();
    break;
  case ROUND_T:
    ruleFile << "Using Rule 73\n";
    gen->WriteCode(0, lex->GetLexeme());
    token = lex->GetToken();
    break;
  case EQUALTO_T:
    ruleFile << "Using Rule 74\n";
    gen->WriteCode(0, lex->GetLexeme());
    token = lex->GetToken();
    break;
  case GT_T:
    ruleFile << "Using Rule 75\n";
    gen->WriteCode(0, lex->GetLexeme());
    token = lex->GetToken();
    break;
  case LT_T:
    ruleFile << "Using Rule 76\n";
    gen->WriteCode(0, lex->GetLexeme());
    token = lex->GetToken();
    break;
  case GTE_T:
    ruleFile << "Using Rule 77\n";
    gen->WriteCode(0, lex->GetLexeme());
    token = lex->GetToken();
    break;
  case LTE_T:
    ruleFile << "Using Rule 78\n";
    gen->WriteCode(0, lex->GetLexeme());
    token = lex->GetToken();
    break;
  case SQUOTE_T:
    ruleFile << "Using Rule 79\n";
    gen->WriteCode(0, lex->GetLexeme());
    token = lex->GetToken();
    errors += any_other_token();
    //Should this be a "
    //gen->WriteCode(tabs, lex->GetLexeme());
    break;
  case COND_T:
    ruleFile << "Using Rule 80\n";
    gen->WriteCode(0, lex->GetLexeme());
    token = lex->GetToken();
    break;
  case ELSE_T:
    ruleFile << "Using Rule 81\n";
    gen->WriteCode(-1, lex->GetLexeme());
    token = lex->GetToken();
    break;
  default:
    //no rule found
    errors++;
    ReportError(string("No rule found in any_other_token function, current token is: " + lex->GetTokenName(token)));
    token = lex->GetToken();
  }
  
  //if the current token at this point is not apart of the follows then that is an error?
  ruleFile << "Exiting Any_Other_Token function; current token is: " << lex->GetTokenName(token) << endl;
  return errors;
}

void SyntacticalAnalyzer::ReportError (const string & msg){
	lex->listing << "Syntax error at " << lex->get_line_num() << ',' << lex->get_pos() << ": " << msg << endl;
	lex->debug << "Syntax error at " << lex->get_line_num() << ',' << lex->get_pos() << ": " << msg << endl;

}
