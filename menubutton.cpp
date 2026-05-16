#include "menubutton.h"
#include <QMouseEvent>
#include <QFrame>

MenuButton::MenuButton(const QString& text, SlideMenu::Direction dir, QWidget* parent)
    : QPushButton(text, parent)
    , m_direction(dir)
    , m_menu(nullptr)
    , m_menuBarFrame(nullptr)
    , m_isMenuVisible(false)
{
    setCursor(Qt::PointingHandCursor);
}

MenuButton::~MenuButton()
{
    if (m_menu) delete m_menu;
}

void MenuButton::setMenu(SlideMenu* menu)
{
    if (m_menu == menu) return;
    if (m_menu) delete m_menu;
    m_menu = menu;
    connect(m_menu, &SlideMenu::closed, this, &MenuButton::onMenuClosed);
}

SlideMenu* MenuButton::menu() const
{
    return m_menu;
}

void MenuButton::setMenuBarFrame(QFrame* frame)
{
    m_menuBarFrame = frame;
}

void MenuButton::mousePressEvent(QMouseEvent* e)
{
    QPushButton::mousePressEvent(e);
    if (!m_menu) return;

    if (m_isMenuVisible) {
        hideMenu();
    } else {
        showMenu();
    }
}

void MenuButton::showMenu()
{
    if (!m_menu || !m_menuBarFrame) return;

    QPoint globalMenuBarTopLeft = m_menuBarFrame->mapToGlobal(QPoint(0, 0));
    int yOffset = m_menuBarFrame->height() + 2;
    QPoint anchor = globalMenuBarTopLeft + QPoint(0, yOffset);

    m_menu->showWithDirection(anchor, m_direction);
    m_isMenuVisible = true;
}

void MenuButton::hideMenu()
{
    if (m_menu && m_isMenuVisible) {
        m_menu->hideWithAnimation();
        m_isMenuVisible = false;
    }
}

void MenuButton::onMenuClosed()
{
    m_isMenuVisible = false;
}
