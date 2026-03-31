#include "settings.h"
#include "ui_settings.h"

settings::settings(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::settings)
{
    ui->setupUi(this);

    setDefaultData();

    // Set spin boxes block signals to avoid signal resetting it to min
    ui->wordLengthSpinBox->blockSignals(true);
    ui->guessesSpinBox->blockSignals(true);
    ui->wordLengthSpinBox->setRange(data.minWordLength, data.maxWordLength);
    ui->guessesSpinBox->setRange(data.minGuesses, data.maxGuesses);
    ui->wordLengthSpinBox->setValue(data.wordLength);
    ui->guessesSpinBox->setValue(data.guesses);
    ui->wordLengthSpinBox->blockSignals(false);
    ui->guessesSpinBox->blockSignals(false);

    // Set signals
    connect(ui->backButton, &QPushButton::clicked, this, &settings::backClicked);
}

settings::~settings()
{
    delete ui;
}

void settings::setDefaultData()
{
    data.guesses = 5;
    data.wordLength = 5;
    data.minWordLength = 2;
    data.maxWordLength = 13;
    data.minGuesses = 3;
    data.maxGuesses = 10;
    data.checkValidGuess = true;
}


void settings::on_wordLengthSpinBox_valueChanged(int arg1)
{
    data.wordLength = arg1;
}

void settings::on_guessesSpinBox_valueChanged(int arg1)
{
    data.guesses = arg1;
}


void settings::on_invalidInputCheckBox_checkStateChanged(const Qt::CheckState &arg1)
{
    if (arg1 == Qt::CheckState::Checked)
    {
        data.checkValidGuess = false;
    }
    else
    {
        data.checkValidGuess = true;
    }
}

