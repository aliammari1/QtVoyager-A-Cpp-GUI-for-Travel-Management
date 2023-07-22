#include "mybutton.h"

#include <QEvent>
#include <QDebug>

MyButton::MyButton(QWidget *parent) : QPushButton(parent), m_colorAnimation(this, "color")
{
    this->installEventFilter(this);
}

void MyButton::SetColor(const QColor &color)
{
    m_currentColor = color;
    QString css = "QPushButton { border-radius: 10px; ";
    css.append("border: 1.5px solid rgb(91,231,255); ");
    QString strColor = QString("rgb(%1, %2, %3)").arg(color.red()).arg(color.green()).arg(color.blue());
    css.append("background-color: " + strColor + "; }");
    setStyleSheet(css);
}

const QColor &MyButton::GetColor() const
{
    return m_currentColor;
}

bool MyButton::eventFilter(QObject *obj, QEvent *e)
{
    if (e->type() == QEvent::HoverEnter)
    {
        StartHoverEnterAnimation();
    }

    if (e->type() == QEvent::HoverLeave)
    {
        StartHoverLeaveAnimation();
    }

    return false;
}

void MyButton::StartHoverEnterAnimation()
{
    m_colorAnimation.stop();

    m_colorAnimation.setDuration(900); // set your transition

    m_colorAnimation.setStartValue(GetColor()); // starts from current color

    m_colorAnimation.setEndValue(QColor(128, 255, 255));   // set your hover color
    m_colorAnimation.setEasingCurve(QEasingCurve::Linear); // animation style

    m_colorAnimation.start();
}

void MyButton::StartHoverLeaveAnimation()
{
    m_colorAnimation.stop();

    m_colorAnimation.setDuration(900);                   // set your transition
    m_colorAnimation.setStartValue(GetColor());          // starts from current color
    m_colorAnimation.setEndValue(QColor(255, 255, 255)); // set your regular color

    m_colorAnimation.setEasingCurve(QEasingCurve::Linear); // animation style

    m_colorAnimation.start();
}
