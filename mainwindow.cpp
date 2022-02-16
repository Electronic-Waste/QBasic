#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "program.h"
#include <QMessageBox>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //初始化各个变量
    program = new LinkLine();
    currentNode = NULL;
    isInputMode = false;
    isImmInputMode = false;

    //关联信号槽函数
    connect(this, SIGNAL(loadDetected()), this, SLOT(on_btnLoadCode_clicked()));    //侦测到激活了load指令
    connect(this, SIGNAL(runDetected()), this, SLOT(on_btnRunCode_clicked()));      //侦测到激活了run指令
    connect(this, SIGNAL(clearDetected()), this, SLOT(on_btnClearCode_clicked()));  //侦测到激活了clear指令
    connect(this, SIGNAL(restartDetected()), this, SLOT(restart()));                //侦测到激活了restart指令
}

MainWindow::~MainWindow()
{
    delete ui;
}

//函数名：on_cmdLineEdit_editingFinished（槽函数）
//作用：处理QLineEdit编辑完成后的各种事件
void MainWindow::on_cmdLineEdit_editingFinished()
{
    QString cmd = ui->cmdLineEdit->text();
    ui->cmdLineEdit->setText("");

    //INPUT模式
    if (isInputMode) {
        int length = cmd.toStdString().length();
        inputVal = stoi(cmd.toStdString().substr(1, length - 1)); //获取输入的值(除去开头的问号）
        isInputMode = false;                //重置INPUT模式
        //编辑resultDisplay的内容
        ui->resultDisplay->append(QString::fromStdString("?" + to_string(inputVal)));
        //发出重新执行代码的信号
        emit restartDetected();
        return;
    }

    //三种会被立即执行的Statement:let,print,input
    Tokenizer tk;
    string washQStr(QString qStr);
    //let类型的表达式
    if (tk.getStmtType(washQStr(cmd)) == LETSTMT) {
        Statement *stmt = tk.getStmt(washQStr(cmd));
        stmt->getExp()->eval(context);  //赋值
        return;
    }
    //print类型的表达式
    else if (tk.getStmtType(washQStr(cmd)) == PRINTSTMT) {
        Statement *stmt = tk.getStmt(washQStr(cmd));
        int output = stmt->getExp()->eval(context); //获取表达式的值
        ui->resultDisplay->append(QString::fromStdString(to_string(output)));
        return;
    }
    //input类型的表达式
    else if (tk.getStmtType(washQStr(cmd)) == INPUTSTMT) {
        isInputMode = true;     //开启INPUT模式
        isImmInputMode = true;  //表明是立即执行的Statement而不是按照正常顺序执行的Statement
        immInputStmt = tk.getStmt(washQStr(cmd));
        ui->cmdLineEdit->setText("?");
        return;
    }

    //NORMAL,DELETE,COMMAND类型的命令
    roughLine r(cmd);
    if (r.content == "") return;  //若全为空格，则不做任何操作
    //COMMAND类型的指令
    if (r.type == COMMAND) {
        if (r.content == "RUN") emit runDetected();
        else if (r.content == "LOAD") emit loadDetected();
        else if (r.content == "LIST") return;
        else if (r.content == "CLEAR") emit clearDetected();
        else if (r.content == "HELP")
            QMessageBox::information(this, "Help", QObject::trUtf8("This is my MiniBasic interpreter, using BASIC language!"));
        else if (r.content == "QUIT") exit(0);
        else QMessageBox::critical(this, "Warn", QObject::trUtf8("Invalid Command!"));
    }
    //DELETE类型的指令
    else if (r.type == DELETE) {
        if (!program->search(r.lineNum)) return;    //行号不存在，不作任何操作
        else {                                      //行号存在
            program->remove(r.lineNum);
            ui->CodeDisplay->clear();
            ui->CodeDisplay->append(QString::fromStdString(program->returnRawString()));
        }
    }
    //NORMAL类型的指令
    else {
        program->insert(r);     //插入该行
        ui->CodeDisplay->clear();
        ui->CodeDisplay->append(QString::fromStdString(program->returnRawString()));
    }


}

//函数名：on_btnLoadCode_clicked（槽函数）
//作用：处理load按钮摁下的指令，加载一个文件并存储
void MainWindow::on_btnLoadCode_clicked()
{
    //选择文件并加载
    QString fileName = QFileDialog::getOpenFileName(this, tr("选择加载程序文件"), "./loadFiles", tr("文本文件(*txt)"));
    //若加载文件成功则清除原先的代码,重置变量环境
    if (!fileName.isNull()) emit clearDetected();

    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    QByteArray line = file.readLine();
    QString str(line);

    //逐行读入并存储
    while (!str.isNull()) {
        roughLine r(str);
        //若全为空格，则不做任何操作
        if (r.content == "") return;
        //读入NORMAL(行号+statement)类型的指令
        program->insert(r);
        ui->CodeDisplay->clear();

        //继续读入下一行
        line = file.readLine();
        str = line;
    }

    //显示加载内容
    ui->CodeDisplay->append(QString::fromStdString(program->returnRawString()));

}

//函数名：on_btnRunCode_clicked（槽函数）
//作用：处理run按钮摁下的情况，开始执行程序
void MainWindow::on_btnRunCode_clicked()
{
    //显示表达式树
    ui->treeDisplay->append(QString::fromStdString(program->returnTree()));
    //从第一行代码开始执行程序
    currentNode = program->returnHead()->next;
    run(currentNode, context);
}

//函数名：on_btnClearCode_clicked（槽函数）
//作用：处理clear按钮摁下的情况，清除程序
void MainWindow::on_btnClearCode_clicked()
{
    program->clear();
    context.clearTable();
    ui->CodeDisplay->clear();
    ui->treeDisplay->clear();
    ui->resultDisplay->clear();
    ui->cmdLineEdit->setText("");
}

//函数名：on_btnClearCode_clicked（槽函数）
//作用：处理clear按钮摁下的情况，清除程序
void MainWindow::restart()
{
    //NORMAL类型的input指令
    if (!isImmInputMode) {
        string var = currentNode->stmt->getExp()->getIdentifierName();
        context.setValue(var, inputVal);    //设置变量的值
        currentNode = currentNode->next;    //执行节点变成下一个
        run(currentNode, context);  //重新启动
    }
    //Immediate类型的input指令
    else {
        string var = immInputStmt->getExp()->getIdentifierName();
        context.setValue(var, inputVal);    //设置变量的值
        isImmInputMode = false;     //关闭immInputMode模式
        delete immInputStmt;
    }

}

//函数名：run
//作用：接收待执行行对应的node和“变量-值”表context,从node行开始执行程序
void MainWindow::run(node *&p, EvaluationContext &context)
{
    while (p != NULL) {
        //REMSTMT类型
        if (p->stmt->type() == REMSTMT) p = p->next;
        //LETSTMT类型
        else if (p->stmt->type() == LETSTMT) {
            p->stmt->getExp()->eval(context);
            p = p->next;
        }
        //PRINTSTMT类型
        else if (p->stmt->type() == PRINTSTMT) {
            int output = p->stmt->getExp()->eval(context);
            ui->resultDisplay->append(QString::fromStdString(to_string(output)));
            p = p->next;
        }
        //INPUTSTMT类型
        else if (p->stmt->type() == INPUTSTMT) {
            isInputMode = true;
            ui->cmdLineEdit->setText("?");
            return;
        }
        //GOTOSTMT类型
        else if (p->stmt->type() == GOTOSTMT) {
            int lineNum = p->stmt->getLineNum();
            if (program->returnNode(lineNum) == NULL) {
                p = p->next;
                QMessageBox::critical(this, "Warn", QObject::trUtf8("Invalid Line Number!"));
            }
            else p = program->returnNode(lineNum);
        }
        //IFSTMT类型
        else if (p->stmt->type() == IFSTMT) {
            int result1 = p->stmt->getExp1()->eval(context);
            int result2 = p->stmt->getExp2()->eval(context);
            int lineNum = p->stmt->getLineNum();
            string op = p->stmt->getOp();
            bool flag = false;
            //进行if语句的比较
            if (op == "<" && result1 < result2
                    ||op == ">" && result1 > result2
                    ||op == "=" && result1 == result2) flag = true;
            //结果为True,跳转到指定行
            if (flag && program->returnNode(lineNum) != NULL)
                p = program->returnNode(lineNum);
            //结果为False,执行下一行
            else p = p->next;
        }
        //ENDSTMT类型
        else if (p->stmt->type() == ENDSTMT) break;
        //ERRORSTMT类型
        else if (p->stmt->type() == ERRORSTMT) p = p->next; //该行不执行，执行下一行
    }
}
