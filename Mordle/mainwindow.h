#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets/qstackedwidget.h>
#include "game.h"
#include "settings.h"
#include "mainmenu.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QStackedWidget *screenStack;
    game* gameWidget;
    settings* settingsWidget;
    mainMenu* menuWidget;
};
#endif // MAINWINDOW_H
