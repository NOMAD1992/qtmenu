#ifndef SLIDEMENU_H
#define SLIDEMENU_H

#include <QFrame>
#include <QPropertyAnimation>
#include <functional>

class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QPushButton;
class QCheckBox;
class QMenu;

class SlideMenu : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(QPoint pos READ pos WRITE move)

public:
    enum Direction { LeftToRight, RightToLeft };

    explicit SlideMenu(QWidget *parent = nullptr);
    ~SlideMenu();

    void addSeparator();
    void addLabel(const QString &text);
    void addButton(const QString &text, const std::function<void()> &callback = nullptr);
    void addCheckbox(const QString &text);
    void addMenuButton(const QString &text, QMenu *menu);
    void setIconLabel(const QIcon &icon);

    // Основной метод показа с анимацией
    void showWithDirection(const QPoint &anchorTopLeft, Direction dir);
    void hideWithAnimation();

    Direction currentDirection() const { return m_currentDirection; }

signals:
    void closed();

private slots:
    void onCloseClicked();
    void onAnimationFinished();

private:
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_topLayout;
    QLabel *m_iconLabel;
    QPushButton *m_closeButton;
    QPropertyAnimation *m_animation;
    QPoint m_startPos;
    QPoint m_endPos;
    Direction m_currentDirection;

    void setupUi();
    void installShadow();
    void startAnimation(bool show);

    // QObject interface
public:
    bool eventFilter(QObject *obj, QEvent *event);

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event);
};

#endif // SLIDEMENU_H
