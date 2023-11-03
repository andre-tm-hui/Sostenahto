#include "SustainData.h"

SustainData::SustainData(std::vector<float> buffer, AudioParameterFloat* rise, AudioParameterFloat* tail) :
	buffer(buffer), rise(rise), tail(tail), size(buffer.size())
{}

std::vector<float> SustainData::getSample(int nSamples, float wet, int samplingRate)
{
	if (size == 0) return {};

	auto out = std::vector<float>(nSamples, 0);
	float step;
	if (fadeIn) {
		step = rise->get();
	}
	else {
		step = -tail->get();
	}
	step = 1 / (step * samplingRate);
	
	for (int i = 0; i < nSamples; i++) {
		out[i] = buffer[pointer] * volume * wet;
		pointer++;
		if (pointer == buffer.size()) pointer = 0;

		volume += step;
		volume = std::min(std::max(volume, 0.f), 1.0f);
	}

	if (volume == 0.f) {
		buffer = {};
		size = 0;
	}

	return out;
}