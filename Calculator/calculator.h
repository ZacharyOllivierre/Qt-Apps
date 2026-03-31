#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QWidget>
#include <QString>
#include <QVector>
#include <qstack.h>
#include <qqueue.h>

enum class Type
{
    Number,
    Operator,
    Function,
    Invalid
};

struct Token
{
    Type type;
    QString value;
    bool positive;
};

QT_BEGIN_NAMESPACE
namespace Ui {
class Calculator;
}
QT_END_NAMESPACE

class Calculator : public QWidget
{
    Q_OBJECT

public:
    Calculator(QWidget *parent = nullptr);
    ~Calculator();

private slots:
    void onNumberClicked();
    void onOperatorClicked();
    void onFunctionClicked();

private:
    Ui::Calculator *ui;

    QVector<Token> equation;
    QVector<Token> postFixEquation;

    bool error;

private:
    void connectButtons();

    void printEquation();

    Type stringToType(QString type);

    Token* getLastToken();

    void evaluateEquation();
    void debugPrintEquation();

    void calcPostFixEquation();
    int precedence(Token& token);

    double calcAnswer();
    double tokenToValue(Token& token);
};
#endif // CALCULATOR_H
