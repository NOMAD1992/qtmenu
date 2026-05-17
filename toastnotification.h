#ifndef TOASTNOTIFICATION_H
#define TOASTNOTIFICATION_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPropertyAnimation>
#include <QTimer>
#include <QQueue>
#include <QList>
#include <QPair>

// Размеры уведомлений
#define MIN_WIDTH 300
#define MIN_HEIGHT 120

/**
 * @brief Перечисление типов уведомлений.
 */
enum class ToastStatus {
    Information,      ///< Информационное уведомление
    Warning,          ///< Предупреждение
    Error,            ///< Ошибка
    NewChatMessage    ///< Новое письмо в чате
};

/**
 * @brief Класс отдельного уведомления (toast).
 * 
 * ToastWidget представляет собой одно всплывающее уведомление
 * с заголовком, сообщением и кнопкой закрытия.
 */
class ToastWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Конструктор уведомления.
     * @param title Заголовок уведомления.
     * @param message Текст сообщения.
     * @param status Тип уведомления.
     * @param parent Родительский виджет.
     */
    explicit ToastWidget(const QString &title, 
                         const QString &message, 
                         ToastStatus status,
                         QWidget *parent = nullptr);
    
    /**
     * @brief Конструктор уведомления с пользовательской иконкой.
     * @param title Заголовок уведомления.
     * @param message Текст сообщения.
     * @param icon Пользовательская иконка.
     * @param parent Родительский виджет.
     */
    explicit ToastWidget(const QString &title, 
                         const QString &message, 
                         const QIcon &icon,
                         QWidget *parent = nullptr);
    
    /**
     * @brief Деструктор уведомления.
     */
    ~ToastWidget();

    /**
     * @brief Устанавливает время отображения уведомления.
     * @param milliseconds Время в миллисекундах.
     */
    void setDisplayDuration(int milliseconds);

    /**
     * @brief Запускает таймер исчезновения уведомления.
     */
    void startDismissTimer();

    /**
     * @brief Возвращает минимальную высоту уведомления.
     * @return Минимальная высота в пикселях.
     */
    int getMinimumHeight() const;

protected:
    /**
     * @brief Обработчик события входа мыши.
     * @param event Событие входа мыши.
     */
    void enterEvent(QEvent *event) override;
    
    /**
     * @brief Обработчик события выхода мыши.
     * @param event Событие выхода мыши.
     */
    void leaveEvent(QEvent *event) override;
    
    /**
     * @brief Обработчик события отрисовки для рисования фона.
     * @param event Событие отрисовки.
     */
    void paintEvent(QPaintEvent *event) override;

private:
    /**
     * @brief Инициализация UI компонента.
     */
    void setupUi();
    
    /**
     * @brief Применение стилей в зависимости от статуса.
     */
    void applyStyles();
    
    /**
     * @brief Создание кнопки закрытия.
     * @return Указатель на кнопку.
     */
    QPushButton* createCloseButton();
    
    /**
     * @brief Слот для скрытия уведомления с анимацией.
     */
    void dismiss();
    
    /**
     * @brief Слот завершения анимации скрытия.
     */
    void onAnimationFinished();

    QLabel *m_titleLabel;       ///< Метка заголовка
    QLabel *m_messageLabel;     ///< Метка сообщения
    QPushButton *m_closeButton; ///< Кнопка закрытия
    QTimer *m_dismissTimer;     ///< Таймер автоисчезновения
    QPropertyAnimation *m_hideAnimation; ///< Анимация скрытия
    
    int m_displayDuration;      ///< Время отображения в мс
    bool m_isHovered;           ///< Флаг нахождения курсора над уведомлением
};

/**
 * @brief Менеджер всплывающих уведомлений.
 * 
 * ToastNotification управляет очередью уведомлений,
 * отображая максимум 3 уведомления одновременно в правом нижнем углу.
 */
class ToastNotification : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Конструктор менеджера уведомлений.
     * @param parent Родительский виджет (обычно главное окно).
     */
    explicit ToastNotification(QWidget *parent = nullptr);
    
    /**
     * @brief Деструктор менеджера уведомлений.
     */
    ~ToastNotification();

    /**
     * @brief Показывает уведомление.
     * @param title Заголовок уведомления.
     * @param message Текст сообщения.
     * @param status Тип уведомления.
     */
    void showToast(const QString &title, 
                   const QString &message, 
                   ToastStatus status);

    /**
     * @brief Показывает уведомление с пользовательской иконкой.
     * @param title Заголовок уведомления.
     * @param message Текст сообщения.
     * @param icon Пользовательская иконка.
     */
    void showToast(const QString &title, 
                   const QString &message, 
                   const QIcon &icon);

    /**
     * @brief Устанавливает время отображения уведомлений.
     * @param milliseconds Время в миллисекундах (по умолчанию 3000).
     */
    void setDisplayDuration(int milliseconds);

    /**
     * @brief Возвращает текущее время отображения.
     * @return Время в миллисекундах.
     */
    int displayDuration() const;

    /**
     * @brief Устанавливает максимальное количество одновременных уведомлений.
     * @param maxNotifications Максимальное количество (по умолчанию 3).
     */
    void setMaxNotifications(int maxNotifications);

    /**
     * @brief Возвращает максимальное количество одновременных уведомлений.
     * @return Максимальное количество.
     */
    int maxNotifications() const;

    /**
     * @brief Закрывает все активные уведомления.
     */
    void closeAll();
    /**
     * @brief setBottomMargin установка отступа снизу в зависимотсти от вкл/выкл.
     * системные рамки у главного окна
     * @param value значение в px
     */
    void setBottomMargin(const int &value);

private slots:
    /**
     * @brief Обработка очереди уведомлений.
     */
    void processQueue();
    
    /**
     * @brief Обновление позиций всех уведомлений.
     */
    void updatePositions();

private:
    /**
     * @brief Вычисление позиции для уведомления по индексу.
     * @param index Индекс уведомления.
     * @return Позиция в глобальных координатах.
     */
    QPoint calculatePosition(int index);

    QWidget *m_parentWidget;              ///< Родительский виджет
    QList<ToastWidget*> m_activeToasts;   ///< Список активных уведомлений
    
    int m_displayDuration;                ///< Время отображения в мс
    int m_maxNotifications;               ///< Максимум одновременных уведомлений
    int m_spacing;                        ///< Отступ между уведомлениями
    int m_rightMargin;                    ///< Отступ справа
    int m_bottomMargin;                   ///< Отступ снизу
};

#endif // TOASTNOTIFICATION_H
