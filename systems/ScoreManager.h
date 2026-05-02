#pragma once
#include <QObject>
#include <QTimer>

class ScoreManager : public QObject {
    Q_OBJECT
public:
    explicit ScoreManager(QObject* parent = nullptr);
    void startTimer(int seconds);
    void stopTimer();
    void addScore(int points);
    void reset();
    int  currentScore() const { return m_score;    }
    int  timeLeft()     const { return m_timeLeft;  }
signals:
    void scoreChanged(int newScore);
    void timeChanged(int secondsLeft);
    void timeUp();
private slots:
    void onTick();
private:
    int    m_score=0, m_timeLeft=0;
    QTimer m_timer;
};