#include "exp.h"
#include <iostream>
#include <cmath>

using namespace std;

//虚函数，无实际意义
Expression::Expression()
{

}

//虚函数，无实际意义
Expression::~Expression()
{

}

//虚函数，无实际意义
int Expression::getConstantValue()
{

}

//虚函数，无实际意义
string Expression::getOperator()
{

}

//虚函数，无实际意义
string Expression::getIdentifierName()
{

}

//虚函数，无实际意义
Expression* Expression::getLHS()
{
    return NULL;
}

//虚函数，无实际意义
Expression* Expression::getRHS()
{
    return NULL;
}

//函数名:ConstantExp
//接收常量值val为参数，构造函数
ConstantExp::ConstantExp(int val)
{
    value = val;
}

//函数名:~ConstantExp
//作用：析构函数
ConstantExp::~ConstantExp()
{

}

//函数名：toString
//作用：将表达式内容以string形式返回，ConstantExp返回常量值
string ConstantExp::toString()
{
    return to_string(value);
}

//函数名:type
//作用：返回表达式类型
ExpressionType ConstantExp::type()
{
    return CONSTANT;
}

//函数名:getConstantValue
//作用：返回ConstantExp中存储的常量值
int ConstantExp::getConstantValue()
{
    return value;
}

//函数名:IdentifierExp
//作用：接收string类型str为参数，构造函数
IdentifierExp::IdentifierExp(string str)
{
    name = str;
}

//函数名：~IdentifierExp
//作用：析构函数
IdentifierExp::~IdentifierExp()
{

}

//函数名:type
//作用:返回表达式类型
ExpressionType IdentifierExp::type()
{
    return IDENTIFIER;
}

//函数名:toString
//作用:将表达式内容以string形式返回，identifierExp返回变量名
string IdentifierExp::toString()
{
    return name;
}

//函数名:getIdentifierName
//作用：返回表达式中变量的名称
string IdentifierExp::getIdentifierName()
{
    return name;
}

//函数名:CompoundExp
//作用:接收运算符、左表达式指针、右表达式指针为参数构造函数，初始化变量
CompoundExp::CompoundExp(string Op, Expression *Lhs, Expression *Rhs)
{
    op = Op;
    lhs = Lhs;
    rhs = Rhs;
}

//函数名:~CompoundExp
//作用:析构函数
CompoundExp::~CompoundExp()
{
    delete lhs;
    delete rhs;
}

//函数名:type
//作用:返回表达式类型
ExpressionType CompoundExp::type()
{
    if (lhs->type() != ERROR && lhs->type() != ERROR)
        return COMPOUND;
    else
        return ERROR;
}

//函数名:toString
//作用:将表达式内容以string形式返回，CompoundExp返回运算符
string CompoundExp::toString()
{
    return op;
}

//函数名：getOperator
//作用：返回运算符
string CompoundExp::getOperator()
{
    return op;
}

//函数名：getLHS
//作用：返回左表达式
Expression* CompoundExp::getLHS()
{
    return lhs;
}

//函数名：getRHS
//作用：返回右表达式
Expression* CompoundExp::getRHS()
{
    return rhs;
}

//函数名：eval
//作用：接收"变量-值"对应表context类为参数，计算CONSTANT类型表达式的值
int ConstantExp::eval(EvaluationContext &context)
{
    return value;
}

//函数名：eval
//作用：接收"变量-值"对应表context类为参数，计算IDENTIFIER类型表达式的值
int IdentifierExp::eval(EvaluationContext &context)
{
    if (!context.isDefined(name)) throw(name + "is undefined");
    return context.getValue(name);
}

//函数名：eval
//作用：接收"变量-值"对应表context类为参数，计算COMPOUND类型表达式的值
int CompoundExp::eval(EvaluationContext &context)
{
    int right = rhs->eval(context);
    if (op == "=") {
        context.setValue(lhs->getIdentifierName(),right);
        return right;
    }

    int left = lhs->eval(context);
    if (op == "+") return left + right;
    if (op == "-") return left - right;
    if (op == "*") return left * right;
    if (op == "**") return pow(left, right);
    if (op == "/") {
        if (right == 0) throw("Division by 0");
        return left / right;
    }

    throw ("Illegal operator in expression");
    return 0;
}

//函数名:setValue
//作用:接收变量名var和整型常量value为参数，设置变量的值
void EvaluationContext::setValue(std::string var, int value)
{
    //如果已经定义，则擦除该元素
    if (isDefined(var))
        symbolTable.erase(var);
    //插入新的对应元素
    symbolTable.insert(pair <string, int> {var, value});
}

//函数名：getValue
//作用:接收变量名var为参数，返回变量的值
int EvaluationContext::getValue(std::string var)
{
    map<string, int>::iterator ret_it = symbolTable.find(var);
    return ret_it->second;
}

//函数名: isDefined
//作用:接收变量名var为参数，判断该变量是否在context中存在，返回bool值
bool EvaluationContext::isDefined(std::string var)
{
    map<string, int>::iterator ret_it = symbolTable.find(var);
    if (ret_it == symbolTable.end()) return false;
    else return true;

}

//函数名：clearTable
//作用：清除symbolTable中所有的对应关系
void EvaluationContext::clearTable()
{
    symbolTable.clear();
}
