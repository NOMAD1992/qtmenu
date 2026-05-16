#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>
#include "slidemenu.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        setWindowTitle("SlideMenu Demo");
        resize(800, 600);

        // Центральная панель с контентом
        QWidget* centralWidget = new QWidget(this);
        QVBoxLayout* layout = new QVBoxLayout(centralWidget);

        QLabel* titleLabel = new QLabel("Главное окно приложения", this);
        titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #333;");
        titleLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(titleLabel);

        QLabel* infoLabel = new QLabel("Нажмите кнопку ниже, чтобы открыть меню", this);
        infoLabel->setStyleSheet("font-size: 16px; color: #666;");
        infoLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(infoLabel);

        // Кнопка открытия меню
        QPushButton* openMenuBtn = new QPushButton("Open Menu", this);
        openMenuBtn->setFixedSize(150, 50);
        openMenuBtn->setStyleSheet(
            "QPushButton { "
            "   background-color: #4CAF50; "
            "   color: white; "
            "   font-size: 16px; "
            "   border-radius: 8px; "
            "   border: none; "
            "}"
            "QPushButton:hover { "
            "   background-color: #45a049; "
            "}"
        );
        layout->addStretch();
        layout->addWidget(openMenuBtn, 0, Qt::AlignCenter);
        layout->addStretch();

        setCentralWidget(centralWidget);

        // Создаем SlideMenu
        m_slideMenu = new SlideMenu(this);
        m_slideMenu->setAutoHide(true);

        // Добавляем элементы в меню
        m_slideMenu->addLabel("НАВИГАЦИЯ");
        m_slideMenu->addButton("🏠 Главная");
        m_slideMenu->addButton("⚙️ Настройки");
        m_slideMenu->addButton("👤 Профиль");
        
        m_slideMenu->addLabel("ОПЦИИ");
        m_slideMenu->addCheckBox("Включить уведомления");
        m_slideMenu->addCheckBox("Тёмная тема");
        
        // Добавляем выпадающее меню
        QMenu* toolsMenu = m_slideMenu->addMenu("Инструменты");
        toolsMenu->addAction("✂️ Вырезать");
        toolsMenu->addAction("📋 Копировать");
        toolsMenu->addAction("📄 Вставить");
        toolsMenu->addSeparator();
        toolsMenu->addAction("🔍 Найти");

        m_slideMenu->addLabel("СИСТЕМА");
        m_slideMenu->addButton("❓ Помощь");
        m_slideMenu->addButton("🚪 Выход");

        // Подключаем сигналы
        connect(openMenuBtn, &QPushButton::clicked, [this]() {
            m_slideMenu->slideIn(SlideMenu::SlideInFromRight);
        });

        connect(m_slideMenu, &SlideMenu::hidden, []() {
            qDebug() << "Меню закрыто";
        });

        connect(m_slideMenu, &SlideMenu::menuActionTriggered, [](QAction* action) {
            qDebug() << "Действие:" << action->text();
        });
    }

private:
    SlideMenu* m_slideMenu;
};

#include "main.moc"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    MainWindow window;
    window.show();
    
    return app.exec();
}
