#pragma once

#include <QMainWindow>
#include <QCheckBox>
#include <QSlider>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
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
    void connectParam1();
    void connectParam2();
    void connectFocalLength();
    void connectFocalPlane();
    void connectAperature();
    void connectNear();
    void connectFar();

    // From old Project 6
    // void connectPerPixelFilter();
    // void connectKernelBasedFilter();

    void connectUploadFile();
    void connectSaveImage();
    void connectExtraCredit();

    Realtime *realtime;
    AspectRatioWidget *aspectRatioWidget;

    // From old Project 6
    // QCheckBox *filter1;
    // QCheckBox *filter2;

    QPushButton *uploadFile;
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

    // Extra Credit:
    QCheckBox *ec1;
    QCheckBox *ec2;
    QCheckBox *ec3;
    QCheckBox *ec4;

private slots:
    // From old Project 6
    // void onPerPixelFilter();
    // void onKernelBasedFilter();

    void onUploadFile();
    void onSaveImage();
    void onValChangeP1(int newValue);
    void onValChangeP2(int newValue);
    void onValChangeAperatureSlider(int newValue);
    void onValChangeAperatureBox(double newValue);
    void onValChangeFocalPlaneSlider(int newValue);
    void onValChangeFocalPlaneBox(double newValue);
    void onValChangeFocalLengthSlider(int newValue);
    void onValChangeFocalLengthBox(double newValue);
    void onValChangeNearSlider(int newValue);
    void onValChangeFarSlider(int newValue);
    void onValChangeNearBox(double newValue);
    void onValChangeFarBox(double newValue);

    // Extra Credit:
    void onExtraCredit1();
    void onExtraCredit2();
    void onExtraCredit3();
    void onExtraCredit4();
};
