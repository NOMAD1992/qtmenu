#ifndef BOTTOMSHEET_H
#define BOTTOMSHEET_H

#include <QWidget>
#include <QListView>
#include <QVBoxLayout>
#include <QMouseEvent>

class BottomSheet : public QWidget
{
    Q_OBJECT

public:
    explicit BottomSheet(QWidget *parent = nullptr);
    ~BottomSheet();

    // Метод для установки QListView на всю шторку без отступов
    void addListView(QListView *view);

    // Геттер для listView
    QListView* listView() const { return m_listView; }
    
    // Метод для обновления максимальной высоты (вызывается из MainWindow)
    void updateMaxHeight(int maxHeight);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void setupUi();
    void applyStyles();
    void installParentEventFilter();
    void updateSheetWidth();
    void updateSheetHeight(int height);
    void recalculateMaxHeight();

    // Элементы интерфейса
    QWidget *m_handleWidget;      // Узкая полоска сверху для перетаскивания
    QListView *m_listView;
    QVBoxLayout *m_mainLayout;

    bool m_isVisible;
    int m_sheetHeight;
    int m_minHeight;
    int m_maxHeight;
    int m_menubarHeight;  // Высота верхней панели меню
    
    // Для перетаскивания
    bool m_dragging;
    int m_dragStartY;
    int m_startHeight;
};

#endif // BOTTOMSHEET_H
