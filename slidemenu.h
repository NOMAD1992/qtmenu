#ifndef SLIDEMENU_H
#define SLIDEMENU_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QMenu>
#include <QAction>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QGraphicsOpacityEffect>

class SlideMenu : public QWidget
{
    Q_OBJECT

public:
    enum SlideDirection {
        SlideInFromRight,
        SlideInFromLeft,
        SlideInFromTop,
        SlideInFromBottom
    };

    explicit SlideMenu(QWidget *parent = nullptr);
    ~SlideMenu();

    void slideIn(SlideDirection direction = SlideInFromRight);
    void hideAnimated();
    void setAutoHide(bool autoHide);
    bool isMenuVisible() const;

    // Методы для добавления элементов
    void addLabel(const QString& text);
    void addButton(const QString& text);
    void addCheckBox(const QString& text);
    QMenu* addMenu(const QString& title);

signals:
    void hidden();
    void menuActionTriggered(QAction* action);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onMenuActionTriggered(QAction* action);

private:
    void setupUI();
    void setupAnimations();
    void adjustPosition();
    void createAnimations();

    SlideDirection m_direction;
    bool m_autoHide;
    bool m_isVisible;
    int m_animationDuration;
    
    QWidget* m_contentWidget;
    QVBoxLayout* m_mainLayout;
    QVBoxLayout* m_contentLayout;
    
    QPushButton* m_closeButton;
    
    QPropertyAnimation* m_posAnimation;
    QPropertyAnimation* m_opacityAnimation;
    QParallelAnimationGroup* m_animationGroup;
    
    QGraphicsOpacityEffect* m_opacityEffect;
};

#endif // SLIDEMENU_H
