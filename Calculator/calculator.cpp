#include "calculator.h"
#include "ui_calculator.h"

Calculator::Calculator(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Calculator)
{
    ui->setupUi(this);

    connectButtons();
}

Calculator::~Calculator()
{
    delete ui;
}

// private slots
void Calculator::onNumberClicked()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    Token token = {Type::Number, button->text(), true};

    // If last token is a number add number to last token
    if (!equation.empty() && getLastToken()->type == Type::Number)
    {
        getLastToken()->value += token.value;
    }
    else
    {
        equation.push_back(token);
    }

    printEquation();
}

void Calculator::onOperatorClicked()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    Token token = {Type::Operator, button->text(), true};

    equation.push_back(token);

    printEquation();
}

void Calculator::onFunctionClicked()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());

    if (button == ui->clearButton)
    {
        ui->inputLineEdit->clear();
        equation.clear();
    }
    else if (button == ui->plusMinusButton)
    {
        if (getLastToken() != nullptr&& getLastToken()->type != Type::Operator)
        {
            getLastToken()->positive ? getLastToken()->positive = false : getLastToken()->positive = true;
        }

        printEquation();
    }
    else if (button == ui->equalButton)
    {
        qDebug() << "equal button clicked";
        evaluateEquation();
    }
    else if (button == ui->backspaceButton)
    {
        if (!equation.empty())
        {
            equation.pop_back();
        }
        printEquation();
    }
    else
    {
        qDebug() << "unknown function type: " << button->text();
    }
}

// private functions
void Calculator::connectButtons()
{
    QList<QPushButton*> buttons = this->findChildren<QPushButton*>();

    for (auto &button : buttons)
    {
        if (button)
        {
            Type type = stringToType(button->property("type").toString());

            switch (type)
            {
            case Type::Number:
                connect(button, &QPushButton::clicked, this, &Calculator::onNumberClicked);
                break;
            case Type::Operator:
                connect(button, &QPushButton::clicked, this, &Calculator::onOperatorClicked);
                break;
            case Type::Function:
                connect(button, &QPushButton::clicked, this, &Calculator::onFunctionClicked);
                break;
            case Type::Invalid:
                qDebug() << "connect buttons button: " << button->text() << " invalid";
                break;
            };
            // qDebug() << "connected button " << button->property("type").toString();
        }
    }
}

void Calculator::printEquation()
{
    QString strEquation = "";

    for (Token &token : equation)
    {
        QString tokenVal;

        token.positive ? tokenVal = " " : tokenVal = " -";
        tokenVal += token.value;

        strEquation += tokenVal;
    }

    ui->inputLineEdit->setText(strEquation);
}

Type Calculator::stringToType(QString type)
{
    if (type == "number") {return Type::Number;}
    if (type == "operation") {return Type::Operator;}
    if (type == "function") {return Type::Function;}

    qDebug() << "string to type invalid type: " << type;
    return Type::Invalid;
}

Token* Calculator::getLastToken()
{
    if (equation.empty())
    {
        return nullptr;
    }
    return &equation[equation.size() - 1];
}

void Calculator::evaluateEquation()
{
    QString errorMsg = "Err";
    calcPostFixEquation();
    debugPrintEquation();

    double answer = calcAnswer();
    if (!error)
    {
        ui->inputLineEdit->setText(QString::number(answer));
    }
    else
    {
        ui->inputLineEdit->setText(errorMsg);
    }
    equation.clear();
}

void Calculator::debugPrintEquation()
{
    qDebug() << "Infix";
    for (Token &t : equation)
    {
        qDebug() << t.value << " ";
    }

    qDebug() << "Post fix";
    for (Token &t : postFixEquation)
    {
        qDebug() << t.value << " ";
    }
}

void Calculator::calcPostFixEquation()
{
    QQueue<Token> postFix;
    QStack<Token> opStack;

    for (Token &token : equation)
    {
        // numbers go straight onto queue
        if (token.type == Type::Number)
        {
            postFix.enqueue(token);
        }
        // as to opening brackets
        else if (token.value == "(")
        {
            opStack.push(token);
        }
        // close brackets add all ops to queue until opening brack is found
        else if (token.value == ")")
        {
            while (!opStack.empty())
            {
                Token topToken = opStack.pop();
                if (topToken.value == "(")
                {
                    break;
                }
                else
                {
                    postFix.enqueue(topToken);
                }
            }
        }
        /*standard operator pops n enques ops until one of lower precedence is found
        then gets added onto op stack*/
        else
        {
            while (!opStack.empty())
            {
                Token topToken = opStack.top();

                if (topToken.value == "(") break;


                if (precedence(topToken) >= precedence(token))
                {
                    postFix.enqueue(opStack.pop());
                }
                else
                {
                    break;
                }
            }
            opStack.push(token);
        }
    }

    // pop off any ops remaining
    while (!opStack.empty())
    {
        Token topToken = opStack.pop();
        if (topToken.value == "(" || topToken.value == ")")
        {
            qDebug() << "mismatched parentheses";
        }
        else
        {
            postFix.enqueue(topToken);
        }
    }

    // Copy to postFixEquation vector
    postFixEquation.clear();
    while (!postFix.empty())
    {
        postFixEquation.push_back(postFix.dequeue());
    }
}

// */ = 3, +- = 2, () = 1
int Calculator::precedence(Token& token)
{
    if (token.type != Type::Operator)
    {
        qDebug() << "Tried to check precendence of non operator token";
        return -1;
    }

    char symbol = token.value[0].toLatin1();

    switch (symbol)
    {
    case '(':
    case ')':
        break;
        return 1;
    case 'x':
    case '/':
        return 3;
        break;
    case '+':
    case '-':
        return 2;
        break;
    default:
        qDebug() << "Unknown symbol in precendence " << token.value;
    }

    return -1;
}

double Calculator::calcAnswer()
{
    QStack<double> stack;
    error = false;

    for (Token &token : postFixEquation)
    {
        if (token.type == Type::Number)
        {
            stack.push(tokenToValue(token));
        }
        else
        {
            if (stack.size() < 2)
            {
                qDebug() << "Not enough operands";
                error = true;
                return 0;
            }

            double secondOperand = stack.pop();
            double firstOperand = stack.pop();

            double result;

            if (token.value == "+") result = firstOperand + secondOperand;
            else if (token.value == "-") result = firstOperand - secondOperand;
            else if (token.value == "x") result = firstOperand * secondOperand;
            else if (token.value == "/")
            {
                if (secondOperand == 0)
                {
                    qDebug() << "division by zero";
                    error = true;
                    return 0;
                }
                result = firstOperand / secondOperand;
            }
            else
            {
                qDebug() << "unknown operator: " << token.value;
                error = true;
                return 0;
            }


            stack.push(result);
        }
    }

    if (stack.size() != 1)
    {
        qDebug() << "invalid postfix expression";
        error = true;
        return 0;
    }

    return stack.top();
}

double Calculator::tokenToValue(Token& token)
{
    double value = token.value.toDouble();

    if (token.positive == false)
    {
        value *= -1;
    }

    return value;
}