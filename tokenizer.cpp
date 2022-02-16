#include "tokenizer.h"
#include <exp.h>

using namespace std;

//函数名：getExp
//作用：接收Expression的string形式str为参数,将其转化为Expression树,并返回根节点地址
Expression* Tokenizer::getExp(string str)
{
    int pre = 0, pos = 0;
    ExpressionType formerType = COMPOUND;   //记录前一个入栈的元素的类型(用于区别负号和减号）

    while (str[pos] != '\0') {
        //跳过空格
        while (str[pos] == ' ') ++pos;
        pre = pos;

        //类型为IDENTIFIER
        if (str[pos] == '_' || str[pos] >= 'a' && str[pos] <= 'z'
                || str[pos] >= 'A' && str[pos] <= 'Z') {
            //快进到非变量位
            while (str[pos] == '_' || str[pos] >= 'a' && str[pos] <= 'z'
                   || str[pos] >= 'A' && str[pos] <= 'Z'
                   || str[pos] >= '0' && str[pos] <= '9') ++pos;
            //获取变量名并生成IdentifierExp
            string identifierName = str.substr(pre, pos - pre);
            formerType = IDENTIFIER;
            IdentifierExp *stmt = new IdentifierExp(identifierName);
            expStack.push((Expression *) stmt);
        }

        //类型为CONSTANT
        else if (str[pos] >= '0' && str[pos] <= '9') {
            //快进到非数字位置
            while (str[pos] >= '0' && str[pos] <= '9') ++pos;
            //获取常量值并生成ConstantExp
            string constant = str.substr(pre, pos - pre);
            formerType = CONSTANT;
            ConstantExp *stmt = new ConstantExp(stoi(constant));
            expStack.push((Expression *) stmt);
        }

        //类型为op,读到'(',进栈
        else if (str[pos] == '(') {
            formerType = COMPOUND;
            opStack.push("(");
            ++pos;
        }
        //类型为op,读到')',开闭括号之间所有运算均可执行
        else if (str[pos] == ')') {
            while (opStack.top() != "(") {
                Expression *exp2 = expStack.top();
                expStack.pop();
                Expression *exp1 = expStack.top();
                expStack.pop();
                string operand = opStack.top();
                opStack.pop();
                CompoundExp *comStmt = new CompoundExp(operand, exp1, exp2);
                expStack.push((Expression *) comStmt);
            }
            formerType = COMPOUND;
            opStack.pop();
            ++pos;
        }
        //类型为op,读到了"=",进栈
        else if (str[pos] == '=') {
            formerType = COMPOUND;
            opStack.push("=");
            ++pos;
        }
        //类型为op,读到了"**",进栈
        else if (str.substr(pos, 2) == "**") {
            formerType = COMPOUND;
            opStack.push("**");
            pos += 2;
        }
        //类型为op,读到了'*'或'/',栈中的'*''/''**'退栈执行
        else if (str[pos] == '*' || str[pos] == '/') {
            while (!opStack.empty() &&
                   (opStack.top() == "*" || opStack.top() == "/" || opStack.top() == "**")) {
                Expression *exp2 = expStack.top();
                expStack.pop();
                Expression *exp1 = expStack.top();
                expStack.pop();
                string operand = opStack.top();
                opStack.pop();
                CompoundExp *comStmt = new CompoundExp(operand, exp1, exp2);
                expStack.push((Expression *) comStmt);
            }
            formerType = COMPOUND;
            opStack.push(str.substr(pos, 1));
            ++pos;
        }
        //类型为op,读到了'+''-',栈中运算符依次出栈，直至遇到'=''('或为空
        //特殊情况处理：负数
        else if (str[pos] == '+' || str[pos] == '-') {
            //特殊情况处理：负数
            if (str[pos] == '-' && formerType == COMPOUND) {
                ++pos;  //跳至空格位(或数字位）
                while (str[pos] == ' ') ++pos;
                pre = pos;      //最前端对准数字位
                while (str[pos] >= '0' && str[pos] <= '9') ++pos;
                string constant = str.substr(pre, pos - pre);
                formerType = CONSTANT;
                ConstantExp *stmt = new ConstantExp(-stoi(constant));
                expStack.push((Expression *) stmt);
                continue;    //开始新一轮循环
            }
            //读到了'+''-',栈中运算符依次出栈，直至遇到'=''('或为空
            while (!opStack.empty() && opStack.top() != "=" && opStack.top() != "(") {
                Expression *exp2 = expStack.top();
                expStack.pop();
                Expression *exp1 = expStack.top();
                expStack.pop();
                string operand = opStack.top();
                opStack.pop();
                CompoundExp *comStmt = new CompoundExp(operand, exp1, exp2);
                expStack.push((Expression *) comStmt);
            }
            formerType = COMPOUND;
            opStack.push(str.substr(pos, 1));
            ++pos;
        }
    }
    //清空运算符栈
    while (!opStack.empty()) {
        Expression *exp2 = expStack.top();
        expStack.pop();
        Expression *exp1 = expStack.top();
        expStack.pop();
        string operand = opStack.top();
        opStack.pop();
        CompoundExp *comStmt = new CompoundExp(operand, exp1, exp2);
        expStack.push((Expression *) comStmt);
    }

    //返回表达式栈栈顶元素
    Expression *retExp = expStack.top();
    expStack.pop();
    return retExp;
}

//函数名：getStmt
//作用：接收Statement的string形式为参数，将其转化为Statement,并返回其地址
Statement* Tokenizer::getStmt(string str)
{
    //生成REMSTMT类型Statement
    if (getStmtType(str) == REMSTMT) {
        RemStmt *rem = new RemStmt;
        return (Statement *) rem;
    }
    //生成LETSTMT类型Statement
    else if (getStmtType(str) == LETSTMT) {
        Expression* letExp = getExp(str.substr(3, str.length() - 3));
        LetStmt *let = new LetStmt(letExp);
        return (Statement *) let;
    }
    //PRINTSTMT类型Statement
    else if (getStmtType(str) == PRINTSTMT) {
        Expression* printExp = getExp(str.substr(5, str.length() - 5));
        PrintStmt *print = new PrintStmt(printExp);
        return (Statement *) print;
    }
    //生成INPUTSTMT类型Statement
    else if (getStmtType(str) == INPUTSTMT) {
        Expression* inputExp = getExp(str.substr(5, str.length() - 5));
        InputStmt *input = new InputStmt(inputExp);
        return (Statement *) input;
    }
    //生成GOTOSTMT类型Statement
    else if (getStmtType(str) == GOTOSTMT) {
        int num = stoi(str.substr(4, str.length() - 4));
        GotoStmt *go = new GotoStmt(num);
        return (Statement *) go;

    }
    //生成IFSTMT类型Statement
    else if (getStmtType(str) == IFSTMT) {
        int mid = 0, pos = 0;
        for (int i = 0; i < str.length(); ++i) {
            if (str[i] == '=' || str[i] == '>' || str[i] == '<') mid = i;
            if (str.substr(i, 4) == "THEN") pos = i;
        }
        string op(1, str[mid]);
        Expression* exp1 = getExp(str.substr(2, mid - 2));
        Expression* exp2 = getExp(str.substr(mid + 1, pos - mid - 1));
        int num = stoi(str.substr(pos + 4, str.length() - pos - 4));
        IfStmt *stmt = new IfStmt(exp1, exp2, op, num);
        return (Statement *) stmt;
    }
    //生成ENDSTMT类型Statement
    else if (getStmtType(str) == ENDSTMT) {
        EndStmt *end = new EndStmt;
        return (Statement *) end;
    }
    //否则生成ERRORSTMT类型的Statement
    else {
        ErrorStmt *err = new ErrorStmt;
        return (Statement *) err;
    }
}

//函数名：getStmtType
//作用：接收String形式的Statement为参数，返回该Statement的类型
StatementType Tokenizer::getStmtType(std::string str)
{
    if (str.substr(0, 3) == "REM") return REMSTMT;
    else if (str.substr(0, 3) == "LET") return LETSTMT;
    else if (str.substr(0, 5) == "PRINT") return PRINTSTMT;
    else if (str.substr(0, 5) == "INPUT") return INPUTSTMT;
    else if (str.substr(0, 4) == "GOTO") return GOTOSTMT;
    else if (str.substr(0, 2) == "IF") return IFSTMT;
    else if (str.substr(0, 3) == "END") return ENDSTMT;
    else return ERRORSTMT;
}
