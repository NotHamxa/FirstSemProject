#include <QApplication>
#include <QAction>

#include "mainwindow.h"
void onExit() {
    qApp->quit();
}
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow window;
    window.setWindowTitle("Audio Mixer");
    window.setFixedSize(800,600);
    window.show();

    QAction *button = window.findChild<QAction*>("exitApplication");
    if (button) {
        QObject::connect(button, &QAction::triggered, &onExit);
    }

    return app.exec();
}
