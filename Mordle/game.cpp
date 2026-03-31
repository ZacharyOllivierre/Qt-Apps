#include "game.h"
#include "ui_game.h"

game::game(settings* settingsPtr, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::game)
{
    ui->setupUi(this);

    // Get data from settings
    this->settingsPtr = settingsPtr;
    settingsData = settingsPtr->getData();

    // Connect signals
    connect(ui->backButton, &QPushButton::clicked, this, &game::backClicked);

    setUpUi();

    loadDictionary(":/new/prefix1/wordList.txt");
}

game::~game()
{
    delete ui;
}

void game::newGame()
{
    // Update saved settings data
    settingsData = settingsPtr->getData();

    clearBoard();
    populateGrid();
    populateKeyboard();
    updateTilePositions();

    // Get new word to guess
    secretWord = getSecretWord();

    // Clear game data
    gameData.currentGuess = 0;
    gameData.currentLetter = 0;
    gameData.guess = "";
    gameData.gameOver = false;

    /*Update positions again? Not sure why both are needed
    but assures frame is calculated after layout is done*/
    QTimer::singleShot(0, this, [this]() {
        updateTilePositions();
    });
}

// Overrides
void game::keyPressEvent(QKeyEvent *event)
{
    // early out if out of guesses or game is over
    if (gameData.currentGuess >= settingsData.guesses || gameData.gameOver) return;

    // If key is a natural letter
    if (event->key() >= Qt::Key_A && event->key() <= Qt::Key_Z)
    {
        // Typing space is not full
        if (gameData.currentLetter < settingsData.wordLength)
        {
            // Ammend current game data
            QChar letter = event->text().toUpper().at(0);

            gameData.guess += letter;

            board[gameData.currentGuess][gameData.currentLetter]->setLetter(letter);

            gameData.currentLetter++;
        }
    }
    // Backspace
    else if (event->key() == Qt::Key_Backspace)
    {
        // If field is not empty
        if (gameData.currentLetter > 0)
        {
            gameData.currentLetter--;
            gameData.guess.chop(1);
            board[gameData.currentGuess][gameData.currentLetter]->setLetter(' ');
        }
    }
    // Enter
    else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
        // If typing field is full
        if (gameData.currentLetter == settingsData.wordLength)
        {
            checkGuess();
        }
    }
}

void game::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateTilePositions();
}

void game::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    this->setFocus(Qt::FocusReason::ActiveWindowFocusReason);
}

// Private
void game::setUpUi()
{
    // Set focus
    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    // Setup keyboard
    ui->keyboardLayout->setSpacing(0);
    ui->keyboardLayout->setContentsMargins(0, 0, 0, 0);
    ui->keyboardLayout->setAlignment(Qt::AlignCenter);
}

void game::populateGrid()
{
    // Resize board vec
    board.resize(settingsData.guesses);
    for(int r = 0; r < settingsData.guesses; r++)
    {
        board[r].resize(settingsData.wordLength);
    }

    // Build grid
    for (int row = 0; row < settingsData.guesses; row++)
    {
        for (int col = 0; col < settingsData.wordLength; col++)
        {
            tile* piece = new tile(this);
            piece->setParent(ui->gameFrame);

            board[row][col] = piece;
        }
    }
}

/*Calculates and updates game tiles with position and size refrenced to frame size.
 * Additionally updates size of keyboard tiles*/
void game::updateTilePositions()
{
    if (board.isEmpty()) return;

    // Update game tiles size and position
    float marginPercent = 0.05;
    int vertMargin = ui->gameFrame->height() * marginPercent;
    int rows = settingsData.guesses;
    int cols = settingsData.wordLength;

    int w = ui->gameFrame->width();
    int h = ui->gameFrame->height() - 2 * vertMargin;

    int tileSize = qMin(w / cols, h / rows);

    int boardWidth = tileSize * cols;
    int boardHeight = tileSize * rows;

    int offsetX = (w - boardWidth) / 2;
    int offsetY = vertMargin + (h - boardHeight) / 2;

    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            tile* t = board[r][c];
            t->setFixedSize(tileSize, tileSize);
            t->move(offsetX + c * tileSize, offsetY + r * tileSize);
            t->show();
        }
    }

    // Update keyboard
    updateKeyboardSize(tileSize);
}

// Update keyboard tiles size beyond a minimum
void game::updateKeyboardSize(int tileSize)
{
    for (tile* kTile : keyboard)
    {
        double sizePercent = 0.75;
        int minSize = 40;
        int keySize;

        // Enforce minimum size
        if (tileSize * sizePercent < minSize)
            keySize = minSize;
        else
            keySize = tileSize * sizePercent;

        kTile->setFixedSize(keySize, keySize);
        kTile->show();
    }
}

void game::clearBoard()
{
    // Clear Board | layout and 2d vec
    for (auto &row : board)
    {
        for (auto &t : row)
        {
            if (t)
            {
                t->hide();
                t->deleteLater();
                t = nullptr;
            }
        }
    }
    board.clear();
}

void game::checkGuess()
{
    // Early out if guess is invalid and checking valid guesses
    if (!guessIsValid() && settingsData.checkValidGuess)
    {
        // Start invalid guess animations shake / red
        shakeRow();

        qDebug() << "Guess " << gameData.guess << " was invalid";
        return;
    }

    qDebug() << "User guessed " << gameData.guess;

    // Enter word onto board with states
    enterGuessWithState();

    // Move to next input line and reset typing col
    gameData.currentGuess++;
    gameData.currentLetter = 0;

    bool won = (gameData.guess == secretWord);

    // Game is over
    if (gameData.guess == secretWord || gameData.currentGuess >= settingsData.guesses)
    {
        // Show summary
        summary s(secretWord, won, this);
        s.exec();
        gameData.gameOver = true;
        return;
    }

    gameData.guess.clear();
}

bool game::guessIsValid()
{
    if (validWords.contains(gameData.guess))
    {
        return true;
    }
    return false;
}

void game::enterGuessWithState()
{
    // early out
    if (gameData.currentGuess >= settingsData.guesses)
    {
        qDebug() << "Number of guesses greater than allowed guesses";
    }

    QString word = gameData.guess;

    QVector<State> guessLetterStates = getStatesForGuess();

    for (int i = 0; i < word.size(); i++)
    {
        // Set letter in board with state
        board[gameData.currentGuess][i]->setLetter(word[i]);
        board[gameData.currentGuess][i]->setState(guessLetterStates[i]);

        // Update keyboard with states
        updateKeyboardState(guessLetterStates[i], word[i]);
    }
}

QVector<State> game::getStatesForGuess()
{
    QVector<State> wordStates;

    // Write all as absent
    for (int i = 0; i < settingsData.wordLength; i++)
    {
        wordStates.push_back(Absent);
    }

    // Track letters in secret word to mark Present
    QMap<QChar, int> letterCount;
    for (const QChar &c: secretWord)
    {
        letterCount[c]++;
    }

    // Mark correct letters
    for (int i = 0; i < secretWord.size(); i++)
    {
        if (gameData.guess[i] == secretWord[i])
        {
            wordStates[i] = Correct;
            letterCount[secretWord[i]]--;
        }
    }

    /* Marks present letters. Letters which not currently correct,
     * within the letterCount map, and not already exhausted by previous letter*/
    for (int i = 0; i < secretWord.size(); i++)
    {
        if (wordStates[i] == Absent &&
            letterCount.contains(gameData.guess[i]) &&
            letterCount[gameData.guess[i]] > 0)
        {
            wordStates[i] = Present;
            letterCount[gameData.guess[i]]--;
        }
    }

    return wordStates;
}

QString game::getSecretWord()
{
    updateValidWords();

    // Early out if no valid words found
    if (validWords.isEmpty())
    {
        qDebug() << "No words of valid length found in dictionary";
        return "";
    }

    // Keep picking random words until one that isnt in used words is chosen
    bool picking = true;
    int indexChoice;
    while (picking)
    {
        indexChoice = QRandomGenerator::global()->bounded(validWords.size());

        if (!usedWords.contains(validWords[indexChoice]))
        {
            picking = false;
            qDebug() << "Chose " << validWords[indexChoice];
        }
        else
        {
            qDebug() << "Chose " << validWords[indexChoice] << " but was already chosen";
        }
    }

    usedWords.push_back(validWords[indexChoice]);
    return validWords[indexChoice];
}

void game::loadDictionary(const QString &filePath)
{
    QFile file(filePath);

    // File open failed
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "File open failed";
        return;
    }

    QTextStream in(&file);

    while(!in.atEnd())
    {
        QString word = in.readLine().trimmed().toUpper();

        if (!word.isEmpty())
        {
            int length = word.length();
            dictionaryByLength[length].push_back(word);

        }
    }
}

void game::updateValidWords()
{
    validWords.clear();
    validWords = dictionaryByLength[settingsData.wordLength];
}

void game::shakeRow()
{
    int amplitude = 10;
    int duration = 300;

    QParallelAnimationGroup* animGroup = new QParallelAnimationGroup;

    for (tile* tile : board[gameData.currentGuess])
    {
        // Create animation
        QPropertyAnimation* anim = new QPropertyAnimation(tile, "pos");
        anim->setDuration(duration);

        // original position
        QPoint oPos = tile->pos();

        // Shake row
        anim->setKeyValueAt(0, oPos);
        anim->setKeyValueAt(0.1, oPos + QPoint(-amplitude, 0));
        anim->setKeyValueAt(0.2, oPos + QPoint(amplitude, 0));
        anim->setKeyValueAt(0.3, oPos + QPoint(-amplitude, 0));
        anim->setKeyValueAt(0.4, oPos + QPoint(amplitude, 0));
        anim->setKeyValueAt(0.5, oPos);
        anim->setKeyValueAt(1, oPos);

        animGroup->addAnimation(anim);
    }

    animGroup->start(QAbstractAnimation::DeleteWhenStopped);
}

// Clears and repopulates keyboard tiles 2 rows
void game::populateKeyboard()
{
    // Clear keyboard
    QLayoutItem* item;
    while ((item = ui->keyboardLayout->takeAt(0)) != nullptr)
    {
        if (item->widget())
        {
            item->widget()->deleteLater();
        }
        delete item;
    }
    keyboard.clear();

    int rows = 2;
    int perCol = 26 / rows;
    char letter = 'A';

    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < perCol; c++)
        {
            tile* piece = new tile(this);

            ui->keyboardLayout->addWidget(piece, r, c);
            piece->setLetter(letter);
            letter++;

            keyboard.push_back(piece);
        }
    }
}


void game::updateKeyboardState(State state, QChar letter)
{
    // Find letter in keyboard and update state
    bool found = false;
    for (int t = 0; t < keyboard.count() && !found; t++)
    {
        if (keyboard[t]->getLetter() == letter)
        {
            // Only update state to higher level of precendence
            if (keyboard[t]->getState() < state)
            {
                keyboard[t]->setState(state);
            }
            found = true;
        }
    }
}

// Private slots
void game::on_resetButton_clicked()
{
    newGame();
}

