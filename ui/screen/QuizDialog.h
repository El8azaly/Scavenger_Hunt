#pragma once

#include <QWidget>
#include <QVector>
#include <QString>
#include <QStringList>
#include <QRect>

#include "entities/InteractionResult.h"
#include "ui/sprite/PixelFont.h"
#include "ui/sprite/SlicedSprite.h"

class Game;
class SpriteButton;

class QuizDialog : public QWidget {
    Q_OBJECT
public:
    explicit QuizDialog(Game* game, QWidget* parent = nullptr);

    void startQuiz(const PuzzleData& puzzle);
    bool passed() const { return m_passed; }

signals:
    void quizFinished(bool passed, const QString& objectId);

protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    struct QuizQuestion {
        QString question;
        QStringList options;
        int correctIndex = 0;
    };

    Game* m_game = nullptr;
    PixelFont m_font;
    SlicedSprite m_panel;
    SlicedSprite m_paper;

    QRect panelRect() const;
    QRect questionRect() const;

    QVector<QuizQuestion> m_questions;
    QVector<SpriteButton*> m_answerButtons;
    SpriteButton* m_continueButton = nullptr;

    QString m_objectId;
    int m_currentQuestion = 0;
    int m_score = 0;
    int m_passScore = -1;
    bool m_passed = false;
    bool m_showingResult = false;

    void parsePuzzleData(const PuzzleData& puzzle);
    void showCurrentQuestion();
    void answerCurrentQuestion(int selectedIndex);
    void showResultScreen();
    void clearButtons();
    void layoutButtons();
    QStringList wrapText(const QString& text, int maxChars) const;
};
