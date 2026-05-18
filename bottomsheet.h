#ifndef BOTTOMSHEET_H
#define BOTTOMSHEET_H

#include <QWidget>
#include <QListView>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QLabel>
#include <QStringListModel>

/**
 * @brief Класс нижней выдвижной шторки.
 * 
 * BottomSheet предоставляет функционал для создания нижней выдвижной панели,
 * которая может перетаскиваться пользователем. Поддерживает размещение QListView
 * на всю площадь шторки и настройку прозрачности фона.
 */
class BottomSheet : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Конструктор нижней шторки.
     * @param parent Родительский виджет (по умолчанию nullptr).
     * @param menuBarObjectName Имя объекта панели меню для поиска в родителе (по умолчанию "fMenuBar").
     */
    explicit BottomSheet(QWidget *parent = nullptr, const QString &menuBarObjectName = "fMenuBar");
    
    /**
     * @brief Деструктор нижней шторки.
     */
    ~BottomSheet();

    /**
     * @brief Устанавливает QListView на всю шторку без отступов.
     * @param view Указатель на QListView для добавления.
     */
    void addListView(QListView *view);

    /**
     * @brief Возвращает указатель на listView.
     * @return Указатель на QListView.
     */
    QListView* listView() const { return m_listView; }
    
    /**
     * @brief Добавляет сообщение в лог (QListView).
     * @param message Сообщение для добавления.
     */
    void appendLogMessage(const QString &message);
    
    /**
     * @brief Обновляет максимальную высоту шторки.
     * @param maxHeight Максимальная высота в пикселях.
     */
    void updateMaxHeight(int maxHeight);
    
    /**
     * @brief Настраивает прозрачность фона.
     * @param alpha Значение прозрачности от 0 до 255.
     */
    void setOpacity(int alpha);
    
    /**
     * @brief Устанавливает текст на ручке перетаскивания.
     * @param text Текст для отображения на ручке.
     */
    void setHandleText(const QString &text);

    /**
     * @brief Устанавливает отступ сверху для ограничения высоты шторки.
     * @param offset Отступ в пикселях.
     */
    void setTopOffset(int offset);
    
    /**
     * @brief Возвращает текущий отступ сверху.
     * @return Отступ в пикселях.
     */
    int topOffset() const { return m_topOffset; }
    
    /**
     * @brief Обновляет позицию и размеры шторки при изменении видимости fMenuBar.
     * @param menuBarVisible Видима ли панель меню.
     */
    void updateForMenuBarVisibility(bool menuBarVisible);

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
     * @brief Обработчик события нажатия кнопки мыши.
     * @param event Событие нажатия кнопки мыши.
     */
    void mousePressEvent(QMouseEvent *event) override;
    
    /**
     * @brief Обработчик события перемещения мыши.
     * @param event Событие перемещения мыши.
     */
    void mouseMoveEvent(QMouseEvent *event) override;
    
    /**
     * @brief Обработчик события отпускания кнопки мыши.
     * @param event Событие отпускания кнопки мыши.
     */
    void mouseReleaseEvent(QMouseEvent *event) override;
    
    /**
     * @brief Обработчик события нажатия клавиши.
     * @param event Событие нажатия клавиши.
     */
    void keyPressEvent(QKeyEvent *event) override;

private:
    /**
     * @brief Настраивает элементы интерфейса.
     */
    void setupUi();
    
    /**
     * @brief Применяет стили к виджету.
     */
    void applyStyles();
    
    /**
     * @brief Устанавливает фильтр событий на родительский виджет.
     */
    void installParentEventFilter();
    
    /**
     * @brief Обновляет ширину шторки.
     */
    void updateSheetWidth();
    
    /**
     * @brief Обновляет высоту шторки.
     * @param height Новая высота в пикселях.
     */
    void updateSheetHeight(int height);
    
    /**
     * @brief Пересчитывает максимальную высоту на основе родителя.
     */
    void recalculateMaxHeight();

    // Элементы интерфейса
    QWidget *m_handleWidget;      ///< Узкая полоска сверху для перетаскивания.
    QLabel *m_handleLabel;        ///< Надпись на ручке перетаскивания.
    QListView *m_listView;        ///< Список в шторке.
    QVBoxLayout *m_mainLayout;    ///< Основной layout.

    bool m_isVisible;             ///< Флаг видимости шторки.
    int m_sheetHeight;            ///< Текущая высота шторки.
    int m_minHeight;              ///< Минимальная высота шторки.
    int m_maxHeight;              ///< Максимальная высота шторки.
    int m_menubarHeight;          ///< Высота панели меню.
    int m_menuBarY;               ///< Позиция Y панели меню относительно родителя.
    int m_topOffset;              ///< Отступ сверху для ограничения высоты шторки.
    int m_opacity;                ///< Прозрачность фона (0-255).
    QString m_menuBarObjectName;  ///< Имя объекта панели меню для поиска в родителе.
    
    // Для перетаскивания
    bool m_dragging;              ///< Флаг активного перетаскивания.
    int m_dragStartY;             ///< Начальная позиция Y при перетаскивании.
    int m_startHeight;            ///< Начальная высота при перетаскивании.
};

#endif // BOTTOMSHEET_H
