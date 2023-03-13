#pragma once
#include <JuceHeader.h>
#include <algorithm>

class SustainData
{
public:
	SustainData(std::vector<float> buffer, double* rise, double* tail);
	~SustainData();

	std::vector<float> getSample(int nSamples, double wet, int samplingRate);

	void fade(bool in) { fadeIn = in; }

private:
	float volume = 0.f;
	double* rise, * tail;
	bool fadeIn = true;
	std::vector<float> buffer;
	int size, pointer = 0;
};

