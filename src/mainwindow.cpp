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
    aspectRatioWidget->setAspectWidget(realtime, 9.f/16.f);
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

    // Create file uploader for scene file
    uploadFile = new QPushButton();
    uploadFile->adjustSize();
    uploadFile->setText(QStringLiteral("S"));

    uploadBump = new QPushButton();
    uploadBump->setText(QStringLiteral("B"));
    uploadBump->adjustSize();


    // bumpBox = new QSpinBox();
    // bumpBox->setMinimum(-50);
    // bumpBox->setMaximum(50);
    // bumpBox->setSingleStep(1);
    // bumpBox->setValue(10);

    // b->addWidget(bumpSlider);
    // b->addWidget(bumpBox);
    // bumpLayout->setLayout(b);

    invert = new QCheckBox("Invert");  //post processing
    invert->setChecked(false);      // default

    colorgrade = new QCheckBox("Color Grade");
    colorgrade->setChecked(false);

    // Creates the boxes containing the parameter sliders and number boxes
    // QGroupBox *p1Layout = new QGroupBox(); // horizonal slider 1 alignment
    // QHBoxLayout *l1 = new QHBoxLayout();
    // QGroupBox *p2Layout = new QGroupBox(); // horizonal slider 2 alignment
    // QHBoxLayout *l2 = new QHBoxLayout();

    // // Create slider controls to control parameters
    // p1Slider = new QSlider(Qt::Orientation::Horizontal); // Parameter 1 slider
    // p1Slider->setTickInterval(1);
    // p1Slider->setMinimum(1);
    // p1Slider->setMaximum(25);
    // p1Slider->setValue(1);

    // p1Box = new QSpinBox();
    // p1Box->setMinimum(1);
    // p1Box->setMaximum(25);
    // p1Box->setSingleStep(1);
    // p1Box->setValue(1);

    // p2Slider = new QSlider(Qt::Orientation::Horizontal); // Parameter 2 slider
    // p2Slider->setTickInterval(1);
    // p2Slider->setMinimum(1);
    // p2Slider->setMaximum(25);
    // p2Slider->setValue(1);

    // p2Box = new QSpinBox();
    // p2Box->setMinimum(1);
    // p2Box->setMaximum(25);
    // p2Box->setSingleStep(1);
    // p2Box->setValue(1);

    // // Adds the slider and number box to the parameter layouts
    // l1->addWidget(p1Slider);
    // l1->addWidget(p1Box);
    // p1Layout->setLayout(l1);

    // l2->addWidget(p2Slider);
    // l2->addWidget(p2Box);
    // p2Layout->setLayout(l2);

    // // Creates the boxes containing the camera sliders and number boxes
    // QGroupBox *nearLayout = new QGroupBox(); // horizonal near slider alignment
    // QHBoxLayout *lnear = new QHBoxLayout();
    // QGroupBox *farLayout = new QGroupBox(); // horizonal far slider alignment
    // QHBoxLayout *lfar = new QHBoxLayout();

    // // Create slider controls to control near/far planes
    // nearSlider = new QSlider(Qt::Orientation::Horizontal); // Near plane slider
    // nearSlider->setTickInterval(1);
    // nearSlider->setMinimum(1);
    // nearSlider->setMaximum(1000);
    // nearSlider->setValue(10);

    // nearBox = new QDoubleSpinBox();
    // nearBox->setMinimum(0.01f);
    // nearBox->setMaximum(10.f);
    // nearBox->setSingleStep(0.1f);
    // nearBox->setValue(0.1f);

    // farSlider = new QSlider(Qt::Orientation::Horizontal); // Far plane slider
    // farSlider->setTickInterval(1);
    // farSlider->setMinimum(1000);
    // farSlider->setMaximum(10000);
    // farSlider->setValue(10000);

    // farBox = new QDoubleSpinBox();
    // farBox->setMinimum(10.f);
    // farBox->setMaximum(100.f);
    // farBox->setSingleStep(0.1f);
    // farBox->setValue(100.f);

    // Adds the slider and number box to the parameter layouts
    // lnear->addWidget(nearSlider);
    // lnear->addWidget(nearBox);
    // nearLayout->setLayout(lnear);

    // lfar->addWidget(farSlider);
    // lfar->addWidget(farBox);
    // farLayout->setLayout(lfar);


    // // Creates the boxes containing the camera sliders and number boxes
    // QGroupBox *aperLayout = new QGroupBox(); // horizonal near slider alignment
    // QHBoxLayout *aper = new QHBoxLayout();
    // QGroupBox *focal_PlaneLayout = new QGroupBox(); // horizonal near slider alignment
    // QHBoxLayout *foc_plane = new QHBoxLayout();
    // QGroupBox *focal_lengthLayout = new QGroupBox(); // horizonal near slider alignment
    // QHBoxLayout *foc_len = new QHBoxLayout();
    // // QGroupBox *nearLayout = new QGroupBox(); // horizonal near slider alignment
    // // QHBoxLayout *lnear = new QHBoxLayout();
    // // QGroupBox *farLayout = new QGroupBox(); // horizonal far slider alignment
    // // QHBoxLayout *lfar = new QHBoxLayout();

    // // Create slider controls to control near/far planes
    // focal_length_slider = new QSlider(Qt::Orientation::Horizontal); // focal length slider
    // focal_length_slider->setTickInterval(1.0f);
    // focal_length_slider->setMinimum(1);
    // focal_length_slider->setMaximum(1000);
    // focal_length_slider->setValue(10);

    // focal_length_box = new QDoubleSpinBox();
    // focal_length_box->setMinimum(1.0f);
    // focal_length_box->setMaximum(100.0f);
    // focal_length_box->setSingleStep(0.1f);
    // focal_length_box->setValue(1.0f);

    // focal_plane_slider = new QSlider(Qt::Orientation::Horizontal); // focal Plane slider
    // focal_plane_slider->setTickInterval(0.1f);
    // focal_plane_slider->setMinimum(1.f);
    // focal_plane_slider->setMaximum(2000);
    // focal_plane_slider->setValue(10.f);

    // focal_plane_box = new QDoubleSpinBox();
    // focal_plane_box->setMinimum(0.1f);
    // focal_plane_box->setMaximum(200.0f);
    // focal_plane_box->setSingleStep(0.01f);
    // focal_plane_box->setValue(0.1f);

    // aperatureSlider = new QSlider(Qt::Orientation::Horizontal); // Aperature slider
    // aperatureSlider->setTickInterval(1);
    // aperatureSlider->setMinimum(100);
    // aperatureSlider->setMaximum(4000);
    // aperatureSlider->setValue(100);

    // aperatureBox = new QDoubleSpinBox();
    // aperatureBox->setMinimum(1.f);
    // aperatureBox->setMaximum(40.0f);
    // aperatureBox->setSingleStep(0.1f);
    // aperatureBox->setValue(1.f);



    // // Adds the slider and number box to the parameter layouts
    // foc_plane->addWidget(focal_plane_slider);
    // foc_plane->addWidget(focal_plane_box);
    // focal_PlaneLayout->setLayout(foc_plane);

    // foc_len->addWidget(focal_length_slider);
    // foc_len->addWidget(focal_length_box);
    // focal_lengthLayout->setLayout(foc_len);

    // aper->addWidget(aperatureSlider);
    // aper->addWidget(aperatureBox);
    // aperLayout->setLayout(aper);

    // lbloom->addWidget(bloomSlider);
    // lbloom->addWidget(bloomBox);
    // bloomLayout->setLayout(lbloom);

    vLayout->addWidget(uploadFile);
    vLayout->addWidget(uploadBump);
    // vLayout->addWidget(bump_depth_label);
    // vLayout->addWidget(bumpLayout);
    // // vLayout->addWidget(invert);
    // vLayout->addWidget(colorgrade);

    // vLayout->addWidget(focal_length_label);
    // vLayout->addWidget(focal_lengthLayout);
    // vLayout->addWidget(focal_plane_label);
    // vLayout->addWidget(focal_PlaneLayout);
    // vLayout->addWidget(aperature_label);
    // vLayout->addWidget(aperLayout);


    // vLayout->addWidget(near_label);
    // vLayout->addWidget(nearLayout);
    // vLayout->addWidget(far_label);
    // vLayout->addWidget(farLayout);

    // vLayout->addWidget(exposure_label);
    // vLayout->addWidget(exposureLayout);
    // vLayout->addWidget(bloom_threshold);
    // vLayout->addWidget(bloomLayout);

     connectUIElements();

    // // Set default values of 5 for tesselation parameters
    // onValChangeP1(5);
    // onValChangeP2(5);

    // // Set default values for near and far planes
    // onValChangeNearBox(0.1f);
    // onValChangeFarBox(10.f);
    // onValChangeBloomBox(0.3f);
    // onValChangeExposureBox(0.5f);
}

void MainWindow::finish() {
    realtime->finish();
    delete(realtime);
}

void MainWindow::connectUIElements() {
    // From old Project 6
    //connectPerPixelFilter();
    //connectKernelBasedFilter();
    connectUploadFile();
    // connectFocalPlane();
    // connectFocalLength();
    // connectAperature();


    // connectParam1();
    // connectParam2();
    // connectNear();
    // connectFar();
    // connectExposure();
    // connectBloom();
    connectUploadBump();
    // connectBump();
    // connectInvert();
    // connectColor();

}

void MainWindow::connectUploadFile() {
    connect(uploadFile, &QPushButton::clicked, this, &MainWindow::onUploadFile);
}

void MainWindow::connectUploadBump() {
    connect(uploadBump, &QPushButton::clicked, this, &MainWindow::onUploadBump);
}

void MainWindow::connectBump(){
    connect(bumpSlider, &QSlider::valueChanged, this, &MainWindow::onBumpChange);
    connect(bumpBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::onBumpChange);

}

void MainWindow::connectInvert(){
    connect(invert, &QCheckBox::stateChanged,
            this, &MainWindow::onInvertChange);
}

void MainWindow::connectColor(){
    connect(colorgrade, &QCheckBox::stateChanged,
            this, &MainWindow::onColorChange);
}
void MainWindow::connectSaveImage() {
    connect(saveImage, &QPushButton::clicked, this, &MainWindow::onSaveImage);
}

void MainWindow::connectParam1() {
    connect(p1Slider, &QSlider::valueChanged, this, &MainWindow::onValChangeP1);
    connect(p1Box, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::onValChangeP1);
}

void MainWindow::connectParam2() {
    connect(p2Slider, &QSlider::valueChanged, this, &MainWindow::onValChangeP2);
    connect(p2Box, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::onValChangeP2);
}


void MainWindow::connectFocalPlane() {
    connect(focal_plane_slider, &QSlider::valueChanged, this, &MainWindow::onValChangeFocalPlaneSlider);
    connect(focal_plane_box, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeFocalPlaneBox);
}

void MainWindow::connectFocalLength() {
    connect(focal_length_slider, &QSlider::valueChanged, this, &MainWindow::onValChangeFocalLengthSlider);
    connect(focal_length_box, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeFocalLengthBox);
}

void MainWindow::connectAperature() {
    connect(aperatureSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeAperatureSlider);
    connect(aperatureBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeAperatureBox);
}


void MainWindow::connectNear() {
    connect(nearSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeNearSlider);
    connect(nearBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeNearBox);
}

void MainWindow::connectFar() {
    connect(farSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeFarSlider);
    connect(farBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeFarBox);
}

void MainWindow::connectBloom() {
    connect(bloomSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeBloomSlider);
    connect(bloomBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeBloomBox);
}

void MainWindow::connectExposure() {
    connect(exposureSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeExposureSlider);
    connect(exposureBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeExposureBox);
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

    realtime->bindTexture();

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

void MainWindow::onBumpChange(int newValue){
    bumpSlider->setValue(newValue);
    bumpBox->setValue(newValue);
    settings.bumpDepth = bumpSlider->value();
    realtime->settingsChanged();
}

void MainWindow::onInvertChange(int state){
    if (state == Qt::Checked) {
        colorgrade->blockSignals(true);   // prevent infinite loop
        colorgrade->setChecked(false);
        colorgrade->blockSignals(false);

        settings.color = false;
    }

    settings.invert = (state == Qt::Checked);
    realtime->settingsChanged();

}

void MainWindow::onColorChange(int state){
    if (state == Qt::Checked) {
        invert->blockSignals(true);
        invert->setChecked(false);
        invert->blockSignals(false);

        settings.invert = false;
    }

    settings.color = (state == Qt::Checked);
    realtime->settingsChanged();
}
void MainWindow::onValChangeP1(int newValue) {
    p1Slider->setValue(newValue);
    p1Box->setValue(newValue);
    settings.shapeParameter1 = p1Slider->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeP2(int newValue) {
    p2Slider->setValue(newValue);
    p2Box->setValue(newValue);
    settings.shapeParameter2 = p2Slider->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeFocalLengthSlider(int newValue) {
    focal_length_box->setValue(newValue/100.f);
    settings.focal_length = focal_length_box->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeAperatureSlider(int newValue) {
    aperatureBox->setValue(newValue/100.f);
    settings.aperature = aperatureBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeFocalPlaneSlider(int newValue) {
    focal_plane_box->setValue((newValue/100.f));
    //nearBox->setValue(newValue);
    settings.plane_in_focus = focal_plane_box->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeNearSlider(int newValue) {
    //nearSlider->setValue(newValue);
    nearBox->setValue(newValue/100.f);
    settings.nearPlane = nearBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeFarSlider(int newValue) {
    //farSlider->setValue(newValue);
    farBox->setValue(newValue/100.f);
    settings.farPlane = farBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeBloomSlider(int newValue) {
    bloomSlider->setValue(newValue);
    bloomBox->setValue(newValue/100.f);
    settings.bloomThreshold = bloomBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeExposureSlider(int newValue) {
   exposureSlider->setValue(newValue);
    exposureBox->setValue(newValue/100.f);
    settings.exposure = exposureBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeFocalLengthBox(double newValue) {
    focal_length_slider->setValue(int(newValue*100.f));
    //nearBox->setValue(newValue);
    settings.focal_length = focal_length_box->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeFocalPlaneBox(double newValue) {
    focal_plane_slider->setValue(int(newValue*100.f));
    //nearBox->setValue(newValue);
    settings.plane_in_focus = focal_plane_box->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeAperatureBox(double newValue) {
    aperatureSlider->setValue(int(newValue*100.f));
    //nearBox->setValue(newValue);
    settings.aperature = aperatureBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeNearBox(double newValue) {
    nearSlider->setValue(int(newValue*100.f));
    //nearBox->setValue(newValue);
    settings.nearPlane = nearBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeFarBox(double newValue) {
    farSlider->setValue(int(newValue*100.f));
    //farBox->setValue(newValue);
    settings.farPlane = farBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeExposureBox(double newValue) {
    exposureSlider->setValue(int(newValue*100.f));
    //farBox->setValue(newValue);
    settings.exposure = exposureBox->value();
    realtime->settingsChanged();
}
void MainWindow::onValChangeBloomBox(double newValue) {
    bloomSlider->setValue(int(newValue*100.f));
    //farBox->setValue(newValue);
    settings.bloomThreshold = bloomBox->value();
    realtime->settingsChanged();
}

