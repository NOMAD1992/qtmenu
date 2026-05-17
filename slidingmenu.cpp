#include "slidingmenu.h"
#include <QHBoxLayout>
#include <QFrame>
#include <QScrollArea>
#include <QResizeEvent>
#include <QShowEvent>
#include <QHideEvent>
// ◁
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
    m_iconLabel->setObjectName("iconLabel");
    m_iconLabel->setFixedSize(32, 32);
    topLayout->addWidget(m_iconLabel);
    
    // Заголовок
    m_titleLabel = new QLabel("Menu", this);
    m_titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    topLayout->addWidget(m_titleLabel, 1);
    
    // Кнопка закрытия (справа вверху)
    m_closeButton = new QPushButton("X", this);
    m_closeButton->setObjectName("closeButton");
    m_closeButton->setCursor(Qt::PointingHandCursor);
    connect(m_closeButton, &QPushButton::clicked, this, &SlidingMenu::onCloseClicked);
    topLayout->addWidget(m_closeButton);
    
    mainLayout->addLayout(topLayout);
    
    // Скролл область для контента
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    QWidget *contentWidget = new QWidget();
    m_contentLayout = new QVBoxLayout(contentWidget);
    m_contentLayout->setContentsMargins(15, 10, 15, 10);
    m_contentLayout->setSpacing(15);
    
    // Разделитель после заголовка
    QFrame *separator1 = new QFrame(contentWidget);
    separator1->setFrameShape(QFrame::HLine);
    m_contentLayout->addWidget(separator1);
    
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
    // Стили теперь загружаются из styles.qss в MainWindow
    // Эта функция может оставаться пустой или использоваться для специфичных стилей
}

QPushButton* SlidingMenu::createButton(const QString &text, const QPixmap &icon, QWidget *parent)
{
    QPushButton *btn = new QPushButton(text, parent);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    if (!icon.isNull()) {
        btn->setIcon(icon);
        btn->setIconSize(QSize(24, 24));
    }
    return btn;
}

QPushButton* SlidingMenu::addButton(const QString &text, const QPixmap &icon)
{
    QWidget *contentWidget = findChild<QScrollArea*>()->widget();
    if (!contentWidget || !m_contentLayout) return nullptr;
    
    // Удаляем stretch если есть (перед добавлением нового элемента)
    QLayoutItem *lastItem = m_contentLayout->itemAt(m_contentLayout->count() - 1);
    if (lastItem && lastItem->spacerItem()) {
        m_contentLayout->removeItem(lastItem);
        delete lastItem;
    }
    
    QPushButton *btn = createButton(text, icon, contentWidget);
    m_contentLayout->addWidget(btn);
    m_buttons.append(btn);
    
    m_contentLayout->addStretch();
    
    return btn;
}

QCheckBox* SlidingMenu::addCheckBox(const QString &text)
{
    QWidget *contentWidget = findChild<QScrollArea*>()->widget();
    if (!contentWidget || !m_contentLayout) return nullptr;
    
    // Удаляем stretch если есть
    QLayoutItem *lastItem = m_contentLayout->itemAt(m_contentLayout->count() - 1);
    if (lastItem && lastItem->spacerItem()) {
        m_contentLayout->removeItem(lastItem);
        delete lastItem;
    }
    
    m_checkBox = new QCheckBox(text, contentWidget);
    m_checkBox->setCursor(Qt::PointingHandCursor);
    connect(m_checkBox, &QCheckBox::toggled, this, &SlidingMenu::checkBoxToggled);
    m_contentLayout->addWidget(m_checkBox);
    
    m_contentLayout->addStretch();
    
    return m_checkBox;
}

QMenu* SlidingMenu::addMenu(const QString &title, const QPixmap &icon)
{
    QWidget *contentWidget = findChild<QScrollArea*>()->widget();
    if (!contentWidget || !m_contentLayout) return nullptr;
    
    // Удаляем stretch если есть
    QLayoutItem *lastItem = m_contentLayout->itemAt(m_contentLayout->count() - 1);
    if (lastItem && lastItem->spacerItem()) {
        m_contentLayout->removeItem(lastItem);
        delete lastItem;
    }
    
    // Разделитель перед меню
    QFrame *separator2 = new QFrame(contentWidget);
    separator2->setFrameShape(QFrame::HLine);
    m_contentLayout->addWidget(separator2);
    m_separator = separator2;
    
    // QMenu с возможностью создания подменю
    QMenu *menu = new QMenu(contentWidget);
    menu->setTitle(title);
    
    // Кнопка для открытия меню
    QPushButton *menuButton = createButton(QString("%1 ▷").arg(title), icon, contentWidget);
    connect(menuButton, &QPushButton::clicked, [menu, menuButton]() {
        menu->exec(menuButton->mapToGlobal(QPoint(menuButton->width(), 0)));
    });
    m_contentLayout->addWidget(menuButton);
    
    m_contentLayout->addStretch();
    
    return menu;
}

void SlidingMenu::addSplitter()
{
    QWidget *contentWidget = findChild<QScrollArea*>()->widget();
    if (!contentWidget || !m_contentLayout) return;
    
    // Удаляем stretch если есть
    QLayoutItem *lastItem = m_contentLayout->itemAt(m_contentLayout->count() - 1);
    if (lastItem && lastItem->spacerItem()) {
        m_contentLayout->removeItem(lastItem);
        delete lastItem;
    }
    
    QFrame *separator = new QFrame(contentWidget);
    separator->setFrameShape(QFrame::HLine);
    m_contentLayout->addWidget(separator);
    
    m_contentLayout->addStretch();
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
