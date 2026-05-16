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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_slidingMenu(nullptr)
    , m_userMenu(nullptr)
    , m_bottomSheet(nullptr)
    , m_listView(nullptr)
    , framelessCheckBox_(nullptr)
{
    ui->setupUi(this);

    setStyleSheet("QPushButton {"
                  "   background-color: rgba(255, 255, 255, 30);"
                  "   color: white;"
                  "   border: 1px solid rgba(255, 255, 255, 50);"
                  "   padding: 2px 4px;"
                  "   border-radius: 2px;"
                  "   font-size: 12px;"
                  "   min-width: 80px;"
                  "}"
                  "QPushButton:hover {"
                  "   background-color: rgba(255, 255, 255, 50);"
                  "   border-color: rgba(255, 255, 255, 80);"
                  "}"
                  "QPushButton:pressed {"
                  "   background-color: rgba(255, 255, 255, 20);"
                  "}");
          
    // Кнопка свернуть
    ui->pbRollup->setText("─");
    ui->pbRollup->setVisible(false);
    ui->pbRollup->setStyleSheet("QPushButton {min-width: 16px;}");
    connect(ui->pbRollup, &QPushButton::clicked, this, &MainWindow::minimizeWindow);

    // Создаем кнопку меню в стиле GitHub "Open menu"
    ui->pbMenu->setToolTip("Открыть меню");
    ui->pbMenu->setCursor(Qt::PointingHandCursor);
    ui->pbMenu->setStyleSheet("QPushButton {min-width: 20px;}");
    connect(ui->pbMenu, &QPushButton::clicked, this, &MainWindow::toggleMenu);
    
    // Создаем кнопку пользователя в стиле GitHub "Open user navigation menu"
    ui->pbUserMenu->setToolTip("Профиль пользователя");
    ui->pbUserMenu->setCursor(Qt::PointingHandCursor);
    ui->pbUserMenu->setStyleSheet("QPushButton {min-width: 20px;}");
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
    QPushButton *settingsBtn = m_slidingMenu->addButton("Settings");
    
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
    QMenu *actionsMenu = m_slidingMenu->addMenu("Actions Menu");
    
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
    QMenu *resolutionMenu = m_slidingMenu->addMenu("Разрешение");
    
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
    
    // Добавляем иконки для кнопок пользователя
    QPixmap usersIcon(16, 16);
    usersIcon.fill(Qt::transparent);
    QPixmap exitIcon(16, 16);
    exitIcon.fill(Qt::transparent);
    
    // Кнопка "Пользователи"
    QPushButton *usersBtn = m_userMenu->addButton("Пользователи", usersIcon);
    connect(usersBtn, &QPushButton::clicked, []() {
        qDebug() << "Users clicked";
    });

    // Разделитель
    m_userMenu->addSplitter();

    // Кнопка "Релогин"
    QPushButton *reloginBtn = m_userMenu->addButton("Сменить пользователя", usersIcon);
    connect(reloginBtn, &QPushButton::clicked, []() {
        qDebug() << "User relogin";
    });
    
    // Разделитель
    m_userMenu->addSplitter();
    
    // Кнопка "Выход"
    QPushButton *exitBtn = m_userMenu->addButton("Выход", exitIcon);
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
    } else {
        setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
        ui->pbRollup->setVisible(false);
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

