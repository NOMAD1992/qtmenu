#ifndef SLIDINGMENU_H
#define SLIDINGMENU_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QMenu>
#include <QVBoxLayout>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

class SlidingMenu : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int menuOffset READ menuOffset WRITE setMenuOffset)

public:
    enum class SlideDirection {
        FromLeft,
        FromRight
    };

    explicit SlidingMenu(QWidget *parent = nullptr, 
                         SlideDirection direction = SlideDirection::FromLeft,
                         int menuWidth = 300);
    ~SlidingMenu();

    // Установка содержимого меню
    void setTitle(const QString &title);
    void setIcon(const QPixmap &icon);
    void addButtons(const QStringList &buttonTexts);
    void setCheckBoxText(const QString &text);
    void setCheckBoxChecked(bool checked);
    void setMenu(QMenu *menu);
    
    // Показать/скрыть меню
    void showMenu();
    void hideMenu();
    bool isMenuVisible() const;

    // Геттеры для элементов
    QLabel *titleLabel() const { return m_titleLabel; }
    QLabel *iconLabel() const { return m_iconLabel; }
    QPushButton *closeButton() const { return m_closeButton; }
    QCheckBox *checkBox() const { return m_checkBox; }
    QMenu *menu() const { return m_menu; }
    QList<QPushButton*> buttons() const { return m_buttons; }

signals:
    void menuShown();
    void menuHidden();
    void checkBoxToggled(bool checked);

private slots:
    void onAnimationFinished();
    void onCloseClicked();

private:
    int menuOffset() const;
    void setMenuOffset(int offset);
    
    void setupUi();
    void setupAnimations();
    void applyStyles();

    // Направление скольжения
    SlideDirection m_direction;
    int m_menuWidth;
    
    // Элементы интерфейса
    QLabel *m_iconLabel;
    QLabel *m_titleLabel;
    QPushButton *m_closeButton;
    QList<QPushButton*> m_buttons;
    QCheckBox *m_checkBox;
    QMenu *m_menu;
    QWidget *m_separator;
    
    // Анимация
    QPropertyAnimation *m_animation;
    QGraphicsOpacityEffect *m_opacityEffect;
    bool m_isVisible;
};

#endif // SLIDINGMENU_H
