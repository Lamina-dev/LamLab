#include "LaminaHighlighter.h"

void LaminaHighlighter::highlightBlock(const QString &text)
{
    // 应用高亮规则
    for (const HighlightingRule &rule : std::as_const(highlightingRules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}

void LaminaHighlighter::setupRules()
{
    // 关键字格式
    keywordFormat.setForeground(QColor(198, 120, 221)); // 紫色
    keywordFormat.setFontWeight(QFont::Bold);

    // 函数格式
    functionFormat.setForeground(QColor(97, 175, 239)); // 蓝色
    functionFormat.setFontItalic(true);

    // 数字格式
    numberFormat.setForeground(QColor(209, 154, 102)); // 橙色

    // 字符串格式
    stringFormat.setForeground(QColor(152, 195, 121)); // 绿色

    // 单行注释格式
    singleLineCommentFormat.setForeground(QColor(127, 132, 142)); // 灰色

    // 多行注释格式
    multiLineCommentFormat.setForeground(QColor(127, 132, 142)); // 灰色
    multiLineCommentFormat.setBackground(QColor(240, 240, 240)); // 浅灰背景

    // 运算符格式
    operatorFormat.setForeground(QColor(86, 182, 194)); // 青色
    operatorFormat.setFontWeight(QFont::Bold);

    // 类型格式
    typeFormat.setForeground(QColor(229, 192, 123)); // 黄色

    // 关键字列表
    QStringList keywordPatterns;
    keywordPatterns << "\\bvar\\b" << "\\bfunc\\b" << "\\bif\\b"
                     << "\\belse\\b" << "\\bwhile\\b" << "\\bfor\\b"
                     << "\\breturn\\b" << "\\bbreak\\b" << "\\bcontinue\\b"
                     << "\\bprint\\b" << "\\btrue\\b" << "\\bfalse\\b"
                     << "\\bnull\\b" << "\\binclude\\b" << "\\bdefine\\b"
                     << "\\bbigint\\b" << "\\binput\\b";

    for (const QString &pattern : keywordPatterns)
    {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    // 类型
    QStringList typePatterns;
    typePatterns << "\\bint\\b" << "\\bfloat\\b" << "\\brational\\b"
                 << "\\birrational\\b" << "\\bbool\\b" << "\\bstring\\b"
                 << "\\bbigint\\b";

    for (const QString &pattern : typePatterns)
    {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = typeFormat;
        highlightingRules.append(rule);
    }

    // 函数
    HighlightingRule funcRule;
    funcRule.pattern = QRegularExpression("\\b[A-Za-z0-9_]+(?=\\()");
    funcRule.format = functionFormat;
    highlightingRules.append(funcRule);

    // 数字
    HighlightingRule numberRule;
    numberRule.pattern = QRegularExpression("\\b\\d+\\.?\\d*\\b");
    numberRule.format = numberFormat;
    highlightingRules.append(numberRule);

    // 字符串
    HighlightingRule stringRule;
    stringRule.pattern = QRegularExpression("\".*?\"");
    stringRule.format = stringFormat;
    highlightingRules.append(stringRule);

    // 单行注释
    HighlightingRule singleLineCommentRule;
    singleLineCommentRule.pattern = QRegularExpression("//[^\n]*");
    singleLineCommentRule.format = singleLineCommentFormat;
    highlightingRules.append(singleLineCommentRule);

    // 运算符
    QStringList operatorPatterns;
    operatorPatterns << "\\+" << "\\-" << "\\*" << "\\/" << "\\%"
                     << "\\^" << "\\!" << "\\=" << "\\==" << "\\!="
                     << "\\>" << "\\<" << "\\>=" << "\\<=" << "\\&\\&"
                     << "\\|\\|";

    for (const QString &pattern : operatorPatterns)
    {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = operatorFormat;
        highlightingRules.append(rule);
    }
}