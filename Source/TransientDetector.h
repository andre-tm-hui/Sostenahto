#pragma once
#include <vector>
#include <JuceHeader.h>

enum class TDState {
	FILLING = -1,
	NO_TRANSIENT,
	TRANSIENT
};

class TransientDetector
{
public:
	TransientDetector(int sampleRate, int bufferSize, double frameTime = 0.04, double overlap = 2.0, double trackingTime = 2.0, double blockForNSeconds = 0.5);
	void process(std::vector<float> buffer, std::vector<float>& tailBuffer);

	void setSampleRate(int val) { sampleRate = val; setup(); }
	void setBufferSize(int val) { bufferSize = val; setup(); }
	void setOverlap(double val) { overlap = val; setup(); }
	void setFrameTime(double val) { frameTime = val; setup(); }
	void setTrackingTime(double val) { trackingTime = val; setup(); }
	void setBlockForNSeconds(double val) { blockForNSeconds = val; setup(); }

	std::vector<float> getSpectralFluxes() { return thresholdedFluxes; }

private:
	void setup();
	bool detectTransient();
	std::vector<float> getWindow();
	float getSpectralFlux(std::vector<float> window);
	bool applyThreshold(float spectralFlux);

	dsp::FFT fft;

	std::vector<float> penultimateFrame,
		previousFrame,
		currentFrame,
		previousFFT,
		spectralFluxes,
		thresholdedFluxes;
	std::vector<bool> isTransient;

	int sampleRate, bufferSize, frameSize, paddingSize, windowSize, fftSize, nTrackedFrames, blockForNFrames, blockingCounter = 0;
	double overlap, frameTime, trackingTime, transientLockoutTime, blockForNSeconds;
	float totalSpectralFlux;
};

float getStandardDeviation(std::vector<float> values, float mean, float normalizeBy = 1.0f);