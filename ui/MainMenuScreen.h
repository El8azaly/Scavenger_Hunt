#pragma once
#include <QWidget>
#include <QRect>

#include "SlicedSprite.h"

class MainMenuScreen : public QWidget {
    Q_OBJECT
public:
    explicit MainMenuScreen(QWidget* parent = nullptr);
signals:
    void startRequested();
protected:
    void paintEvent(QPaintEvent* event) override;
private:
    SlicedSprite m_background;
    QRect m_startButtonRect;
    bool  m_isHovering = false;
};
