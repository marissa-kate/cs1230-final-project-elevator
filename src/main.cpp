#include "mainwindow.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QScreen>
#include <iostream>
#include "utils/sceneparser.h"
#include <QSettings>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QCoreApplication::setApplicationName("Project 5: Realtime");
    QCoreApplication::setOrganizationName("CS 1230");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);

    QSurfaceFormat fmt;
    fmt.setVersion(4, 1);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(fmt);
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addPositionalArgument("config", "Path of the config file.");
    parser.process(a);

    // auto positionalArgs = parser.positionalArguments();
    // if (positionalArgs.size() != 1) {
    //     std::cerr << "Not enough arguments. Please provide a path to a config file (.ini) as a command-line argument." << std::endl;
    //     a.exit(1);
    //     return 1;
    // }


    MainWindow w;
    w.initialize();
    w.resize(800, 600);
    w.show();

    int return_val = a.exec();
    w.finish();
    return return_val;
}
