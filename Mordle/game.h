#ifndef GAME_H
#define GAME_H

#include <QWidget>
#include <QtWidgets/qgridlayout.h>
#include <QKeyEvent>
#include <QFile>
#include <QRandomGenerator>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QTimer>
#include "settings.h"
#include "tile.h"
#include "summary.h"

// Tracks active data for a running game
struct GameData
{
    // Trakcs row and col for active typing
    int currentGuess = 0; //row
    int currentLetter = 0;  //col
    QString guess = "";

    bool gameOver = false;
};

namespace Ui {
class game;
}

class game : public QWidget
{
    Q_OBJECT

public:
    explicit game(settings* settingsPtr, QWidget *parent = nullptr);
    ~game();

    void newGame();

protected:
    void keyPressEvent(QKeyEvent *event) override;

    // Calls update tile positions upon resize
    void resizeEvent(QResizeEvent *event) override;

    /*Calls upon game coming into view, set strong focus on board,
     * allows typing to work instead of clicking on the grid*/
    void showEvent(QShowEvent* event) override;

private:
    Ui::game *ui;

    settings* settingsPtr;
    SettingsData settingsData;

    // Tracks current gamestate
    GameData gameData;

    // Holds pointers to board and keyboard structure
    QVector<QVector<tile*>> board;
    QVector<tile*> keyboard;

    // Word user is guessing and words user has guessed
    QString secretWord;
    QVector<QString> usedWords;

    /*Every new game updates new list of valid words of set length (from dictionary),
     *Done to reference for secret word choice and valid user input*/
    QVector<QString> validWords;
    QMap<int, QVector<QString>> dictionaryByLength;

private:
    void setUpUi();
    void populateGrid();

    /*Calculates tile size and position, seperated to be called
     *independently on game start*/
    void updateTilePositions();
    void updateKeyboardSize(int tileSize);

    void clearBoard();

    void checkGuess();
    bool guessIsValid();

    void enterGuessWithState();
    QVector<State> getStatesForGuess();

    QString getSecretWord();
    void loadDictionary(const QString &filePath);
    void updateValidWords();

    void shakeRow();

    void populateKeyboard();
    void updateKeyboardState(State state, QChar letter);

signals:
    void backClicked();

private slots:
    void on_resetButton_clicked();
};

#endif // GAME_H
