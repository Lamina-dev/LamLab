#include "LaminaIDE.h"

LaminaIDE::LaminaIDE(QWidget *parent) : QMainWindow(parent) {
    interpreterPath = "lamina";
    setupUI();
    setupConnections();
    loadSettings();

    setWindowTitle(tr("Lamina IDE"));
    resize(1000, 700);

    // 设置初始状态
    updateWindowTitle();
    statusBar()->showMessage(tr("就绪"));
}

void LaminaIDE::newFile()
{
    if (maybeSave()) {
        currentFile.clear();
        clearCells();
        updateWindowTitle();
    }
}

void LaminaIDE::openFile()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this, tr("打开文件"), "", tr("Lamina 文件 (*.lm);;所有文件 (*)"));
        if (!fileName.isEmpty()) {
            loadFile(fileName);
        }
    }
}

bool LaminaIDE::save()
{
    if (currentFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(currentFile);
    }
}

bool LaminaIDE::saveAs() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("另存为"), "", tr("Lamina 文件 (*.lm);;所有文件 (*)"));
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}


void LaminaIDE::runCode(const QString &code) {
    // 清除之前的输出
    outputConsole->clear();

    // 检查解释器路径是否设置
    if (interpreterPath.isEmpty()) {
        QMessageBox::warning(this, tr("错误"), tr("请先设置Lamina解释器路径"));
        return;
    }

    // 创建临时文件
    QTemporaryFile *tempFile = new QTemporaryFile(QDir::tempPath() + "/Lamina_XXXXXX.lm", this);
    tempFile->setAutoRemove(false);
    if (!tempFile->open()) {
        QMessageBox::critical(this, tr("错误"), tr("无法创建临时文件"));
        return;
    }

    // 写入代码到临时文件
    QTextStream out(tempFile);
    out << code;
    tempFile->close();

    // 执行代码
    QProcess *process = new QProcess(this);
    connect(process, &QProcess::readyReadStandardOutput, this, [this, process]() {
        outputConsole->appendPlainText(process->readAllStandardOutput());
    });
    connect(process, &QProcess::readyReadStandardError, this, [this, process]() {
        outputConsole->appendHtml("<font color='red'>" + process->readAllStandardError() + "</font>");
    });
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this, tempFile, process](int exitCode, QProcess::ExitStatus exitStatus) {
        if (exitCode != 0) {
            statusBar()->showMessage(tr("执行失败，代码: %1").arg(exitCode));
        } else {
            statusBar()->showMessage(tr("执行完成"));
        }
                // 删除临时文件
        tempFile->remove();
        tempFile->deleteLater();
    });

    process->start(interpreterPath, QStringList() << tempFile->fileName());
    if (!process->waitForStarted()) {
        QMessageBox::critical(this, tr("错误"), tr("无法启动Lamina解释器"));
        return;
    }

    statusBar()->showMessage(tr("正在执行..."));
}

void LaminaIDE::showAbout() {
    QMessageBox::about(this, tr("关于 Lamina IDE"),
                       tr("<h2>Lamina IDE 1.0</h2>"
                          "<p>基于Qt 6.9开发的Lamina语言集成开发环境</p>"
                          "<p>提供类似Jupyter Notebook的交互式编程体验</p>"
                          "<p>&copy; 2025 Lamina开发团队</p>"));
}

void LaminaIDE::updateWindowTitle() {
    QString title = "Lamina IDE - ";
    if (currentFile.isEmpty()) {
        title += "未命名.lm";
    } else {
        title += QFileInfo(currentFile).fileName();
    }
    setWindowTitle(title);
}

void LaminaIDE::setInterpreterPath() {
    QString path = QFileDialog::getOpenFileName(this, tr("选择Lamina解释器"), "", tr("可执行文件 (*.exe);;所有文件 (*)"));
    if (!path.isEmpty()) {
        interpreterPath = path;
        QSettings settings("Lamina", "LaminaIDE");
        settings.setValue("interpreterPath", interpreterPath);
        statusBar()->showMessage(tr("解释器路径已设置为: %1").arg(interpreterPath), 3000);
    }
}

void LaminaIDE::addCodeCell() {
    CodeCell *cell = new CodeCell;
    connect(cell, &CodeCell::runCell, this, &LaminaIDE::runCode);
    cellsLayout->addWidget(cell);
    cells.push_back(cell);
}

void LaminaIDE::addTextCell() {
    TextCell *cell = new TextCell;
    cellsLayout->addWidget(cell);
    cells.push_back(cell);
}

void LaminaIDE::loadFile(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("错误"),
                             tr("无法打开文件 %1: %2")
                             .arg(QDir::toNativeSeparators(fileName))
                             .arg(file.errorString()));
        return;
    }

    // 清除现有单元格
    clearCells();

    // 读取文件内容
    QTextStream in(&file);
    QString content = in.readAll();

    // 解析文件内容
    parseContent(content);

    currentFile = fileName;
    updateWindowTitle();
    statusBar()->showMessage(tr("文件已加载"), 2000);
}

void LaminaIDE::clearCells() {
    for (QWidget *cell : cells) {
        cellsLayout->removeWidget(cell);
        delete cell;
    }
    cells.clear();
}

void LaminaIDE::setupUI() {
    // 创建中央部件
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // 创建分割器
    QSplitter *splitter = new QSplitter(Qt::Vertical, centralWidget);
    mainLayout->addWidget(splitter, 1);

    // 创建滚动区域
    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("QScrollArea { border: none; }");

    // 创建单元格容器
    QWidget *cellsContainer = new QWidget;
    cellsContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    cellsLayout = new QVBoxLayout(cellsContainer);
    cellsLayout->setAlignment(Qt::AlignTop);
    cellsLayout->setContentsMargins(10, 10, 10, 10);
    cellsLayout->setSpacing(10);
    cellsLayout->addStretch(1);

    scrollArea->setWidget(cellsContainer);
    splitter->addWidget(scrollArea);

    // 创建输出控制台
    outputConsole = new QPlainTextEdit(this);
    outputConsole->setObjectName("outputConsole");
    outputConsole->setReadOnly(true);
    outputConsole->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    outputConsole->setStyleSheet("QPlainTextEdit { background-color: #1e222a; color: #abb2bf; }");

    splitter->addWidget(outputConsole);
    splitter->setSizes({500, 200});

    // 添加初始单元格
    addTextCell();
    addCodeCell();

    // 创建菜单栏
    createMenus();

    // 创建工具栏
    createToolBars();

    // 状态栏
    statusBar()->setStyleSheet("QStatusBar { background-color: #282c34; color: #abb2bf; }");
}


void LaminaIDE::parseContent(const QString &content) {
    // 使用正则表达式分割文本和代码块
    QRegularExpression re("/\\*(.*?)\\*/", QRegularExpression::DotMatchesEverythingOption);
    QRegularExpressionMatchIterator i = re.globalMatch(content);

    int lastPos = 0;
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        int startPos = match.capturedStart();
        int endPos = match.capturedEnd();

        // 添加之前的代码块
        if (startPos > lastPos) {
            QString code = content.mid(lastPos, startPos - lastPos).trimmed();
            if (!code.isEmpty()) {
                CodeCell *cell = new CodeCell;
                cell->setCode(code);
                connect(cell, &CodeCell::runCell, this, &LaminaIDE::runCode);
                cellsLayout->addWidget(cell);
                cells.push_back(cell);
            }
        }

        // 添加文本块
        QString text = match.captured(1).trimmed();
        if (!text.isEmpty()) {
            TextCell *cell = new TextCell;
            cell->setText(text);
            cellsLayout->addWidget(cell);
            cells.push_back(cell);
        }

        lastPos = endPos;
    }

    // 添加最后的代码块
    if (lastPos < content.length()) {
        QString code = content.mid(lastPos).trimmed();
        if (!code.isEmpty()) {
            CodeCell *cell = new CodeCell;
            cell->setCode(code);
            connect(cell, &CodeCell::runCell, this, &LaminaIDE::runCode);
            cellsLayout->addWidget(cell);
            cells.push_back(cell);
        }
    }
}

void LaminaIDE::createMenus() {
    // 文件菜单
    QMenu *fileMenu = menuBar()->addMenu(tr("文件(&F)"));
    fileMenu->addAction(tr("新建(&N)"), this, &LaminaIDE::newFile, QKeySequence::New);
    fileMenu->addAction(tr("打开(&O)..."), this, &LaminaIDE::openFile, QKeySequence::Open);
    fileMenu->addAction(tr("保存(&S)"), this, &LaminaIDE::save, QKeySequence::Save);
    fileMenu->addAction(tr("另存为(&A)..."), this, &LaminaIDE::saveAs, QKeySequence::SaveAs);
    fileMenu->addAction(tr("设置解释器路径..."), this, &LaminaIDE::setInterpreterPath);
    fileMenu->addSeparator();
    fileMenu->addAction(tr("退出(&X)"), qApp, &QApplication::quit, QKeySequence::Quit);

    // 编辑菜单
    QMenu *editMenu = menuBar()->addMenu(tr("编辑(&E)"));
    editMenu->addAction(tr("添加代码单元"), this, [this]() { addCodeCell(); });
    editMenu->addAction(tr("添加文本单元"), this, [this]() { addTextCell(); });

    // 运行菜单
    QMenu *runMenu = menuBar()->addMenu(tr("运行(&R)"));
    runMenu->addAction(tr("运行所有单元"), this, [this]() {
        for (QWidget *cell : cells) {
            if (CodeCell *codeCell = qobject_cast<CodeCell*>(cell)) {
                runCode(codeCell->getCode());
            }
        }
    });

    // 帮助菜单
    QMenu *helpMenu = menuBar()->addMenu(tr("帮助(&H)"));
    helpMenu->addAction(tr("关于(&A)..."), this, &LaminaIDE::showAbout);
    helpMenu->addAction(tr("关于 Qt(&Q)..."), qApp, &QApplication::aboutQt);
}

void LaminaIDE::createToolBars() {
    QToolBar *fileToolBar = addToolBar(tr("文件"));
    fileToolBar->setObjectName("fileToolBar");
    fileToolBar->setMovable(false);

    // 创建图标（在实际应用中应使用真实图标）
    QIcon newIcon, openIcon, saveIcon, runIcon, addCodeIcon, addTextIcon;

    QAction *newAction = new QAction(newIcon, tr("新建"), this);
    newAction->setShortcut(QKeySequence::New);
    connect(newAction, &QAction::triggered, this, &LaminaIDE::newFile);
    fileToolBar->addAction(newAction);

    QAction *openAction = new QAction(openIcon, tr("打开"), this);
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, this, &LaminaIDE::openFile);
    fileToolBar->addAction(openAction);

    QAction *saveAction = new QAction(saveIcon, tr("保存"), this);
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, this, &LaminaIDE::save);
    fileToolBar->addAction(saveAction);

    fileToolBar->addSeparator();

    QAction *addCodeAction = new QAction(addCodeIcon, tr("添加代码单元"), this);
    connect(addCodeAction, &QAction::triggered, this, [this]() { addCodeCell(); });
    fileToolBar->addAction(addCodeAction);

    QAction *addTextAction = new QAction(addTextIcon, tr("添加文本单元"), this);
    connect(addTextAction, &QAction::triggered, this, [this]() { addTextCell(); });
    fileToolBar->addAction(addTextAction);
}

void LaminaIDE::setupConnections() {
    // 保存快捷键
    new QShortcut(QKeySequence::Save, this, SLOT(save()));
}

void LaminaIDE::loadSettings() {
    QSettings settings("Lamina", "LaminaIDE");

    // 窗口几何
    QRect defaultRect = QRect(100, 100, 1000, 700);
    setGeometry(settings.value("geometry", defaultRect).toRect());

    // 最近文件
    currentFile = settings.value("currentFile", "").toString();

    // 解释器路径
    interpreterPath = settings.value("interpreterPath", "lamina").toString();

    if (!currentFile.isEmpty() && QFile::exists(currentFile)) {
        loadFile(currentFile);
    }
}

void LaminaIDE::saveSettings() {
    QSettings settings("Lamina", "LaminaIDE");

    // 窗口几何
    settings.setValue("geometry", geometry());

    // 当前文件
    settings.setValue("currentFile", currentFile);

    // 解释器路径
    settings.setValue("interpreterPath", interpreterPath);
}

bool LaminaIDE::saveFile(const QString &fileName)
{
    QString actualFileName = fileName;
    if (!actualFileName.endsWith(".lm", Qt::CaseInsensitive)) {
        actualFileName += ".lm";
    }

    // 构建文件内容
    QString content;
    for (QWidget *cell : cells) {
        if (CodeCell *codeCell = qobject_cast<CodeCell*>(cell)) {
            content += codeCell->getCode() + "\n\n";
        }
        else if (TextCell *textCell = qobject_cast<TextCell*>(cell)) {
            QString text = textCell->getText();
            if (!text.isEmpty()) {
                content += "/*" + text + "*/\n\n";
            }
        }
    }

    // 保存到文件
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("错误"),
                             tr("无法保存文件 %1: %2")
                             .arg(QDir::toNativeSeparators(fileName))
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    out << content;

    currentFile = fileName;
    updateWindowTitle();
    statusBar()->showMessage(tr("文件已保存"), 2000);
    return true;
}


bool LaminaIDE::maybeSave() {
    if (currentFile.isEmpty()) {
        return true;
    }

    const QMessageBox::StandardButton ret = QMessageBox::warning(this, tr("Lamina IDE"),
        tr("文档已被修改，是否保存更改?"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    switch (ret) {
    case QMessageBox::Save:
        return save();
    case QMessageBox::Cancel:
        return false;
    default:
        break;
    }
    return true;
}
