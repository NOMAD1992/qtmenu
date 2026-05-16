#ifndef SIDEMENUWIDGET_H
#define SIDEMENUWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QTimer>

class SideMenuWidget; // Forward declaration

class MenuActionItem : public QWidget {
    Q_OBJECT
public:
    explicit MenuActionItem(const QString &text, SideMenuWidget *parent = nullptr);
    void setSubMenu(SideMenuWidget *menu);
    void setActive(bool active);
    QString text() const { return m_label->text(); }
    SideMenuWidget* subMenu() const { return m_subMenu; } // Геттер для доступа к подменю

signals:
    void clicked(); // Сигнал клика

protected:
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QLabel *m_label;
    SideMenuWidget *m_subMenu;
    SideMenuWidget *m_parentMenu;
    bool m_isActive;
};

class SideMenuWidget : public QWidget {
    Q_OBJECT
public:
    explicit SideMenuWidget(QWidget *parent = nullptr);
    
    MenuActionItem* addAction(const QString &text);
    void addSeparator();
    
    // Показать меню справа от точки (для корневого меню)
    void showAt(const QPoint &globalPos);
    
    // Скрыть меню и все вложенные
    void hideAll();

signals:
    void actionTriggered(const QString &text);

private:
    QVBoxLayout *m_layout;
    QList<MenuActionItem*> m_items;
    MenuActionItem *m_currentHovered;
    
    void closeEvent(QCloseEvent *event) override;
};

#endif // SIDEMENUWIDGET_H
