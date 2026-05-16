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
    
    // Настраиваем чекбокс
    m_slidingMenu->setCheckBoxText("Enable notifications");
    m_slidingMenu->setCheckBoxChecked(false);
    
    // Можно добавить свои кнопки
    QStringList buttons;
    buttons << "Dashboard" << "Repositories" << "Projects" << "Settings";
    m_slidingMenu->addButtons(buttons);
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

