#include "SustainData.h"

SustainData::SustainData(std::vector<float> buffer, double* rise, double* tail) : buffer(buffer), rise(rise), tail(tail) {
	size = buffer.size();
}

SustainData::~SustainData() 
{ 
}

std::vector<float> SustainData::getSample(int nSamples, double wet, int samplingRate)
{
	if (size == 0) return {};

	auto out = std::vector<float>(nSamples, 0);
	double step = fadeIn ? *rise : -*tail;
	step /= samplingRate;
	
	for (int i = 0; i < nSamples; i++) {
		out[i] = buffer[pointer] * volume * wet;
		pointer = (pointer + 1) % size;
		volume += step;
		volume = std::min(std::max(volume, 0.f), 1.0f);
	}

	if (volume == 0.f) {
		buffer = {};
		size = 0;
	}

	return out;
}