# My lil' Compiler Cave!

### This repo is just a way to keep track of all the new things I learn about compilers as I go about my life.

1. [Kaleidoscope](https://llvm.org/docs/tutorial/index.html) by LLVM

---

2019/09/16
  * For my first foray into the abyss I've chosen the Kaleidoscope toy language project by LLVM. I've gotten through the lexer
but it's still mindblowing how it all works. I don't fully understand everything but the fog is certainly clearing. Especially the
way they describe how to 'tokenize' the inputs and separate it into parts that the AST and parser (I'm assuming) will understand...
whatever that's going to mean lmao.

---

2019/09/22
  * I dare you to say Abstract Syntax Tree five times fast. But before that, I went over the Lexer again. I think that after a solid week of ponderous contemplation, soul-seeking, eating good Thai food and rest, I understand more of it. Lord help me if I try to explain it but the whole looping through the entire inputted program, checking each character seems pretty logical and not too crazy...but yea know, looks deceive I'm sure.
  ![Dcoetzee](/llvm_kaleidoscope/wiki_ast.png)
  * AST: the parser's child/output and the father of my nightmares tonight. I really tried...I really tried to wrap my mind around the very basics of how this _thing_ works. Okay so, AST blindness aside, LLVM talks about two important things that I'm lost about but are apparently important for this specific AST. 
  * [This thing](https://en.wikipedia.org/wiki/Recursive_descent_parser) and [this other thing](http://en.wikipedia.org/wiki/Operator-precedence_parser). What are they? I don't know. Recursive descent parser is a 'top-down' parser...like an upside-down tree, and has a whole bunch of 'mutually recursive' procedures where stuff happens related to implementing the 'nonterminals of the grammar'(wtf). Operator precedence parser is a bottom-up parser, like most calculators (don't take my word for it, take Wikipedia's). Why are these really important to know? I'll find out soon and report back here when everything breaks as I try to compile this monstrosity.
  * So far we've got like, a few 'nodes'. So basically a base class and a whole bunch of offshoots named `NumberExprAST` and the like. The C++ code right now confuses the hell out of me so I'm going to go through it little-by-little sometime ~tonight~ under another bullet point.
  
  ---
  
  2019/12/08
   * Turns out sometime tonight is in early December, who knew.
   * So, harkening back to some good ole' days I had to re-remember what `virtual` and the `~` were. `virtual` turned out to be something about late (dynamic/runtime) and early (static/compile time) binding or something (really hope I got that right). But, anyways, "a virtual function allows derived classes to replace the implementation provided by the base class". The tilde was just a destructor ~deconstructor~ and I must've hit my head on something to forget that. Also I had to do a quick refresh on what a `&` reference was (please don't squeal to my future employers).
   * After that I looked into how this thing called a `std::unique_ptr` worked. Now according to the suprisingly good definition @ [docs.microsoft.com](https://docs.microsoft.com/en-us/cpp/standard-library/unique-ptr-class?view=vs-2019) a unique_ptr is basically a manager/garbage-collection person for a pointer, or "stores a pointer to an owned object or array. The object/array is owned by no other unique_ptr. The object/array is destroyed when the unique_ptr is destroyed". Well put MS. Then inside the lil' `< >` is where you put the type.
   * This entry sounds like it's me just trying to figure C++ out and by golly you're kinda right but don't worry compiler stuff is coming soon.
   * For whatever reason I had forgotten to add the `PrototypeAST` and the `FunctionAST`, so I did that, but I had to understand what these `std::move` things were a bit. Apparently there's such a [thing](https://en.cppreference.com/w/cpp/language/value_category) as an lvalue and an xvalue in C++...not sure what any of it means but it seems like forcing something to be an xvalue allows other stuff to move anything the object is holding. Not a good explanation but it's in that ballpark or whatever.
   * In this toy Kaleidoscope language it says on LLVM's site that "...functions are typed with just a count of their arguments", apparently the arguments are just floating-points and so explicitly setting a type or something in the `ExprAST` class isn't really needed for this. All of this is a little (read: big) confusing but I'll roll with it. 
   * Despite not fully *fully* getting everything, I shall be moving onto the implementation of the parser, thank god.

---

2019/12/09
 * Two days in a row? Must be close to Christmas. I looked ahead and saw dragons so I felt compelled to start on the Parser stuff today.
 * Apparently this parser here in chapter 2 is what we need before we build the AST. Now as I've mentioned before this parser is a combo of some recursive descent stuff and operator-precedence stuff, really not sure what any of it means and I'm *really* not sure why it implies we need to build a parser _then_ an Abstract Syntax Tree when the instructions have the AST first and then the parser...but idk my intuition is telling me that I'm probably just not understanding something.
 * "_parse something like “x+y” (which is returned as three tokens by the lexer) into an AST that can be generated with calls like this_" then goes onto showing this:
```
auto LHS = std::make_unique<VariableExprAST>("x");
auto RHS = std::make_unique<VariableExprAST>("y");
auto Result = std::make_unique<BinaryExprAST>('+', std::move(LHS),
                                              std::move(RHS));
```
 * Beautiful isn't it? (joke) 
 * Had to review what a few things were: `auto` means a deduced type, so 'become' int if value is int or something like that. `std::make_unique` works with `unique_ptr` in some way where if we used `new` instead it would be some sort of hassle so...guess we just use `make_unique` instead, [here's](https://stackoverflow.com/questions/37514509/advantages-of-using-stdmake-unique-over-new-operator) stackoverflow word-vomit related to its use.
 * There's so much more to the C++ code but I can't wrap my head around it right now, maybe later when I've found patience.
 * From the tutorial it made me write something called a token buffer which basically implements a buffer around the lexer, looking one token ahead and presumably we'll use this to determine what the "user's" code.
 * I think also wrote a small LogError function that takes a `const char *Str` and then returns a `nullptr` which'll help us debug our code a bit but is bare minimum.
 * Sometime this week is creating my first piece of grammar. AKA: numeric literals :O

---

2019/12/10
 ```
 static std::unique_ptr<ExprAST> ParseNumberExpr() {
  auto Result = std::make_unique<NumberExprAST>(NumVal);
  getNextToken(); // consume the number
  return std::move(Result);
}
 ```
 * `static` I'm actually not sure what static means in this context considering it has (I think) a few meanings in C++...
 * Apparently this parsing routine "_expects to be called when the current token is a tok_number token_" and "_takes current number value, creates a NumberExprAst node, advances the lexer to the next token_" and then returns it.
 * Seems premature but that little bit of code is all I have energy for today, we'll get to the end eventually...right? Lol

---

2019/12/11
 * I didn't gain a lot from sleep. Still don't fully understand this stuff. But I might as well get a little more done today.
 * So it turns out breaking down each little grouping of the code is called a subexpression. This `ParseParenExpr()` will obviously expect a trailing ')' if a '(' pops up. Apparently we just use the `LogError` function to pass a lil' message to the user...which makes sense! _That_ makes *sense*!
 * So the reason the `ParseParenExpr()` has a so-far-undefined `ParseExpression()` is that this `ParseExpression()` can actually called the `ParseParenExpr()`, *RECURSION*! This helps "handle recursive grammars"...more info tbd. Also, this function doesn't create an AST node, as per the absence of something like `auto blah = std::make_unique<WhateverExprAST>(whateva)`. You _can_ have it make a node but the tutorial says "_the most important role of (parens) are to guide the parser and provide grouping. Once the parser constructs the AST, (parens) are not needed_". Makes sense in this context.That last part of the sentence for whatever reason made me visualize this tree structure that they've been talking about much better. Idk why tho.
 * I _really_ better brush up on recursion. Funny that I literally directly mention recursion in the first entry of this whole project but the word recursion went in one ear and out the other.
 * `ParseIdentifierpr()` uses something called a "_look-ahead_" and checks if "_if the current identifier is a stand alone variable...or if function call_" by checking the token after the '(', making `VariableExprAST` or `CallExprAST` as apropos. Now how exactly it's doing that in the code is left as an exercise for the reader- just kidding. I believe it's from the `ParseExpression()` call in this if statement:
 ```
 if (auto Arg = ParseExpression())
                Args.push_back(std::move(Arg));
 ```
 * I think the `.push_back` just adds an element to the end of the list/vector/group-o'-numbers. So `.push_back` is putting the result(?) of `std::move(Arg)`, which returns an "_iterator to the initial position in the destination sequence_". A little over my head but...seems interesting and cool!
 * Up to this in the parsing section is the basic expression-parsing logic. And its class is considered "*primary*" expressions.
 * Next is wrapping up the *primary* expressions...but that'll be for tomorrow :>

---

2019/12/13
 * Last part (I think) of the primary expressions is this `ParsePrimary()` function. Don't laugh but I'm just a little confused as to why the `ParsePrimary()` needs to be declared as a `std::unique_ptr<ExprAST>`? I'm throwing out a really uninformed guess here but is it because, and yes I did read back to the beginning of the chapter to remember, we want "_one object for each construct in the language_"? Cause declaring it as that `unique_ptr` thing makes sense then. Hi future self, let me know if that's right when you get smarter.
 * The LLVM masters are right, it _does_ make more sense that all these primary expression functions have CurTok as a known thing...look-aheads lookin' ahead.
 * Time for binary expressions! What does binary expression mean? Something like `x+y*z` as an example I guess (it's the + and \*).
 * Apparently we handle this specific binary expression stuff using the [Operator-Precedence Parsing](http://en.wikipedia.org/wiki/Operator-precedence_parser) which I have mentioned before if I may add.
 * "Don't you remember?!" I ask the air next to me. "It's a bottom-up parser, it interprets operator-precedence grammar DUH!" Righttt, like a calculator.
 * So now we got this `std::map<char, int> BinopPrecedence` (which is a "sorted associative container"_rolling eyes emoji_ which "contains key-value pairs" w/ "unique keys") to basically hold the `BinopPrecedence`s we're 'installing' in the `int main()`. Then we're using `GetTokPrecedence()`...which will then presumably be used later by something else which we will find out!
 * "_Having a map makes it easy to add new operators_" from their mouth to your ear-eyes.
 * Hmmm so this is interesting, "_Note that because parentheses are primary expressions, the binary expression parser doesn’t need to worry about nested subexpressions like (c+d) at all_" in regards to `a+b+(c+d)*e*f+g` specifically. Oh boy this is gonna take me a week to understand this one little part...

---

2019/12/14
 * `ParseBinOpRHS(int ExprPrec...)` parses this -> \[binop, primaryexpr], so a "sequence of pairs" essentially. The `primaryexpr` is an expression's pointer for what's already parsed. If the binoprhs is empty it'll just return the expression parameter passed to it. The "precedence value passed into `ParseBinOpRHS` indicates the _minimal operator precedence_ that the function is allowed to \[consume]". Don't ask me what all this really means, I'm figuring it out as I go lol.
 * As an example for a "pair stream" we can take \[+,x] '+' being the token and 'x' being the expression, and if the precedence: `BinopPrecedence['*'] = 40;` (as an example) is passed...it won't consume any tokens! The token of '+' is 20! Wow! idk!
 * The `ParseBinOpRHS` stuff gets precedence of current token, check if too low, because we have invalid tokens' precedence as -1 this check knows the "_pair-stream_" ends when the "_token stream runs out of binary operators_". If the check is :greencheckmark: then we *know* the token is a binary operator and then we write this code:
 ```
 // we know this is a binop
int BinOp = CurTok;
getNextToken(); // nom binop
// Parse primary expression after binary operator.
auto RHS = ParsePrimary();
if (!RHS)
  return nullptr;
 ```
 * This chunk-o-code here noms and remembers the binop & then parses the primary expr that follows that binop. This "builds up the whole pair" which from the example we're drawing from is `a+b+(c+d)*e*f+g`, "which is \[+,b]".
 * Next part is explaining how we parsed the lefthand side of the expression and "_one pair of the RHS sequence..._" but I don't feel like continuing right now, so, that's that. I'm gonna go read a book.

---

2019/12/15
 * This while loop that is dealing with the LHS and RHS of the expressions is difficult for me to understand but it seems this loop goes through the entire expression, binding together expressions and somehow knowing how to deal with the order of it all...it's honestly over my head a bit right now but at least I've been exposed to it.
 * The next parts deal with handling function prototypes and then the driver function for all of what we've (they've really lmao) written so far. But today's a short day. :peaceemoji:
 
 ---
 
 2019/12/17
  * Wrote the function prototype parsing code. Which literally just checks the tokens given in case there's odd stuff then `LogErrorP`s out. After it assigns the function name given (I think) and then just pushes the arguments into a string vector and returns the pointer to the `PrototypeAST` object with the argument names with `std::move()` which again produces an 'rvalue-reference' to an object...what rvalue is can be [explained](http://thbecker.net/articles/rvalue_references/section_01.html) from this Thomas Becker fellow.
  * Next we're writing the definition object for the function which inside of declares `auto Proto = ParsePrototype()`, with that function being the last thing we just wrote up above over yonder. And then it just makes sure that `ParseExpression()` works (which calls from `ParsePrimary()` and so on), checks with an `if` statement and if it's allll :okhand: then it just simply returns a unique pointer to this new(?) object with `std::move(Proto), std::move(E))` as arguments. `E` is just `ParseExpression()`'s returned value btw.
