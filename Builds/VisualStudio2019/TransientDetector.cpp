#include "TransientDetector.h"

std::mutex m;

TransientDetector::TransientDetector(int sampleRate, int bufferSize, double frameTime, double overlap, double trackingTime, double transientLockoutTime) : 
	fft(1), 
	sampleRate(sampleRate), 
	bufferSize(bufferSize), 
	frameTime(frameTime), 
	overlap(overlap), 
	trackingTime(trackingTime),
	transientLockoutTime(transientLockoutTime)
{
	setup();
}

void TransientDetector::setup()
{
	frameSize = 1;
	int timeToSamples = sampleRate * frameTime;
	for (; timeToSamples > 0; timeToSamples /= 2, frameSize *= 2) {}
	if (frameSize < bufferSize) frameSize = bufferSize;

	paddingSize = frameSize * (overlap - 1.0) / 2.0;
	windowSize = frameSize * overlap;
	fftSize = windowSize * 2;
	size_t order = 0, power = 1;
	while (power < windowSize) {
		order++;
		power *= 2;
	}

	fft = dsp::FFT(order);

	nTrackedFrames = trackingTime * sampleRate / frameSize;
	nLockoutFrames = transientLockoutTime * sampleRate / frameSize;

	totalSpectralFlux = 0.f;
}

void TransientDetector::process(std::vector<float> buffer, std::vector<float>& tailBuffer)
{
	for (int i = 0; i < bufferSize; i++) {
		currentFrame.push_back(buffer[i]);
	}
	if (currentFrame.size() < frameSize) return;
	
	bool transient = detectTransient();
	if (transient) tailBuffer.clear();
}

bool TransientDetector::detectTransient()
{
	auto checkFrame = penultimateFrame.size() == 0 ? &penultimateFrame : &previousFrame;
	if (checkFrame->size() == 0) {
		*checkFrame = currentFrame;
		currentFrame.clear();
		return false;
	}
	
	// Generate a window using a Hann windowing function, padded with 0s to account for the FFT size
	auto window = getWindow();
	// Get the normalized spectral flux of the window
	auto spectralFlux = getSpectralFlux(window);
	penultimateFrame = previousFrame;
	previousFrame = currentFrame;
	currentFrame = {};
	if (spectralFlux < 0.f) return false;

	// Threshold to filter out low signals
	bool keep = applyThreshold(spectralFlux);
	if (!keep) {
		thresholdedFluxes.push_back(0);
		return false;
	}
	thresholdedFluxes.push_back(spectralFlux);

	// Check if the last 3 spectral fluxes indicate a peak
	auto tfSize = thresholdedFluxes.size();
	if (tfSize < 5 || !(thresholdedFluxes[tfSize - 2] > thresholdedFluxes[tfSize - 1] 
		&& thresholdedFluxes[tfSize - 2] > thresholdedFluxes[tfSize - 5]
		&& thresholdedFluxes[tfSize - 2] > thresholdedFluxes[tfSize - 3]))
		return false;
	
	return true;
}

std::vector<float> TransientDetector::getWindow()
{
	std::vector<float> window(fftSize, 0);
	dsp::WindowingFunction<float>::fillWindowingTables(&window[0], windowSize, dsp::WindowingFunction<float>::WindowingMethod::hann);

	int j = 0;
	for (size_t i = frameSize - paddingSize; i < (size_t)frameSize; i++, j++) {
		window[j] *= penultimateFrame[i];
	}

	for (size_t i = 0; i < (size_t)frameSize; i++, j++) {
		window[j] *= previousFrame[i];
	}

	for (size_t i = 0; i < (size_t)paddingSize && j < fftSize; i++, j++) {
		window[j] *= currentFrame[i];
	}
	return window;
}

float TransientDetector::getSpectralFlux(std::vector<float> window)
{
	fft.performFrequencyOnlyForwardTransform(&window[0]);
	if (previousFFT.size() == 0) {
		previousFFT = window;
		return -1.f;
	}

	float spectralFlux = 0.f;
	for (int i = 0; i < previousFFT.size(); i++) {
		spectralFlux += abs(previousFFT[i] - window[i]);
	}
	spectralFluxes.push_back(spectralFlux);
	totalSpectralFlux += spectralFlux;

	if (spectralFluxes.size() > nTrackedFrames) {
		totalSpectralFlux -= spectralFluxes[0];
		spectralFluxes.erase(spectralFluxes.begin());
		thresholdedFluxes.erase(thresholdedFluxes.begin());
	}

	previousFFT = window;
	return spectralFlux;
}

bool TransientDetector::applyThreshold(float spectralFlux)
{
	float mean = totalSpectralFlux / spectralFluxes.size();
	float std = getStandardDeviation(spectralFluxes, mean);
	return std < 3 ? false : spectralFlux > mean + std * 1.f;
}

float getStandardDeviation(std::vector<float> values, float mean, float normalizeBy) {
	if (mean == 0.f) {
		for (auto v : values) mean += v;
		mean /= values.size();
	}
	mean /= normalizeBy;
	float sd = 0.f;
	for (auto v : values) {
		float diff = v - mean;
		sd += diff * diff;
	}
	sd /= values.size();
	return sqrt(sd / (values.size() - 1));
}