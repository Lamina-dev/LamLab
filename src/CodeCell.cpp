#include "CodeCell.h"

CodeCell::CodeCell(QWidget *parent) : ::QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 5, 0, 5);

    // 创建代码编辑器 - 禁用滚动条
    editor = new QPlainTextEdit(this);
    editor->setObjectName("codeEditor");
    editor->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 禁用垂直滚动条
    editor->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 禁用水平滚动条
    new LaminaHighlighter(editor->document());

    // 创建运行按钮
    runButton = new QPushButton("▶", this);
    runButton->setFixedSize(30, 30);
    runButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; border-radius: 5px; }");

    // 设置等宽字体
    QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    font.setPointSize(12);
    editor->setFont(font);
    editor->setStyleSheet("QPlainTextEdit { background-color: #282c34; color: #abb2bf; border: none; }");

    // 连接文本变化信号以调整高度
    connect(editor->document(), &QTextDocument::contentsChanged, this, &CodeCell::adjustHeight);

    // 确保编辑器可以垂直扩展
    editor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    layout->addWidget(editor, 1);
    layout->addWidget(runButton);

    // 连接按钮信号
    connect(runButton, &QPushButton::clicked, this, &CodeCell::runRequested);
}

QString CodeCell::getCode() const
{
    return editor->toPlainText();
}

void CodeCell::setCode(const QString &code)
{
    editor->setPlainText(code);
    adjustHeight(); // 设置代码后调整高度
}

void CodeCell::runRequested()
{
    emit runCell(editor->toPlainText());
}

void CodeCell::adjustHeight()
{
    // 计算文档理想高度
    QFontMetrics fm(editor->font());
    int lineCount = editor->document()->lineCount();
    int lineHeight = fm.lineSpacing();
    int docHeight = lineCount * lineHeight + 10; // 添加额外边距

    // 设置编辑器高度
    editor->setFixedHeight(docHeight);

    // 更新整个单元格高度
    setMinimumHeight(docHeight + 10);
    setMaximumHeight(docHeight + 10);

    // 通知布局更新（确保父容器知道尺寸变化）
    updateGeometry();

    // 强制父容器重新布局
    if (QWidget *parent = parentWidget()) {
        if (QLayout *layout = parent->layout()) {
            layout->invalidate();
            layout->activate();
        }
        parent->updateGeometry();
    }
}

void CodeCell::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    adjustHeight(); // 首次显示时调整高度
}