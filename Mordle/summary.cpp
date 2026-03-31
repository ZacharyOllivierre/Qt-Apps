#include "summary.h"
#include "ui_summary.h"

summary::summary(const QString& secretWord, bool won, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::summary)
{
    ui->setupUi(this);

    // Change body message
    if (won)
    {
        ui->bodyLabel->setText("You won the word was " + secretWord);
    }
    else
    {
        ui->bodyLabel->setText("You lost the word was " + secretWord);
    }

    connect(ui->backButton, &QPushButton::clicked, this, &QDialog::accept);

    setWindowModality(Qt::ApplicationModal);
}

summary::~summary()
{
    delete ui;
}
