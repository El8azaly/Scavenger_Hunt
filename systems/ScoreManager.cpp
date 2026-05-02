#include "systems/ScoreManager.h"
ScoreManager::ScoreManager(QObject* p):QObject(p){
    m_timer.setInterval(1000);
    connect(&m_timer,&QTimer::timeout,this,&ScoreManager::onTick);
}
void ScoreManager::startTimer(int s){ m_timeLeft=s; m_timer.start(); }
void ScoreManager::stopTimer(){ m_timer.stop(); }
void ScoreManager::addScore(int pts){ m_score+=pts; emit scoreChanged(m_score); }
void ScoreManager::reset(){ m_score=0; m_timeLeft=0; m_timer.stop(); emit scoreChanged(0); emit timeChanged(0); }
void ScoreManager::onTick(){
    if(m_timeLeft<=0){ m_timer.stop(); emit timeUp(); return; }
    emit timeChanged(--m_timeLeft);
}