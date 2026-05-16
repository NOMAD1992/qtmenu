#include "bottomsheet.h"
#include <QResizeEvent>
#include <QShowEvent>
#include <QMouseEvent>

BottomSheet::BottomSheet(QWidget *parent)
    : QWidget(parent)
    , m_handleWidget(nullptr)
    , m_listView(nullptr)
    , m_mainLayout(nullptr)
    , m_isVisible(true)  // Шторка всегда открыта по умолчанию
    , m_sheetHeight(200)
    , m_minHeight(100)   // Минимальная высота
    , m_maxHeight(400)   // Максимальная высота
    , m_dragging(false)
    , m_dragStartY(0)
    , m_startHeight(0)
{
    setAttribute(Qt::WA_StyledBackground, true);
    
    setupUi();
    applyStyles();
    installParentEventFilter();
    
    // Позиционируем шторку внизу родителя
    if (parent) {
        move(0, parent->height() - m_sheetHeight);
    }
}

BottomSheet::~BottomSheet()
{
}

void BottomSheet::setupUi()
{
    // Устанавливаем начальную высоту
    updateSheetWidth();
    setFixedHeight(m_sheetHeight);
    
    // Основной layout без отступов
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);
    
    // Создаем узкую полоску-ручку сверху для перетаскивания
    m_handleWidget = new QWidget(this);
    m_handleWidget->setFixedHeight(20);  // Высота полоски для захвата
    m_handleWidget->setCursor(Qt::SizeVerCursor);  // Курсор изменения размера
    
    // Добавляем ручку в layout
    m_mainLayout->addWidget(m_handleWidget);
    
    setLayout(m_mainLayout);
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
    
    // Стиль для ручки (полоски сверху)
    if (m_handleWidget) {
        m_handleWidget->setStyleSheet(
            "QWidget {"
            "   background-color: rgba(255, 255, 255, 30);"
            "   border-bottom: 1px solid rgba(255, 255, 255, 50);"
            "}"
        );
    }
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
        move(0, parentWidget()->height() - m_sheetHeight);
    }
    return QWidget::eventFilter(obj, event);
}

void BottomSheet::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}

void BottomSheet::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
}

void BottomSheet::mousePressEvent(QMouseEvent *event)
{
    // Проверяем, что клик был по ручке (верхней полоске)
    if (m_handleWidget && m_handleWidget->rect().contains(m_handleWidget->mapFromGlobal(event->globalPos()))) {
        m_dragging = true;
        m_dragStartY = event->globalY();
        m_startHeight = m_sheetHeight;
        event->accept();
    } else {
        QWidget::mousePressEvent(event);
    }
}

void BottomSheet::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging) {
        int deltaY = m_dragStartY - event->globalY();
        int newHeight = m_startHeight + deltaY;
        
        // Ограничиваем высоту минимальным и максимальным значениями
        newHeight = qBound(m_minHeight, newHeight, m_maxHeight);
        
        updateSheetHeight(newHeight);
        event->accept();
    } else {
        QWidget::mouseMoveEvent(event);
    }
}

void BottomSheet::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_dragging) {
        m_dragging = false;
        event->accept();
    } else {
        QWidget::mouseReleaseEvent(event);
    }
}

void BottomSheet::updateSheetWidth()
{
    if (parentWidget()) {
        setFixedWidth(parentWidget()->width());
    }
}

void BottomSheet::updateSheetHeight(int height)
{
    m_sheetHeight = height;
    setFixedHeight(m_sheetHeight);
    
    // Перепозиционируем шторку, чтобы она оставалась прижатой к низу
    if (parentWidget()) {
        move(x(), parentWidget()->height() - m_sheetHeight);
    }
}
