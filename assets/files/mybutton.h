#ifndef MYBUTTON_H
#define MYBUTTON_H

#include <QPushButton>
#include <QColor>
#include <QPropertyAnimation>

class MyButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ GetColor WRITE SetColor)

public:
    explicit MyButton(QWidget *parent = nullptr);

    void SetColor(const QColor &color);
    const QColor &GetColor() const;
    bool eventFilter(QObject *obj, QEvent *e) override;

private:
    QColor m_currentColor;

    QPropertyAnimation m_colorAnimation;

    void StartHoverEnterAnimation();
    void StartHoverLeaveAnimation();
};

#endif // MYBUTTON_H
