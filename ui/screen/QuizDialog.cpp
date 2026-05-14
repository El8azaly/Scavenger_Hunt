#include "ui/screen/QuizDialog.h"

#include "core/Constants.h"
#include "core/Game.h"
#include "ui/sprite/SpriteButton.h"

#include <QKeyEvent>
#include <QPainter>

constexpr auto ui = Constants::UI_SCALE;

QuizDialog::QuizDialog(Game* game, QWidget* parent)
    : QWidget(parent),
    m_game(game),
    m_panel("window_bg"),
    m_paper("paper_blank") {
    setFocusPolicy(Qt::StrongFocus);
    setFixedSize(Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT);

    startTimer(16);
}

void QuizDialog::startQuiz(const PuzzleData& puzzle) {
    clearButtons();

    m_objectId = puzzle.objectId;
    m_questions.clear();
    m_currentQuestion = 0;
    m_score = 0;
    m_passScore = -1;
    m_passed = false;
    m_showingResult = false;

    parsePuzzleData(puzzle);
    showCurrentQuestion();
    setFocus();
    update();
}

void QuizDialog::parsePuzzleData(const PuzzleData& puzzle) {
    for (const QString& rawLine : puzzle.data) {
        const QString line = rawLine.trimmed();
        if (line.isEmpty()) continue;

        if (line.startsWith("pass=", Qt::CaseInsensitive)) {
            bool ok = false;
            int score = line.mid(5).trimmed().toInt(&ok);
            if (ok && score > 0) {
                m_passScore = score;
            }
            continue;
        }

        QStringList parts = line.split('|');
        if (parts.size() < 4) continue;

        bool ok = false;
        int parsedCorrect = parts.takeLast().trimmed().toInt(&ok);
        if (!ok) continue;

        QuizQuestion question;
        question.question = parts.takeFirst().trimmed();

        for (QString option : parts) {
            option = option.trimmed();
            if (!option.isEmpty()) {
                question.options.append(option);
            }
        }

        if (question.question.isEmpty() || question.options.size() < 2) continue;

        if (parsedCorrect == 0) {
            question.correctIndex = -1;
        } else if (parsedCorrect >= 1 && parsedCorrect <= question.options.size()) {
            question.correctIndex = parsedCorrect - 1;
        } else {
            continue;
        }

        m_questions.append(question);
    }

    if (m_questions.isEmpty()) {
        QuizQuestion fallback;
        fallback.question = "which language is this project mainly written in?";
        fallback.options = QStringList{
            "c++ with qt",
            "python with pygame",
            "javascript",
            "java swing"
        };
        fallback.correctIndex = 0;
        m_questions.append(fallback);
    }

    if (m_passScore <= 0) {
        m_passScore = (m_questions.size() == 1)
        ? 1
        : ((m_questions.size() / 2) + 1);
    }

    if (m_passScore > m_questions.size()) {
        m_passScore = m_questions.size();
    }
}

void QuizDialog::showCurrentQuestion() {
    clearButtons();
    m_showingResult = false;

    if (m_currentQuestion >= m_questions.size()) {
        showResultScreen();
        return;
    }

    const QuizQuestion& question = m_questions[m_currentQuestion];

    for (int i = 0; i < question.options.size(); ++i) {
        const QString optionText = QString("%1 %2").arg(i + 1).arg(question.options[i]);

        auto* optionButton = SpriteButton::createYellow(optionText, this);
        optionButton->setText(optionText, 1, PixelFont::Dark);

        optionButton->setTextOffset(0, 1);

        optionButton->setCursor(Qt::PointingHandCursor);

        connect(optionButton, &SpriteButton::clicked, this, [this, i]() {
            answerCurrentQuestion(i);
        });

        m_answerButtons.append(optionButton);
    }

    layoutButtons();

    for (auto* button : m_answerButtons) {
        button->show();
    }

    update();
}

void QuizDialog::answerCurrentQuestion(int selectedIndex) {
    if (m_currentQuestion >= m_questions.size()) return;

    if (m_questions[m_currentQuestion].correctIndex == -1 ||
        selectedIndex == m_questions[m_currentQuestion].correctIndex) {
        ++m_score;
    }

    ++m_currentQuestion;
    showCurrentQuestion();
}

void QuizDialog::showResultScreen() {
    clearButtons();

    m_showingResult = true;
    m_passed = (m_score >= m_passScore);

    m_continueButton = m_passed
                           ? SpriteButton::createGreen("continue", this)
                           : SpriteButton::createYellow("continue", this);

    m_continueButton->setText("continue", 1, PixelFont::Dark);
    m_continueButton->setTextOffset(0, 0);
    m_continueButton->setCursor(Qt::PointingHandCursor);

    connect(m_continueButton, &SpriteButton::clicked, this, [this]() {
        emit quizFinished(m_passed, m_objectId);
    });

    layoutButtons();
    m_continueButton->show();
    update();
}

void QuizDialog::clearButtons() {
    for (auto* button : m_answerButtons) {
        if (button) {
            button->hide();
            button->deleteLater();
        }
    }

    m_answerButtons.clear();

    if (m_continueButton) {
        m_continueButton->hide();
        m_continueButton->deleteLater();
        m_continueButton = nullptr;
    }
}

QRect QuizDialog::panelRect() const {
    const int panelW = 268 * ui;
    const int panelH = 220 * ui;

    return QRect(
        (Constants::WINDOW_WIDTH - panelW) / 2,
        (Constants::WINDOW_HEIGHT - panelH) / 2,
        panelW,
        panelH
        );
}

QRect QuizDialog::questionRect() const {
    const QRect panel = panelRect();

    return QRect(
        panel.x() + 30 * ui,
        panel.y() + 50 * ui,
        panel.width() - 60 * ui,
        60 * ui
        );
}

void QuizDialog::layoutButtons() {
    const QRect panel = panelRect();
    const QRect questionBox = questionRect();

    if (m_showingResult && m_continueButton) {
        const int buttonW = 104 * ui;
        const int buttonH = 32 * ui;

        m_continueButton->setFixedSize(buttonW, buttonH);
        m_continueButton->move(
            panel.x() + (panel.width() - buttonW) / 2,
            panel.bottom() - buttonH - 22 * ui
            );

        return;
    }

    const int buttonW = panel.width() - 56 * ui;
    const int buttonH = 28 * ui;
    const int buttonX = panel.x() + 28 * ui;

    const int startY = questionBox.bottom()- 10*ui;

    const int gap = 26 * ui;

    for (int i = 0; i < m_answerButtons.size(); ++i) {
        SpriteButton* button = m_answerButtons[i];

        button->setFixedSize(buttonW, buttonH);
        button->move(buttonX, startY + i * gap);
    }
}

QStringList QuizDialog::wrapText(const QString& text, int maxChars) const {
    QStringList words = text.toLower().split(' ', Qt::SkipEmptyParts);
    QStringList lines;
    QString current;

    for (const QString& word : words) {
        QString candidate = current.isEmpty()
        ? word
        : current + " " + word;

        if (candidate.length() > maxChars && !current.isEmpty()) {
            lines.append(current);
            current = word;
        } else {
            current = candidate;
        }
    }

    if (!current.isEmpty()) {
        lines.append(current);
    }

    return lines;
}

void QuizDialog::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QPainter painter(this);

    if (m_game) {
        m_game->draw(painter);
    }

    painter.fillRect(rect(), QColor(0, 0, 0, 165));

    const QRect panel = panelRect();
    const QRect questionBox = questionRect();

    m_panel.draw(
        painter,
        panel.x(),
        panel.y(),
        panel.width(),
        panel.height()
    );

    m_paper.draw(
        painter,
        questionBox.x(),
        questionBox.y(),
        questionBox.width(),
        questionBox.height()
    );

    const QString title = "quiz keeper";
    const int titleScale = ui;
    const int titleW = title.length() * 11 * titleScale;

    m_font.drawText(
        painter,
        title,
        panel.x() + (panel.width() - titleW) / 2,
        panel.y() + 15 * ui,
        titleScale,
        PixelFont::Dark,
        true
    );

    const QString hint = m_showingResult
                             ? "press enter or click continue"
                             : "choose the correct answer";

    const int hintW = hint.length() * 6 * ui;

    m_font.drawText(
        painter,
        hint,
        panel.x() + (panel.width() - hintW) / 2,
        panel.y() + 53 * ui,
        ui,
        PixelFont::Dark
    );

    const QRect questionContent(
        questionBox.x() + 12 * ui,
        questionBox.y() + 12 * ui,
        questionBox.width() - 24 * ui,
        questionBox.height() - 24 * ui
    );

    if (m_showingResult) {
        const QString scoreLine = QString("score: %1/%2")
                                      .arg(m_score)
                                      .arg(m_questions.size());

        const int scoreW = scoreLine.length() * 6 * ui;

        m_font.drawText(
            painter,
            scoreLine,
            panel.x() + (panel.width() - scoreW) / 2,
            questionBox.y() - 9 * ui,
            ui,
            PixelFont::Dark
        );

        const QString resultText = m_passed
                                       ? "nice work. the emerald is yours."
                                       : "not quite. talk to me and try again.";

        const int resultMaxChars = qMax(
            14,
            (questionContent.width() / (6 * ui)) - 3
        );

        QStringList lines = wrapText(resultText, resultMaxChars);

        const int textH = 6 * ui;
        const int lineStep = 8 * ui;
        const int totalH = lines.size() * lineStep - (lineStep - textH);

        int y = questionContent.y() + qMax(0, (questionContent.height() - totalH) / 2);

        for (const QString& line : lines) {
            const int lineW = line.length() * 6 * ui;

            m_font.drawText(
                painter,
                line,
                questionContent.x() + (questionContent.width() - lineW) / 2,
                y,
                ui,
                PixelFont::Dark
            );

            y += lineStep;
        }

        return;
    }

    if (m_currentQuestion < m_questions.size()) {
        const QString progress = QString("question %1 of %2")
                                     .arg(m_currentQuestion + 1)
                                     .arg(m_questions.size());

        const int progressW = progress.length() * 6 * ui;

        m_font.drawText(
            painter,
            progress,
            panel.x() + (panel.width() - progressW) / 2,
            questionBox.y() - 9 * ui,
            ui,
            PixelFont::Dark
        );

        const int questionMaxChars = qMax(
            14,
            (questionContent.width() / (6 * ui)) - 3
        );

        QStringList lines = wrapText(
            m_questions[m_currentQuestion].question,
            questionMaxChars
        );

        const int textH = 6 * ui;
        const int lineStep = 8 * ui;
        const int totalH = lines.size() * lineStep - (lineStep - textH);

        int y = questionContent.y() + qMax(0, (questionContent.height() - totalH) / 2);

        for (const QString& line : lines) {
            const int lineW = line.length() * 6 * ui;

            m_font.drawText(
                painter,
                line,
                questionContent.x() + (questionContent.width() - lineW) / 2,
                y,
                ui,
                PixelFont::Dark
            );

            y += lineStep;
        }
    }
}

void QuizDialog::keyPressEvent(QKeyEvent* event) {
    bool handled = false;
    if (!m_showingResult) {
        int selected = -1;
        if (event->key() >= Qt::Key_1 && event->key() <= Qt::Key_9) {
            selected = event->key() - Qt::Key_1;
        }
        if (selected >= 0 && selected < m_answerButtons.size()) {
            answerCurrentQuestion(selected);
            handled = true;
        }
    } else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter || event->key() == Qt::Key_Space) {
        handled = true;
    }
    if (!handled && m_game) {
        m_game->handleKeyPress(event->key());
    }
    QWidget::keyPressEvent(event);
}

void QuizDialog::keyReleaseEvent(QKeyEvent* event) {
    if (m_game) {
        m_game->handleKeyRelease(event->key());
    }
    QWidget::keyReleaseEvent(event);
}

void QuizDialog::timerEvent(QTimerEvent* event) {
    Q_UNUSED(event);
    update();
}