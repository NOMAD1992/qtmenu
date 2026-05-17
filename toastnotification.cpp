#include "toastnotification.h"
#include <QApplication>
#include <QScreen>
#include <QGraphicsOpacityEffect>
#include <QtMath>

// ============================================
// Реализация ToastWidget
// ============================================

ToastWidget::ToastWidget(const QString &title, 
                         const QString &message, 
                         ToastStatus status,
                         QWidget *parent)
    : QWidget(parent)
    , m_titleLabel(nullptr)
    , m_messageLabel(nullptr)
    , m_closeButton(nullptr)
    , m_dismissTimer(nullptr)
    , m_hideAnimation(nullptr)
    , m_displayDuration(3000)
    , m_isHovered(false)
{
    setupUi();
    applyStyles();
    
    // Устанавливаем заголовок и сообщение
    m_titleLabel->setText(title);
    m_messageLabel->setText(message);
    
    // Настраиваем атрибуты окна
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);
    
    // Создаем таймер
    m_dismissTimer = new QTimer(this);
    m_dismissTimer->setSingleShot(true);
    connect(m_dismissTimer, &QTimer::timeout, this, &ToastWidget::dismiss);
    
    // Создаем анимацию скрытия
    m_hideAnimation = new QPropertyAnimation(this, "windowOpacity");
    m_hideAnimation->setDuration(300);
    m_hideAnimation->setStartValue(1.0);
    m_hideAnimation->setEndValue(0.0);
    connect(m_hideAnimation, &QPropertyAnimation::finished, 
            this, &ToastWidget::onAnimationFinished);
}

ToastWidget::~ToastWidget()
{
}

void ToastWidget::setDisplayDuration(int milliseconds)
{
    m_displayDuration = milliseconds;
}

void ToastWidget::startDismissTimer()
{
    if (m_dismissTimer && !m_isHovered) {
        m_dismissTimer->start(m_displayDuration);
    }
}

int ToastWidget::getMinimumHeight() const
{
    return minimumHeight();
}

void ToastWidget::enterEvent(QEnterEvent *event)
{
    m_isHovered = true;
    if (m_dismissTimer) {
        m_dismissTimer->stop();
    }
    QWidget::enterEvent(event);
}

void ToastWidget::leaveEvent(QEvent *event)
{
    m_isHovered = false;
    startDismissTimer();
    QWidget::leaveEvent(event);
}

void ToastWidget::dismiss()
{
    m_hideAnimation->start();
}

void ToastWidget::onAnimationFinished()
{
    close();
    deleteLater();
}

void ToastWidget::setupUi()
{
    // Основной layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 8, 8, 8);
    mainLayout->setSpacing(6);
    
    // Заголовок с кнопкой закрытия
    QHBoxLayout *headerLayout = new QHBoxLayout();
    headerLayout->setSpacing(8);
    
    m_titleLabel = new QLabel(this);
    m_titleLabel->setObjectName("toastTitleLabel");
    m_titleLabel->setFont(QFont("Segoe UI", 11, QFont::Bold));
    m_titleLabel->setWordWrap(true);
    headerLayout->addWidget(m_titleLabel);
    
    headerLayout->addStretch();
    
    m_closeButton = createCloseButton();
    headerLayout->addWidget(m_closeButton);
    
    mainLayout->addLayout(headerLayout);
    
    // Тело с сообщением
    m_messageLabel = new QLabel(this);
    m_messageLabel->setObjectName("toastMessageLabel");
    m_messageLabel->setFont(QFont("Segoe UI", 10));
    m_messageLabel->setWordWrap(true);
    m_messageLabel->setMaximumWidth(350);
    mainLayout->addWidget(m_messageLabel);
    
    setLayout(mainLayout);
    adjustSize();
    
    // Минимальная высота
    setMinimumHeight(70);
}

void ToastWidget::applyStyles()
{
    QString baseStyle = R"(
        QWidget {
            background-color: rgba(40, 40, 40, 250);
            border-radius: 8px;
            border: 1px solid rgba(255, 255, 255, 40);
        }
        
        QLabel#toastTitleLabel {
            color: #ffffff;
            padding: 2px;
        }
        
        QLabel#toastMessageLabel {
            color: rgba(255, 255, 255, 200);
            padding: 2px;
        }
        
        QPushButton#toastCloseButton {
            background-color: transparent;
            border: none;
            color: rgba(255, 255, 255, 150);
            font-size: 16px;
            font-weight: bold;
            min-width: 20px;
            max-width: 20px;
            min-height: 20px;
            max-height: 20px;
            padding: 0px;
            border-radius: 10px;
        }
        
        QPushButton#toastCloseButton:hover {
            background-color: rgba(255, 255, 255, 30);
            color: #ffffff;
        }
        
        QPushButton#toastCloseButton:pressed {
            background-color: rgba(255, 255, 255, 50);
        }
    )";
    
    // Цвета в зависимости от статуса
    QString statusColor;
    QString borderColor;
    switch (property("status").toInt()) {
        case static_cast<int>(ToastStatus::Warning):
            statusColor = "#FFA726";  // Оранжевый
            borderColor = "rgba(255, 167, 38, 150)";
            break;
        case static_cast<int>(ToastStatus::Error):
            statusColor = "#EF5350";  // Красный
            borderColor = "rgba(239, 83, 80, 150)";
            break;
        case static_cast<int>(ToastStatus::NewChatMessage):
            statusColor = "#42A5F5";  // Синий
            borderColor = "rgba(66, 165, 245, 150)";
            break;
        default: // Information
            statusColor = "#66BB6A";  // Зеленый
            borderColor = "rgba(102, 187, 106, 150)";
            break;
    }
    
    QString statusStyle = QString(R"(
        QWidget {
            border: 2px solid %1;
        }
        
        QLabel#toastTitleLabel {
            color: %2;
        }
    )").arg(borderColor).arg(statusColor);
    
    setStyleSheet(baseStyle + statusStyle);
    m_closeButton->setObjectName("toastCloseButton");
}

QPushButton* ToastWidget::createCloseButton()
{
    QPushButton *button = new QPushButton("×", this);
    button->setObjectName("toastCloseButton");
    button->setCursor(Qt::PointingHandCursor);
    connect(button, &QPushButton::clicked, this, &ToastWidget::dismiss);
    return button;
}


// ============================================
// Реализация ToastNotification
// ============================================

ToastNotification::ToastNotification(QWidget *parent)
    : QObject(parent)
    , m_parentWidget(parent)
    , m_displayDuration(3000)
    , m_maxNotifications(3)
    , m_spacing(10)
    , m_rightMargin(20)
    , m_bottomMargin(20)
{
}

ToastNotification::~ToastNotification()
{
    closeAll();
}

void ToastNotification::showToast(const QString &title, 
                                  const QString &message, 
                                  ToastStatus status)
{
    // Добавляем в очередь
    m_queue.enqueue(qMakePair(title, qMakePair(message, status)));
    
    // Обрабатываем очередь
    processQueue();
}

void ToastNotification::setDisplayDuration(int milliseconds)
{
    m_displayDuration = milliseconds;
    
    // Обновляем время для всех активных уведомлений
    for (ToastWidget *toast : m_activeToasts) {
        toast->setDisplayDuration(milliseconds);
    }
}

int ToastNotification::displayDuration() const
{
    return m_displayDuration;
}

void ToastNotification::setMaxNotifications(int maxNotifications)
{
    m_maxNotifications = qMax(1, maxNotifications);
    processQueue();
}

int ToastNotification::maxNotifications() const
{
    return m_maxNotifications;
}

void ToastNotification::closeAll()
{
    for (ToastWidget *toast : m_activeToasts) {
        toast->deleteLater();
    }
    m_activeToasts.clear();
    
    // Очищаем очередь
    while (!m_queue.isEmpty()) {
        m_queue.dequeue();
    }
}

void ToastNotification::processQueue()
{
    // Удаляем уведомления, которые уже закрыты
    for (int i = m_activeToasts.size() - 1; i >= 0; --i) {
        if (!m_activeToasts.at(i)->isVisible()) {
            m_activeToasts.removeAt(i);
        }
    }
    
    // Добавляем новые уведомления из очереди, если есть место
    while (m_activeToasts.size() < m_maxNotifications && !m_queue.isEmpty()) {
        auto notification = m_queue.dequeue();
        
        ToastWidget *toast = new ToastWidget(
            notification.first,                      // title
            notification.second.first,               // message
            notification.second.second,              // status
            m_parentWidget
        );
        
        // Сохраняем статус как свойство для стилизации
        toast->setProperty("status", static_cast<int>(notification.second.second));
        toast->applyStyles();
        
        // Устанавливаем длительность отображения
        toast->setDisplayDuration(m_displayDuration);
        
        // Вычисляем позицию
        int index = m_activeToasts.size();
        QPoint pos = calculatePosition(index);
        toast->move(pos);
        
        // Показываем с анимацией появления
        QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(toast);
        toast->setGraphicsEffect(effect);
        
        QPropertyAnimation *showAnimation = new QPropertyAnimation(effect, "opacity");
        showAnimation->setDuration(300);
        showAnimation->setStartValue(0.0);
        showAnimation->setEndValue(1.0);
        
        toast->show();
        showAnimation->start(QAbstractAnimation::DeleteWhenStopped);
        
        // Подключаемся к сигналу закрытия для обновления позиций
        connect(toast, &QWidget::destroyed, this, [this]() {
            updatePositions();
            processQueue();
        });
        
        m_activeToasts.append(toast);
        
        // Запускаем таймер исчезновения
        toast->startDismissTimer();
    }
}

void ToastNotification::updatePositions()
{
    // Сначала удаляем закрытые уведомления
    for (int i = m_activeToasts.size() - 1; i >= 0; --i) {
        if (!m_activeToasts.at(i)->isVisible()) {
            m_activeToasts.removeAt(i);
        }
    }
    
    // Пересчитываем позиции оставшихся
    for (int i = 0; i < m_activeToasts.size(); ++i) {
        ToastWidget *toast = m_activeToasts.at(i);
        QPoint pos = calculatePosition(i);
        
        // Анимация перемещения (опционально)
        toast->move(pos);
    }
}

QPoint ToastNotification::calculatePosition(int index)
{
    if (!m_parentWidget) {
        // Если нет родительского виджета, используем экран
        QScreen *screen = QApplication::primaryScreen();
        QRect screenGeometry = screen->availableGeometry();
        
        int x = screenGeometry.right() - m_rightMargin - 400; // 400 - примерная ширина
        int y = screenGeometry.bottom() - m_bottomMargin - (index * 100);
        
        return QPoint(x, y);
    }
    
    // Используем геометрию родительского виджета
    QRect parentRect = m_parentWidget->frameGeometry();
    QScreen *screen = m_parentWidget->screen();
    QRect screenGeometry = screen ? screen->availableGeometry() : parentRect;
    
    // Позиция в глобальных координатах
    int x = screenGeometry.right() - m_rightMargin - 400;
    int y = screenGeometry.bottom() - m_bottomMargin - (index * 100);
    
    return QPoint(x, y);
}
