#ifndef PROGRAM_H
#define PROGRAM_H

#include <string>
#include <QString>
#include "exp.h"
#include "statement.h"
#include <tokenizer.h>
#include <QMainWindow>

using namespace std;

enum lineType{NORMAL,DELETE,COMMAND};

struct roughLine {
    lineType type;
    int lineNum;
    string content;
    string rawContent;

    roughLine(QString qStr);
    roughLine(int num, string c);
    roughLine(){}
    ~roughLine(){}
};


struct node {
    roughLine rLine;
    node *next;
    Statement *stmt;
    node(const roughLine &r, node *p = NULL) {
        rLine = r;
        next = p;
        Tokenizer tk;
        stmt = tk.getStmt(rLine.content);
    }
    node():next(NULL){};
    ~node(){}
};

class LinkLine {
private:
    node *head;
    int currentLength;

    node *move(int num) const;  //返回行号为num的节点位置

public:
    LinkLine();
    ~LinkLine();
    void insert(roughLine r);
    void remove(int num);   //删除行号为num的指令
    void clear();
    int length(){return currentLength;}
    bool search(int num);
    node *returnHead(){return head;}
    node *returnNode(int num);
    string returnRawString();
    string returnTree();

};


#endif // PROGRAM_H
