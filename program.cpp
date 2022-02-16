 #include "program.h"
#include "statement.h"

using namespace std;

//函数名：washQStr
//作用：接收QString类型字符串，去掉前后空白元素后以string类型返回
string washQStr(QString qStr){
    string str = qStr.toStdString();
    int beginPos = 0;
    int endPos = 0;
    bool isEmpty = true;
    for (int i = 0; i < str.length(); ++i) {
        //寻找第一个不是空白元素的位置
        if (str[i] != ' ' && str[i] != '\n'
                && str[i] != '\t' && str[i] != '\r'
                && str[i] != '\0' && isEmpty) {
            beginPos = i;
            isEmpty = false;
        }
        //寻找结尾位置
        if ((str[i] == '\r' || str[i] == '\n') && !endPos) endPos = i - 1;
    }

    if (isEmpty) return "";
    else if (!endPos) return str.substr(beginPos, str.length() - beginPos + 1);
    else return str.substr(beginPos, endPos - beginPos + 1);
}

//函数名：roughLine
//作用：接受QString类参数qStr,构造函数
roughLine::roughLine(QString qStr)
{
    string str = washQStr(qStr);

    //若为空白则将内容设置为空字符串返回
    if (str == "") {
        content = "";
        return;
    }
    rawContent = str;

    //判断该行代码的类型
    bool isTypeD = true;
    bool isTypeC = true;
    for (int i = 0; i < str.length(); ++i) {
        if (str[i] == ' ') continue;
        if (str[i] >= '0' && str[i] <= '9') isTypeC = false;
        if (str[i] >= 'a' && str[i] <= 'z'
                || str[i] >= 'A' && str[i] <= 'Z') isTypeD = false;
    }

    if (isTypeC) type = COMMAND;
    else if (isTypeD) type = DELETE;
    else type = NORMAL;

    //COMMAND类型，命令行语句
    if (type == COMMAND) {
        lineNum = 0;
        for (int i = 0; i < str.length(); ++i)
            str[i] = toupper(str[i]);
        content = str;
    }
    //DELETE类型，删除行号
    else if (type == DELETE) {
        lineNum = stoi(str);
        content = " ";
    }
    //NORMAL类型
    else {
        //从字符串中获得行号和内容
        string numString, contentString;        
        for (int i = 0; i < str.length(); ++i) {
            if (str[i] == ' ') {
                str[i] = '\0';
                numString = str.substr(0, i + 1);
                contentString = str.substr(i + 1, str.length() - i + 1);
                break;
            }
        }

        lineNum = stoi(numString);
        content = contentString;
    }

}

//函数名：roughLine
//作用：接受int类型的num和string类型的内容c为参数，构造函数
roughLine::roughLine(int num, string c)
{
    lineNum = num;
    content = c;
}

//函数名：LinkLine
//作用：LinkLine类的构造函数
LinkLine::LinkLine():currentLength(0)
{
    //将head的行号值设为0（优先级最高）
    roughLine r(0, " ");
    head = new node(r);
}

//函数名：~LinkLine
//作用：LinkLine类的析构函数
LinkLine::~LinkLine()
{
    clear();
    delete head;
}

//函数名:move
//作用：接受行号num为参数，返回行号为num的代码行所在地址（若不存在，则返回空指针）
node* LinkLine::move(int num) const
{
    node* p = head;

    while (p != NULL) {
        if (p->rLine.lineNum == num) return p;
        else p = p->next;
    }

    return NULL;
}

//函数名：insert
//作用：接受roughLine类型参数，将代码插入存储链表中
void LinkLine::insert(roughLine r)
{
    node *iNode = new node(r);
    node *p = head;

    while (p != NULL) {
        //若后面没有节点，则将新建节点放在最后
        if (p->next == NULL) {
            p->next = iNode;
            break;
        }
        //如果行号值大于当前节点和当前节点下一个的行号值，则当前节点向后移动一位
        else if(iNode->rLine.lineNum > p->rLine.lineNum
                && iNode->rLine.lineNum > p->next->rLine.lineNum) p = p->next;
        //否则在当前节点后面插入该节点（或替代该节点）
        else {
            //如果行号冲突,替代节点
            if (iNode->rLine.lineNum == p->next->rLine.lineNum)
                remove(p->next->rLine.lineNum);
            iNode->next = p->next;
            p->next = iNode;
            break;
        }
    }

    ++currentLength;
}

//函数名:remove
//作用：接受行号num为参数，删除行号为num的代码
void LinkLine::remove(int num)
{
    node *targetNode = move(num);

    if (targetNode == NULL) return;

    node *p = head;
    while (p->next != targetNode) p = p->next;
    p->next = targetNode->next;
    delete targetNode;
    --currentLength;

}

//函数名：clear
//作用：清除存储的代码
void LinkLine::clear()
{
    node* p = head->next, *q;
    head->next = NULL;
    while (p != NULL) {
        q = p->next;
        delete p;
        p = q;
    }
    currentLength = 0;
}

//函数名：search
//作用：接受行号num为参数，判断行号为num的代码行是否存在，返回bool值
bool LinkLine::search(int num)
{
    node* p = move(num);
    if (p == NULL) return false;
    else return true;
}

//函数名：returnNode
//作用:接受行号num为参数，返回行号为num的代码行所在地址（若不存在，则返回空指针）
node* LinkLine::returnNode(int num)
{
    return move(num);
}

//函数名：returnRawString
//作用：以string形式返回源代码
string LinkLine::returnRawString()
{
    node *p = head->next;
    string str = "";

    while (p != NULL) {
        str += p->rLine.rawContent + "\n";
        p = p->next;
    }

    return str;
}

//函数名：returnTree
//作用：以string形式返回表达式树
string LinkLine::returnTree()
{
    string retStr = "";
    node *p = head->next;
    while (p != NULL) {
        retStr += p->stmt->toString(p->rLine.lineNum, p->rLine.content);
        p = p->next;
    }

    return retStr;
}





