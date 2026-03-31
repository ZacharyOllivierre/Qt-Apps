#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>

struct SettingsData
{
    int minWordLength;
    int maxWordLength;
    int wordLength;

    int guesses;
    int minGuesses;
    int maxGuesses;

    bool checkValidGuess;
};

namespace Ui {
class settings;
}

class settings : public QWidget
{
    Q_OBJECT

public:
    explicit settings(QWidget *parent = nullptr);
    ~settings();

    inline SettingsData getData() {return data;}

private slots:
    void on_wordLengthSpinBox_valueChanged(int arg1);

    void on_guessesSpinBox_valueChanged(int arg1);

    void on_invalidInputCheckBox_checkStateChanged(const Qt::CheckState &arg1);

private:
    Ui::settings *ui;

    SettingsData data;

    void setDefaultData();

signals:
    void backClicked();
};

#endif // SETTINGS_H
