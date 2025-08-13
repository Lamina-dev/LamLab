#ifndef LAMLAB_LAMINAIDE_H
#define LAMLAB_LAMINAIDE_H
#include <qboxlayout.h>
#include <QFileDialog>
#include <QMainWindow>
#include <qstring.h>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QProcess>
#include <QScrollArea>
#include <QSettings>
#include <QSplitter>
#include <QStatusBar>
#include <QTemporaryFile>
#include <QToolBar>
#include <QApplication>
#include <QShortcut>

#include "CodeCell.h"
#include "TextCell.h"

class LaminaIDE : public QMainWindow {
    Q_OBJECT
public:
    LaminaIDE(QWidget *parent = nullptr);

    ~LaminaIDE() { saveSettings(); }

public slots:
    void newFile();

    void openFile();

    bool save();

    bool saveAs();

    void runCode(const QString &code);

    void showAbout();

    void updateWindowTitle();

    void setInterpreterPath();

    void addCodeCell();

    void addTextCell();

    void loadFile(const QString &fileName);

    void clearCells();
private:
    void setupUI();

    void parseContent(const QString &content);

    void createMenus();

    void createToolBars();

    void setupConnections();

    void loadSettings();

    void saveSettings();

    bool saveFile(const QString &fileName);

    bool maybeSave();

    // 成员变量
    QVBoxLayout *cellsLayout;
    std::vector<QWidget*> cells;
    QPlainTextEdit *outputConsole;
    QString currentFile;
    QString interpreterPath;
};


#endif //LAMLAB_LAMINAIDE_H