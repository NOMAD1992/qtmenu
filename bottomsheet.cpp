#include "bottomsheet.h"
#include <QResizeEvent>
#include <QShowEvent>
#include <QHideEvent>

BottomSheet::BottomSheet(QWidget *parent)
    : QWidget(parent)
    , m_listView(nullptr)
    , m_mainLayout(nullptr)
    , m_animation(nullptr)
    , m_isVisible(false)
    , m_sheetHeight(200)  // Начальная высота шторки
{
    setAttribute(Qt::WA_StyledBackground, true);
    
    setupUi();
    setupAnimations();
    applyStyles();
    installParentEventFilter();
    
    // Начальная позиция (скрыто внизу)
    move(0, parent->height());
}

BottomSheet::~BottomSheet()
{
    delete m_animation;
}

void BottomSheet::setupUi()
{
    // Устанавливаем фиксированную высоту, ширину по родителю
    updateSheetWidth();
    setFixedHeight(m_sheetHeight);
    
    // Основной layout без отступов
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);
    
    setLayout(m_mainLayout);
}

void BottomSheet::setupAnimations()
{
    // Анимация перемещения по вертикали
    m_animation = new QPropertyAnimation(this, "sheetOffset", this);
    m_animation->setDuration(300);
    m_animation->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_animation, &QPropertyAnimation::finished, this, &BottomSheet::onAnimationFinished);
}

void BottomSheet::applyStyles()
{
    // Полупрозрачный фон в стиле шторки
    setStyleSheet(
        "BottomSheet {"
        "   background-color: rgba(40, 40, 40, 200);"
        "   border: none;"
        "}"
    );
}

void BottomSheet::addListView(QListView *view)
{
    if (!view || !m_mainLayout) return;
    
    // Удаляем существующий listView если есть
    if (m_listView) {
        m_mainLayout->removeWidget(m_listView);
        m_listView->setParent(nullptr);
    }
    
    m_listView = view;
    
    // Устанавливаем виджет на всю шторку без отступов
    view->setParent(this);
    view->setFrameShape(QFrame::NoFrame);
    view->setStyleSheet(
        "QListView {"
        "   background-color: transparent;"
        "   color: white;"
        "   border: none;"
        "   outline: none;"
        "}"
        "QListView::item {"
        "   padding: 8px 12px;"
        "   border-bottom: 1px solid rgba(255, 255, 255, 30);"
        "}"
        "QListView::item:hover {"
        "   background-color: rgba(255, 255, 255, 20);"
        "}"
        "QListView::item:selected {"
        "   background-color: rgba(100, 150, 255, 100);"
        "}"
    );
    
    m_mainLayout->addWidget(view);
}

int BottomSheet::sheetOffset() const
{
    return pos().y();
}

void BottomSheet::setSheetOffset(int offset)
{
    move(x(), offset);
}

void BottomSheet::showSheet()
{
    if (m_isVisible) return;
    
    raise();
    show();
    
    int parentHeight = parentWidget() ? parentWidget()->height() : height();
    int startPos = parentHeight;
    int endPos = parentHeight - m_sheetHeight;
    
    m_animation->setStartValue(startPos);
    m_animation->setEndValue(endPos);
    m_animation->start();
    
    m_isVisible = true;
}

void BottomSheet::hideSheet()
{
    if (!m_isVisible) return;
    
    int parentHeight = parentWidget() ? parentWidget()->height() : height();
    int startPos = parentHeight - m_sheetHeight;
    int endPos = parentHeight;
    
    m_animation->setStartValue(startPos);
    m_animation->setEndValue(endPos);
    m_animation->start();
    
    m_isVisible = false;
}

bool BottomSheet::isSheetVisible() const
{
    return m_isVisible;
}

void BottomSheet::onAnimationFinished()
{
    if (!m_isVisible) {
        hide();
        emit sheetHidden();
    } else {
        emit sheetShown();
    }
}

void BottomSheet::installParentEventFilter()
{
    if (parentWidget()) {
        parentWidget()->installEventFilter(this);
    }
}

bool BottomSheet::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == parentWidget() && event->type() == QEvent::Resize) {
        updateSheetWidth();
        // Перепозиционируем шторку при изменении размера родителя
        if (!m_isVisible) {
            move(0, parentWidget()->height());
        } else {
            move(0, parentWidget()->height() - m_sheetHeight);
        }
    }
    return QWidget::eventFilter(obj, event);
}

void BottomSheet::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);
}

void BottomSheet::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}

void BottomSheet::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
}

void BottomSheet::updateSheetWidth()
{
    if (parentWidget()) {
        setFixedWidth(parentWidget()->width());
    }
}
