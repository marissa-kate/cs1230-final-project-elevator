#include "audiocapture.h"
#include <iostream>
#include <cmath>

float AudioCapture::getFrequency() const {
    return std::min(m_currentFreq * 2.0f, 1.0f); // 0.0〜1.0に正規化
}

AudioCapture::AudioCapture(QObject *parent) : QObject(parent) {
    // finding default mic
    QAudioDevice inputDevice = QMediaDevices::defaultAudioInput();
    if (inputDevice.isNull()) {
        std::cerr << "[Audio] No microphone found!" << std::endl;
        return;
    }

    // format settings
    QAudioFormat format;
    format.setSampleRate(44100);
    format.setChannelCount(1);
    format.setSampleFormat(QAudioFormat::Int16);

    if (!inputDevice.isFormatSupported(format)) {
        format = inputDevice.preferredFormat();
    }

    m_audioSource = new QAudioSource(inputDevice, format, this);
    m_audioSource->setBufferSize(4096); // buffersize

    // start recording
    m_ioDevice = m_audioSource->start();

    if (m_ioDevice) {
        connect(m_ioDevice, &QIODevice::readyRead, this, &AudioCapture::processAudioData);
        std::cout << "[Audio] Microphone started: " << inputDevice.description().toStdString() << std::endl;
    }
}

AudioCapture::~AudioCapture() {
    if (m_audioSource) {
        m_audioSource->stop();
    }
}

float AudioCapture::getLevel() const {
    // amplify
    return std::min(m_currentLevel * 5.0f, 1.0f);
}

void AudioCapture::processAudioData() {
    if (!m_ioDevice) return;

    //data from mic
    QByteArray data = m_ioDevice->readAll();
    if (data.isEmpty()) return;

    // 16bit binary
    const qint16 *samples = reinterpret_cast<const qint16*>(data.constData());
    int sampleCount = data.size() / 2;

    // use RMS for volume
    long long sumSquares = 0;
    int zeroCrossings = 0;
    for (int i = 0; i < sampleCount; ++i) {
        sumSquares += samples[i] * samples[i];
        if (i > 0) {
            if ((samples[i] > 0 && samples[i-1] <= 0) ||
                (samples[i] <= 0 && samples[i-1] > 0)) {
                zeroCrossings++;
            }
        }
    }

    if (sampleCount > 0) {
        float rms = std::sqrt(sumSquares / (float)sampleCount);
        // normalize
        float rawLevel = rms / 32768.0f;

        // make it smooth
        m_currentLevel = m_currentLevel * 0.6f + rawLevel * 0.4f;

        //freq using zero crossing: how many times did it go through + and -?
        float rawFreq = (float)zeroCrossings / sampleCount;
        m_currentFreq = m_currentFreq * 0.6f + rawFreq * 0.4f;
    }
}
