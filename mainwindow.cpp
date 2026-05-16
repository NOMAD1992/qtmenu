#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPushButton>
#include <QLabel>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_slidingMenu(nullptr)
    , m_menuButton(nullptr)
{
    ui->setupUi(this);
    
    // Создаем кнопку меню в стиле GitHub "Open menu"
    m_menuButton = new QPushButton("☰ Open menu", this);
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
    
    // Добавляем кнопку меню в верхнюю панель
    ui->m_menuBarLayout->addWidget(m_menuButton);
    ui->m_menuBarLayout->addStretch();
    
    // Создаем выезжающее меню слева направо
    m_slidingMenu = new SlidingMenu(this, SlidingMenu::SlideDirection::FromLeft, 300);
    m_slidingMenu->setTitle("GitHub Style Menu");
    
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
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::toggleMenu()
{
    if (m_slidingMenu->isMenuVisible()) {
        m_slidingMenu->hideMenu();
    } else {
        m_slidingMenu->showMenu();
    }
}

