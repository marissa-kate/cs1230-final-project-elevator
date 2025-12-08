#pragma once
#include <QObject>
#include <QAudioSource>
#include <QMediaDevices>
#include <QAudioDevice>

class AudioCapture : public QObject {
    Q_OBJECT
public:
    explicit AudioCapture(QObject *parent = nullptr);
    ~AudioCapture();

    // sound
    float getLevel() const;
    float getFrequency() const;

private slots:
    void processAudioData();

private:
    QAudioSource *m_audioSource = nullptr;
    QIODevice *m_ioDevice = nullptr;
    float m_currentLevel = 0.0f;
    float m_currentFreq = 0.0f;
};
