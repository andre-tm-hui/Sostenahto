/*
  ==============================================================================

    TransientDetect.cpp
    Created: 19 Apr 2023 11:04:46am
    Author:  andre

  ==============================================================================
*/

#include "TransientDetect.h"

TransientDetector::TransientDetector(int sampleRate, int bufferSize, double frameTime, double overlap, double trackingTime) :
    sampleRate(sampleRate), bufferSize(bufferSize), frameTime(frameTime), overlap(overlap), trackingTime(trackingTime) {
    setup();
}

void TransientDetector::setup() {
    windowSize = 1;
    int timeToSamples = sampleRate * frameTime;
    for (; timeToSamples > 0; timeToSamples /= 2, windowSize *= 2) {}
    if (windowSize < bufferSize) windowSize = bufferSize;
    fftSize = windowSize / 2;

    stepSize = (1.0 - overlap) * windowSize;
    nTrackedFrames = trackingTime * sampleRate / stepSize;

    windowFunction = std::vector<float>(windowSize, 0.f);
    sumFunctions = std::vector<float>(fftSize, 0.f);
    dsp::WindowingFunction<float>::fillWindowingTables(&windowFunction[0], windowSize, dsp::WindowingFunction<float>::blackmanHarris);
}

void TransientDetector::process(std::vector<float> buffer, std::vector<float>& tailBuffer) {
    for (int i = 0; i < buffer.size(); i++) {
        inputStream.push_back(buffer[i]);
    }
    if (inputStream.size() >= windowSize) {
        bool transient = detectTransient(std::vector<float>(inputStream.begin(), inputStream.begin() + windowSize));
        if (transient) tailBuffer.clear();
        inputStream.erase(inputStream.begin(), inputStream.begin() + stepSize);
    }
}

bool TransientDetector::detectTransient(std::vector<float> input) {
    auto fft = pffft::Fft<float>(windowSize);
    auto in = fft.valueVector();
    auto out = fft.spectrumVector();
    for (int i = 0; i < windowSize; i++) {
        in[i] = input[i] * windowFunction[i];
    }
    fft.forward(in, out);

    std::vector<float> mags, phases;
    for (auto cmpl : out) {
        mags.push_back(abs(cmpl));
    }
    magStore.push_back(mags);
    if (magStore.size() > 3) {
        magStore.erase(magStore.begin());
    }
    if (magStore.size() != 3) return false;
    auto processWindow = magStore[1];
    auto backT = processWindow, frontT = processWindow;
    for (int i = 0; i < fftSize; i++) {
        backT[i] -= magStore[0][i];
        frontT[i] -= magStore[2][i];
    }

    // generate F values for each frequency band
    std::vector<float> funcs;
    float total = 0.f;
    for (int i = 0; i < fftSize; i++) {
        float val = 0.f;
        for (int k = std::max(i - 9, 0); k < std::min(i + 10, fftSize); k++) {
            val += (1 + sgn(backT[k])) * backT[k]
                + (1 + sgn(frontT[k])) * frontT[k] * 0;
        }
        val *= 0.5f;
        funcs.push_back(val);
        sumFunctions[i] += val - (fftFunction.size() == 7 ? fftFunction[0][i] : 0.f);
        total += val;
    }
    fftFunction.push_back(funcs);
    if (fftFunction.size() > 7) {
        fftFunction.erase(fftFunction.begin());
    }
    if (fftFunction.size() != 7) return false;

    // Dynamic thresholding
    int flagged = 0;
    for (int j = 0; j < fftSize; j++) {
        float thresh = 2 * sumFunctions[j] / (2 * 3 + 1);
        flagged += fftFunction[3][j] > thresh ? 1 : 0;
    }
    frameFlags.push_back((float)flagged);
    transientFrames.push_back(flagged > fftSize / 4);
    if (frameFlags.size() > nTrackedFrames) {
        frameFlags.erase(frameFlags.begin());
        transientFrames.erase(transientFrames.begin());
    }

    if (!transientFrames[transientFrames.size() - 1])
        return false;
    
    return true;
}

void TransientDetector::applyWindow(std::vector<float>& buf) {
    for (int i = 0; i < windowSize; i++) {
        buf[i] *= windowFunction[i];
    }
}

int TransientDetector::sgn(float val) {
    return val >= 0 ? 1 : -1;
}