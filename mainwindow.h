#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListView>
#include "slidingmenu.h"
#include "usermenu.h"
#include "bottomsheet.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // Метод для добавления QListView в шторку
    void addListView(QListView *view);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void toggleMenu();
    void toggleUserMenu();
    void toggleFrameless(bool checked);
    void minimizeWindow();
    void maximizeRestoreWindow();
    void closeWindow();
    void toggleFullScreen();

private:
    Ui::MainWindow *ui;
    SlidingMenu *m_slidingMenu;
    UserMenu *m_userMenu;
    BottomSheet *m_bottomSheet;
    QListView *m_listView;
    bool m_frameless;
};

#endif // MAINWINDOW_H
