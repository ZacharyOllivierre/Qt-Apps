#include "mainmenu.h"
#include "ui_mainmenu.h"

mainMenu::mainMenu(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::mainMenu)
{
    ui->setupUi(this);

    // Connect signals
    connect(ui->gameButton, &QPushButton::clicked, this, &mainMenu::playClicked);
    connect(ui->settingsButton, &QPushButton::clicked, this, &mainMenu::settingsClicked);

    buildTitle("Mordle");
}

mainMenu::~mainMenu()
{
    delete ui;
}


void mainMenu::buildTitle(QString title)
{
    for (int i = 0; i < title.size(); i++)
    {
        tile* t = new tile(this);
        ui->titleLayout->addWidget(t);

        int stateInt = i % 3 + 1;
        State state = static_cast<State>(stateInt);

        t->setLetter(title[i]);
        t->setState(state);
        t->setFixedSize(110, 110);
        t->show();
    }

    ui->titleLayout->setAlignment(Qt::AlignCenter);

}
