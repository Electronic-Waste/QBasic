   #ifndef EXP_H
#define EXP_H

#include <string>
#include <map>

class EvaluationContext;

enum ExpressionType {CONSTANT,IDENTIFIER,COMPOUND,ERROR};

class Expression {

public:
    Expression();
    virtual ~Expression();
    virtual int eval(EvaluationContext & context) = 0;
    virtual std::string toString() = 0;
    virtual ExpressionType type() = 0;

    virtual int getConstantValue();
    virtual std::string getIdentifierName();
    virtual std::string getOperator();
    virtual Expression *getLHS();
    virtual Expression *getRHS();

};

class ConstantExp: public Expression {

public:
    ConstantExp(int val);
    virtual ~ConstantExp();
    virtual int eval(EvaluationContext & context);
    virtual std::string toString();
    virtual ExpressionType type();
    virtual int getConstantValue();

private:
    int value;
};

class IdentifierExp: public Expression{

public:
    IdentifierExp(std::string name);
    virtual ~IdentifierExp();
    virtual int eval(EvaluationContext & context);
    virtual std::string toString();
    virtual ExpressionType type();
    virtual std::string getIdentifierName();

private:
    std::string name;

};

class CompoundExp: public Expression {

public:
    CompoundExp(std::string Op, Expression *Lhs, Expression *Rhs);
    virtual ~CompoundExp();
    virtual int eval(EvaluationContext & context);
    virtual std::string toString();
    virtual ExpressionType type();
    virtual std::string getOperator();
    virtual Expression *getLHS();
    virtual Expression *getRHS();

private:
    std::string op;
    Expression *lhs, *rhs;
};

class EvaluationContext {

public:
    void setValue(std::string var, int value);
    int getValue(std::string var);
    bool isDefined(std::string var);
    void clearTable();

private:
    std::map<std::string,int> symbolTable;
};

#endif // EXP_H
