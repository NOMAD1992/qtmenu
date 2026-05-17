#ifndef STANDARDICONSWIDGET_H
#define STANDARDICONSWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QGridLayout>

/**
 * @brief Виджет для отображения всех стандартных иконок Qt5.
 * 
 * StandardIconsWidget создает сетку кнопок, каждая из которых отображает
 * одну из стандартных иконок темы Qt5 с подписью названия иконки.
 */
class StandardIconsWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Конструктор виджета стандартных иконок.
     * @param parent Родительский виджет (по умолчанию nullptr).
     */
    explicit StandardIconsWidget(QWidget *parent = nullptr);
    
    /**
     * @brief Деструктор виджета.
     */
    ~StandardIconsWidget();

private:
    /**
     * @brief Инициализирует интерфейс виджета.
     */
    void setupUi();
    
    /**
     * @brief Создает кнопки со стандартными иконками.
     */
    void createIconButtons();
    
    /**
     * @brief Добавляет кнопку с иконкой в сетку.
     * @param iconName Название стандартной иконки.
     * @param row Строка в сетке.
     * @param column Столбец в сетке.
     */
    void addIconButton(const QString &iconName, int row, int column);

private:
    QVBoxLayout *m_mainLayout;      ///< Основной вертикальный布局.
    QScrollArea *m_scrollArea;      ///< Область прокрутки для иконок.
    QWidget *m_scrollWidget;        ///< Виджет содержимого прокрутки.
    QGridLayout *m_gridLayout;      ///< Сетка для размещения кнопок.
};

#endif // STANDARDICONSWIDGET_H
