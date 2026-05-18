#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListView>
#include <QTimer>
#include "slidingmenu.h"
#include "usermenu.h"
#include "bottomsheet.h"
#include "toastnotification.h"
#include "standardiconswidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * @brief Основной класс главного окна приложения.
 * 
 * MainWindow управляет основным интерфейсом приложения, включая выдвижные меню,
 * пользовательское меню и нижнюю шторку с списком.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Конструктор главного окна.
     * @param parent Родительский виджет (по умолчанию nullptr).
     */
    MainWindow(QWidget *parent = nullptr);
    
    /**
     * @brief Деструктор главного окна.
     */
    ~MainWindow();

    /**
     * @brief Добавляет QListView в нижнюю шторку.
     * @param view Указатель на QListView для добавления.
     */
    void addListView(QListView *view);

protected:
    /**
     * @brief Обработчик событий нажатия клавиш.
     * @param event Событие нажатия клавиши.
     */
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    /**
     * @brief Переключает видимость выдвижного меню.
     */
    void toggleMenu();
    
    /**
     * @brief Переключает видимость пользовательского меню.
     */
    void toggleUserMenu(const QPoint &point);
    
    /**
     * @brief Переключает режим безрамочного окна.
     * @param checked Состояние чекбокса (true - безрамочный режим включен).
     */
    void toggleFrameless(bool checked);
    
    /**
     * @brief Сворачивает окно приложения.
     */
    void minimizeWindow();
    
    /**
     * @brief Закрывает окно приложения.
     */
    void closeWindow();
    
    /**
     * @brief Изменяет разрешение экрана при выборе действия из меню.
     * @param action Действие, содержащее информацию о разрешении.
     */
    void changeResolution(QAction *action);
    
    /**
     * @brief Показывает тестовое уведомление.
     */
    void showTestToast();
    
    /**
     * @brief Слот обработки нажатия на информационное уведомление.
     */
    void onInformationClicked();
    
    /**
     * @brief Слот обработки нажатия на уведомление-предупреждение.
     */
    void onWarningClicked();
    
    /**
     * @brief Слот обработки нажатия на уведомление об ошибке.
     */
    void onErrorClicked();
    
    /**
     * @brief Слот обработки нажатия на уведомление о новом сообщении чата.
     */
    void onNewChatMessageClicked();

private:
    /**
     * @brief Инициализация системы уведомлений Toast.
     */
    void initToastNotifications();
    
    /**
     * @brief Загрузка стилей из файла QSS.
     */
    void loadStyles();
    
    /**
     * @brief Настройка кнопок управления окном.
     */
    void setupWindowControlButtons();
    
    /**
     * @brief Настройка главного выдвижного меню (SlidingMenu).
     */
    void setupSlidingMenu();
    
    /**
     * @brief Настройка элементов главного меню (кнопки, чекбоксы, подменю).
     */
    void setupSlidingMenuItems();
    
    /**
     * @brief Настройка меню разрешения экрана.
     */
    void setupResolutionMenu();
    
    /**
     * @brief Настройка пользовательского меню (UserMenu).
     */
    void setupUserMenu();
    
    /**
     * @brief Настройка нижней шторки (BottomSheet) и ListView.
     */
    void setupBottomSheet();

private:
    Ui::MainWindow *ui;           ///< Указатель на интерфейс пользователя.
    SlidingMenu *m_slidingMenu;   ///< Указатель на выдвижное меню.
    UserMenu *m_userMenu;         ///< Указатель на пользовательское меню.
    BottomSheet *m_bottomSheet;   ///< Указатель на нижнюю шторку.
    QListView *m_listView;        ///< Указатель на список в шторке.
    QCheckBox *framelessCheckBox_; ///< Указатель на чекбокс безрамочного режима.
    ToastNotification *m_toastNotification; ///< Указатель на систему уведомлений.
    QTimer *m_toastTimer;         ///< Таймер для показа тестовых уведомлений.
    int m_toastCounter;           ///< Счётчик для генерации разных сообщений.
    StandardIconsWidget *m_standardIconsWidget; ///< Указатель на виджет стандартных иконок.
};

#endif // MAINWINDOW_H
