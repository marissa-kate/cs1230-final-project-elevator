#pragma once

#include <QMainWindow>
#include <QCheckBox>
#include <QSlider>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include "inputs.h"
#include "realtime.h"
#include "utils/aspectratiowidget/aspectratiowidget.hpp"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    void initialize();
    void finish();

private:
    void connectUIElements();
    void connectUploadBump();
    void connectUploadFile();
    void connectSaveImage();

    Realtime *realtime;
    AspectRatioWidget *aspectRatioWidget;

    // From old Project 6
    // QCheckBox *filter1;
    // QCheckBox *filter2;

    QPushButton *uploadFile;
    QPushButton *uploadBump;
    QSlider *bumpSlider;
    QSpinBox *bumpBox;
    QCheckBox *invert;
    QCheckBox *colorgrade;
    QPushButton *saveImage;
    QSlider *p1Slider;
    QSlider *p2Slider;
    QSpinBox *p1Box;
    QSpinBox *p2Box;
    QSlider *aperatureSlider;
    QSlider *focal_length_slider;
    QSlider *focal_plane_slider;
    QSlider *nearSlider;
    QSlider *farSlider;
    QDoubleSpinBox *aperatureBox;
    QDoubleSpinBox *focal_plane_box;
    QDoubleSpinBox *focal_length_box;
    QDoubleSpinBox *nearBox;
    QDoubleSpinBox *farBox;
    QSlider *exposureSlider;
    QSlider *bloomSlider;
    QDoubleSpinBox *exposureBox;
    QDoubleSpinBox *bloomBox;

    // Extra Credit:
    QCheckBox *ec1;
    QCheckBox *ec2;
    QCheckBox *ec3;
    QCheckBox *ec4;

private slots:
    void onUploadFile();
    void onUploadBump();
};
