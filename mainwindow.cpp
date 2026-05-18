#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPushButton>
#include <QLabel>
#include <QDebug>
#include <QStringListModel>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QActionGroup>
#include <QScreen>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include "toastnotification.h"

#define TOAST_TIMEOUT 6000

// Глобальный указатель на BottomSheet для перехвата qDebug
static BottomSheet *g_bottomSheet = nullptr;

// Структура для хранения оригинальных хуков
struct QtDebugHooks {
    QtMessageHandler originalHandler;
};
static QtDebugHooks g_debugHooks;

/**
 * @brief Пользовательская функция для обработки сообщений Qt (qDebug, qInfo, qWarning, qCritical).
 * @param type Тип сообщения.
 * @param context Контекст сообщения.
 * @param msg Текст сообщения.
 */
void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // Сначала вызываем оригинальный обработчик (чтобы сообщения всё ещё выводились в консоль)
    if (g_debugHooks.originalHandler) {
        g_debugHooks.originalHandler(type, context, msg);
    }
    
    // Добавляем сообщение в BottomSheet если он доступен
    if (g_bottomSheet) {
        QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
        QString prefix;
        
        switch (type) {
            case QtDebugMsg:
                prefix = "[DEBUG]";
                break;
            case QtInfoMsg:
                prefix = "[INFO]";
                break;
            case QtWarningMsg:
                prefix = "[WARN]";
                break;
            case QtCriticalMsg:
                prefix = "[ERROR]";
                break;
            case QtFatalMsg:
                prefix = "[FATAL]";
                break;
        }
        
        QString logMessage = QString("%1 %2").arg(prefix).arg(msg);
        g_bottomSheet->appendLogMessage(logMessage);
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_slidingMenu(nullptr)
    , m_userMenu(nullptr)
    , m_bottomSheet(nullptr)
    , m_listView(nullptr)
    , framelessCheckBox_(nullptr)
    , m_toastNotification(nullptr)
    , m_toastTimer(nullptr)
    , m_toastCounter(0)
    , m_standardIconsWidget(nullptr)
{
    ui->setupUi(this);
    
    // Сначала создаём BottomSheet, чтобы перехватчик qDebug мог его использовать
    setupBottomSheet();
    
    // Инициализация компонентов
    loadStyles();
    setupWindowControlButtons();
    initToastNotifications();
    setupSlidingMenu();
    setupUserMenu();
}

void MainWindow::initToastNotifications()
{
    m_toastNotification = new ToastNotification(this);
    m_toastNotification->setDisplayDuration(TOAST_TIMEOUT);
    
    connect(m_toastNotification, &ToastNotification::informationClicked, 
            this, &MainWindow::onInformationClicked);
    connect(m_toastNotification, &ToastNotification::warningClicked, 
            this, &MainWindow::onWarningClicked);
    connect(m_toastNotification, &ToastNotification::errorClicked, 
            this, &MainWindow::onErrorClicked);
    connect(m_toastNotification, &ToastNotification::newChatMessageClicked, 
            this, &MainWindow::onNewChatMessageClicked);
    
    m_toastTimer = new QTimer(this);
    connect(m_toastTimer, &QTimer::timeout, this, &MainWindow::showTestToast);
}

void MainWindow::loadStyles()
{
    QFile styleFile(":/styles.qss");
    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&styleFile);
        QString stylesheet = stream.readAll();
        setStyleSheet(stylesheet);
        styleFile.close();
    } else {
        qDebug() << "Не удалось загрузить файл стилей styles.qss";
    }
}

void MainWindow::setupWindowControlButtons()
{
    ui->pbRollup->setText("─");
    ui->pbRollup->setVisible(false);
    connect(ui->pbRollup, &QPushButton::clicked, this, &MainWindow::minimizeWindow);
    
    ui->pbMenu->setToolTip("Открыть меню");
    ui->pbMenu->setCursor(Qt::PointingHandCursor);
    connect(ui->pbMenu, &QPushButton::clicked, this, &MainWindow::toggleMenu);
        
    m_standardIconsWidget = new StandardIconsWidget(this);
    ui->vlIcons->addWidget(m_standardIconsWidget);
}

void MainWindow::setupSlidingMenu()
{
    m_slidingMenu = new SlidingMenu(this, SlidingMenu::SlideDirection::FromLeft, 300);
    m_slidingMenu->setTitle("Главное меню");
    
    // Настраиваем кнопку для открытия пользовательского меню
    QPushButton *userMenuBtn = m_slidingMenu->userMenuButton();
    if (userMenuBtn) {
        userMenuBtn->setIcon(QIcon::fromTheme("user-available"));
        userMenuBtn->setToolTip("Профиль пользователя");
    }
    
    // Подключаем сигнал открытия пользовательского меню
    connect(m_slidingMenu, &SlidingMenu::userMenuRequested, this, &MainWindow::toggleUserMenu);
    
    // Подключаем сигнал завершения работы
    connect(m_slidingMenu, &SlidingMenu::exitRequested, this, &MainWindow::closeWindow);
    
    setupSlidingMenuItems();
}

void MainWindow::setupSlidingMenuItems()
{
    QPushButton *dashboardBtn = m_slidingMenu->addButton("Dashboard");
    QPushButton *repositoriesBtn = m_slidingMenu->addButton("Repositories");
    QPushButton *projectsBtn = m_slidingMenu->addButton("Projects");
    QPushButton *settingsBtn = m_slidingMenu->addButton("О программе", QIcon::fromTheme("dialog-information"));
    
    m_slidingMenu->addSplitter();
    
    QCheckBox *notificationsCheckBox = m_slidingMenu->addCheckBox("Уведомления");
    notificationsCheckBox->setChecked(false);
    notificationsCheckBox->setStyleSheet("QCheckBox { color: white; }");
    connect(notificationsCheckBox, &QCheckBox::toggled, this, [=](bool checked) {
        if (checked) {
            m_toastTimer->start(2000);
        } else {
            m_toastTimer->stop();
        }
    });
    
    connect(notificationsCheckBox, &QCheckBox::toggled, [](bool checked) {
        qDebug() << "Notifications:" << (checked ? "enabled" : "disabled");
    });
    
    framelessCheckBox_ = m_slidingMenu->addCheckBox("F12 Полноэкранный режим");
    framelessCheckBox_->setChecked(false);
    framelessCheckBox_->setStyleSheet("QCheckBox { color: white; }");
    connect(framelessCheckBox_, &QCheckBox::toggled, this, &MainWindow::toggleFrameless);
    
    QMenu *actionsMenu = m_slidingMenu->addMenu("Документация", QIcon::fromTheme("help-faq"));
    
    if (actionsMenu) {
        QAction *action1 = actionsMenu->addAction("Action 1");
        QAction *action2 = actionsMenu->addAction("Action 2");
        
        QMenu *subMenu = actionsMenu->addMenu("Submenu");
        subMenu->addAction("Sub Action 1");
        subMenu->addAction("Sub Action 2");
        
        QMenu *nestedSubMenu = subMenu->addMenu("Nested Submenu");
        nestedSubMenu->addAction("Nested Action 1");
        nestedSubMenu->addAction("Nested Action 2");
        
        actionsMenu->addSeparator();
        actionsMenu->addAction("Action 3");
    }
    
    setupResolutionMenu();
    
    // Подключаем сигналы кнопок меню
    connect(dashboardBtn, &QPushButton::clicked, []() {
        qDebug() << "Dashboard clicked";
    });
    connect(repositoriesBtn, &QPushButton::clicked, []() {
        qDebug() << "Repositories clicked";
    });
    connect(projectsBtn, &QPushButton::clicked, []() {
        qDebug() << "Projects clicked";
    });
    connect(settingsBtn, &QPushButton::clicked, []() {
        qDebug() << "Settings clicked";
    });
}

void MainWindow::setupResolutionMenu()
{
    QMenu *resolutionMenu = m_slidingMenu->addMenu("Разрешение", QIcon::fromTheme("view-fullscreen"));
    
    if (resolutionMenu) {
        QActionGroup *resolutionGroup = new QActionGroup(this);
        resolutionGroup->setExclusive(true);
        
        QList<QSize> resolutions = {
            QSize(1920, 1080),
            QSize(1536, 864),
            QSize(1366, 768),
            QSize(1280, 720),
            QSize(1440, 900),
            QSize(1024, 768)
        };
        
        for (const QSize &res : resolutions) {
            QString actionText = QString("%1×%2").arg(res.width()).arg(res.height());
            QAction *action = resolutionMenu->addAction(actionText);
            action->setCheckable(true);
            action->setData(QVariant::fromValue(res));
            resolutionGroup->addAction(action);
            connect(action, &QAction::triggered, this, [this, action]() {
                changeResolution(action);
            });
        }
    }
}

void MainWindow::setupUserMenu()
{
    m_userMenu = new UserMenu(this, 180);
       
    QPushButton *usersBtn = m_userMenu->addButton("Пользователи", QIcon::fromTheme("system-users"));
    connect(usersBtn, &QPushButton::clicked, []() {
        qDebug() << "Users clicked";
    });

    m_userMenu->addSplitter();

    QPushButton *reloginBtn = m_userMenu->addButton("Сменить пользователя", QIcon::fromTheme("system-switch-user"));
    connect(reloginBtn, &QPushButton::clicked, []() {
        qDebug() << "User relogin";
    });
    
    m_userMenu->addSplitter();
    
    QPushButton *exitBtn = m_userMenu->addButton("Выход", QIcon::fromTheme("application-exit"));
    connect(exitBtn, &QPushButton::clicked, this, &MainWindow::closeWindow);
}

void MainWindow::setupBottomSheet()
{
    m_bottomSheet = new BottomSheet(this);
    
    m_listView = new QListView(this);
    QStringListModel *model = new QStringListModel(this);
    m_listView->setModel(model);
    
    addListView(m_listView);
    
    // Устанавливаем глобальный указатель для перехватчика qDebug
    g_bottomSheet = m_bottomSheet;
    
    // Сохраняем оригинальный обработчик сообщений Qt
    g_debugHooks.originalHandler = qInstallMessageHandler(customMessageHandler);
    
    QStringList messages;
    messages << "Сообщение 1: Приложение запущено"
             << "Сообщение 2: Меню инициализировано"
             << "Сообщение 3: Шторка готова к работе";
    model->setStringList(messages);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addListView(QListView *view)
{
    if (m_bottomSheet) {
        m_bottomSheet->addListView(view);
    }
}

void MainWindow::toggleMenu()
{
    if (m_slidingMenu->isMenuVisible()) {
        m_slidingMenu->hideMenu();
    } else {
        m_slidingMenu->showMenu();
    }
}

void MainWindow::toggleUserMenu(const QPoint &point)
{
    if (m_userMenu->isMenuVisible()) {
        m_userMenu->hideMenu();
    } else {
        int menuX = point.x();
        int menuY = point.y();
        
        m_userMenu->move(menuX, menuY);
        m_userMenu->showMenu();
    }
}

void MainWindow::toggleFrameless(bool checked)
{
    if (checked) {
        setWindowFlags(Qt::FramelessWindowHint);
        ui->pbRollup->setVisible(true);
        m_toastNotification->setBottomMargin(5);
    } else {
        setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
        ui->pbRollup->setVisible(false);
        m_toastNotification->setBottomMargin(36);
    }
    show();

    if (checked) {
        showMaximized();
    } else {
        showNormal();
    }
}

void MainWindow::minimizeWindow()
{
    showMinimized();
}

void MainWindow::closeWindow()
{
    close();
}

void MainWindow::changeResolution(QAction *action)
{
    if (!action) return;
    
    QSize resolution = action->data().toSize();
    if (resolution.isValid()) {
        resize(resolution);
        
        // Центрируем окно на экране
        QScreen *screen = QApplication::primaryScreen();
        if (screen) {
            QRect screenGeometry = screen->availableGeometry();
            int x = (screenGeometry.width() - resolution.width()) / 2 + screenGeometry.x();
            int y = (screenGeometry.height() - resolution.height()) / 2 + screenGeometry.y();
            move(x, y);
        }
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F12) {
        framelessCheckBox_->setChecked(!framelessCheckBox_->isChecked());
    }
    if (event->key() == Qt::Key_F11) {
        toggleMenuBar();
    }
    QMainWindow::keyPressEvent(event);
}

void MainWindow::toggleMenuBar()
{
    bool isVisible = ui->fMenuBar->isVisible();
    if (isVisible) {
        ui->fMenuBar->hide();
    } else {
        ui->fMenuBar->show();
    }
    
    // Обновляем позицию шторки в зависимости от видимости панели меню
    if (m_bottomSheet) {
        m_bottomSheet->updateForMenuBarVisibility(!isVisible);
    }
}

void MainWindow::showTestToast()
{
    m_toastCounter++;
    
    // Циклически меняем типы уведомлений
    ToastStatus status;
    QString title;
    QString message;
    
    int typeIndex = m_toastCounter % 4;
    
    switch (typeIndex) {
        case 0:
            status = ToastStatus::Information;
            title = "Информация";
            message = QString("Это информационное сообщение #%1. Система работает нормально.").arg(m_toastCounter);
            break;
        case 1:
            status = ToastStatus::Warning;
            title = "Предупреждение";
            message = QString("Внимание! Обнаружено предупреждение #%1. Проверьте настройки.").arg(m_toastCounter);
            break;
        case 2:
            status = ToastStatus::Error;
            title = "Ошибка";
            message = QString("Произошла ошибка #%1. Требуется вмешательство пользователя.").arg(m_toastCounter);
            break;
        case 3:
            status = ToastStatus::NewChatMessage;
            title = "Новое сообщение";
            message = QString("В чате получено новое сообщение #%1 от пользователя.").arg(m_toastCounter);
            break;
        default:
            status = ToastStatus::Information;
            title = "Информация";
            message = QString("Сообщение #%1").arg(m_toastCounter);
    }
    
    // Показываем уведомление
    m_toastNotification->showToast(title, message, status);
    
    qDebug() << "Показано уведомление:" << title << "-" << message;
}


void MainWindow::onInformationClicked()
{
    qDebug() << "MainWindow: Information notification clicked!";
}

void MainWindow::onWarningClicked()
{
    qDebug() << "MainWindow: Warning notification clicked!";
}

void MainWindow::onErrorClicked()
{
    qDebug() << "MainWindow: Error notification clicked!";
}

void MainWindow::onNewChatMessageClicked()
{
    qDebug() << "MainWindow: New chat message notification clicked!";
}
