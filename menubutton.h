#ifndef MENUBUTTON_H
#define MENUBUTTON_H

#include <QPushButton>
#include "slidemenu.h"   // для SlideMenu::Direction

class QFrame;

class MenuButton : public QPushButton
{
    Q_OBJECT
public:
    // Теперь используем SlideMenu::Direction
    explicit MenuButton(const QString& text, SlideMenu::Direction dir, QWidget* parent = nullptr);
    ~MenuButton();

    void setMenu(SlideMenu* menu);
    SlideMenu* menu() const;
    void setMenuBarFrame(QFrame* frame);

protected:
    void mousePressEvent(QMouseEvent* e) override;

private slots:
    void onMenuClosed();

private:
    SlideMenu::Direction m_direction;
    SlideMenu* m_menu;
    QFrame* m_menuBarFrame;
    bool m_isMenuVisible;

    void showMenu();
    void hideMenu();
};

#endif // MENUBUTTON_H
