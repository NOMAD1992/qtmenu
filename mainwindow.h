#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "slidingmenu.h"
#include "usermenu.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void toggleMenu();
    void toggleUserMenu();

private:
    Ui::MainWindow *ui;
    SlidingMenu *m_slidingMenu;
    UserMenu *m_userMenu;
    QPushButton *m_menuButton;
    QPushButton *m_userMenuButton;
};

#endif // MAINWINDOW_H
