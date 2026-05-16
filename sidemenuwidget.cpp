#include "sidemenuwidget.h"
#include <QPainter>
#include <QApplication>
#include <QScreen>
#include <QGuiApplication>
#include <QFrame>

// --- MenuActionItem Implementation ---

MenuActionItem::MenuActionItem(const QString &text, SideMenuWidget *parent)
    : QWidget(parent), m_subMenu(nullptr), m_parentMenu(parent), m_isActive(false) {
    setCursor(Qt::PointingHandCursor);
    
    m_label = new QLabel(text, this);
    m_label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_label->setStyleSheet("color: white; font-size: 14px; padding: 8px;");
    
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_label);
    setLayout(layout);
    setFixedHeight(36);
}

void MenuActionItem::setSubMenu(SideMenuWidget *menu) {
    m_subMenu = menu;
    // Добавляем индикатор наличия подменю (стрелочка)
    m_label->setText(m_label->text() + " ▸");
}

void MenuActionItem::setActive(bool active) {
    m_isActive = active;
    update();
}

void MenuActionItem::enterEvent(QEvent *event) {
    setActive(true);
    
    // Показываем подменю справа при наведении
    if (m_subMenu && m_parentMenu) {
        QPoint parentGlobalPos = m_parentMenu->mapToGlobal(pos());
        int menuWidth = m_parentMenu->width();
        
        // Проверяем, есть ли место справа
        QScreen *screen = QApplication::screenAt(QCursor::pos());
        QRect screenGeometry;
        if (screen) {
            screenGeometry = screen->availableGeometry();
        } else {
            screenGeometry = QGuiApplication::primaryScreen()->availableGeometry();
        }
        
        int subMenuX = parentGlobalPos.x() + menuWidth;
        
        // Если справа нет места, открываем слева
        if (subMenuX + m_subMenu->sizeHint().width() > screenGeometry.right()) {
            subMenuX = parentGlobalPos.x() - m_subMenu->sizeHint().width();
        }
        
        m_subMenu->move(subMenuX, parentGlobalPos.y());
        m_subMenu->show();
    }
    
    QWidget::enterEvent(event);
}

void MenuActionItem::leaveEvent(QEvent *event) {
    setActive(false);
    
    // Скрываем подменю с задержкой, чтобы успеть навести на него курсор
    if (m_subMenu) {
        QTimer::singleShot(200, m_subMenu, [this]() {
            if (!m_subMenu->underMouse() && !underMouse()) {
                m_subMenu->hide();
            }
        });
    }
    
    QWidget::leaveEvent(event);
}

void MenuActionItem::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    
    if (m_isActive) {
        painter.fillRect(rect(), QColor(50, 55, 65));
    } else {
        painter.fillRect(rect(), QColor(36, 41, 47, 230)); // 90% непрозрачности
    }
    
    QWidget::paintEvent(event);
}

void MenuActionItem::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    }
    QWidget::mousePressEvent(event);
}

// --- SideMenuWidget Implementation ---

SideMenuWidget::SideMenuWidget(QWidget *parent)
    : QWidget(parent), m_currentHovered(nullptr) {
    setWindowFlags(Qt::FramelessWindowHint | Qt::Popup | Qt::NoDropShadowWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, false);
    setAttribute(Qt::WA_NoSystemBackground);
    
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    setLayout(m_layout);
    
    setStyleSheet(R"(
        SideMenuWidget {
            background-color: rgba(36, 41, 47, 230);
            border: 1px solid #444d56;
            border-radius: 6px;
        }
    )");
}

MenuActionItem* SideMenuWidget::addAction(const QString &text) {
    auto *item = new MenuActionItem(text, this);
    m_items.append(item);
    m_layout->addWidget(item);
    
    connect(item, &MenuActionItem::clicked, this, [this, item]() {
        if (!item->subMenu()) {
            emit actionTriggered(item->text());
            hideAll();
        }
    });
    
    return item;
}

void SideMenuWidget::addSeparator() {
    auto *separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    separator->setStyleSheet("background-color: #444d56; margin: 4px 8px;");
    separator->setFixedHeight(1);
    m_layout->addWidget(separator);
}

void SideMenuWidget::showAt(const QPoint &globalPos) {
    move(globalPos);
    show();
    raise();
    activateWindow();
}

void SideMenuWidget::hideAll() {
    // Рекурсивно скрываем все подменю
    for (auto *item : m_items) {
        if (item->subMenu()) {
            item->subMenu()->hideAll();
        }
    }
    hide();
}

void SideMenuWidget::closeEvent(QCloseEvent *event) {
    hideAll();
    QWidget::closeEvent(event);
}
