#include "standardiconswidget.h"
#include <QIcon>
#include <QLabel>
#include <QDebug>

/**
 * Список стандартных иконок Qt5 (Freedesktop.org icon names).
 * Эти имена иконок поддерживаются большинством тем в Linux/Unix системах,
 * а также частично в Windows и macOS.
 */
static const QStringList STANDARD_ICONS = {
    // Общие
    "application-exit",
    "application-menu",
    "application-x-executable",
    "document-new",
    "document-open",
    "document-save",
    "document-save-as",
    "document-print",
    "document-properties",
    "document-close",
    
    // Правка
    "edit-copy",
    "edit-cut",
    "edit-paste",
    "edit-delete",
    "edit-find",
    "edit-find-replace",
    "edit-undo",
    "edit-redo",
    "edit-select-all",
    
    // Навигация
    "go-up",
    "go-down",
    "go-left",
    "go-right",
    "go-next",
    "go-previous",
    "go-home",
    "go-jump",
    
    // Просмотр
    "view-fullscreen",
    "view-refresh",
    "view-sort-ascending",
    "view-sort-descending",
    "view-list",
    "view-grid",
    "view-zoom-in",
    "view-zoom-out",
    "view-zoom-fit",
    "view-zoom-original",
    
    // Вставка
    "insert-image",
    "insert-link",
    "insert-object",
    "insert-text",
    
    // Форматирование
    "format-text-bold",
    "format-text-italic",
    "format-text-underline",
    "format-text-strikethrough",
    "format-indent-more",
    "format-indent-less",
    "format-justify-left",
    "format-justify-center",
    "format-justify-right",
    "format-justify-fill",
    
    // Медиа
    "media-playback-start",
    "media-playback-pause",
    "media-playback-stop",
    "media-playback-record",
    "media-seek-backward",
    "media-seek-forward",
    "media-skip-backward",
    "media-skip-forward",
    "media-eject",
    "audio-volume-high",
    "audio-volume-medium",
    "audio-volume-low",
    "audio-volume-muted",
    "audio-x-generic",
    "video-x-generic",
    
    // Устройства
    "drive-harddisk",
    "drive-optical",
    "drive-removable-media",
    "input-keyboard",
    "input-mouse",
    "computer",
    "printer",
    "scanner",
    "camera-photo",
    "camera-video",
    
    // Места
    "folder",
    "folder-open",
    "folder-new",
    "user-home",
    "user-desktop",
    "user-trash",
    "network-workgroup",
    "network-server",
    
    // Приложения
    "accessories-calculator",
    "accessories-character-map",
    "accessories-text-editor",
    "office-calendar",
    "office-chart",
    "office-contact",
    "office-database",
    "office-document",
    "office-presentation",
    "office-spreadsheet",
    "preferences-desktop",
    "preferences-system",
    "preferences-color",
    "preferences-font",
    "preferences-plugins",
    
    // Система
    "system-help",
    "system-lock-screen",
    "system-log-out",
    "system-run",
    "system-search",
    "system-shutdown",
    "system-software-install",
    "system-software-update",
    "system-switch-user",
    "system-users",
    
    // Интернет
    "applications-internet",
    "applications-email",
    "emblem-web",
    "network-wired",
    "network-wireless",
    "web-browser",
    "mail-message-new",
    "mail-mark-read",
    "mail-mark-unread",
    
    // Статусы и эмблемы
    "dialog-information",
    "dialog-warning",
    "dialog-error",
    "dialog-question",
    "dialog-ok",
    "dialog-cancel",
    "dialog-apply",
    "dialog-ok-apply",
    "emblem-important",
    "emblem-favorite",
    "emblem-symbolic-link",
    "emblem-readonly",
    
    // Действия
    "list-add",
    "list-remove",
    "list-add-file",
    "process-stop",
    "window-close",
    "window-maximize",
    "window-minimize",
    "window-restore",
    "bookmark-new",
    "contact-new",
    "tab-new",
    
    // Безопасность
    "changes-allow",
    "changes-prevent",
    "security-high",
    "security-medium",
    "security-low",
    
    // Время
    "appointment-new",
    "chronometer",
    "clock",
    
    // Игры
    "applications-games",
    "applications-toys",
    "face-smile",
    "face-sad",
    "face-surprise",
    
    // Графика
    "applications-graphics",
    "color-picker",
    "draw-freehand",
    "draw-circle",
    "draw-square",
    
    // Наука
    "applications-science",
    "laboratory",
    "math",
    
    // Утилиты
    "utilities-terminal",
    "utilities-system-monitor",
    "file-manager",
    "archive-extract",
    "compress",
    "uncompress"
};

StandardIconsWidget::StandardIconsWidget(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
    , m_scrollArea(nullptr)
    , m_scrollWidget(nullptr)
    , m_gridLayout(nullptr)
{
    setupUi();
    createIconButtons();
}

StandardIconsWidget::~StandardIconsWidget()
{
    // Виджеты будут удалены автоматически благодаря системе родитель-потомок в Qt
}

void StandardIconsWidget::setupUi()
{
    // Создаем основной вертикальный layout
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);
    
    // Создаем область прокрутки
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    // Создаем виджет для содержимого прокрутки
    m_scrollWidget = new QWidget();
    m_gridLayout = new QGridLayout(m_scrollWidget);
    m_gridLayout->setSpacing(10);
    m_gridLayout->setContentsMargins(10, 10, 10, 10);
    
    // Устанавливаем виджет в область прокрутки
    m_scrollArea->setWidget(m_scrollWidget);
    
    // Добавляем область прокрутки в основной layout
    m_mainLayout->addWidget(m_scrollArea);
}

void StandardIconsWidget::createIconButtons()
{
    int row = 0;
    int column = 0;
    const int columnsCount = 8; // Количество колонок в сетке
    
    for (const QString &iconName : STANDARD_ICONS) {
        addIconButton(iconName, row, column);
        
        column++;
        if (column >= columnsCount) {
            column = 0;
            row++;
        }
    }
    
    // Добавляем растягиватель внизу, чтобы кнопки прижимались к верху
    QSpacerItem *spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    m_gridLayout->addItem(spacer, row + 1, 0, 1, columnsCount);
}

void StandardIconsWidget::addIconButton(const QString &iconName, int row, int column)
{
    // Создаем кнопку
    QPushButton *button = new QPushButton(m_scrollWidget);
    button->setFixedSize(80, 90); // Фиксированный размер кнопки
    button->setCheckable(false);
    
    // Пытаемся получить стандартную иконку
    QIcon icon = QIcon::fromTheme(iconName);
    
    if (!icon.isNull()) {
        // Если иконка найдена, устанавливаем её
        button->setIcon(icon);
        button->setIconSize(QSize(32, 32));
        button->setText("");
        button->setToolTip(iconName);
    } else {
        // Если иконка не найдена, показываем placeholder
        button->setText("?");
        button->setToolTip(QString("Иконка '%1' не найдена").arg(iconName));
    }
    
    // Создаем подпись с названием иконки под кнопкой
    QLabel *label = new QLabel(iconName, m_scrollWidget);
    label->setAlignment(Qt::AlignCenter);
    label->setWordWrap(true);
    label->setMaximumWidth(80);
    label->setMinimumHeight(40);
    label->setToolTip(iconName);
    
    // Вычисляем позицию в сетке с учетом подписи
    int gridRow = row * 2;
    
    // Добавляем кнопку и подпись в сетку
    m_gridLayout->addWidget(button, gridRow, column, Qt::AlignCenter);
    m_gridLayout->addWidget(label, gridRow + 1, column, Qt::AlignCenter);
    
    // Подключаем сигнал клика для отладки
    connect(button, &QPushButton::clicked, this, [iconName]() {
        qDebug() << "Clicked icon:" << iconName;
    });
}
