/* https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/index.html
MAJORITY (PRETTY MUCH ALL) OF CODE FROM ^^^ LLVM SITE */
#include "llvm/ADT/STLExtras.h"
#include <algorithm>
#include <map>
#include <cstdio>
#include <cctype>
#include <string>
#include <vector>
#include <memory>


/*Chapter 1: the Lexer (precursor to the parser)
A Lexer is essentially a scanner to interpret a language.
It breaks the input up into tokens, each token returned by lexer includes
a token code and metadata (numeric value of number or something).
Returns tokens [0-255] if unknown character, otherwise one of these for
the knowns.*/
// TODO: Add more tokens to recognize perhaps
enum Token {
    tok_eof = -1,
    // Commands
    tok_def = -2,
    tok_extern = -3,
    //Primary
    tok_identifier = -4,
    tok_number = -5,
};

// These are global to be simple
static std::string IdentifierStr; // If token is identifier: tok_identifier holds value 
static double NumVal;             // If token is a num then: tok_number holds the value

static int gettok() {
    // This is what gets the next token from input
    // Unsure why it needs to be this way though
    static int LastChar = ' ';
    // Skip whitespace since...well duh
    while (isspace(LastChar)) {
        // Stores character (not yet processed)
        LastChar = getchar();
    }
    // Next to identify identifiers & specific keywords like 'def'
    // Identify letters and such from input
    if (isalpha(LastChar)) { // identifier: a-Z 0-9
        IdentifierStr = LastChar;
        while(isalnum((LastChar = getchar()))) {
            IdentifierStr += LastChar;
        }
        if(IdentifierStr == "def")
            return tok_def;
        if(IdentifierStr == "extern")
            return tok_extern;
        return tok_identifier;
    }
    // Identify numerical values from input 
    if (isdigit(LastChar) || LastChar == '.') { // Numbers: 0-9
        std::string NumStr;
        // Get all the numbers but this doesn't do sufficient error checking
        // TODO: create better error checking!
        do {
            NumStr += LastChar;
            LastChar = getchar();
        } while (isdigit(LastChar) || LastChar == '.');
        NumVal = strtod(NumStr.c_str(), 0);
        return tok_number;
    }
    // Here we lex comments
    if (LastChar=='#') {
        // Make the lexer understand we comment until the end of the line
        do
            LastChar = getchar();
        while (LastChar != EOF && LastChar != '\n' && LastChar != '\r');

        if (LastChar != EOF)
            return gettok();
    }
    // Checking for end of file
    if (LastChar == EOF) {
        return tok_eof;
    }
    // Otherwise, return character token as its ascii value
    int ThisChar = LastChar;
    // Remember LastChar is global
    LastChar = getchar();
    return ThisChar;
}

/* Chapter 2a: Abstract Syntax Tree
An abstract syntax tree (is the output of the parser),
captures a program's behaviour so that later
stages of the compiler (code generation) can interpret it easier.
One object for each construct in language.
We got expressions, a prototype, & function object.
*/

namespace {
// ExprAST - Base class for all expression nodes(?)
// I see, it's for the one below and other like it...obviously I guess lol
class ExprAST {
public:
    // Tilde in this context is a destructor (wow can't believe I forgot that lol)
    virtual ~ExprAST() {}
};

// NumberExprAST - Expression class for numeric literals
// NumberExprAST is the derived class from ExprAST
class NumberExprAST : public ExprAST {
    double Val;
public:
    NumberExprAST(double Val) : Val(Val) {}
};

// VariableExprAST - Expression class for variables
class VariableExprAST: public ExprAST {
    std::string Name;
public:
    VariableExprAST(const std::string &Name) : Name(Name) {}
};

// BinaryExprAST - Expression class for binary operators
class BinaryExprAST: public ExprAST {
    char Op;
    // unique_ptr -> smart pointer that 'owns & manages another object thru
    // a pointer and disposes of object when unique_ptr goes out of scope'
    // Garbage collection stuff, no other pointer can delete the managed object
    std::unique_ptr<ExprAST> LHS, RHS; // left-hand side, right-hand side.
    // < ? > the signs hold the type
public:
    // So ugly, don't fully understand yet oh well
    BinaryExprAST(char op, std::unique_ptr<ExprAST> LHS,
                  std::unique_ptr<ExprAST> RHS)
        : Op(op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
};

// CallExprAST - Expression class for func calls (?)
class CallExprAST : public ExprAST {
    std::string Callee;
    std::vector<std::unique_ptr<ExprAST>> Args;
public:
    CallExprAST(const std::string &Callee,
                std::vector<std::unique_ptr<ExprAST>> Args)
        : Callee(Callee), Args(std::move(Args)) {}
};

// PrototypeAST - class is the prototype for a func,
// gets name and argument names + # of args func takes
class PrototypeAST {
    std::string Name;
    std::vector<std::string> Args;
public:
    PrototypeAST(const std::string &name, std::vector<std::string> Args)
        : Name(name), Args(std::move(Args)) {}
    const std::string &getName() const { return Name; }
};

// FunctionAST - represents function definition itself
class FunctionAST {
    std::unique_ptr<PrototypeAST> Proto;
    std::unique_ptr<ExprAST> Body;
public:
    FunctionAST(std::unique_ptr<PrototypeAST> Proto,
                std::unique_ptr<ExprAST> Body)
        : Proto(std::move(Proto)), Body(std::move(Body)) {}
};

} // end anon namespace
/* Chapter 2b: Parser
"referring to the syntactic analysis of the 
input code into its component parts in order 
to facilitate the writing of compilers and interpreters" - Wikipedia
My current idea of what it is TBD
*/

// CurTok/getNextToken - token buffer (?). CurTok is current token parser watching,
// getNextToken reads another token from lexer and updates CurTok w/ results (?)
static int CurTok;
static int getNextToken() {
    // implements token buffer around lexer, and looks at one token ahead as explained above
    return CurTok = gettok();
}

// LogError for helping us with error handling
// Not the best but this is good enough
std::unique_ptr<ExprAST> LogError(const char *Str) {
    fprintf(stderr, "LogError: %s\n", Str);
    return nullptr;
}
std::unique_ptr<PrototypeAST> LogErrorP(const char *Str) {
    LogError(Str);
    return nullptr;
}

// NOW WE MOVE ONTO OUR FIRST GRAMMATICAL CONSTRUCT!!!!
// NUMERIC LITERALSSSSSSSSSSSSSSS
// numberexpr ::= number
static std::unique_ptr<ExprAST> ParseNumberExpr() {
    auto Result = std::make_unique<NumberExprAST>(NumVal);
    getNextToken(); // 'consume' number (?)
    return std::move(Result);
}

// parenexpr ::= '(' expression ')' quotes around parens cause
// that's what this detects
static std::unique_ptr<ExprAST> ParseParenExpr() {
    getNextToken(); // NOM the first '('
    // This is apparently a recursive thingie going on here...
    // I am NOT good with recursion but I better learn quick
    // considering that's legit how this all works lmao
    auto V = ParseExpression(); // this function hasn't appeared yet
    if (!V) {
        return nullptr;
    }
    if (CurTok != ')') {
        return LogError("expected ')'"); // Oooh logerror!
        // Make sure there isn't errant ')'s in the lil' expression
        // Just noticed they call it a 'subexpression' ooonice
    }
    getNextToken(); // gets ')'
    return V;
}

// This has got recursion too + error handling 0.0
// identifierexpr
//  ::= identifier
//  ::= identifier '(' expression* ')' oh a little star
static std::unique_ptr<ExprAST> ParseIdentifierExpr() {
    // Hey IdentifierStr was from the very beginning. It holds the value of the
    // token if it's an identifier fyi
    std::string IdName = IdentifierStr;

    getNextToken(); // gets identifier

    if (CurTok != '(') {
        return std::make_unique<VariableExprAST>(IdName);
    }

    // Call(?)
    getNextToken(); // Nom the '('
    std::vector<std::unique_ptr<ExprAST>> Args;
    if (CurTok != ')') {
        while (1) {
            // Again with this ParseExpression...excited to see what it looks like
            if (auto Arg = ParseExpression())
                Args.push_back(std::move(Arg));
            else
                return nullptr;
            
            // Make sure the inside of the parens got the right stuff I guess?
            if (CurTok == ')')
                break;
            if (CurTok != ',')
                return LogError("Expected ')' or ',' in argument list");
            
            getNextToken();
        }
    }

    // Nom the ')'
    getNextToken();

    return std::make_unique<CallExprAST>(IdName, std::move(Args));
}

// Here's the 'entry point'
// primary
//  ::= identifierexpr
//  ::= numberexpr
//  ::= parenexpr
static std::unique_ptr<ExprAST> ParsePrimary() {
    switch (CurTok) {
    case tok_identifier:
        return ParseIdentifierExpr();
    case tok_number:
        return ParseNumberExpr();
    case '(':
        return ParseParenExpr();
    // Well if it ain't part of these primaries then explode with the message
    default:
        return LogError("unknown token when expecting an expression...c'mon");
    }
}

// BinopPrecedence: holds precedence for each binary operator defined
static std::map<char, int> BinopPrecedence;

// GetTokPrecedence: get precedence of pending binary operator token
static int GetTokPrecedence() {
    if (!isascii(CurTok)) {
        return -1;
    }
    // make sure TokPrec is a declared binop by checking inside BinopPrecedence
    int TokPrec = BinopPrecedence[CurTok];
    if (TokPrec <= 0) {
        return -1;
    }
    return TokPrec;
}

// expression
//  ::= primary binoprhs
static std::unique_ptr<ExprAST> ParseExpression() {
    auto LHS = ParsePrimary();
    if (!LHS) {
        return nullptr; // well duh
    }

    return ParseBinOpRHS(0, std::move(LHS));
}

// parses sequences of pairs for us
// binorprhs
//  ::= ('+' primary)*
static std::unique_ptr<ExprAST> ParseBinOpRHS(int ExprPrec, std::unique_ptr<ExprAST> LHS) {
    // if this is binop, find its precedence
    while (1) {
        // Hey I remember that function from up there!
        int TokPrec = GetTokPrecedence();
        // If this binop binds as tightly (wtf??) as current binop: consume.
        if (TokPrec < ExprPrec) {
            return LHS;
        }

        // we know this is a binop
        int BinOp = CurTok;
        getNextToken(); // nom binop
        // parse primary expr after binary operator
        auto RHS = ParsePrimary();
        if (!RHS) {
            return nullptr;
        }

        // if BinOp binds less tight w/ RHS than operator after RHS, let
        // pending operator take RHS its LHS
        int NextPrec = GetTokPrecedence();
        if (TokPrec < NextPrec) {
            // if precedence of binop to right of RHS is <= to precedence
            // of current operator we know the parens associate as
            // (a+b) binop
            RHS = ParseBinOpRHS(TokPrec+1, std::move(RHS));
            if (!RHS) {
                return nullptr;
            }
        }

        // Merge LHS/RHS
        LHS = std::make_unique<BinaryExprAST>(BinOp, std::move(LHS), std::move(RHS));
    } // loop to top
}

// Prototype
//  ::= id '(' id* ')'
static std::unique_ptr<PrototypeAST> ParsePrototype() {
    if (CurTok != tok_identifier) {
        return LogErrorP("Expected function name in prototype")
    }

    std::string FnName = IdentifierStr; // quite literally the function name I think
    getNextToken(); 

    if (CurTok != '(') {
        return LogErrorP("Expected '(' in prototype");
    }

    // read list of argument names
    std::vector<std::string> ArgNames;
    while (getNextToken() == tok_identifier) {
        ArgNames.push_back(IdentifierStr);
    }
    if (CurTok != ')') {
        return LogErrorP("Expected ')' in prototype");
    }
    // Success
    getNextToken(); // eat ')'
    return std::make_unique<PrototypeAST>(FnName, std::move(ArgNames));
}

// Definition ::= 'def' prototype expression
static std::unique_ptr<FunctionAST> ParseDefinition() {
    getNextToken(); // nom the def 
    auto Proto = ParsePrototype();
    if (!Proto) return nullptr; // obviously if nothing there then gtfo

    if (auto E = ParseExpression()) {
        return std::make_unique<FunctionAST>(std::move(Proto), std::move(E));
    }
    return nullptr;
}

// external ::= 'extern' prototype
static std::unique_ptr<PrototypeAST> ParseExtern() {
    getNextToken(); // nom the extern token
    return ParsePrototype();
}

// toplevelexpr ::= expression
static std::unique_ptr<FunctionAST> ParseTopLevelExpr() {
    if (auto E = ParseExpression()) {
        // anonymous proto
        auto Proto = std::make_unique<PrototypeAST>("", std::vector<std::string>());
        return std::make_unique<FunctionAST>(std::move(Proto), std::move(E));
    }
    return nullptr;
}

/* Top-level Parsing
*/
static void HandleDefinition() {
    if (ParseDefinition()) {
        fprintf(stderr, "Parsed func definition.\n");
    } else {
        getNextToken(); // skip token for error recovery
    }
    }
}

static void HandleExtern() {
    if (ParseExtern()) {
        fprintf(stderr, "Parsed extern.\n");
    } else {
        getNextToken(); // skip token for error recovery
    }
}

static void HandleTopLevelExpression() {
    // eval top-level expression into anon function
    if (ParseTopLevelExpr()) {
        fprintf(stderr, "Parsed top-level expr\n");
    } else {
        getNextToken(); // skip token for error recovery
    }
}

// top ::= definition | external | expression | ';' ???
static void MainLoop() {
    while (true) {
        fprintf(stderr, "ready>");
        switch (CurTok) {
            case tok_eof:
                return;
            case ';': // ignore top-level semicolons
                getNextToken();
                break;
            case tok_def:
                HandleDefinition();
                break;
            case tok_extern:
                HandleExtern();
                break;
            default:
                HandleTopLevelExpression();
                break;
        }
    }
}

int main() {
    // install binary operators?!?!!? (aka mapping them in that std::map thing)
    // w/ 1 being lowest precedence :O
    BinopPrecedence['<'] = 10;
    BinopPrecedence['+'] = 20;
    BinopPrecedence['-'] = 30;
    BinopPrecedence['*'] = 40;

    // Make sure first token is ready
    fprintf(stderr, "ready> ");
    getNextToken();

    // Run main interpreter loop
    MainLoop();

    return 0;
}