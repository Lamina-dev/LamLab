#ifndef LAMLAB_TEXTCELL_H
#define LAMLAB_TEXTCELL_H

#include <QTextEdit>
#include <qtmetamacros.h>
#include <QWidget>
#include <QHBoxLayout>

class TextCell : public QWidget {
    Q_OBJECT
public:
    TextCell(QWidget *parent = nullptr);

    QString getText() const;

    void setText(const QString &text);

private slots:
    void adjustHeight();

private:
    QTextEdit *textEdit;
};

#endif //LAMLAB_TEXTCELL_H