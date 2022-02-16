#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "program.h"
#include "exp.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void run(node *&p, EvaluationContext &context);

private slots:
    void on_cmdLineEdit_editingFinished();
    void on_btnLoadCode_clicked();
    void on_btnRunCode_clicked();
    void on_btnClearCode_clicked();
    void restart();

signals:
    void runDetected();
    void loadDetected();
    void clearDetected();
    void delDetected(int num);
    void restartDetected();

private:
    Ui::MainWindow *ui;
    LinkLine *program;
    node *currentNode;
    Statement *immInputStmt;
    EvaluationContext context;
    bool isInputMode;
    bool isImmInputMode;
    int inputVal;
};
#endif // MAINWINDOW_H
