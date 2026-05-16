#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPushButton>
#include <QLabel>

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
    
    // Очищаем стандартное содержимое для демонстрации универсального API
    m_slidingMenu->clearContent();
    
    // Пример 1: Добавление кнопки созданной в MainWindow
    QPushButton *customButton1 = new QPushButton("📊 Dashboard");
    customButton1->setStyleSheet(
        "QPushButton {"
        "   background-color: rgba(100, 150, 200, 80);"
        "   color: white;"
        "   border: none;"
        "   padding: 12px;"
        "   text-align: left;"
        "   border-radius: 6px;"
        "   font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "   background-color: rgba(100, 150, 200, 120);"
        "}"
    );
    connect(customButton1, &QPushButton::clicked, [this]() {
        qDebug() << "Dashboard clicked!";
        m_slidingMenu->hideMenu();
    });
    m_slidingMenu->addButton(customButton1);
    
    // Пример 2: Добавление другой кнопки
    QPushButton *customButton2 = new QPushButton("📁 Repositories");
    connect(customButton2, &QPushButton::clicked, [this]() {
        qDebug() << "Repositories clicked!";
        m_slidingMenu->hideMenu();
    });
    m_slidingMenu->addButton(customButton2);
    
    // Пример 3: Добавление разделителя
    m_slidingMenu->addSeparator();
    
    // Пример 4: Добавление QLabel
    QLabel *infoLabel = new QLabel("Settings:");
    infoLabel->setStyleSheet("color: rgba(255, 255, 255, 150); font-size: 12px;");
    m_slidingMenu->addWidget(infoLabel);
    
    // Пример 5: Добавление чекбокса
    QCheckBox *notificationsCheckBox = new QCheckBox("Enable notifications");
    notificationsCheckBox->setStyleSheet(
        "QCheckBox {"
        "   color: white;"
        "   font-size: 14px;"
        "   spacing: 8px;"
        "}"
        "QCheckBox::indicator {"
        "   width: 18px;"
        "   height: 18px;"
        "   border-radius: 4px;"
        "   border: 2px solid rgba(255, 255, 255, 150);"
        "   background-color: transparent;"
        "}"
        "QCheckBox::indicator:checked {"
        "   background-color: rgba(100, 200, 100, 200);"
        "}"
    );
    connect(notificationsCheckBox, &QCheckBox::toggled, [](bool checked) {
        qDebug() << "Notifications:" << (checked ? "enabled" : "disabled");
    });
    m_slidingMenu->addWidget(notificationsCheckBox);
    
    // Пример 6: Добавление разделителя
    m_slidingMenu->addSeparator();
    
    // Пример 7: Добавление layout с несколькими виджетами
    QHBoxLayout *profileLayout = new QHBoxLayout();
    QLabel *avatarLabel = new QLabel();
    avatarLabel->setFixedSize(40, 40);
    avatarLabel->setStyleSheet("background-color: rgba(255, 255, 255, 100); border-radius: 20px;");
    QLabel *nameLabel = new QLabel("User Name");
    nameLabel->setStyleSheet("color: white; font-size: 14px; font-weight: bold;");
    profileLayout->addWidget(avatarLabel);
    profileLayout->addWidget(nameLabel);
    profileLayout->addStretch();
    m_slidingMenu->addLayout(profileLayout);
    
    // Пример 8: Добавление QMenu через кнопку
    QPushButton *actionsButton = new QPushButton("⚙ Actions");
    actionsButton->setStyleSheet(
        "QPushButton {"
        "   background-color: rgba(255, 255, 255, 50);"
        "   color: white;"
        "   border: none;"
        "   padding: 10px;"
        "   border-radius: 5px;"
        "}"
        "QPushButton:hover {"
        "   background-color: rgba(255, 255, 255, 80);"
        "}"
    );
    QMenu *actionsMenu = new QMenu(this);
    actionsMenu->addAction("Profile", []() { qDebug() << "Profile selected"; });
    actionsMenu->addAction("Settings", []() { qDebug() << "Settings selected"; });
    QMenu *subMenu = actionsMenu->addMenu("Advanced");
    subMenu->addAction("Export", []() { qDebug() << "Export selected"; });
    subMenu->addAction("Import", []() { qDebug() << "Import selected"; });
    connect(actionsButton, &QPushButton::clicked, [this, actionsButton, actionsMenu]() {
        actionsMenu->exec(actionsButton->mapToGlobal(QPoint(0, actionsButton->height())));
    });
    m_slidingMenu->addWidget(actionsButton);
    
    // Пример 9: Добавление stretch для прижатия контента к верху
    m_slidingMenu->addWidget(new QWidget(), 1);  // stretch widget
    
    // Пример 10: Добавление кнопки в низ меню
    QPushButton *logoutButton = new QPushButton("🚪 Logout");
    logoutButton->setStyleSheet(
        "QPushButton {"
        "   background-color: rgba(200, 50, 50, 80);"
        "   color: white;"
        "   border: none;"
        "   padding: 12px;"
        "   border-radius: 6px;"
        "   font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "   background-color: rgba(200, 50, 50, 120);"
        "}"
    );
    connect(logoutButton, &QPushButton::clicked, [this]() {
        qDebug() << "Logout clicked!";
        m_slidingMenu->hideMenu();
    });
    m_slidingMenu->addWidget(logoutButton);
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

