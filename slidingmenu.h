#ifndef SLIDINGMENU_H
#define SLIDINGMENU_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QMenu>
#include <QVBoxLayout>
#include <QPropertyAnimation>
#include <QResizeEvent>

/**
 * @brief Класс выдвижного меню с анимацией.
 * 
 * SlidingMenu предоставляет функционал для создания анимированного выдвижного меню,
 * которое может появляться слева или справа. Поддерживает добавление кнопок,
 * чекбоксов, меню и разделителей.
 */
class SlidingMenu : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int menuOffset READ menuOffset WRITE setMenuOffset)

public:
    /**
     * @brief Направление появления меню.
     */
    enum class SlideDirection {
        FromLeft,   ///< Появление слева
        FromRight   ///< Появление справа
    };

    /**
     * @brief Конструктор выдвижного меню.
     * @param parent Родительский виджет (по умолчанию nullptr).
     * @param direction Направление появления меню (по умолчанию слева).
     * @param menuWidth Ширина меню в пикселях (по умолчанию 300).
     */
    explicit SlidingMenu(QWidget *parent = nullptr, 
                         SlideDirection direction = SlideDirection::FromLeft,
                         int menuWidth = 300);
    
    /**
     * @brief Деструктор выдвижного меню.
     */
    ~SlidingMenu();

    /**
     * @brief Устанавливает заголовок меню.
     * @param title Текст заголовка.
     */
    void setTitle(const QString &title);
    
    /**
     * @brief Устанавливает иконку меню.
     * @param iconPixmap Изображение иконки.
     */
    void setIcon(const QPixmap &icon);
    
    /**
     * @brief Добавляет кнопку в меню.
     * @param text Текст кнопки.
     * @param icon Иконка кнопки (по умолчанию пустая).
     * @return Указатель на созданную кнопку.
     */
    QPushButton* addButton(const QString &text, const QPixmap &icon = QPixmap());
    
    /**
     * @brief Добавляет чекбокс в меню.
     * @param text Текст чекбокса.
     * @return Указатель на созданный чекбокс.
     */
    QCheckBox* addCheckBox(const QString &text);
    
    /**
     * @brief Добавляет выпадающее меню.
     * @param title Заголовок меню.
     * @param icon Иконка меню (по умолчанию пустая).
     * @return Указатель на созданное меню.
     */
    QMenu* addMenu(const QString &title, const QPixmap &icon);
    
    /**
     * @brief Добавляет разделитель в меню.
     */
    void addSplitter();
    
    /**
     * @brief Показывает меню с анимацией.
     */
    void showMenu();
    
    /**
     * @brief Скрывает меню с анимацией.
     */
    void hideMenu();
    
    /**
     * @brief Проверяет видимость меню.
     * @return true, если меню видимо, иначе false.
     */
    bool isMenuVisible() const;

    /**
     * @brief Возвращает указатель на метку заголовка.
     * @return Указатель на QLabel заголовка.
     */
    QLabel *titleLabel() const { return m_titleLabel; }
    
    /**
     * @brief Возвращает указатель на метку иконки.
     * @return Указатель на QLabel иконки.
     */
    QLabel *iconLabel() const { return m_iconLabel; }
    
    /**
     * @brief Возвращает указатель на кнопку закрытия.
     * @return Указатель на QPushButton закрытия.
     */
    QPushButton *closeButton() const { return m_closeButton; }
    
    /**
     * @brief Возвращает указатель на чекбокс.
     * @return Указатель на QCheckBox.
     */
    QCheckBox *checkBox() const { return m_checkBox; }
    
    /**
     * @brief Возвращает указатель на меню.
     * @return Указатель на QMenu.
     */
    QMenu *menu() const { return m_menu; }
    
    /**
     * @brief Возвращает список всех кнопок в меню.
     * @return QList<QPushButton*> со всеми кнопками.
     */
    QList<QPushButton*> buttons() const { return m_buttons; }

signals:
    /**
     * @brief Сигнал о завершении показа меню.
     */
    void menuShown();
    
    /**
     * @brief Сигнал о завершении скрытия меню.
     */
    void menuHidden();
    
    /**
     * @brief Сигнал об изменении состояния чекбокса.
     * @param checked Новое состояние чекбокса.
     */
    void checkBoxToggled(bool checked);

protected:
    /**
     * @brief Обработчик событий фильтрации объектов.
     * @param obj Объект, от которого пришло событие.
     * @param event Событие.
     * @return true, если событие обработано, иначе false.
     */
    bool eventFilter(QObject *obj, QEvent *event) override;
    
    /**
     * @brief Обработчик события изменения размера.
     * @param event Событие изменения размера.
     */
    void resizeEvent(QResizeEvent *event) override;
    
    /**
     * @brief Обработчик события показа виджета.
     * @param event Событие показа.
     */
    void showEvent(QShowEvent *event) override;
    
    /**
     * @brief Обработчик события скрытия виджета.
     * @param event Событие скрытия.
     */
    void hideEvent(QHideEvent *event) override;

private slots:
    /**
     * @brief Слот вызывается по завершении анимации.
     */
    void onAnimationFinished();
    
    /**
     * @brief Слот обработки нажатия кнопки закрытия.
     */
    void onCloseClicked();
    
    /**
     * @brief Слот обновления высоты меню.
     */
    void updateMenuHeight();

private:
    /**
     * @brief Возвращает текущее смещение меню.
     * @return Значение смещения.
     */
    int menuOffset() const;
    
    /**
     * @brief Устанавливает смещение меню.
     * @param offset Значение смещения.
     */
    void setMenuOffset(int offset);
    
    /**
     * @brief Настраивает элементы интерфейса.
     */
    void setupUi();
    
    /**
     * @brief Настраивает анимации.
     */
    void setupAnimations();
    
    /**
     * @brief Применяет стили к виджету.
     */
    void applyStyles();
    
    /**
     * @brief Устанавливает фильтр событий на родительский виджет.
     */
    void installParentEventFilter();
    
    /**
     * @brief Создает кнопку с заданными параметрами.
     * @param text Текст кнопки.
     * @param icon Иконка кнопки.
     * @param parent Родительский виджет.
     * @return Указатель на созданную кнопку.
     */
    QPushButton* createButton(const QString &text, const QPixmap &icon, QWidget *parent);

    // Направление скольжения
    SlideDirection m_direction;   ///< Направление появления меню.
    int m_menuWidth;              ///< Ширина меню в пикселях.
    
    // Элементы интерфейса
    QLabel *m_iconLabel;          ///< Метка для отображения иконки.
    QLabel *m_titleLabel;         ///< Метка для отображения заголовка.
    QPushButton *m_closeButton;   ///< Кнопка закрытия меню.
    QList<QPushButton*> m_buttons; ///< Список кнопок меню.
    QCheckBox *m_checkBox;        ///< Чекбокс в меню.
    QMenu *m_menu;                ///< Выпадающее меню.
    QWidget *m_separator;         ///< Виджет разделителя.
    
    // Layout для контента
    QVBoxLayout *m_contentLayout; ///< Основной layout для содержимого меню.
    
    // Анимация
    QPropertyAnimation *m_animation; ///< Анимация свойства menuOffset.
    bool m_isVisible;             ///< Флаг видимости меню.
};

#endif // SLIDINGMENU_H
