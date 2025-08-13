#ifndef LAMLAB_CODECELL_H
#define LAMLAB_CODECELL_H

#include <QHBoxLayout>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QWidget>

#include "LaminaHighlighter.h"

class CodeCell : public QWidget {
    Q_OBJECT
public:
    CodeCell(QWidget *parent = nullptr);

    QString getCode() const;

    void setCode(const QString &code);

    signals:
        void runCell(const QString &code);

private slots:
    void runRequested();

    void adjustHeight();
    void showEvent(QShowEvent* event);

private:
    QPushButton *runButton;
    QPlainTextEdit *editor;
};


#endif //LAMLAB_CODECELL_H