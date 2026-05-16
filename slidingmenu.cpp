#include "slidingmenu.h"
#include <QHBoxLayout>
#include <QFrame>
#include <QScrollArea>
#include <QResizeEvent>
#include <QShowEvent>
#include <QHideEvent>

SlidingMenu::SlidingMenu(QWidget *parent, SlideDirection direction, int menuWidth)
    : QWidget(parent)
    , m_direction(direction)
    , m_menuWidth(menuWidth)
    , m_iconLabel(nullptr)
    , m_titleLabel(nullptr)
    , m_closeButton(nullptr)
    , m_checkBox(nullptr)
    , m_menu(nullptr)
    , m_separator(nullptr)
    , m_contentLayout(nullptr)
    , m_animation(nullptr)
    , m_isVisible(false)
{
    // Убираем любую прозрачность на уровне виджета
    setAttribute(Qt::WA_StyledBackground, true);
    
    setupUi();
    setupAnimations();
    applyStyles();
    installParentEventFilter();
    
    // Начальная позиция (скрыто)
    if (m_direction == SlideDirection::FromLeft) {
        move(-m_menuWidth, 0);
    } else {
        move(parent->width(), 0);
    }
}

SlidingMenu::~SlidingMenu()
{
    delete m_animation;
}

void SlidingMenu::setupUi()
{
    // Устанавливаем фиксированную ширину, но высоту по родителю
    setFixedWidth(m_menuWidth);
    updateMenuHeight();
    
    // Основной layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // Верхняя панель с иконкой и кнопкой закрытия
    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->setContentsMargins(10, 10, 10, 10);
    topLayout->setSpacing(10);
    
    // Иконка (слева вверху)
    m_iconLabel = new QLabel(this);
    m_iconLabel->setFixedSize(32, 32);
    m_iconLabel->setStyleSheet("background-color: transparent;");
    topLayout->addWidget(m_iconLabel);
    
    // Заголовок
    m_titleLabel = new QLabel("Menu", this);
    m_titleLabel->setStyleSheet("color: white; font-size: 18px; font-weight: bold;");
    m_titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    topLayout->addWidget(m_titleLabel, 1);
    
    // Кнопка закрытия (справа вверху)
    m_closeButton = new QPushButton("✕", this);
    m_closeButton->setFixedSize(30, 30);
    m_closeButton->setCursor(Qt::PointingHandCursor);
    connect(m_closeButton, &QPushButton::clicked, this, &SlidingMenu::onCloseClicked);
    topLayout->addWidget(m_closeButton);
    
    mainLayout->addLayout(topLayout);
    
    // Скролл область для контента
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("border: none; background-color: transparent;");
    
    QWidget *contentWidget = new QWidget();
    m_contentLayout = new QVBoxLayout(contentWidget);
    m_contentLayout->setContentsMargins(15, 10, 15, 10);
    m_contentLayout->setSpacing(15);
    
    // Разделитель после заголовка
    QFrame *separator1 = new QFrame(contentWidget);
    separator1->setFrameShape(QFrame::HLine);
    separator1->setStyleSheet("background-color: rgba(255, 255, 255, 100); min-height: 1px; max-height: 1px;");
    m_contentLayout->addWidget(separator1);
    
    // Контейнер для кнопок
    QVBoxLayout *buttonsLayout = new QVBoxLayout();
    buttonsLayout->setSpacing(8);
    m_contentLayout->addLayout(buttonsLayout);
    
    // Создаем 3 кнопки по умолчанию
    for (int i = 0; i < 3; ++i) {
        QPushButton *btn = new QPushButton(QString("Button %1").arg(i + 1), contentWidget);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet(
            "QPushButton {"
            "   background-color: rgba(255, 255, 255, 50);"
            "   color: white;"
            "   border: none;"
            "   padding: 10px;"
            "   text-align: left;"
            "   border-radius: 5px;"
            "}"
            "QPushButton:hover {"
            "   background-color: rgba(255, 255, 255, 80);"
            "}"
        );
        buttonsLayout->addWidget(btn);
        m_buttons.append(btn);
    }
    
    m_contentLayout->addLayout(buttonsLayout);
    
    // Чекбокс с подписью
    m_checkBox = new QCheckBox("Enable feature", contentWidget);
    m_checkBox->setCursor(Qt::PointingHandCursor);
    m_checkBox->setStyleSheet(
        "QCheckBox {"
        "   color: white;"
        "   font-size: 14px;"
        "   spacing: 8px;"
        "}"
        "QCheckBox::indicator {"
        "   width: 18px;"
        "   height: 18px;"
        "   border-radius: 4px;"
        "   border: 2px solid rgba(255, 255, 255, 150);"
        "   background-color: transparent;"
        "}"
        "QCheckBox::indicator:checked {"
        "   background-color: rgba(100, 200, 100, 200);"
        "}"
    );
    connect(m_checkBox, &QCheckBox::toggled, this, &SlidingMenu::checkBoxToggled);
    m_contentLayout->addWidget(m_checkBox);
    
    // Разделитель перед меню
    QFrame *separator2 = new QFrame(contentWidget);
    separator2->setFrameShape(QFrame::HLine);
    separator2->setStyleSheet("background-color: rgba(255, 255, 255, 100); min-height: 1px; max-height: 1px;");
    m_contentLayout->addWidget(separator2);
    m_separator = separator2;
    
    // QMenu с возможностью создания подменю
    m_menu = new QMenu(contentWidget);
    m_menu->setTitle("Actions Menu");
    m_menu->setStyleSheet(
        "QMenu {"
        "   background-color: rgba(50, 50, 50, 200);"
        "   color: white;"
        "   border: 1px solid rgba(255, 255, 255, 50);"
        "   border-radius: 5px;"
        "   padding: 5px;"
        "}"
        "QMenu::item {"
        "   padding: 8px 20px;"
        "   border-radius: 3px;"
        "}"
        "QMenu::item:selected {"
        "   background-color: rgba(255, 255, 255, 30);"
        "}"
        "QMenu::separator {"
        "   height: 1px;"
        "   background: rgba(255, 255, 255, 50);"
        "   margin: 5px;"
        "}"
    );
    
    // Добавляем действия в меню с подменю
    QAction *action1 = m_menu->addAction("Action 1");
    QAction *action2 = m_menu->addAction("Action 2");
    
    QMenu *subMenu = m_menu->addMenu("Submenu");
    subMenu->addAction("Sub Action 1");
    subMenu->addAction("Sub Action 2");
    
    QMenu *nestedSubMenu = subMenu->addMenu("Nested Submenu");
    nestedSubMenu->addAction("Nested Action 1");
    nestedSubMenu->addAction("Nested Action 2");
    
    m_menu->addSeparator();
    m_menu->addAction("Action 3");
    
    // Кнопка для открытия меню
    QPushButton *menuButton = new QPushButton("◁ Actions Menu ▷", contentWidget);
    menuButton->setCursor(Qt::PointingHandCursor);
    menuButton->setStyleSheet(
        "QPushButton {"
        "   background-color: rgba(255, 255, 255, 50);"
        "   color: white;"
        "   border: none;"
        "   padding: 10px;"
        "   border-radius: 5px;"
        "   text-align: left;"
        "}"
        "QPushButton:hover {"
        "   background-color: rgba(255, 255, 255, 80);"
        "}"
    );
    connect(menuButton, &QPushButton::clicked, [this, menuButton]() {
        m_menu->exec(menuButton->mapToGlobal(QPoint(menuButton->width(), 0)));
    });
    m_contentLayout->addWidget(menuButton);
    
    m_contentLayout->addStretch();
    
    scrollArea->setWidget(contentWidget);
    mainLayout->addWidget(scrollArea, 1);
    
    setLayout(mainLayout);
}

void SlidingMenu::setupAnimations()
{
    // Анимация перемещения
    m_animation = new QPropertyAnimation(this, "menuOffset", this);
    m_animation->setDuration(300);
    m_animation->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_animation, &QPropertyAnimation::finished, this, &SlidingMenu::onAnimationFinished);
}

void SlidingMenu::applyStyles()
{    
    // Фон в стиле GitHub - полностью непрозрачный
    setStyleSheet(
        "SlidingMenu {"
        "   background-color: rgba(0, 0, 0, 240);"
        "   border: none;"
        "}"
    );
}

int SlidingMenu::menuOffset() const
{
    return pos().x();
}

void SlidingMenu::setMenuOffset(int offset)
{
    move(offset, y());
}

void SlidingMenu::setTitle(const QString &title)
{
    if (m_titleLabel) {
        m_titleLabel->setText(title);
    }
}

void SlidingMenu::setIcon(const QPixmap &icon)
{
    if (m_iconLabel) {
        m_iconLabel->setPixmap(icon.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

void SlidingMenu::addButtons(const QStringList &buttonTexts)
{
    // Удаляем существующие кнопки
    for (QPushButton *btn : m_buttons) {
        btn->deleteLater();
    }
    m_buttons.clear();
    
    if (!m_contentLayout) return;
    
    // Находим layout с кнопками и очищаем его
    QLayoutItem *item = m_contentLayout->itemAt(1);
    if (item && item->layout()) {
        QVBoxLayout *buttonsLayout = qobject_cast<QVBoxLayout*>(item->layout());
        if (buttonsLayout) {
            QLayoutItem *child;
            while ((child = buttonsLayout->takeAt(0)) != nullptr) {
                delete child->widget();
                delete child;
            }
        }
    }
    
    // Добавляем новые кнопки
    for (const QString &text : buttonTexts) {
        QPushButton *btn = new QPushButton(text, findChild<QScrollArea*>()->widget());
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet(
            "QPushButton {"
            "   background-color: rgba(255, 255, 255, 50);"
            "   color: white;"
            "   border: none;"
            "   padding: 10px;"
            "   text-align: left;"
            "   border-radius: 5px;"
            "}"
            "QPushButton:hover {"
            "   background-color: rgba(255, 255, 255, 80);"
            "}"
        );
        
        // Вставляем кнопки в существующий buttonsLayout или добавляем напрямую
        QLayoutItem *buttonsItem = m_contentLayout->itemAt(1);
        if (buttonsItem && buttonsItem->layout()) {
            QVBoxLayout *buttonsLayout = qobject_cast<QVBoxLayout*>(buttonsItem->layout());
            if (buttonsLayout) {
                buttonsLayout->addWidget(btn);
            } else {
                m_contentLayout->insertWidget(1, btn);
            }
        } else {
            m_contentLayout->insertWidget(1, btn);
        }
        m_buttons.append(btn);
    }
}

void SlidingMenu::setCheckBoxText(const QString &text)
{
    if (m_checkBox) {
        m_checkBox->setText(text);
    }
}

void SlidingMenu::setCheckBoxChecked(bool checked)
{
    if (m_checkBox) {
        m_checkBox->setChecked(checked);
    }
}

void SlidingMenu::setMenu(QMenu *menu)
{
    m_menu = menu;
}

// Универсальные методы для добавления произвольных виджетов

void SlidingMenu::addWidget(QWidget *widget, int stretch)
{
    if (m_contentLayout && widget) {
        m_contentLayout->addWidget(widget, stretch);
        widget->setParent(findChild<QScrollArea*>()->widget());
    }
}

void SlidingMenu::addButton(QPushButton *button)
{
    if (m_contentLayout && button) {
        // Применяем стандартный стиль кнопок меню
        button->setCursor(Qt::PointingHandCursor);
        if (button->styleSheet().isEmpty()) {
            button->setStyleSheet(
                "QPushButton {"
                "   background-color: rgba(255, 255, 255, 50);"
                "   color: white;"
                "   border: none;"
                "   padding: 10px;"
                "   text-align: left;"
                "   border-radius: 5px;"
                "}"
                "QPushButton:hover {"
                "   background-color: rgba(255, 255, 255, 80);"
                "}"
            );
        }
        m_contentLayout->addWidget(button);
        m_buttons.append(button);
    }
}

void SlidingMenu::addLayout(QLayout *layout, int stretch)
{
    if (m_contentLayout && layout) {
        m_contentLayout->addLayout(layout, stretch);
    }
}

void SlidingMenu::addSeparator()
{
    if (m_contentLayout) {
        QFrame *separator = new QFrame(findChild<QScrollArea*>()->widget());
        separator->setFrameShape(QFrame::HLine);
        separator->setStyleSheet("background-color: rgba(255, 255, 255, 100); min-height: 1px; max-height: 1px;");
        m_contentLayout->addWidget(separator);
    }
}

void SlidingMenu::clearContent()
{
    if (!m_contentLayout) return;
    
    // Очищаем все виджеты из layout, кроме кнопок в buttonsLayout
    QLayoutItem *item;
    while ((item = m_contentLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        } else if (item->layout()) {
            QLayoutItem *child;
            while ((child = item->layout()->takeAt(0)) != nullptr) {
                if (child->widget()) {
                    child->widget()->deleteLater();
                }
                delete child;
            }
            delete item->layout();
        }
        delete item;
    }
    m_buttons.clear();
}

QVBoxLayout* SlidingMenu::contentLayout() const
{
    return m_contentLayout;
}

void SlidingMenu::showMenu()
{
    if (m_isVisible) return;
    
    raise();
    show();
    
    int endPos, startPos;
    if (m_direction == SlideDirection::FromLeft) {
        startPos = -m_menuWidth;
        endPos = 0;
    } else {
        startPos = parentWidget() ? parentWidget()->width() : width();
        endPos = (parentWidget() ? parentWidget()->width() : width()) - m_menuWidth;
    }
    
    m_animation->setStartValue(startPos);
    m_animation->setEndValue(endPos);
    m_animation->start();
    
    m_isVisible = true;
}

void SlidingMenu::hideMenu()
{
    if (!m_isVisible) return;
    
    int endPos, startPos;
    if (m_direction == SlideDirection::FromLeft) {
        startPos = 0;
        endPos = -m_menuWidth;
    } else {
        startPos = (parentWidget() ? parentWidget()->width() : width()) - m_menuWidth;
        endPos = parentWidget() ? parentWidget()->width() : width();
    }
    
    m_animation->setStartValue(startPos);
    m_animation->setEndValue(endPos);
    m_animation->start();
    
    m_isVisible = false;
}

bool SlidingMenu::isMenuVisible() const
{
    return m_isVisible;
}

void SlidingMenu::onAnimationFinished()
{
    if (!m_isVisible) {
        hide();
        emit menuHidden();
    } else {
        emit menuShown();
    }
}

void SlidingMenu::onCloseClicked()
{
    hideMenu();
}

void SlidingMenu::installParentEventFilter()
{
    if (parentWidget()) {
        parentWidget()->installEventFilter(this);
    }
}

bool SlidingMenu::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == parentWidget() && event->type() == QEvent::Resize) {
        updateMenuHeight();
    }
    return QWidget::eventFilter(obj, event);
}

void SlidingMenu::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);
    updateMenuHeight();
}

void SlidingMenu::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateMenuHeight();
}

void SlidingMenu::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    updateMenuHeight();
}

void SlidingMenu::updateMenuHeight()
{
    if (parentWidget()) {
        setFixedHeight(parentWidget()->height());
    }
}
