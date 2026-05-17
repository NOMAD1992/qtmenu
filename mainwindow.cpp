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
#include "toastnotification.h"

#define TOAST_TIMEOUT 6000

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
{
    ui->setupUi(this);
    
    // Инициализация системы уведомлений
    m_toastNotification = new ToastNotification(this);
    
    // Настраиваем длительность отображения (5 секунды по умолчанию)
    m_toastNotification->setDisplayDuration(TOAST_TIMEOUT);
    
    // Создаём таймер для автоматического показа тестовых уведомлений
    m_toastTimer = new QTimer(this);
    connect(m_toastTimer, &QTimer::timeout, this, &MainWindow::showTestToast);
    m_toastTimer->start(2000); // Показываем новое уведомление каждые 2 секунды
    
    // Загружаем стили из файла styles.qss
    QFile styleFile(":/styles.qss");
    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&styleFile);
        QString stylesheet = stream.readAll();
        setStyleSheet(stylesheet);
        styleFile.close();
    } else {
        qDebug() << "Не удалось загрузить файл стилей styles.qss";
    }
          
    // Кнопка свернуть
    ui->pbRollup->setText("─");
    ui->pbRollup->setVisible(false);
    connect(ui->pbRollup, &QPushButton::clicked, this, &MainWindow::minimizeWindow);

    // Создаем кнопку меню в стиле GitHub "Open menu"
    ui->pbMenu->setToolTip("Открыть меню");
    ui->pbMenu->setCursor(Qt::PointingHandCursor);
    connect(ui->pbMenu, &QPushButton::clicked, this, &MainWindow::toggleMenu);
    
    // Создаем кнопку пользователя в стиле GitHub "Open user navigation menu"
    ui->pbUserMenu->setToolTip("Профиль пользователя");
    ui->pbUserMenu->setCursor(Qt::PointingHandCursor);
    connect(ui->pbUserMenu, &QPushButton::clicked, this, &MainWindow::toggleUserMenu);
    
    // Создаем выезжающее меню слева направо
    m_slidingMenu = new SlidingMenu(this, SlidingMenu::SlideDirection::FromLeft, 300);
    m_slidingMenu->setTitle("Главное меню");
    
    // Устанавливаем иконку (можно заменить на свою)
    QPixmap icon(32, 32);
    icon.fill(Qt::transparent);
    // Здесь можно загрузить реальную иконку
    // m_slidingMenu->setIcon(QPixmap(":/path/to/icon.png"));
    
    // Создаем элементы меню через новые методы
    // Добавляем кнопки
    QPushButton *dashboardBtn = m_slidingMenu->addButton("Dashboard");
    QPushButton *repositoriesBtn = m_slidingMenu->addButton("Repositories");
    QPushButton *projectsBtn = m_slidingMenu->addButton("Projects");
    QPushButton *settingsBtn = m_slidingMenu->addButton("О программе", QPixmap(":/img/documentation.png"));
    
    // Добавляем разделитель
    m_slidingMenu->addSplitter();
    
    // Добавляем чекбокс
    QCheckBox *notificationsCheckBox = m_slidingMenu->addCheckBox("Enable notifications");
    // Чекбокс для включения/выключения рамки
    framelessCheckBox_ = m_slidingMenu->addCheckBox("F12 Полноэкранный режим");
    framelessCheckBox_->setChecked(false);
    framelessCheckBox_->setStyleSheet("QCheckBox { color: white; }");
    connect(framelessCheckBox_, &QCheckBox::toggled, this, &MainWindow::toggleFrameless);
    
    // Добавляем меню с действиями
    QMenu *actionsMenu = m_slidingMenu->addMenu("Документация", QPixmap(":/img/documentation_v2.png"));
    
    // Настраиваем созданное меню (добавляем пункты и подменю)
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
    
    // Добавляем меню "Разрешение" с подменю для смены разрешения окна
    QMenu *resolutionMenu = m_slidingMenu->addMenu("Разрешение", QPixmap(":/img/screen_resolution.png"));
    
    if (resolutionMenu) {
        // Создаем группу действий для взаимоисключающего выбора
        QActionGroup *resolutionGroup = new QActionGroup(this);
        resolutionGroup->setExclusive(true);
        
        // Список разрешений
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
    
    // Можно подключиться к сигналам созданных элементов
    if (notificationsCheckBox) {
        connect(notificationsCheckBox, &QCheckBox::toggled, [](bool checked) {
            qDebug() << "Notifications:" << (checked ? "enabled" : "disabled");
        });
    }
    
    // Пример подключения к кнопкам
    if (dashboardBtn) {
        connect(dashboardBtn, &QPushButton::clicked, []() {
            qDebug() << "Dashboard clicked";
        });
    }
    if (repositoriesBtn) {
        connect(repositoriesBtn, &QPushButton::clicked, []() {
            qDebug() << "Repositories clicked";
        });
    }
    if (projectsBtn) {
        connect(projectsBtn, &QPushButton::clicked, []() {
            qDebug() << "Projects clicked";
        });
    }
    if (settingsBtn) {
        connect(settingsBtn, &QPushButton::clicked, []() {
            qDebug() << "Settings clicked";
        });
    }
    
    // Создаем выпадающее меню пользователя справа
    m_userMenu = new UserMenu(this, 180);
       
    // Кнопка "Пользователи"
    QPushButton *usersBtn = m_userMenu->addButton("Пользователи", QPixmap(":/img/users.png"));
    connect(usersBtn, &QPushButton::clicked, []() {
        qDebug() << "Users clicked";
    });

    // Разделитель
    m_userMenu->addSplitter();

    // Кнопка "Релогин"
    QPushButton *reloginBtn = m_userMenu->addButton("Сменить пользователя", QPixmap(":/img/relogin.jpg"));
    connect(reloginBtn, &QPushButton::clicked, []() {
        qDebug() << "User relogin";
    });
    
    // Разделитель
    m_userMenu->addSplitter();
    
    // Кнопка "Выход"
    QPushButton *exitBtn = m_userMenu->addButton("Выход", QPixmap(":/img/exit.png"));
    connect(exitBtn, &QPushButton::clicked, this, &MainWindow::closeWindow);
    
    // Создаем нижнюю полупрозрачную шторку (всегда открыта)
    m_bottomSheet = new BottomSheet(this);
    
    // Создаем QListView для отображения сообщений вместо QDebug
    m_listView = new QListView(this);
    QStringListModel *model = new QStringListModel(this);
    m_listView->setModel(model);
    
    // Добавляем listView в шторку
    addListView(m_listView);
    
    // Добавляем тестовые сообщения
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

void MainWindow::toggleUserMenu()
{
    if (m_userMenu->isMenuVisible()) {
        m_userMenu->hideMenu();
    } else {
        // Вычисляем позицию: правый верхний угол меню должен быть там, где правый нижний угол кнопки
        QPoint buttonBottomRight = ui->pbUserMenu->mapToGlobal(ui->pbUserMenu->rect().bottomRight());
        int menuX = buttonBottomRight.x() - m_userMenu->width();
        int menuY = buttonBottomRight.y();
        
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
    QMainWindow::keyPressEvent(event);
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

