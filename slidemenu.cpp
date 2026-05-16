#include "slidemenu.h"

#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QMenu>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QApplication>
#include <QScreen>
#include <QMouseEvent>
#include <QPainter>

SlideMenu::SlideMenu(QWidget *parent)
    : QFrame(parent, Qt::Popup | Qt::WindowStaysOnTopHint)
    , m_animation(nullptr)
    , m_currentDirection(LeftToRight)
{
    setupUi();
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAutoFillBackground(false);
    // Не используем setStyleSheet для фона, оставляем для дочерних элементов,
    // но они получат прозрачность через setPalette.
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::transparent);
    setPalette(pal);

    setFixedWidth(300);
    hide();
    installEventFilter(this);
}

SlideMenu::~SlideMenu()
{
    if (m_animation) delete m_animation;
}

void SlideMenu::setupUi()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(12, 12, 12, 12);
    m_mainLayout->setSpacing(8);
    // НЕТ QSpacerItem или stretch – элементы будут прижаты к верху

    m_topLayout = new QHBoxLayout();
    m_topLayout->setContentsMargins(0, 0, 0, 0);

    m_iconLabel = new QLabel(this);
    m_iconLabel->setFixedSize(24, 24);
    m_iconLabel->setScaledContents(true);
    m_topLayout->addWidget(m_iconLabel);

    m_topLayout->addStretch();

    m_closeButton = new QPushButton("✕", this);
    m_closeButton->setFixedSize(24, 24);
    m_closeButton->setStyleSheet(
                "QPushButton { border: none; background: transparent; font-size: 16px; color: #ccc; }"
                "QPushButton:hover { color: white; background: rgba(255,255,255,0.2); border-radius: 4px; }"
                );
    connect(m_closeButton, &QPushButton::clicked, this, &SlideMenu::onCloseClicked);
    m_topLayout->addWidget(m_closeButton);

    m_mainLayout->addLayout(m_topLayout);
    addSeparator();
}

void SlideMenu::installShadow()
{
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(20);
    shadow->setColor(QColor(0, 0, 0, 120));
    shadow->setOffset(0, 4);
    setGraphicsEffect(shadow);
}

void SlideMenu::addSeparator()
{
    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setStyleSheet("background-color: rgba(255,255,255,40); max-height: 1px; margin: 4px 0;");
    m_mainLayout->addWidget(line);
}

void SlideMenu::addLabel(const QString &text)
{
    QLabel *label = new QLabel(text, this);
    label->setStyleSheet("font-weight: bold; padding: 4px 0;");
    m_mainLayout->addWidget(label);
}

void SlideMenu::addButton(const QString &text, const std::function<void()> &callback)
{
    QPushButton *btn = new QPushButton(text, this);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setStyleSheet(
                "QPushButton { text-align: left; padding: 8px; background: transparent; border: none; }"
                "QPushButton:hover { background: rgba(255,255,255,0.2); border-radius: 4px; }"
                );
    if (callback) {
        connect(btn, &QPushButton::clicked, this, [callback, this]() {
            callback();
            hideWithAnimation();
        });
    }
    m_mainLayout->addWidget(btn);
}

void SlideMenu::addCheckbox(const QString &text)
{
    QCheckBox *chk = new QCheckBox(text, this);
    chk->setStyleSheet(
                "QCheckBox { spacing: 8px; }"
                "QCheckBox::indicator { width: 16px; height: 16px; }"
                );
    m_mainLayout->addWidget(chk);
}

void SlideMenu::addMenuButton(const QString &text, QMenu *menu)
{
    QPushButton *btn = new QPushButton(text, this);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setStyleSheet(
                "QPushButton { text-align: left; padding: 8px; background: transparent; border: none; }"
                "QPushButton:hover { background: rgba(255,255,255,0.2); border-radius: 4px; }"
                );

    // Показываем меню вручную при клике
    connect(btn, &QPushButton::clicked, this, [this, btn, menu]() {
        QPoint btnTopLeft = btn->mapToGlobal(QPoint(0, 0)); // верхний левый угол кнопки
        int menuWidth = menu->sizeHint().width();

        if (m_currentDirection == LeftToRight) {
            // Подменю СПРАВА от кнопки, на том же уровне по вертикали
            menu->popup(btnTopLeft + QPoint(btn->width(), 0));
        } else {
            // Подменю СЛЕВА от кнопки, на том же уровне
            menu->popup(btnTopLeft + QPoint(-menuWidth, 0));
        }
    });

    m_mainLayout->addWidget(btn);
}

void SlideMenu::setIconLabel(const QIcon &icon)
{
    m_iconLabel->setPixmap(icon.pixmap(m_iconLabel->size()));
}

void SlideMenu::showWithDirection(const QPoint &anchorTopLeft, Direction dir)
{
    m_currentDirection = dir;
    adjustSize();   // подстраиваем высоту под содержимое

    int menuWidth = width();
    int menuHeight = height();

    QPoint finalPos = anchorTopLeft;

    // Коррекция, чтобы не вылезать за экран
    QRect screenGeo = QApplication::primaryScreen()->availableGeometry();
    if (finalPos.x() + menuWidth > screenGeo.right())
        finalPos.setX(screenGeo.right() - menuWidth);
    if (finalPos.y() + menuHeight > screenGeo.bottom())
        finalPos.setY(screenGeo.bottom() - menuHeight);
    if (finalPos.x() < 0) finalPos.setX(0);
    if (finalPos.y() < 0) finalPos.setY(0);

    // Начальная позиция для анимации (выезд из-за края окна)
    QPoint startPos = finalPos;
    if (dir == LeftToRight) {
        startPos.setX(finalPos.x() - menuWidth);
    } else {
        startPos.setX(finalPos.x() + menuWidth);
    }

    m_startPos = startPos;
    m_endPos = finalPos;

    if (m_animation) {
        m_animation->stop();
        delete m_animation;
        m_animation = nullptr;
    }

    move(m_startPos);
    show();
    startAnimation(true);
}

void SlideMenu::hideWithAnimation()
{
    if (m_animation && m_animation->state() == QAbstractAnimation::Running)
        m_animation->stop();
    if (!isVisible()) return;
    startAnimation(false);
}

void SlideMenu::startAnimation(bool show)
{
    QPoint start = show ? m_startPos : m_endPos;
    QPoint end   = show ? m_endPos   : m_startPos;

    m_animation = new QPropertyAnimation(this, "pos", this);
    m_animation->setDuration(250);
    m_animation->setEasingCurve(QEasingCurve::OutCubic);
    m_animation->setStartValue(start);
    m_animation->setEndValue(end);

    if (!show) {
        connect(m_animation, &QPropertyAnimation::finished, this, &SlideMenu::onAnimationFinished);
    }
    m_animation->start();
}

bool SlideMenu::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (!rect().contains(mouseEvent->pos())) {
            hideWithAnimation();
            return true;
        }
    }
    return QFrame::eventFilter(obj, event);
}

void SlideMenu::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Полупрозрачный фон: цвет 40,40,45 с альфа 100 (0–255) для лучшей видимости элементов под меню
    QColor bgColor(40, 40, 45, 100);
    painter.setBrush(bgColor);
    painter.setPen(Qt::NoPen);

    // Закруглённые углы
    painter.drawRoundedRect(rect(), 8, 8);
}

void SlideMenu::onAnimationFinished()
{
    if (pos() == m_startPos && m_startPos != m_endPos) {
        hide();
        emit closed();
    }
    if (m_animation) {
        m_animation->deleteLater();
        m_animation = nullptr;
    }
}

void SlideMenu::onCloseClicked()
{
    hideWithAnimation();
}
