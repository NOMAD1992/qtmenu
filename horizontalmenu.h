#ifndef HORIZONTALMENU_H
#define HORIZONTALMENU_H

#include <QMenu>
#include <QAction>
#include <QStyleOptionMenuItem>
#include <QPainter>
#include <QApplication>
#include <QScreen>

class HorizontalMenu : public QMenu
{
    Q_OBJECT

public:
    explicit HorizontalMenu(QWidget *parent = nullptr);
    explicit HorizontalMenu(const QString &title, QWidget *parent = nullptr);

protected:
    bool event(QEvent *e) override;

private:
    void repositionSubMenu();
};

#endif // HORIZONTALMENU_H
