#ifndef SUMMARY_H
#define SUMMARY_H

#include <QWidget>
#include <QDialog>

namespace Ui {
class summary;
}

class summary : public QDialog
{
    Q_OBJECT

public:
    explicit summary(const QString& secretWord, bool won, QWidget *parent = nullptr);
    ~summary();

private:
    Ui::summary *ui;

signals:
    void backButton();
};

#endif // SUMMARY_H
