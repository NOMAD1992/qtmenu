#ifndef SLIDINGMENU_H
#define SLIDINGMENU_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QMenu>
#include <QVBoxLayout>
#include <QPropertyAnimation>
#include <QResizeEvent>

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
    
    // Добавление произвольных виджетов (универсальный API)
    void addWidget(QWidget *widget, int stretch = 0);
    void addButton(QPushButton *button);
    void addLayout(QLayout *layout, int stretch = 0);
    void addSeparator();
    void clearContent();
    
    // Получить доступ к layout контента для прямой манипуляции
    QVBoxLayout* contentLayout() const;
    
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

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

private slots:
    void onAnimationFinished();
    void onCloseClicked();
    void updateMenuHeight();

private:
    int menuOffset() const;
    void setMenuOffset(int offset);
    
    void setupUi();
    void setupAnimations();
    void applyStyles();
    void installParentEventFilter();

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
    QVBoxLayout *m_contentLayout;  // Layout для контента (для внешнего доступа)
    
    // Анимация
    QPropertyAnimation *m_animation;
    bool m_isVisible;
};

#endif // SLIDINGMENU_H
