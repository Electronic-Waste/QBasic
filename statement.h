#ifndef STATEMENT_H
#define STATEMENT_H

#include "exp.h"

enum StatementType{REMSTMT, LETSTMT, PRINTSTMT, INPUTSTMT, GOTOSTMT, IFSTMT, ENDSTMT, ERRORSTMT};

class Statement
{
public:
    Statement();
    virtual StatementType type() = 0;
    virtual std::string toString(int lineNum, std::string str) = 0;
    virtual Expression *getExp();
    virtual Expression *getExp1();
    virtual Expression *getExp2();
    virtual std::string getOp();
    virtual int getLineNum();

};

class RemStmt: public Statement {

public:
    virtual StatementType type();
    virtual std::string toString(int lineNum, std::string str);

};

class LetStmt: public Statement {

public:
    LetStmt(Expression *exp);
    virtual StatementType type();
    virtual std::string toString(int lineNum, std::string str);
    virtual Expression *getExp();

private:
    Expression *letExp;
};

class PrintStmt: public Statement {

public:
    PrintStmt(Expression *exp);
    virtual StatementType type();
    virtual std::string toString(int lineNum, std::string str);
    virtual Expression *getExp();

private:
    Expression *printExp;
};

class InputStmt: public Statement {

public:
    InputStmt(Expression *exp);
    virtual StatementType type();
    virtual std::string toString(int lineNum, std::string str);
    virtual Expression *getExp();

private:
    Expression *inputExp;
};

class GotoStmt: public Statement {

public:
    GotoStmt(int num);
    virtual StatementType type();
    virtual std::string toString(int lineNum, std::string str);
    virtual int getLineNum();

private:
    int goLineNum;
};

class IfStmt: public Statement {

public:
    IfStmt(Expression *exp1, Expression *exp2, std::string Op, int num);
    virtual StatementType type();
    virtual std::string toString(int lineNum, std::string str);
    virtual Expression *getExp1();
    virtual Expression *getExp2();
    virtual std::string getOp();
    virtual int getLineNum();

private:
    Expression *lExp;
    Expression *rExp;
    std::string op;
    int goLineNum;
};

class EndStmt: public Statement {

public:
    virtual StatementType type();
    virtual std::string toString(int lineNum, std::string str);
};

class ErrorStmt: public Statement {

public:
    virtual StatementType type();
    virtual std::string toString(int lineNum, std::string str);
};

#endif // STATEMENT_H
