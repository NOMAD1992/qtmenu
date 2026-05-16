#ifndef BOTTOMSHEET_H
#define BOTTOMSHEET_H

#include <QWidget>
#include <QListView>
#include <QPropertyAnimation>
#include <QVBoxLayout>

class BottomSheet : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int sheetOffset READ sheetOffset WRITE setSheetOffset)

public:
    explicit BottomSheet(QWidget *parent = nullptr);
    ~BottomSheet();

    // Метод для установки QListView на всю шторку без отступов
    void addListView(QListView *view);

    // Показать/скрыть шторку
    void showSheet();
    void hideSheet();
    bool isSheetVisible() const;

    // Геттер для listView
    QListView* listView() const { return m_listView; }

signals:
    void sheetShown();
    void sheetHidden();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

private slots:
    void onAnimationFinished();
    void updateSheetWidth();

private:
    int sheetOffset() const;
    void setSheetOffset(int offset);

    void setupUi();
    void setupAnimations();
    void applyStyles();
    void installParentEventFilter();

    // Элементы интерфейса
    QListView *m_listView;
    QVBoxLayout *m_mainLayout;

    // Анимация
    QPropertyAnimation *m_animation;
    bool m_isVisible;
    int m_sheetHeight;
};

#endif // BOTTOMSHEET_H
