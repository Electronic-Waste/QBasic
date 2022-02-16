#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <statement.h>
#include <exp.h>
#include <stack>

class Tokenizer
{
public:
    Expression *getExp(std::string str);
    Statement *getStmt(std::string str);
    StatementType getStmtType(std::string str);

private:
    std::stack<Expression *> expStack;
    std::stack<std::string> opStack;
};

#endif // TOKENIZER_H
