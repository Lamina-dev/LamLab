#include "LaminaIDE.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // 设置应用程序信息
    app.setApplicationName("Lamina IDE");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("Lamina");
    app.setOrganizationDomain("lamina.dev");

    // 设置样式
    app.setStyle("Fusion");

    // 创建主窗口
    LaminaIDE ide;
    ide.show();

    // 处理命令行参数
    if (app.arguments().size() > 1) {
        QString fileName = app.arguments().at(1);
        if (QFile::exists(fileName)) {
            ide.loadFile(fileName);
        }
    }

    return app.exec();
}

#include "main.moc"