#ifndef MAINMENU_H
#define MAINMENU_H

#include <QWidget>
#include "tile.h"

namespace Ui {
class mainMenu;
}

class mainMenu : public QWidget
{
    Q_OBJECT

public:
    explicit mainMenu(QWidget *parent = nullptr);
    ~mainMenu();

    void buildTitle(QString title);

private:
    Ui::mainMenu *ui;

signals:
    void playClicked();
    void settingsClicked();
};

#endif // MAINMENU_H

