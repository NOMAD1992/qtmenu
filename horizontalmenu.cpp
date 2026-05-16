#include "horizontalmenu.h"
#include <QEvent>
#include <QActionEvent>
#include <QApplication>
#include <QScreen>

HorizontalMenu::HorizontalMenu(QWidget *parent)
    : QMenu(parent)
{
}

HorizontalMenu::HorizontalMenu(const QString &title, QWidget *parent)
    : QMenu(title, parent)
{
}

bool HorizontalMenu::event(QEvent *e)
{
    if (e->type() == QEvent::Show) {
        // Проверяем, есть ли родительское меню
        if (QWidget *parentWidget = qobject_cast<QWidget*>(parent())) {
            if (QMenu *parentMenu = qobject_cast<QMenu*>(parentWidget)) {
                // Это подменю - позиционируем его справа от родителя
                repositionSubMenu();
            }
        }
    }
    
    return QMenu::event(e);
}

void HorizontalMenu::repositionSubMenu()
{
    QWidget *parentWidget = qobject_cast<QWidget*>(parent());
    if (!parentWidget)
        return;

    QMenu *parentMenu = qobject_cast<QMenu*>(parentWidget);
    if (!parentMenu)
        return;

    // Получаем активное действие в родительском меню
    QAction *activeAction = parentMenu->activeAction();
    if (!activeAction)
        return;

    // Получаем геометрию активного действия
    QRect actionRect = parentMenu->actionGeometry(activeAction);
    
    // Получаем геометрию родительского меню
    QRect parentGeometry = parentMenu->geometry();
    
    // Вычисляем позицию для отображения подменю справа от активного действия
    int x = parentGeometry.left() + parentGeometry.width();
    int y = parentGeometry.top() + actionRect.y();
    
    // Определяем, с какой стороны экрана находится родительское меню
    QScreen *screen = QApplication::screenAt(parentGeometry.center());
    if (!screen)
        screen = QApplication::primaryScreen();
    
    QRect screenGeometry = screen->availableGeometry();
    
    // Проверяем, помещается ли меню справа
    if (x + width() > screenGeometry.right()) {
        // Если не помещается справа, пробуем слева
        x = parentGeometry.left() - width();
    }
    
    // Убедимся, что меню не выходит за верхнюю/нижнюю границу экрана
    if (y + height() > screenGeometry.bottom()) {
        y = screenGeometry.bottom() - height();
    }
    if (y < screenGeometry.top()) {
        y = screenGeometry.top();
    }
    
    move(x, y);
}
