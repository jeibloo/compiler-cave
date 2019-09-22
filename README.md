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
  * AST: the parser's child and the father of my nightmares tonight. I really tried...I really tried to wrap my mind around the very basics of how this _thing_ works. 
