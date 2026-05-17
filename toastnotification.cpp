#include "toastnotification.h"
#include <QApplication>
#include <QScreen>
#include <QGraphicsOpacityEffect>
#include <QtMath>
#include <QPainter>
#include <QStyleOption>
#include <QPainterPath>
#include <QDebug>
#include <QMouseEvent>

// ============================================
// Реализация ToastWidget
// ============================================

// Класс для отрисовки иконки статуса
class StatusIconLabel : public QLabel
{
public:
    // Конструктор с использованием QIcon
    StatusIconLabel(const QIcon &icon, QWidget *parent = nullptr) 
        : QLabel(parent), m_icon(icon), m_status(ToastStatus::Information) {}
    
    // Конструктор с использованием ToastStatus (для обратной совместимости)
    StatusIconLabel(ToastStatus status, QWidget *parent = nullptr) 
        : QLabel(parent), m_status(status) {
        // Инициализируем иконку на основе статуса
        updateIconFromStatus();
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        
        int size = qMin(width(), height());
        
        // Рисуем QIcon без фона - на всю доступную область
        if (!m_icon.isNull()) {
            QRect iconRect(size * 0.1, size * 0.1, size * 0.8, size * 0.8);
            m_icon.paint(&painter, iconRect, Qt::AlignCenter, QIcon::Normal, QIcon::On);
        }
    }

private:
    void updateIconFromStatus()
    {
        // Здесь можно задать иконки по умолчанию для каждого статуса
        // Пока оставляем пустым, иконки будут переданы явно
    }

    QIcon m_icon;
    ToastStatus m_status;
};

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
    // Устанавливаем статус как свойство для использования в стилях
    setProperty("status", static_cast<int>(status));
    
    setupUi();
    applyStyles();
    
    // Устанавливаем заголовок и сообщение
    m_titleLabel->setText(title);
    m_messageLabel->setText(message);
    
    // Настраиваем атрибуты окна - убираем WindowStaysOnTopHint для отображения в окне приложения
    setWindowFlags(Qt::FramelessWindowHint | Qt::BypassWindowManagerHint);
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

// Конструктор с использованием QIcon
ToastWidget::ToastWidget(const QString &title, 
                         const QString &message, 
                         const QIcon &icon,
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
    // Устанавливаем статус Information по умолчанию
    setProperty("status", static_cast<int>(ToastStatus::Information));
    setProperty("customIcon", icon);
    
    setupUi();
    applyStyles();
    
    // Устанавливаем заголовок и сообщение
    m_titleLabel->setText(title);
    m_messageLabel->setText(message);
    
    // Настраиваем атрибуты окна
    setWindowFlags(Qt::FramelessWindowHint | Qt::BypassWindowManagerHint);
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

void ToastWidget::enterEvent(QEvent *event)
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

void ToastWidget::emitClickSignal()
{
    // Испускаем сигнал в зависимости от типа уведомления
    int statusValue = property("status").toInt();
    
    switch (static_cast<ToastStatus>(statusValue)) {
        case ToastStatus::Information:
            emit informationClicked();
            break;
        case ToastStatus::Warning:
            emit warningClicked();
            break;
        case ToastStatus::Error:
            emit errorClicked();
            break;
        case ToastStatus::NewChatMessage:
            emit newChatMessageClicked();
            break;
        default:
            emit informationClicked();
            break;
    }
}

void ToastWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Получаем цвета фона из свойств
    QColor headerBgColor = property("headerBgColor").value<QColor>();
    QColor bodyBgColor = property("bodyBgColor").value<QColor>();
    
    if (!headerBgColor.isValid())
        headerBgColor = QColor(0, 0, 0, 180);
    if (!bodyBgColor.isValid())
        bodyBgColor = QColor(80, 80, 80, 200);
    
    int height = this->height();
    int headerHeight = m_titleLabel ? m_titleLabel->height() + 16 : height / 3;
    
    // Рисуем заголовок с полупрозрачным черным фоном (закругленный сверху)
    QPainterPath headerPath;
    headerPath.moveTo(0, 12);
    headerPath.arcTo(QRectF(0, 0, 24, 24), 180, -90);
    headerPath.lineTo(width() - 12, 0);
    headerPath.arcTo(QRectF(width() - 24, 0, 24, 24), 90, -90);
    headerPath.lineTo(width(), headerHeight - 6);
    headerPath.lineTo(0, headerHeight - 6);
    headerPath.closeSubpath();
    painter.fillPath(headerPath, headerBgColor);
    
    // Рисуем тело с полупрозрачным серым фоном (закругленный снизу)
    QPainterPath bodyPath;
    bodyPath.moveTo(0, headerHeight - 6);
    bodyPath.lineTo(width(), headerHeight - 6);
    bodyPath.lineTo(width(), height - 12);
    bodyPath.arcTo(QRectF(width() - 24, height - 24, 24, 24), 0, -90);
    bodyPath.lineTo(12, height);
    bodyPath.arcTo(QRectF(0, height - 24, 24, 24), 270, -90);
    bodyPath.closeSubpath();
    painter.fillPath(bodyPath, bodyBgColor);
    
    QWidget::paintEvent(event);
}

void ToastWidget::setupUi()
{
    // Основной layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 8, 8, 8);
    mainLayout->setSpacing(0); // Убираем отступ между заголовком и телом, так как будет разделитель
    
    // Заголовок с иконкой и кнопкой закрытия
    QHBoxLayout *headerLayout = new QHBoxLayout();
    headerLayout->setSpacing(8);
    
    // Иконка статуса - проверяем наличие пользовательской иконки
    StatusIconLabel *iconLabel = nullptr;
    QIcon customIcon = property("customIcon").value<QIcon>();
    
    if (!customIcon.isNull()) {
        // Используем пользовательскую иконку
        iconLabel = new StatusIconLabel(customIcon, this);
    } else {
        // Используем иконку по умолчанию на основе статуса
        iconLabel = new StatusIconLabel(
            static_cast<ToastStatus>(property("status").toInt()), this);
    }
    
    iconLabel->setFixedSize(32, 32);
    headerLayout->addWidget(iconLabel);
    
    m_titleLabel = new QLabel(this);
    m_titleLabel->setObjectName("toastTitleLabel");
    m_titleLabel->setFont(QFont("Segoe UI", 11, QFont::Bold));
    m_titleLabel->setWordWrap(true);
    m_titleLabel->setCursor(Qt::PointingHandCursor);
    headerLayout->addWidget(m_titleLabel);
    
    headerLayout->addStretch();
    
    m_closeButton = createCloseButton();
    headerLayout->addWidget(m_closeButton);
    
    mainLayout->addLayout(headerLayout);
    
    // Разделительная полоса между заголовком и телом
    QFrame *separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Plain);
    separator->setObjectName("toastSeparator");
    separator->setFixedHeight(1);
    mainLayout->addWidget(separator);
    
    // Тело с сообщением
    m_messageLabel = new QLabel(this);
    m_messageLabel->setObjectName("toastMessageLabel");
    m_messageLabel->setFont(QFont("Segoe UI", 10));
    m_messageLabel->setWordWrap(true);
    m_messageLabel->setMaximumWidth(MIN_WIDTH - 40); // Ширина уведомления MIN_WIDTH минус отступы
    m_messageLabel->setCursor(Qt::PointingHandCursor);
    mainLayout->addWidget(m_messageLabel);
    
    setLayout(mainLayout);
    adjustSize();
    
    // Устанавливаем фиксированную ширину MIN_WIDTH
    setFixedWidth(MIN_WIDTH);
    // Устанавливаем минимальную высоту MIN_HEIGHT
    setMinimumHeight(MIN_HEIGHT);
    
    // Подключаем сигнал о клике на заголовок и сообщение для испускания сигнала в зависимости от типа
    connect(m_titleLabel, &QLabel::linkActivated, this, [this]() { emitClickSignal(); });
    connect(m_messageLabel, &QLabel::linkActivated, this, [this]() { emitClickSignal(); });
}

void ToastWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emitClickSignal();
    }
    QWidget::mousePressEvent(event);
}

void ToastWidget::applyStyles()
{
    // Определяем цвета в зависимости от статуса
    QString titleColor;
    QColor headerBgColor;
    QColor bodyBgColor;
    
    switch (property("status").toInt()) {
        case static_cast<int>(ToastStatus::Warning):
            titleColor = "white";  // Желтый для предупреждения
            headerBgColor = QColor(0, 0, 0, 180);  // Полупрозрачный черный
            bodyBgColor = QColor(80, 80, 80, 200);  // Полупрозрачный серый
            break;
        case static_cast<int>(ToastStatus::Error):
            titleColor = "white";  // Красный для ошибки
            headerBgColor = QColor(0, 0, 0, 180);  // Полупрозрачный черный
            bodyBgColor = QColor(80, 80, 80, 200);  // Полупрозрачный серый
            break;
        case static_cast<int>(ToastStatus::NewChatMessage):
            titleColor = "white";  // Белый для сообщений чата
            headerBgColor = QColor(0, 0, 0, 180);  // Полупрозрачный черный
            bodyBgColor = QColor(80, 80, 80, 200);  // Полупрозрачный серый
            break;
        default: // Information
            titleColor = "white";  // Синий для информации
            headerBgColor = QColor(0, 0, 0, 180);  // Полупрозрачный черный
            bodyBgColor = QColor(80, 80, 80, 200);  // Полупрозрачный серый
            break;
    }
    
    // Сохраняем цвета как свойства для использования в paintEvent
    setProperty("headerBgColor", headerBgColor);
    setProperty("bodyBgColor", bodyBgColor);
    
    QString baseStyle = QString(R"(
        QWidget {
            background-color: transparent;
            border-radius: 12px;
        }
        
        QLabel#toastTitleLabel {
            color: %1;
            padding: 4px 6px;
            border: none;
            background: transparent;
        }
        
        QLabel#toastMessageLabel {
            color: rgba(255, 255, 255, 220);
            padding: 8px 6px;
            border: none;
            background: transparent;
        }
        
        QFrame#toastSeparator {
            background-color: rgba(255, 255, 255, 60);
            margin: 0px 8px;
        }
        
        QPushButton#toastCloseButton {
            background-color: transparent;
            border: none;
            color: rgba(255, 255, 255, 150);
            font-size: 18px;
            font-weight: bold;
            min-width: 24px;
            max-width: 24px;
            min-height: 24px;
            max-height: 24px;
            padding: 0px;
            border-radius: 12px;
        }
        
        QPushButton#toastCloseButton:hover {
            background-color: rgba(255, 255, 255, 30);
            color: #ffffff;
        }
        
        QPushButton#toastCloseButton:pressed {
            background-color: rgba(255, 255, 255, 50);
        }
    )").arg(titleColor);
    
    setStyleSheet(baseStyle);
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
    , m_rightMargin(5)
    , m_bottomMargin(40)
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
    // Выбираем иконку в зависимости от статуса
    QIcon icon;
    switch (status) {
        case ToastStatus::Error:
            icon = QIcon::fromTheme("dialog-error");
            break;
        case ToastStatus::Warning:
            icon = QIcon::fromTheme("dialog-warning");
            break;
        case ToastStatus::Information:
            icon = QIcon::fromTheme("dialog-information");
            break;
        case ToastStatus::NewChatMessage:
            icon = QIcon::fromTheme("mail-send");
            break;
        default:
            icon = QIcon::fromTheme("dialog-information");
            break;
    }
    
    // Создаем уведомление с выбранной иконкой
    ToastWidget *toast = new ToastWidget(title, message, icon, m_parentWidget);
    
    // Устанавливаем длительность отображения
    toast->setDisplayDuration(m_displayDuration);
    
    // Проверяем, не превышено ли максимальное количество уведомлений
    if (m_activeToasts.size() >= m_maxNotifications) {
        // Если лимит достигнут, не показываем новое уведомление
        toast->deleteLater();
        return;
    }
    
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
    
    // Подключаем сигналы уведомления к сигналам менеджера
    connect(toast, &ToastWidget::informationClicked, this, &ToastNotification::informationClicked);
    connect(toast, &ToastWidget::warningClicked, this, &ToastNotification::warningClicked);
    connect(toast, &ToastWidget::errorClicked, this, &ToastNotification::errorClicked);
    connect(toast, &ToastWidget::newChatMessageClicked, this, &ToastNotification::newChatMessageClicked);
    
    // Подключаемся к сигналу закрытия для обновления позиций
    connect(toast, &QWidget::destroyed, this, [this]() {
        updatePositions();
        processQueue();
    });
    
    m_activeToasts.append(toast);
    
    // Запускаем таймер исчезновения
    toast->startDismissTimer();
}

void ToastNotification::showToast(const QString &title, 
                                  const QString &message, 
                                  const QIcon &icon)
{
    // Создаем уведомление с пользовательской иконкой
    ToastWidget *toast = new ToastWidget(title, message, icon, m_parentWidget);
    
    // Устанавливаем длительность отображения
    toast->setDisplayDuration(m_displayDuration);
    
    // Проверяем, не превышено ли максимальное количество уведомлений
    if (m_activeToasts.size() >= m_maxNotifications) {
        // Если лимит достигнут, не показываем новое уведомление
        toast->deleteLater();
        return;
    }
    
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
    
    // Подключаем сигналы уведомления к сигналам менеджера
    connect(toast, &ToastWidget::informationClicked, this, &ToastNotification::informationClicked);
    connect(toast, &ToastWidget::warningClicked, this, &ToastNotification::warningClicked);
    connect(toast, &ToastWidget::errorClicked, this, &ToastNotification::errorClicked);
    connect(toast, &ToastWidget::newChatMessageClicked, this, &ToastNotification::newChatMessageClicked);
    
    // Подключаемся к сигналу закрытия для обновления позиций
    connect(toast, &QWidget::destroyed, this, [this]() {
        updatePositions();
        processQueue();
    });
    
    m_activeToasts.append(toast);
    
    // Запускаем таймер исчезновения
    toast->startDismissTimer();
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
}

void ToastNotification::setBottomMargin(const int &value)
{
    m_bottomMargin = value;
}

void ToastNotification::processQueue()
{
    // Удаляем уведомления, которые уже закрыты
    for (int i = m_activeToasts.size() - 1; i >= 0; --i) {
        if (!m_activeToasts.at(i)->isVisible()) {
            m_activeToasts.removeAt(i);
        }
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
        
        int x = screenGeometry.right() - m_rightMargin - MIN_WIDTH;
        int y = screenGeometry.bottom() - m_bottomMargin - (index * (MIN_HEIGHT + m_spacing));
        
        return QPoint(x, y);
    }
    
    // Используем геометрию родительского виджета для позиционирования внутри окна
    QRect parentRect = m_parentWidget->geometry();
    
    // Получаем высоту уведомления для расчета позиции
    int toastHeight = MIN_HEIGHT;
    if (!m_activeToasts.isEmpty() && index < m_activeToasts.size()) {
        toastHeight = m_activeToasts.at(index)->height();
    }
    
    // Позиция относительно родительского виджета (в его координатах)
    int x = parentRect.width() - m_rightMargin - MIN_WIDTH;
    int y = parentRect.height() - m_bottomMargin - toastHeight - (index * (toastHeight + m_spacing));
    
    return m_parentWidget->mapToGlobal(QPoint(x, y));
}

// ============================================
// Реализация слотов ToastNotification
// ============================================

void ToastNotification::onInformationClicked()
{
    qDebug() << "ToastNotification: Information notification clicked";
}

void ToastNotification::onWarningClicked()
{
    qDebug() << "ToastNotification: Warning notification clicked";
}

void ToastNotification::onErrorClicked()
{
    qDebug() << "ToastNotification: Error notification clicked";
}

void ToastNotification::onNewChatMessageClicked()
{
    qDebug() << "ToastNotification: New chat message notification clicked";
}
