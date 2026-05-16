#include "slidemenu.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QStyleOption>
#include <QDebug>
#include <QPainterPath>

SlideMenu::SlideMenu(QWidget *parent)
    : QWidget(parent)
    , m_direction(SlideInFromRight)
    , m_autoHide(false)
    , m_isVisible(false)
    , m_animationDuration(300)
    , m_contentWidget(nullptr)
    , m_mainLayout(nullptr)
    , m_contentLayout(nullptr)
    , m_closeButton(nullptr)
    , m_posAnimation(nullptr)
    , m_opacityAnimation(nullptr)
    , m_animationGroup(nullptr)
    , m_opacityEffect(nullptr)
{
    // Устанавливаем флаги для прозрачности и отсутствия рамки
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    
    // Включаем поддержку прозрачности
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setAutoFillBackground(false);
    
    // Устанавливаем фиксированную ширину меню
    setFixedWidth(280);
    
    setupUI();
    createAnimations();
}

SlideMenu::~SlideMenu()
{
    if (m_animationGroup) {
        delete m_animationGroup;
    }
}

void SlideMenu::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);
    
    // Создаем виджет для кнопки закрытия
    QWidget* headerWidget = new QWidget(this);
    QHBoxLayout* headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(10, 10, 10, 10);
    headerLayout->addStretch();
    
    // Кнопка закрытия
    m_closeButton = new QPushButton("×", headerWidget);
    m_closeButton->setFixedSize(32, 32);
    m_closeButton->setStyleSheet(
        "QPushButton { "
        "   background-color: rgba(255, 255, 255, 60); "
        "   color: white; "
        "   border-radius: 16px; "
        "   font-size: 18px; "
        "   font-weight: bold; "
        "   border: none; "
        "}"
        "QPushButton:hover { "
        "   background-color: rgba(255, 100, 100, 180); "
        "}"
    );
    headerLayout->addWidget(m_closeButton);
    
    m_mainLayout->addWidget(headerWidget);
    
    // Создаем контейнер для содержимого
    m_contentWidget = new QWidget(this);
    m_contentLayout = new QVBoxLayout(m_contentWidget);
    m_contentLayout->setContentsMargins(15, 10, 15, 15);
    m_contentLayout->setSpacing(8);
    
    // Добавляем растягивающий элемент вниз
    m_contentLayout->addStretch();
    
    m_mainLayout->addWidget(m_contentWidget);
    m_mainLayout->addStretch();
    
    // Подключаем сигнал кнопки закрытия
    connect(m_closeButton, &QPushButton::clicked, this, &SlideMenu::hideAnimated);
}

void SlideMenu::createAnimations()
{
    // Создаем эффект прозрачности
    m_opacityEffect = new QGraphicsOpacityEffect(this);
    setGraphicsEffect(m_opacityEffect);
    m_opacityEffect->setOpacity(1.0);
    
    // Создаем анимацию позиции
    m_posAnimation = new QPropertyAnimation(this, "geometry", this);
    m_posAnimation->setDuration(m_animationDuration);
    m_posAnimation->setEasingCurve(QEasingCurve::OutCubic);
    
    // Создаем анимацию прозрачности
    m_opacityAnimation = new QPropertyAnimation(m_opacityEffect, "opacity", this);
    m_opacityAnimation->setDuration(m_animationDuration);
    m_opacityAnimation->setEasingCurve(QEasingCurve::OutCubic);
    
    // Создаем группу анимаций
    m_animationGroup = new QParallelAnimationGroup(this);
    m_animationGroup->addAnimation(m_posAnimation);
    m_animationGroup->addAnimation(m_opacityAnimation);
    
    connect(m_animationGroup, &QParallelAnimationGroup::finished, [this]() {
        if (!m_isVisible) {
            hide();
            emit hidden();
        }
    });
}

void SlideMenu::slideIn(SlideDirection direction)
{
    if (m_isVisible) return;
    
    m_direction = direction;
    adjustPosition();
    
    // Показываем виджет перед анимацией
    show();
    raise();
    activateWindow();
    
    QRect startGeometry = geometry();
    QRect endGeometry = geometry();
    
    // Вычисляем начальную позицию в зависимости от направления
    switch(direction) {
    case SlideInFromRight:
        startGeometry.moveLeft(parentWidget()->width());
        break;
    case SlideInFromLeft:
        startGeometry.moveLeft(-width());
        break;
    case SlideInFromTop:
        startGeometry.moveTop(parentWidget()->height());
        break;
    case SlideInFromBottom:
        startGeometry.moveTop(-height());
        break;
    }
    
    // Настраиваем анимацию позиции
    m_posAnimation->setStartValue(startGeometry);
    m_posAnimation->setEndValue(endGeometry);
    
    // Настраиваем анимацию прозрачности
    m_opacityAnimation->setStartValue(0.0);
    m_opacityAnimation->setEndValue(1.0);
    
    m_isVisible = true;
    m_animationGroup->start();
}

void SlideMenu::hideAnimated()
{
    if (!m_isVisible) return;
    
    QRect startGeometry = geometry();
    QRect endGeometry = geometry();
    
    // Вычисляем конечную позицию в зависимости от направления
    switch(m_direction) {
    case SlideInFromRight:
        endGeometry.moveLeft(parentWidget()->width());
        break;
    case SlideInFromLeft:
        endGeometry.moveLeft(-width());
        break;
    case SlideInFromTop:
        endGeometry.moveTop(parentWidget()->height());
        break;
    case SlideInFromBottom:
        endGeometry.moveTop(-height());
        break;
    }
    
    // Настраиваем анимацию позиции
    m_posAnimation->setStartValue(startGeometry);
    m_posAnimation->setEndValue(endGeometry);
    
    // Настраиваем анимацию прозрачности
    m_opacityAnimation->setStartValue(1.0);
    m_opacityAnimation->setEndValue(0.0);
    
    m_isVisible = false;
    m_animationGroup->start();
}

void SlideMenu::adjustPosition()
{
    QWidget* parent = parentWidget();
    if (!parent) return;
    
    // Получаем глобальную позицию родительского окна
    QPoint parentPos = parent->mapToGlobal(QPoint(0, 0));
    QRect screenGeometry = QApplication::desktop()->screenGeometry(parent);
    
    int x = 0, y = 0;
    int w = width(), h = height();
    
    // Устанавливаем высоту равной высоте родительского окна
    h = parent->height();
    
    switch(m_direction) {
    case SlideInFromRight:
        x = parentPos.x() + parent->width() - w;
        y = parentPos.y();
        break;
    case SlideInFromLeft:
        x = parentPos.x();
        y = parentPos.y();
        break;
    case SlideInFromTop:
        x = parentPos.x();
        y = parentPos.y();
        w = parent->width();
        h = 400; // Фиксированная высота для верхнего/нижнего меню
        break;
    case SlideInFromBottom:
        x = parentPos.x();
        y = parentPos.y() + parent->height() - h;
        w = parent->width();
        h = 400;
        break;
    }
    
    setGeometry(x, y, w, h);
}

void SlideMenu::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Рисуем полупрозрачный фон с альфа-каналом 90
    QColor bgColor(30, 30, 35, 90);
    
    // Скругленные углы
    QPainterPath path;
    path.addRoundedRect(rect(), 10, 10);
    
    painter.fillPath(path, bgColor);
}

void SlideMenu::mousePressEvent(QMouseEvent* event)
{
    if (m_autoHide && !m_closeButton->underMouse()) {
        // Проверяем, кликнули ли мы вне содержимого меню
        bool clickedOnContent = false;
        for (QWidget* child : findChildren<QWidget*>()) {
            if (child->geometry().contains(child->mapFromGlobal(event->globalPos()))) {
                clickedOnContent = true;
                break;
            }
        }
        
        if (!clickedOnContent) {
            hideAnimated();
        }
    }
    QWidget::mousePressEvent(event);
}

void SlideMenu::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    // При изменении размера обновляем позицию если меню видимо
    if (m_isVisible) {
        adjustPosition();
    }
}

void SlideMenu::setAutoHide(bool autoHide)
{
    m_autoHide = autoHide;
}

bool SlideMenu::isMenuVisible() const
{
    return m_isVisible;
}

void SlideMenu::addLabel(const QString& text)
{
    QLabel* label = new QLabel(text, m_contentWidget);
    label->setStyleSheet(
        "QLabel { "
        "   color: white; "
        "   font-size: 14px; "
        "   font-weight: bold; "
        "   padding: 5px 0; "
        "}"
    );
    
    // Вставляем перед stretch
    int index = m_contentLayout->count() > 0 ? m_contentLayout->count() - 1 : 0;
    m_contentLayout->insertWidget(index, label);
}

void SlideMenu::addButton(const QString& text)
{
    QPushButton* button = new QPushButton(text, m_contentWidget);
    button->setStyleSheet(
        "QPushButton { "
        "   text-align: left; "
        "   padding: 12px 15px; "
        "   font-size: 14px; "
        "   color: white; "
        "   background-color: rgba(255, 255, 255, 40); "
        "   border: none; "
        "   border-radius: 6px; "
        "}"
        "QPushButton:hover { "
        "   background-color: rgba(255, 255, 255, 70); "
        "}"
        "QPushButton:pressed { "
        "   background-color: rgba(255, 255, 255, 90); "
        "}"
    );
    
    // Вставляем перед stretch
    int index = m_contentLayout->count() > 0 ? m_contentLayout->count() - 1 : 0;
    m_contentLayout->insertWidget(index, button);
}

void SlideMenu::addCheckBox(const QString& text)
{
    QCheckBox* checkBox = new QCheckBox(text, m_contentWidget);
    checkBox->setStyleSheet(
        "QCheckBox { "
        "   color: white; "
        "   font-size: 14px; "
        "   spacing: 8px; "
        "   padding: 8px 0; "
        "}"
        "QCheckBox::indicator { "
        "   width: 18px; "
        "   height: 18px; "
        "   border-radius: 4px; "
        "   background-color: rgba(255, 255, 255, 40); "
        "   border: 1px solid rgba(255, 255, 255, 60); "
        "}"
        "QCheckBox::indicator:checked { "
        "   background-color: rgba(100, 200, 100, 180); "
        "   border: 1px solid rgba(100, 200, 100, 220); "
        "}"
    );
    
    // Вставляем перед stretch
    int index = m_contentLayout->count() > 0 ? m_contentLayout->count() - 1 : 0;
    m_contentLayout->insertWidget(index, checkBox);
}

QMenu* SlideMenu::addMenu(const QString& title)
{
    QPushButton* menuButton = new QPushButton(title + " ▼", m_contentWidget);
    menuButton->setStyleSheet(
        "QPushButton { "
        "   text-align: left; "
        "   padding: 12px 15px; "
        "   font-size: 14px; "
        "   color: white; "
        "   background-color: rgba(255, 255, 255, 40); "
        "   border: none; "
        "   border-radius: 6px; "
        "}"
        "QPushButton:hover { "
        "   background-color: rgba(255, 255, 255, 70); "
        "}"
    );
    
    QMenu* menu = new QMenu(title, this);
    menu->setStyleSheet(
        "QMenu { "
        "   background-color: rgba(30, 30, 35, 230); "
        "   color: white; "
        "   border: 1px solid rgba(255, 255, 255, 50); "
        "   border-radius: 6px; "
        "   padding: 5px; "
        "}"
        "QMenu::item { "
        "   padding: 8px 20px; "
        "   border-radius: 4px; "
        "}"
        "QMenu::item:selected { "
        "   background-color: rgba(255, 255, 255, 60); "
        "}"
    );
    
    menuButton->setMenu(menu);
    
    // Подключаем сигналы действий меню
    connect(menu, &QMenu::triggered, this, &SlideMenu::onMenuActionTriggered);
    
    // Вставляем перед stretch
    int index = m_contentLayout->count() > 0 ? m_contentLayout->count() - 1 : 0;
    m_contentLayout->insertWidget(index, menuButton);
    
    return menu;
}

void SlideMenu::onMenuActionTriggered(QAction* action)
{
    emit menuActionTriggered(action);
}
