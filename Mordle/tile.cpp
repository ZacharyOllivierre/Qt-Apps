#include "tile.h"

tile::tile(QWidget* parent)
{
    // Set label
    setAlignment(Qt::AlignCenter);

    QFont f = font();
    f.setPointSize(20);
    f.setBold(true);
    setFont(f);

    this->setState(Empty);
}

void tile::setLetter(QChar letter)
{
    this->letter = letter;
    setText(QString(letter));
}

void tile::setState(State state)
{
    this->state = state;
    updateStyle();
}

void tile::clear()
{
    letter = ' ';
    setText("");
    this->setState(Empty);
}

void tile::updateStyle()
{
    switch(state)
    {
    case Empty:
        setStyleSheet("border:2px solid #d3d6da; background:#f3f3f3; color: black;");
        break;

    case Correct:
        setStyleSheet("border:2px solid #6aaa64; background:#6aaa64; color:white;");
        break;

    case Present:
        setStyleSheet("border:2px solid #c9b458; background:#c9b458; color:white;");
        break;

    case Absent:
        setStyleSheet("border:2px solid #787c7e; background:#787c7e; color:white;");
        break;
    }
}

void tile::resizeEvent(QResizeEvent *event)
{
    const double fontFactor = 0.35;
    int size = qMax(1, int(width() * fontFactor));

    QFont font = this->font();
    font.setPointSize(size);
    setFont(font);

    QLabel::resizeEvent(event);
}

bool tile::hasHeightForWidth() const
{
    return true;
}

int tile::heightForWidth(int width) const
{
    return width;
}

