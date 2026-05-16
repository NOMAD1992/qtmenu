#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPushButton>
#include <QLabel>
#include <QDebug>
#include <QStringListModel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_slidingMenu(nullptr)
    , m_userMenu(nullptr)
    , m_bottomSheet(nullptr)
    , m_menuButton(nullptr)
    , m_userMenuButton(nullptr)
    , m_listView(nullptr)
{
    ui->setupUi(this);
    
    // Создаем кнопку меню в стиле GitHub "Open menu"
    m_menuButton = new QPushButton("☰", this);
    m_menuButton->setToolTip("Открыть меню");
    m_menuButton->setCursor(Qt::PointingHandCursor);
    m_menuButton->setStyleSheet(
        "QPushButton {"
        "   background-color: rgba(255, 255, 255, 30);"
        "   color: white;"
        "   border: 1px solid rgba(255, 255, 255, 50);"
        "   padding: 8px 16px;"
        "   border-radius: 6px;"
        "   font-size: 14px;"
        "   font-weight: 500;"
        "}"
        "QPushButton:hover {"
        "   background-color: rgba(255, 255, 255, 50);"
        "   border-color: rgba(255, 255, 255, 80);"
        "}"
        "QPushButton:pressed {"
        "   background-color: rgba(255, 255, 255, 20);"
        "}"
    );
    connect(m_menuButton, &QPushButton::clicked, this, &MainWindow::toggleMenu);
    
    // Добавляем кнопку меню в верхнюю панель (слева)
    ui->m_menuBarLayout->addWidget(m_menuButton);
    ui->m_menuBarLayout->addStretch();
    
    // Создаем кнопку пользователя в стиле GitHub "Open user navigation menu"
    m_userMenuButton = new QPushButton("👤", this);
    m_userMenuButton->setToolTip("Профиль пользователя");
    m_userMenuButton->setCursor(Qt::PointingHandCursor);
    m_userMenuButton->setFixedSize(40, 40);
    m_userMenuButton->setStyleSheet(
        "QPushButton {"
        "   background-color: rgba(255, 255, 255, 30);"
        "   color: white;"
        "   border: 1px solid rgba(255, 255, 255, 50);"
        "   border-radius: 6px;"
        "   font-size: 18px;"
        "}"
        "QPushButton:hover {"
        "   background-color: rgba(255, 255, 255, 50);"
        "   border-color: rgba(255, 255, 255, 80);"
        "}"
        "QPushButton:pressed {"
        "   background-color: rgba(255, 255, 255, 20);"
        "}"
    );
    connect(m_userMenuButton, &QPushButton::clicked, this, &MainWindow::toggleUserMenu);
    
    // Добавляем кнопку пользователя в правую часть верхней панели
    ui->m_menuBarLayout->addWidget(m_userMenuButton);
    
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
    connect(exitBtn, &QPushButton::clicked, []() {
        qDebug() << "Exit clicked";
    });
    
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
        QPoint buttonBottomRight = m_userMenuButton->mapToGlobal(m_userMenuButton->rect().bottomRight());
        int menuX = buttonBottomRight.x() - m_userMenu->width();
        int menuY = buttonBottomRight.y();
        
        m_userMenu->move(menuX, menuY);
        m_userMenu->showMenu();
    }
}

