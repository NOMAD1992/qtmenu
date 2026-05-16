#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "menubutton.h"
#include "slidemenu.h"
#include <QHBoxLayout>
#include <QMenu>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupMenuBar();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupMenuBar()
{
    QFrame* menuBarFrame = ui->fMenuBar;
    if (!menuBarFrame) return;

    delete menuBarFrame->layout();
    QHBoxLayout* layout = new QHBoxLayout(menuBarFrame);
    layout->setContentsMargins(10, 5, 10, 5);
    layout->setSpacing(15);

    // Левая кнопка – используем SlideMenu::LeftToRight
    MenuButton* btnLeft = new MenuButton("Меню слева", SlideMenu::LeftToRight, this);
    btnLeft->setStyleSheet(
        "QPushButton { background: #2c3e66; border-radius: 6px; padding: 6px 16px; color: white; }"
        "QPushButton:hover { background: #1e2a47; }"
    );
    btnLeft->setMenuBarFrame(menuBarFrame);

    SlideMenu* menuLeft = new SlideMenu();
    menuLeft->setIconLabel(QIcon()); // замените на реальную иконку
    menuLeft->addLabel("Настройки профиля");
    menuLeft->addSeparator();
    menuLeft->addButton("Редактировать", [](){ qDebug() << "Редактировать"; });
    menuLeft->addButton("Сохранить",     [](){ qDebug() << "Сохранить"; });
    menuLeft->addButton("Отменить",      [](){ qDebug() << "Отменить"; });
    menuLeft->addSeparator();
    menuLeft->addCheckbox("Запомнить выбор");
    menuLeft->addSeparator();

    QMenu* subLeft = new QMenu("Подменю", this);
    subLeft->setStyleSheet(
        "QMenu {"
        "    border-radius: 8px;"
        "    background-color: rgba(40, 40, 45, 100);"
        "    color: white;"
        "    margin: 2px;"
        "}"
        "QMenu::item {"
        "    padding: 6px 20px;"
        "}"
        "QMenu::item:selected {"
        "    background-color: rgba(255, 255, 255, 0.2);"
        "    border-radius: 4px;"
        "}"
    );
    subLeft->addAction("Действие 1");
    subLeft->addAction("Действие 2");
    menuLeft->addMenuButton("Дополнительно", subLeft);

    btnLeft->setMenu(menuLeft);
    layout->addWidget(btnLeft);

    // Правая кнопка – используем SlideMenu::RightToLeft
    MenuButton* btnRight = new MenuButton("Меню справа", SlideMenu::RightToLeft, this);
    btnRight->setStyleSheet(
        "QPushButton { background: #662c3e; border-radius: 6px; padding: 6px 16px; color: white; }"
        "QPushButton:hover { background: #471e2b; }"
    );
    btnRight->setMenuBarFrame(menuBarFrame);

    SlideMenu* menuRight = new SlideMenu();
    menuRight->setIconLabel(QIcon());
    menuRight->addLabel("Действия с файлом");
    menuRight->addSeparator();
    menuRight->addButton("Открыть",   [](){ qDebug() << "Открыть"; });
    menuRight->addButton("Закрыть",   [](){ qDebug() << "Закрыть"; });
    menuRight->addButton("Экспорт",   [](){ qDebug() << "Экспорт"; });
    menuRight->addSeparator();
    menuRight->addCheckbox("Автосохранение");
    menuRight->addSeparator();

    QMenu* subRight = new QMenu("Формат", this);
    subRight->setStyleSheet(
        "QMenu {"
        "    border-radius: 8px;"
        "    background-color: rgba(40, 40, 45, 240);"
        "    color: white;"
        "    margin: 2px;"
        "}"
        "QMenu::item {"
        "    padding: 6px 20px;"
        "}"
        "QMenu::item:selected {"
        "    background-color: rgba(255, 255, 255, 0.2);"
        "    border-radius: 4px;"
        "}"
    );
    subRight->addAction("TXT");
    subRight->addAction("PDF");
    menuRight->addMenuButton("Сохранить как...", subRight);

    btnRight->setMenu(menuRight);
    layout->addWidget(btnRight);

    layout->addStretch();
}
