#include "bottomsheet.h"
#include <QResizeEvent>
#include <QShowEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QFrame>
#include <QDebug>

BottomSheet::BottomSheet(QWidget *parent)
    : QWidget(parent)
    , m_handleWidget(nullptr)
    , m_handleLabel(nullptr)
    , m_listView(nullptr)
    , m_mainLayout(nullptr)
    , m_isVisible(true)  // Шторка всегда открыта по умолчанию
    , m_sheetHeight(200)
    , m_minHeight(35)   // Минимальная высота
    , m_maxHeight(400)   // Максимальная высота (будет пересчитана)
    , m_menubarHeight(0) // Высота menubar будет определена динамически
    , m_opacity(240)     // Прозрачность по умолчанию (200 из 255)
    , m_dragging(false)
    , m_dragStartY(0)
    , m_startHeight(0)
{
    setAttribute(Qt::WA_StyledBackground, true);
    
    setupUi();
    applyStyles();
    installParentEventFilter();
    
    // Определяем высоту fMenuBar и пересчитываем максимальную высоту шторки
    if (parent) {
        // Ищем fMenuBar в родителе (QFrame с именем fMenuBar)
        QFrame *menuBarFrame = parent->findChild<QFrame*>("fMenuBar");
        if (menuBarFrame) {
            m_menubarHeight = menuBarFrame->height();
        }
        recalculateMaxHeight();
        
        // Позиционируем шторку внизу родителя
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
    
    // Включаем фокус для виджета, чтобы он мог получать события клавиатуры
    setFocusPolicy(Qt::StrongFocus);
    
    // Основной layout без отступов
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);
    
    // Создаем узкую полоску-ручку сверху для перетаскивания
    m_handleWidget = new QWidget(this);
    m_handleWidget->setStyleSheet("background-color: black;");
    m_handleWidget->setFixedHeight(20);  // Высота полоски для захвата
    m_handleWidget->setCursor(Qt::SizeVerCursor);  // Курсор изменения размера
    
    // Добавляем надпись на ручку
    m_handleLabel = new QLabel("Лог", m_handleWidget);
    m_handleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_handleLabel->setStyleSheet(
        "QLabel {"
        "   color: rgba(255, 255, 255, 150);"
        "   font-size: 10px;"
        "   padding-left: 8px;"
        "}"
    );
    m_handleLabel->raise();  // Поднимаем label над фоном
    
    // Добавляем ручку в layout
    m_mainLayout->addWidget(m_handleWidget);
    
    setLayout(m_mainLayout);
}

void BottomSheet::applyStyles()
{
    // Полупрозрачный фон в стиле шторки с настраиваемой прозрачностью
    setStyleSheet(
        QString("BottomSheet {") +
        QString("   background-color: rgba(40, 40, 40, %1);").arg(m_opacity) +
        QString("   border: none;") +
        QString("}")
    );
    
    // Стиль для ручки (полоски сверху) - только граница, фон задан в setupUi()
    if (m_handleWidget) {
        m_handleWidget->setStyleSheet(
            "QWidget {"
            "   background-color: black;"
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
        // Пересчитываем максимальную высоту при изменении размера окна
        recalculateMaxHeight();
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
    // Устанавливаем фокус на шторку при показе, чтобы она могла получать события клавиатуры
    setFocus();
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

void BottomSheet::keyPressEvent(QKeyEvent *event)
{
    // Обработка комбинаций Alt+стрелка вверх/вниз для управления шторкой
    if (event->modifiers() & Qt::AltModifier) {
        if (event->key() == Qt::Key_Up) {
            // Поднять шторку - установить максимальную высоту
            updateSheetHeight(m_maxHeight);
            event->accept();
            return;
        } else if (event->key() == Qt::Key_Down) {
            // Опустить шторку - установить минимальную высоту
            updateSheetHeight(m_minHeight);
            event->accept();
            return;
        }
    }
    
    // Передаем событие базовому классу для остальных клавиш
    QWidget::keyPressEvent(event);
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

void BottomSheet::recalculateMaxHeight()
{
    if (parentWidget()) {
        // Максимальная высота = высота окна минус высота menubar минус небольшой отступ
        int availableHeight = parentWidget()->height() - m_menubarHeight;
        m_maxHeight = qMax(m_minHeight, availableHeight);
        
        // Если текущая высота больше новой максимальной, уменьшаем её
        if (m_sheetHeight > m_maxHeight) {
            updateSheetHeight(m_maxHeight);
        }
    }
}

void BottomSheet::updateMaxHeight(int maxHeight)
{
    m_maxHeight = qMax(m_minHeight, maxHeight);
}

void BottomSheet::setOpacity(int alpha)
{
    m_opacity = qBound(0, alpha, 255);
    applyStyles();
}

void BottomSheet::setHandleText(const QString &text)
{
    if (m_handleLabel) {
        m_handleLabel->setText(text);
    }
}
