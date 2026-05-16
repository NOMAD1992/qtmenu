#include "usermenu.h"
#include <QHBoxLayout>
#include <QFrame>
#include <QResizeEvent>
#include <QShowEvent>
#include <QHideEvent>
#include <QApplication>
#include <QDebug>

UserMenu::UserMenu(QWidget *parent, int menuWidth)
    : QWidget(parent)
    , m_menuWidth(menuWidth)
    , m_contentLayout(nullptr)
    , m_animation(nullptr)
    , m_isVisible(false)
{
    setAttribute(Qt::WA_StyledBackground, true);
    
    // Убираем рамку окна
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    
    setupUi();
    setupAnimations();
    applyStyles();
}

UserMenu::~UserMenu()
{
    delete m_animation;
}

void UserMenu::setupUi()
{
    // Основной layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // Контент
    m_contentLayout = new QVBoxLayout();
    m_contentLayout->setContentsMargins(8, 4, 8, 4);
    m_contentLayout->setSpacing(4);
    
    mainLayout->addLayout(m_contentLayout);
    
    setLayout(mainLayout);
}

void UserMenu::setupAnimations()
{
    // Анимация прозрачности и перемещения
    m_animation = new QPropertyAnimation(this, "windowOpacity", this);
    m_animation->setDuration(150);
    m_animation->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_animation, &QPropertyAnimation::finished, this, &UserMenu::onAnimationFinished);
}

void UserMenu::applyStyles()
{    
    // Фон в стиле GitHub - темный с закругленными углами
    setStyleSheet(
        "UserMenu {"
        "   background-color: rgba(30, 30, 30, 250);"
        "   border: 1px solid rgba(255, 255, 255, 60);"
        "   border-radius: 8px;"
        "}"
    );
}

QPushButton* UserMenu::createButton(const QString &text, const QPixmap &icon, QWidget *parent)
{
    QPushButton *btn = new QPushButton(text, parent);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    btn->setMinimumHeight(32);
    
    if (!icon.isNull()) {
        btn->setIcon(icon);
        btn->setIconSize(QSize(16, 16));
    }
    
    btn->setStyleSheet(
        "QPushButton {"
        "   background-color: transparent;"
        "   color: white;"
        "   border: none;"
        "   padding: 6px 12px;"
        "   text-align: left;"
        "   border-radius: 4px;"
        "   font-size: 13px;"
        "}"
        "QPushButton:hover {"
        "   background-color: rgba(255, 255, 255, 30);"
        "}"
        "QPushButton:pressed {"
        "   background-color: rgba(255, 255, 255, 20);"
        "}"
    );
    return btn;
}

QPushButton* UserMenu::addButton(const QString &text, const QPixmap &icon)
{
    if (!m_contentLayout) return nullptr;
    
    QPushButton *btn = createButton(text, icon, this);
    m_contentLayout->addWidget(btn);
    
    // Подстраиваем высоту под содержимое
    updateMenuGeometry();
    
    return btn;
}

void UserMenu::addSplitter()
{
    if (!m_contentLayout) return;
    
    QFrame *separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    separator->setStyleSheet("background-color: rgba(255, 255, 255, 80); min-height: 1px; max-height: 1px; margin: 4px 0;");
    m_contentLayout->addWidget(separator);
    
    // Подстраиваем высоту под содержимое
    updateMenuGeometry();
}

int UserMenu::menuOffset() const
{
    return pos().y();
}

void UserMenu::setMenuOffset(int offset)
{
    move(x(), offset);
}

void UserMenu::showMenu()
{   
    show();
    raise();
    activateWindow();
    
    // Анимация появления
    setWindowOpacity(0.0);
    m_animation->setStartValue(0.0);
    m_animation->setEndValue(1.0);
    m_animation->start();
    
    m_isVisible = true;
}

void UserMenu::hideMenu()
{
    if (!m_isVisible) return;
    m_isVisible = false;
    
    // Анимация исчезновения
    m_animation->setStartValue(1.0);
    m_animation->setEndValue(0.0);
    m_animation->start();
}

bool UserMenu::isMenuVisible() const
{
    return m_isVisible;
}

void UserMenu::onAnimationFinished()
{
    if (!m_isVisible) {
        hide();
        emit menuHidden();
    } else {
        emit menuShown();
    }
}

void UserMenu::updateMenuGeometry()
{
    // Пересчитываем высоту и ширину на основе содержимого
    if (m_contentLayout) {
        int contentHeight = m_contentLayout->sizeHint().height() + 8; // + отступы
        setFixedHeight(qMin(contentHeight, 400)); // Максимум 400px
        
        // Вычисляем оптимальную ширину по самому длинному элементу
        int optimalWidth = calculateOptimalWidth();
        setFixedWidth(optimalWidth);
        
        adjustSize();
    }
}

int UserMenu::calculateOptimalWidth() const
{
    int maxWidth = m_menuWidth; // Минимальная ширина из параметра конструктора
    
    // Проходим по всем виджетам в layout и находим максимальную ширину
    if (m_contentLayout) {
        for (int i = 0; i < m_contentLayout->count(); ++i) {
            QLayoutItem *item = m_contentLayout->itemAt(i);
            if (item && item->widget()) {
                // Получаем предпочитаемую ширину виджета
                int widgetWidth = item->widget()->sizeHint().width();
                if (widgetWidth > maxWidth) {
                    maxWidth = widgetWidth;
                }
            }
        }
    }
    
    // Добавляем отступы layout (левый и правый по 8px каждый)
    maxWidth += 16;
    
    return maxWidth;
}

void UserMenu::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    updateMenuGeometry();
}

void UserMenu::hideEvent(QHideEvent *event)
{
    m_isVisible = false;
    QWidget::hideEvent(event);
}
