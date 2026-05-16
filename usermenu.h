#ifndef USERMENU_H
#define USERMENU_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QPropertyAnimation>
#include <QLabel>
#include <QFrame>

class UserMenu : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int menuOffset READ menuOffset WRITE setMenuOffset)

public:
    explicit UserMenu(QWidget *parent = nullptr, int menuWidth = 200);
    ~UserMenu();

    // Методы для создания элементов меню
    QPushButton* addButton(const QString &text, const QPixmap &icon = QPixmap());
    void addSplitter();

    // Показать/скрыть меню
    void showMenu();
    void hideMenu();
    bool isMenuVisible() const;

signals:
    void menuShown();
    void menuHidden();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

private slots:
    void onAnimationFinished();
    void updateMenuGeometry();

private:
    int menuOffset() const;
    void setMenuOffset(int offset);
    
    void setupUi();
    void setupAnimations();
    void applyStyles();
    
    // Вспомогательный метод для создания кнопки
    QPushButton* createButton(const QString &text, const QPixmap &icon, QWidget *parent);

    int m_menuWidth;
    
    // Layout для контента
    QVBoxLayout *m_contentLayout;
    
    // Анимация
    QPropertyAnimation *m_animation;
    bool m_isVisible;
};

#endif // USERMENU_H
