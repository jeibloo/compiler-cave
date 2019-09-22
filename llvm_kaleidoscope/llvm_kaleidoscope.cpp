/* https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/index.html
MAJORITY OF CODE FROM ^^^ LLVM SITE */
#include <cstdio>
#include <cctype>
#include <string>
#include <vector>

/* Chapter 1: the Lexer (precursor to the parser)
A Lexer is essentially a scanner to interpret a language.
It breaks the input up into tokens, each token returned by lexer includes
a token code and metadata (numeric value of number or something). */

// Returns tokens [0-255] if unknown character, otherwise one of these for
// the knowns.
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

// ExprAST - Base class for all expression nodes(?)
// I see, it's for the one below and other like it...obviously I guess lol
class ExprAST {
public:
    // Tilde in this context is ?
    virtual ~ExprAST() {}
};

// NumberExprAST - Expression class for numeric literals
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
    std::unique_ptr<ExprAST> LHS, RHS;
public:
    BinaryExprAST(char op, std::unique_ptr<ExprAST> LHS,
                  std::unique_ptr<ExprAST> RHS)
        : Op(op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
};

// CallExprAST - Expression class for func calls
class CallExprAST : public ExprAST {
    std::string Callee;
    std::vector<std::unique_ptr<ExprAST>> Args;
public:
    CallExprAST(const std::string &Callee,
                std::vector<std::unique_ptr<ExprAST>> Args)
        : Callee(Callee), Args(std::move(Args)) {}
};

/* Chapter 2b: Parser
*/