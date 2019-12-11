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
