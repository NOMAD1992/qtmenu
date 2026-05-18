#include "bottomsheet.h"
#include <QResizeEvent>
#include <QShowEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QFrame>
#include <QDebug>

#define STEP_HEIGHT_OFFSET 50 // Шаг изменения высоты в пикселях

BottomSheet::BottomSheet(QWidget *parent, const QString &menuBarObjectName)
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
    , m_menuBarY(0)      // Позиция Y панели меню относительно родителя
    , m_topOffset(5)     // Отступ сверху по умолчанию (как в запросе пользователя)
    , m_opacity(240)     // Прозрачность по умолчанию (200 из 255)
    , m_dragging(false)
    , m_dragStartY(0)
    , m_startHeight(0)
    , m_menuBarObjectName(menuBarObjectName)
{
    setAttribute(Qt::WA_StyledBackground, true);
    
    setupUi();
    applyStyles();
    installParentEventFilter();
    
    // Определяем высоту панели меню и пересчитываем максимальную высоту шторки
    if (parent) {
        // Ищем панель меню в родителе по имени объекта
        QFrame *menuBarFrame = parent->findChild<QFrame*>(m_menuBarObjectName);
        if (menuBarFrame) {
            m_menubarHeight = menuBarFrame->height();
            qDebug() << "m_menubarHeight" << m_menubarHeight;
            // Получаем позицию панели меню относительно родителя
            m_menuBarY = menuBarFrame->y();
            qDebug() << "m_menuBarY" << m_menuBarY;
            // Панель меню видима - шторка размещается над ней
            move(0, parent->height() - m_menubarHeight - m_sheetHeight);
        } else {
            // Панель меню не найдена - позиционируем в самом низу
            move(0, parent->height() - m_sheetHeight);
        }
        recalculateMaxHeight();
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
    m_handleWidget->setObjectName("handleWidget");
    m_handleWidget->setFixedHeight(6);  // Высота полоски для захвата
    m_handleWidget->setCursor(Qt::SizeVerCursor);  // Курсор изменения размера
    
    // Добавляем надпись на ручку врменно закомментированно
//    m_handleLabel = new QLabel("Журнал", m_handleWidget);
//    m_handleLabel->setObjectName("handleLabel");
//    m_handleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
//    m_handleLabel->raise();  // Поднимаем label над фоном
    
    // Добавляем ручку в layout
    m_mainLayout->addWidget(m_handleWidget);
    
    setLayout(m_mainLayout);
}

void BottomSheet::applyStyles()
{
    // Стили теперь загружаются из styles.qss в MainWindow
    // Обновляем только прозрачность если нужно
    if (m_opacity != 240) {
        setStyleSheet(
            QString("BottomSheet {") +
            QString("   background-color: rgba(40, 40, 40, %1);").arg(m_opacity) +
            QString("   border: none;") +
            QString("}")
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
    
    m_mainLayout->addWidget(view);
}

void BottomSheet::appendLogMessage(const QString &message)
{
    if (!m_listView) return;
    
    QStringListModel *model = qobject_cast<QStringListModel*>(m_listView->model());
    if (!model) return;
    
    QStringList messages = model->stringList();
    messages << message;
    model->setStringList(messages);
    
    // Прокручиваем к последнему элементу
    m_listView->scrollToBottom();
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
        
        // Проверяем видимость панели меню для правильного позиционирования
        QFrame *menuBarFrame = parentWidget()->findChild<QFrame*>(m_menuBarObjectName);
        bool menuBarVisible = menuBarFrame && menuBarFrame->isVisible();
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
            // Поднять шторку - увеличить высоту на 100px
            int newHeight = qMin(m_sheetHeight + STEP_HEIGHT_OFFSET, m_maxHeight);
            updateSheetHeight(newHeight);
            event->accept();
            return;
        } else if (event->key() == Qt::Key_Down) {
            // Опустить шторку - уменьшить высоту на 100px
            int newHeight = qMax(m_sheetHeight - STEP_HEIGHT_OFFSET, m_minHeight);
            updateSheetHeight(newHeight);
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
    
    // Перепозиционируем шторку, чтобы она оставалась над панелью меню или в самом низу
    if (parentWidget()) {
        QFrame *menuBarFrame = parentWidget()->findChild<QFrame*>(m_menuBarObjectName);
        bool menuBarVisible = menuBarFrame && menuBarFrame->isVisible();
        
        if (menuBarVisible) {
            move(x(), parentWidget()->height() - m_menubarHeight - m_sheetHeight);
        } else {
            move(x(), parentWidget()->height() - m_sheetHeight);
        }
    }
}

void BottomSheet::recalculateMaxHeight()
{
    if (parentWidget()) {
        // Проверяем, видима ли панель меню
        QFrame *menuBarFrame = parentWidget()->findChild<QFrame*>(m_menuBarObjectName);
        bool menuBarVisible = menuBarFrame && menuBarFrame->isVisible();
        
        // Максимальная высота = высота окна минус высота menubar (если видима) минус отступ сверху
        int availableHeight = parentWidget()->height();
        if (menuBarVisible && m_menubarHeight > 0) {
            availableHeight -= m_menubarHeight;
        }
        availableHeight -= m_topOffset;
        
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

void BottomSheet::setTopOffset(int offset)
{
    m_topOffset = qMax(0, offset);  // Отступ не может быть отрицательным
    recalculateMaxHeight();  // Пересчитываем максимальную высоту при изменении отступа
}

void BottomSheet::updateForMenuBarVisibility(bool menuBarVisible)
{
    if (!parentWidget()) return;
    
    if (menuBarVisible) {
        // Панель меню видима - шторка размещается над ней
        m_menubarHeight = m_menubarHeight > 0 ? m_menubarHeight : 35; // Используем сохранённую высоту или дефолтную
        move(0, parentWidget()->height() - m_menubarHeight - m_sheetHeight);
    } else {
        // Панель меню скрыта - шторка опускается в самый низ
        move(0, parentWidget()->height() - m_sheetHeight);
    }
    
    // Пересчитываем максимальную высоту с учётом видимости панели меню
    recalculateMaxHeight();
}
