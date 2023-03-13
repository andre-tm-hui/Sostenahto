#pragma once
#include <JuceHeader.h>
#include <future>
#include <algorithm>

struct AutoCorrelationData {
	int period;
	int targetSize;
};

namespace SamplingUtil
{
	std::vector<float> getSample(std::vector<float> buffer, int targetSampleLength, double crossfadeOverlap);

	AutoCorrelationData getPeriod(std::vector<float> buffer, int min);

	std::vector<float> autocorrelate(std::vector<float> buffer);

	std::vector<float> crossfadeSelf(std::vector<float> buffer, int length, int delayUpTo, double overlap);

	std::vector<float> dynamicRangeCompression(std::vector<float> buffer, int windowSize, double overlap = 2.0);

	float rms(std::vector<float> buffer);
};

