/*
  ==============================================================================

    TransientDetect.h
    Created: 19 Apr 2023 11:04:46am
    Author:  andre

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../source/pffft/pffft.hpp"

class TransientDetector
{
public:
    TransientDetector(int sampleRate, int bufferSize, double frameTime = 0.04, double overlap = 0.75, double trackingTime = 2);
    void process(std::vector<float> buffer, std::vector<float>& tailBuffer);

	void setSampleRate(int val) { sampleRate = val; setup(); }
	void setBufferSize(int val) { bufferSize = val; setup(); }
	void setOverlap(double val) { overlap = val; setup(); }
	void setFrameTime(double val) { frameTime = val; setup(); }
	void setTrackingTime(double val) { trackingTime = val; setup(); }

#if JUCE_DEBUG
	std::vector<float> getFrameFlags() { return frameFlags; }
#endif

private:
	void setup();
	bool detectTransient();
	void applyWindow(std::vector<float>& buf);
	int sgn(float val);

	std::vector<std::vector<std::complex<float>>> fftHistory;
	std::vector<std::vector<float>> magStore, phaseStore, fftFunction;
	std::vector<float> inputStream, frameFlags, windowFunction;
	std::vector<bool> transientFrames;

	int sampleRate, bufferSize, windowSize, fftSize, nTrackedFrames, stepSize;
	double overlap, frameTime, trackingTime;
};