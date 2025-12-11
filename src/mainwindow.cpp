#include "mainwindow.h"
#include "settings.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QSettings>
#include <QLabel>
#include <QGroupBox>
#include <iostream>

void MainWindow::initialize() {
    realtime = new Realtime();
    aspectRatioWidget = new AspectRatioWidget(this);
    aspectRatioWidget->setAspectWidget(realtime, 3.f/4.f);
    QHBoxLayout *hLayout = new QHBoxLayout; // horizontal alignment
    QVBoxLayout *vLayout = new QVBoxLayout(); // vertical alignment
    vLayout->setAlignment(Qt::AlignTop);
    hLayout->addLayout(vLayout);
    hLayout->addWidget(aspectRatioWidget, 1);
    this->setLayout(hLayout);

    // Create labels in sidebox
    QFont font;
    font.setPointSize(12);
    font.setBold(true);

    vLayout->addWidget(uploadFile);
    vLayout->addWidget(uploadBump);

    connectUIElements();
}

void MainWindow::finish() {
    realtime->finish();
    delete(realtime);
}

void MainWindow::connectUIElements() {  
    connectUploadFile();
    connectUploadBump();
}

void MainWindow::connectUploadFile() {
    connect(uploadFile, &QPushButton::clicked, this, &MainWindow::onUploadFile);
}

void MainWindow::connectUploadBump() {
    connect(uploadBump, &QPushButton::clicked, this, &MainWindow::onUploadBump);
}


void MainWindow::onUploadFile() {
    // Get abs path of scene file
    QString configFilePath = QFileDialog::getOpenFileName(this, tr("Upload File"),
                                                          QDir::currentPath()
                                                              .append(QDir::separator())
                                                              .append("scenefiles")
                                                              .append(QDir::separator())
                                                              .append("realtime")
                                                              .append(QDir::separator())
                                                              .append("required"), tr("Scene Files (*.json)"));
    if (configFilePath.isNull()) {
        std::cout << "Failed to load null scenefile." << std::endl;
        return;
    }

    settings.sceneFilePath = configFilePath.toStdString();

    std::cout << "Loaded scenefile: \"" << configFilePath.toStdString() << "\"." << std::endl;

    realtime->sceneChanged();
}

void MainWindow::onUploadBump() {
    // Get abs path of scene file
    QString configFilePath = QFileDialog::getOpenFileName(this, tr("Upload File"),
                                                          QDir::currentPath()
                                                              .append(QDir::separator())
                                                              .append("scenefiles")
                                                              .append(QDir::separator())
                                                              .append("realtime")
                                                              .append(QDir::separator())
                                                              .append("required"), tr("Images (*.png *.jpg *.jpeg)"));


    QImage texture = QImage(configFilePath);
    settings.bumpTexture = texture.convertToFormat(QImage::Format_Grayscale8).mirrored();
    settings.hasTexture = true;

    std::cout << "Loaded bumpfile: \"" << configFilePath.toStdString() << "\"." << std::endl;

    realtime->bindObjectTexture();

    realtime->sceneChanged();
}

void MainWindow::onSaveImage() {
    if (settings.sceneFilePath.empty()) {
        std::cout << "No scene file loaded." << std::endl;
        return;
    }
    std::string sceneName = settings.sceneFilePath.substr(0, settings.sceneFilePath.find_last_of("."));
    sceneName = sceneName.substr(sceneName.find_last_of("/")+1);
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save Image"),
                                                    QDir::currentPath()
                                                        .append(QDir::separator())
                                                        .append("student_outputs")
                                                        .append(QDir::separator())
                                                        .append("realtime")
                                                        .append(QDir::separator())
                                                        .append("required")
                                                        .append(QDir::separator())
                                                        .append(sceneName), tr("Image Files (*.png)"));
    std::cout << "Saving image to: \"" << filePath.toStdString() << "\"." << std::endl;
    realtime->saveViewportImage(filePath.toStdString());
}

