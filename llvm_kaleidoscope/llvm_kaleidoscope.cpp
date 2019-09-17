/* https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/index.html
MAJORITY OF CODE FROM ^^^ LLVM SITE */
#include <cstdio>
#include <cctype>
#include <string>
#include <vector>

/* Chapter 1: the lexer (precursor to the parser)
A lexer is essentially a scanner to interpret a language
It breaks the input up into tokens, each token returned by lexer includes
a token code and metadata (numeric value of number or something) */

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