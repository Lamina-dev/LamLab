#ifndef LAMLAB_LAMINAHIGHLIGHTER_H
#define LAMLAB_LAMINAHIGHLIGHTER_H

#include <qregularexpression.h>
#include <QSyntaxHighlighter>

class LaminaHighlighter : public QSyntaxHighlighter {
    Q_OBJECT
public:
    LaminaHighlighter(QTextDocument *parent = nullptr)
        : QSyntaxHighlighter(parent) {
        setupRules();
    }

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    QVector<HighlightingRule> highlightingRules;

    QTextCharFormat keywordFormat;
    QTextCharFormat functionFormat;
    QTextCharFormat numberFormat;
    QTextCharFormat stringFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat operatorFormat;
    QTextCharFormat typeFormat;

    void setupRules();
};


#endif //LAMLAB_LAMINAHIGHLIGHTER_H