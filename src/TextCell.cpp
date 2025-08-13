#include "TextCell.h"

TextCell::TextCell(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 5, 0, 5);

    // 创建文本编辑器
    textEdit = new QTextEdit(this);
    textEdit->setReadOnly(false);
    textEdit->setAcceptRichText(false);
    textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 禁用垂直滚动条
    textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 禁用水平滚动条
    textEdit->setStyleSheet("QTextEdit { background-color: #1E1E1E; border: none; color: #abb2bf; }");
    textEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // 连接文本变化信号以调整高度
    connect(textEdit->document(), &QTextDocument::contentsChanged, this, &TextCell::adjustHeight);

    layout->addWidget(textEdit, 1);
}

QString TextCell::getText() const
{
    return textEdit->toPlainText();
}

void TextCell::setText(const QString &text)
{
    textEdit->setPlainText(text);
    adjustHeight(); // 设置文本后调整高度
}

void TextCell::adjustHeight()
{
    // 根据内容调整编辑器高度
    int docHeight = textEdit->document()->size().height();
    textEdit->setMinimumHeight(docHeight + 10); // 添加一点边距
    updateGeometry(); // 更新布局
}