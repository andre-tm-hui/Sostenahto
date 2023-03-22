#include "SamplingUtil.h"

std::vector<float> SamplingUtil::getSample(std::vector<float> buffer, int targetSampleLength, double crossfadeOverlap) {
	int adjustedTarget = std::min((double)targetSampleLength, 0.4 * (double)buffer.size());
	AutoCorrelationData acd = getPeriod(buffer, adjustedTarget);//getPeriod(std::vector<float>(buffer.begin() + startPoint, buffer.end()), fftSize * sampleTargetFactor);
	buffer = dynamicRangeCompression(buffer, acd.period * 2);
	auto sample = crossfadeSelf(buffer, acd.targetSize, buffer.size() - acd.targetSize, crossfadeOverlap);
	return sample;// dynamicRangeCompression(sample, acd.period * 2);
}

AutoCorrelationData SamplingUtil::getPeriod(std::vector<float> buffer, int minSampleLength) {
	int originalSize = buffer.size();
	buffer = autocorrelate(buffer);

	AutoCorrelationData out = { minSampleLength, minSampleLength };
	float maxPeriodVal = -1.f, maxTargetVal = -1.f;
	for (size_t i = 5; i < originalSize / 2; i++) {
		if (buffer[i] > buffer[i - 1] && buffer[i] > buffer[i + 1] && buffer[i] > maxPeriodVal) {
			out.period = i;
			maxPeriodVal = buffer[i];
		}
		if (i >= minSampleLength && buffer[i] > buffer[i - (size_t)1] && buffer[i] > buffer[i + (size_t)1] && buffer[i] > maxTargetVal) {
			out.targetSize = i;
			maxTargetVal = buffer[i];
		}
	}
	
	return out;
}

std::vector<float> SamplingUtil::autocorrelate(std::vector<float> buffer) {
	size_t bufSize = 1;
	size_t order = 0;
	while (bufSize < buffer.size()) {
		bufSize *= 2;
		order++;
	}
	size_t windowSize = bufSize * (size_t)2;
	auto fft = dsp::FFT(order);
	buffer.resize(bufSize);
	buffer.resize(windowSize, 0);
	fft.performRealOnlyForwardTransform(&buffer[0]);

	for (size_t i = 0; i < 2 * bufSize; i += 2) {
		buffer[i] = buffer[i] * buffer[i] + buffer[i + 1] + buffer[i + 1];
		buffer[i + 1] = 0.f;
	}

	fft.performRealOnlyInverseTransform(&buffer[0]);
	buffer.resize(buffer.size() / 2);
	return buffer;
}

std::vector<float> SamplingUtil::crossfadeSelf(std::vector<float> buffer, int length, int delayUpTo, double overlap) {
	int windowSize = overlap * length;
	int overlapSize = windowSize * (1.0 - 1.0 / overlap);
	delayUpTo -= overlapSize;
	delayUpTo = std::max(delayUpTo, 0);
	auto sample = std::vector<float>(buffer.begin() + delayUpTo, buffer.begin() + windowSize + delayUpTo);
	std::vector<float> window(size_t(overlapSize * 2), 0);
	dsp::WindowingFunction<float>::fillWindowingTables(&window[0], overlapSize * 2, dsp::WindowingFunction<float>::WindowingMethod::hann, true);
	int N = 2 * overlapSize - 1;
	for (int i = 0, j = sample.size() - 1; i < overlapSize; i++, j--) {
		double hannFactor = 0.5 * (1.0 - cos(MathConstants<double>::twoPi * i / N));
		sample[i] *= hannFactor;
		sample[j] *= hannFactor;
	}

	int paddingSize = (windowSize - length) / 2;
	auto out = std::vector<float>(sample.begin() + paddingSize, sample.end() - paddingSize);
	for (int i = 0, j = out.size() - paddingSize - 1, k = sample.size() - paddingSize - 1; i < paddingSize; i++, j++, k++) {
		out[i] += sample[k];
		out[j] += sample[i];
	}

	out.erase(out.begin() + out.size() - 1);
	return out;
}

std::vector<float> SamplingUtil::dynamicRangeCompression(std::vector<float> buffer, int windowSize, double overlap) {
	buffer.insert(buffer.begin(), buffer.begin(), buffer.begin() + windowSize / 2);
	buffer.insert(buffer.end(), buffer.end() - windowSize / 2, buffer.end());
	std::vector<float> out(buffer.size(), 0.f);
	//float targetLevel = 20.f * log10(rms(buffer) + 1e-6);
	float targetLevel = rms(buffer);
	dsp::WindowingFunction<float> windowingFunction(windowSize, dsp::WindowingFunction<float>::WindowingMethod::triangular);

	for (int i = 0; i < buffer.size() - windowSize - 1; i += windowSize / overlap) {
		std::vector<float> window = std::vector<float>(&buffer[i], &buffer[size_t(i + windowSize)]);
		float windowRms = rms(window) + 1e-6;
		float frameLevel = 20.f * log10(windowRms);
		//float gainLinear = std::pow(10.f, (targetLevel - frameLevel) / 20.f);
		float gainLinear = targetLevel / windowRms;
		windowingFunction.multiplyWithWindowingTable(&window[0], window.size());
		for (int j = 0, k = i; j < windowSize; j++, k++) {
			out[k] += window[j] * gainLinear;
		}
	}

	return std::vector<float>(out.begin() + windowSize / 2, out.end() - windowSize / 2);
}

float SamplingUtil::rms(std::vector<float> buffer) {
	float rms = 0.f;
	for (auto amplitude : buffer) {
		rms += amplitude * amplitude;
	}
	return sqrt(rms / buffer.size());
}