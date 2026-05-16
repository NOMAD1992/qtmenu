#ifndef USERMENU_H
#define USERMENU_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QPropertyAnimation>
#include <QLabel>
#include <QFrame>

/**
 * @brief Класс пользовательского меню с анимацией.
 * 
 * UserMenu предоставляет функционал для создания анимированного выдвижного
 * пользовательского меню, которое появляется справа. Поддерживает добавление
 * кнопок и разделителей.
 */
class UserMenu : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int menuOffset READ menuOffset WRITE setMenuOffset)

public:
    /**
     * @brief Конструктор пользовательского меню.
     * @param parent Родительский виджет (по умолчанию nullptr).
     * @param menuWidth Ширина меню в пикселях (по умолчанию 200).
     */
    explicit UserMenu(QWidget *parent = nullptr, int menuWidth = 200);
    
    /**
     * @brief Деструктор пользовательского меню.
     */
    ~UserMenu();

    /**
     * @brief Добавляет кнопку в меню.
     * @param text Текст кнопки.
     * @param icon Иконка кнопки (по умолчанию пустая).
     * @return Указатель на созданную кнопку.
     */
    QPushButton* addButton(const QString &text, const QPixmap &icon = QPixmap());
    
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

signals:
    /**
     * @brief Сигнал о завершении показа меню.
     */
    void menuShown();
    
    /**
     * @brief Сигнал о завершении скрытия меню.
     */
    void menuHidden();

protected:
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
     * @brief Слот обновления геометрии меню.
     */
    void updateMenuGeometry();

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
     * @brief Создает кнопку с заданными параметрами.
     * @param text Текст кнопки.
     * @param icon Иконка кнопки.
     * @param parent Родительский виджет.
     * @return Указатель на созданную кнопку.
     */
    QPushButton* createButton(const QString &text, const QPixmap &icon, QWidget *parent);
    
    /**
     * @brief Вычисляет оптимальную ширину меню на основе содержимого.
     * @return Оптимальная ширина в пикселях.
     */
    int calculateOptimalWidth() const;

    int m_menuWidth;              ///< Ширина меню в пикселях.
    
    // Layout для контента
    QVBoxLayout *m_contentLayout; ///< Основной layout для содержимого меню.
    
    // Анимация
    QPropertyAnimation *m_animation; ///< Анимация свойства menuOffset.
    bool m_isVisible;             ///< Флаг видимости меню.
};

#endif // USERMENU_H
