#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    screenStack = ui->screenStack;

    // Create stacked widget components and add them
    menuWidget = new mainMenu(this);
    settingsWidget = new settings(this);
    gameWidget = new game(settingsWidget, this);

    screenStack->addWidget(settingsWidget);
    screenStack->addWidget(gameWidget);
    screenStack->addWidget(menuWidget);

    setCentralWidget(screenStack);

    screenStack->setCurrentWidget(menuWidget);

    // Add functionality to buttons with signals
    connect(menuWidget, &mainMenu::playClicked, this, [=]() {
        screenStack->setCurrentWidget(gameWidget);
        gameWidget->newGame();
    });

    connect(menuWidget, &mainMenu::settingsClicked, this, [=]() {
        screenStack->setCurrentWidget(settingsWidget);
    });

    connect(settingsWidget, &settings::backClicked, this, [=]() {
        screenStack->setCurrentWidget(menuWidget);
    });

    connect(gameWidget, &game::backClicked, this, [=]() {
        screenStack->setCurrentWidget(menuWidget);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}


