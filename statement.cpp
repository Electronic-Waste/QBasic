#include "statement.h"
#include <string>
#include <stack>

using namespace std;

//虚函数，无实际意义
Statement::Statement()
{

}

//虚函数，无实际意义
Expression* Statement::getExp()
{
    return NULL;
}

//虚函数，无实际意义
Expression* Statement::getExp1()
{
    return NULL;
}

//虚函数，无实际意义
Expression* Statement::getExp2()
{
    return NULL;
}

//虚函数，无实际意义
string Statement::getOp()
{
    return "";
}

//虚函数，无实际意义
int Statement::getLineNum()
{
    return 0;
}

//虚函数，无实际意义
LetStmt::LetStmt(Expression *exp)
{
    letExp = exp;
}

//虚函数，无实际意义
Expression* LetStmt::getExp()
{
    return letExp;
}

//虚函数，无实际意义
PrintStmt::PrintStmt(Expression *exp)
{
    printExp = exp;
}

//虚函数，无实际意义
Expression* PrintStmt::getExp()
{
    return printExp;
}

//函数名：InputStmt
//作用：接受表达式exp,将其存入类中，构造函数
InputStmt::InputStmt(Expression *exp)
{
    inputExp = exp;
}

//函数名：getExp
//作用：返回表达式的地址
Expression* InputStmt::getExp()
{
    return inputExp;
}

//函数名：GotoStmt
//作用：接受要跳转的行号num为参数，将其存入类中，构造函数
GotoStmt::GotoStmt(int num)
{
    goLineNum = num;
}

//函数名:getLineNum
//作用：返回要跳转至的行号
int GotoStmt::getLineNum()
{
    return goLineNum;
}

//函数名：IfStmt
//作用：接受左右表达式exp1,exp2和要跳转至的行号num为参数，构造函数
IfStmt::IfStmt(Expression *exp1, Expression *exp2, std::string Op, int num)
{
    lExp = exp1;
    rExp = exp2;
    op = Op;
    goLineNum = num;
}

//函数名：getExp1
//作用：返回左表达式的地址
Expression* IfStmt::getExp1()
{
    return lExp;
}

//函数名：getExp2
//作用：返回右表达式的地址
Expression* IfStmt::getExp2()
{
    return rExp;
}

//函数名：getOp
//作用：返回运算符
string IfStmt::getOp()
{
    return op;
}

//函数名：getLineNum
//作用：返回要跳转至的行号
int IfStmt::getLineNum()
{
    return goLineNum;
}

//函数名：type
//作用：返回Statement的类型
StatementType RemStmt::type()
{
    return REMSTMT;
}

//函数名：type
//作用：返回Statement的类型
StatementType LetStmt::type()
{
    return LETSTMT;
}

//函数名：type
//作用：返回Statement的类型
StatementType PrintStmt::type()
{
    return PRINTSTMT;
}

//函数名：type
//作用：返回Statement的类型
StatementType InputStmt::type()
{
    return INPUTSTMT;
}

//函数名：type
//作用：返回Statement的类型
StatementType GotoStmt::type()
{
    return GOTOSTMT;
}

//函数名：type
//作用：返回Statement的类型
StatementType IfStmt::type()
{
    return IFSTMT;
}

//函数名：type
//作用：返回Statement的类型
StatementType EndStmt::type()
{
    return ENDSTMT;
}

//函数名：type
//作用：返回Statement的类型
StatementType ErrorStmt::type()
{
    return ERRORSTMT;
}

//函数名：appendExpStr
//作用：接受所在表达式树层数startLevel,根表达式e和需要返回的字符串retStr为参数
//     将表达式转化为字符串，添加至retStr中
void appendExpStr(int startLevel, Expression *e, string &retStr) {
    struct StNode{
        Expression *exp;
        int level;
        StNode(int l, Expression * e):level(l), exp(e){}
    };

    stack<StNode> expStack;
    StNode root(startLevel, e);
    expStack.push(root);    //根节点进栈
    //前序遍历，依次输出由Expression转化来的字符串
    while (!expStack.empty()) {
        StNode tmp = expStack.top();
        expStack.pop();
        //第i层前面需要有4i个空格
        for (int i = 0; i < tmp.level; ++i)
            retStr += "    ";
        retStr += tmp.exp->toString() + "\n";
        //右节点进栈
        if (tmp.exp->getRHS() != NULL)
            expStack.push(StNode(tmp.level + 1, tmp.exp->getRHS()));
        //左节点进栈
        if (tmp.exp->getLHS() != NULL)
            expStack.push(StNode(tmp.level + 1, tmp.exp->getLHS()));
    }
}

//函数名：toString
//作用：接收行号num和代码行内容str为参数，将statement转化为string返回
string RemStmt::toString(int lineNum, string str)
{
    string retStr = "";
    retStr += to_string(lineNum) + " REM\n";
    str = str.substr(4, str.length() - 4);
    retStr += "    " + str + "\n";
    return retStr;
}

//函数名：toString
//作用：接收行号num和代码行内容str为参数，将statement转化为string返回
string LetStmt::toString(int lineNum, string str)
{
    string retStr = "";
    //错误处理：exp表达式错误时
    if (letExp->type() == ERROR)
        retStr = to_string(lineNum) + " Error\n";
    //正常执行
    else {
        retStr = to_string(lineNum) + " LET ";
        appendExpStr(0, letExp, retStr);
    }
    return retStr;
}

//函数名：toString
//作用：接收行号num和代码行内容str为参数，将statement转化为string返回
string PrintStmt::toString(int lineNum, string str)
{
    string retStr = "";
    //错误处理：exp表达式错误时
    if (printExp->type() == ERROR)
        retStr = to_string(lineNum) + " Error\n";
    //正常执行
    else {
        retStr = to_string(lineNum) + " PRINT\n";
        appendExpStr(1, printExp, retStr);
    }
    return retStr;
}

//函数名：toString
//作用：接收行号num和代码行内容str为参数，将statement转化为string返回
string InputStmt::toString(int lineNum, string str)
{
    string retStr = "";
    //错误处理：exp表达式错误时
    if (inputExp->type() == ERROR)
        retStr = to_string(lineNum) + " Error\n";
    //正常执行
    else {
        retStr = to_string(lineNum) + " INPUT\n";
        retStr += "    " + inputExp->toString() + "\n";
    }
    return retStr;
}

//函数名：toString
//作用：接收行号num和代码行内容str为参数，将statement转化为string返回
string GotoStmt::toString(int lineNum, string str)
{
    string retStr = "";
    retStr += to_string(lineNum) + " GOTO\n";
    retStr += "    " + to_string(goLineNum) + "\n";
    return retStr;
}

//函数名：toString
//作用：接收行号num和代码行内容str为参数，将statement转化为string返回
string IfStmt::toString(int lineNum, string str)
{
    string retStr = "";
    //错误处理：exp表达式错误时
    if (lExp->type() == ERROR || rExp->type() == ERROR
            || op != "=" && op != "<" && op != ">")
        retStr = to_string(lineNum) + " Error\n";
    //正常执行
    else {
        retStr += to_string(lineNum) + " IF THEN\n";
        appendExpStr(1, lExp, retStr);
        retStr += "    " + op + "\n";
        appendExpStr(1, rExp, retStr);
        retStr += "    " + to_string(goLineNum) + "\n";
    }
    return retStr;
}

//函数名：toString
//作用：接收行号num和代码行内容str为参数，将statement转化为string返回
string EndStmt::toString(int lineNum, string str)
{
    string retStr = "";
    retStr += to_string(lineNum) + " END\n";
    return retStr;
}

//函数名：toString
//作用：接收行号num和代码行内容str为参数，将statement转化为string返回
string ErrorStmt::toString(int lineNum, string str)
{
    string retStr = "";
    retStr += to_string(lineNum) + " Error\n";
    return retStr;
}
