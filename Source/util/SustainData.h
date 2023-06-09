#pragma once
#include <JuceHeader.h>
#include <algorithm>

class SustainData
{
public:
	SustainData(std::vector<float> buffer, std::atomic<float>& rise, std::atomic<float>& tail);
	SustainData(const SustainData& copy) :
		rise(copy.rise), tail(copy.tail), buffer(copy.buffer), size(copy.size) {}
	~SustainData() {};
	
	std::vector<float> getSample(int nSamples, float wet, int samplingRate);	

	void fade(bool in) { fadeIn = in; }

private:
	float volume = 0.f;
	std::atomic<float>& rise, & tail;
	bool fadeIn = true;
	std::vector<float> buffer;
	int size, pointer = 0;
};

