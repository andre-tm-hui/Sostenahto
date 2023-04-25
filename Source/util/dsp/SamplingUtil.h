#pragma once
#include <JuceHeader.h>
#include <future>
#include <algorithm>
#include "../source/pffft/pffft.hpp"

struct AutoCorrelationData {
	int period;
	int targetSize;
};

namespace SamplingUtil
{
	std::vector<float> getSample(std::vector<float> buffer, int targetSampleLength, bool forcePeriod, size_t sampleRate);

	AutoCorrelationData getPeriod(std::vector<float> buffer, int min);

	std::vector<float> autocorrelate(std::vector<float> buffer);

	void crossfadeSelf(std::vector<float>& buffer);

	std::vector<float> dynamicRangeCompression(std::vector<float> buffer, int windowSize, double overlap = 2.0);

	float rms(std::vector<float> buffer);

	std::vector<float> timeScale(std::vector<float> buffer, int targetLength, size_t sampleRate, size_t windowSize = 4096);

	float principleDetermination(double input);

	pffft::AlignedVector<std::complex<double>> forward(
		std::vector<float> buf,
		size_t fftSize,
		std::vector<double>& windowingFunction
	);

	pffft::AlignedVector<double> backward(
		pffft::AlignedVector<std::complex<double>> Y,
		size_t fftSize,
		std::vector<double>& windowingFunction
	);
};

