#ifndef TILE_H
#define TILE_H

// suppossed needed fix?
// #include <QtCore/qtmetamacros.h>
#include <QLabel>

// Ordered in precendence of importance (keyboard state updating)
enum State
{
    Empty = 0,
    Absent = 1,
    Present = 2,
    Correct = 3
};

class tile : public QLabel
{
    Q_OBJECT

public:
    explicit tile(QWidget* parent = nullptr);

    void setLetter(QChar letter);
    void setState(State state);
    void clear();

    inline QChar getLetter() {return letter;}
    inline State getState() {return state;}

protected:
    // Adjust font with square size
    void resizeEvent(QResizeEvent *event) override;

    // Make tiles remain square
    bool hasHeightForWidth() const override;
    int heightForWidth(int width) const override;

private:
    QChar letter;
    State state;

    void updateStyle();
};

#endif // TILE_H
