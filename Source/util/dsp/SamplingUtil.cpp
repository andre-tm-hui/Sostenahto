#include "SamplingUtil.h"

std::vector<float> SamplingUtil::getSample(std::vector<float> buffer, int targetSampleLength, bool forcePeriod, size_t sampleRate) {
	targetSampleLength *= 2; // accounts for 50% overlap in crossfadeSelf
	bool processed = false;
	if (forcePeriod && buffer.size() < targetSampleLength && buffer.size() > 4096) {
		// we use a 4096-length window/FFT size for time scaling, as it provides the best balance of quality and performance
		// TODO: investigate quality of this window length for other music-production sample rates - only tested on 48000 currently
		int size = buffer.size();
		buffer = timeScale(buffer, targetSampleLength, sampleRate, 4096);
		if (buffer.size() != size) { // implies that we successfully stretched the sample
			crossfadeSelf(buffer);
			//buffer = dynamicRangeCompression(buffer, 4096);
			processed = true;
		}
	}
	if (!processed) {
		// if the buffer is longer than the target, cap the target length there. Otherwise, make it some fraction of the buffer size, 
		// less than 0.5 since autocorrelation is symmetrical
		int adjustedTarget = std::min((double)targetSampleLength, (double)buffer.size());
		AutoCorrelationData acd = getPeriod(buffer, targetSampleLength);
		int newSize = (int)floor(buffer.size() / acd.period) * acd.period;
		buffer.erase(buffer.begin() + newSize, buffer.end());
		// autocorrelation input is padded to be compatible with FFT libraries (2^n), 
		// hence output can show longer intervals than the original input signal length
		// we should try to get the longest possible interval from the orignal sample as possible
		crossfadeSelf(buffer);
		//buffer = dynamicRangeCompression(buffer, acd.period * 2);
	}
	
	return buffer;
}

AutoCorrelationData SamplingUtil::getPeriod(std::vector<float> buffer, int minSampleLength) {
	if (buffer.size() > minSampleLength)
		buffer.erase(buffer.begin(), buffer.end() - minSampleLength);
	buffer = autocorrelate(buffer);

	AutoCorrelationData out = { minSampleLength, minSampleLength };
	float maxPeriodVal = -1.f, maxTargetVal = -1.f;
	// get the highest peak, corresponding to the period
	for (size_t i = 5; i < buffer.size() - 1; i++) {
		if (buffer[i] > buffer[i - 1] && buffer[i] > buffer[i + 1] && maxPeriodVal < buffer[i]) {
			out.period = i;
			maxPeriodVal = buffer[i];
		}
	}
	
	return out;
}

std::vector<float> SamplingUtil::autocorrelate(std::vector<float> buffer) {
	size_t originalSize = buffer.size();
	size_t bufSize = 1;
	while (bufSize < originalSize) {
		bufSize *= 2;
	}
	size_t windowSize = bufSize;
	size_t paddingSize = (windowSize - originalSize) / 2;

	// zero padding
	for (int i = 0; i < paddingSize; i++) {
		buffer.insert(buffer.begin(), 0.f);
		buffer.push_back(0.f);
	}

	// setup FFT
	const int N = windowSize;
	pffft::Fft<float> fft(N);
	auto X = fft.valueVector();
	auto Y = fft.spectrumVector();
	auto Z = fft.valueVector();

	for (int i = 0; i < windowSize; i++) {
		X[i] = buffer[i];
	}

	fft.forward(X, Y);

	for (auto& cmpl : Y) {
		cmpl = std::complex<float>(abs(cmpl), 0.f);
	}

	fft.inverse(Y, Z);

	buffer = std::vector<float>(Z.begin(), Z.begin() + originalSize);
	return buffer;
}

void SamplingUtil::crossfadeSelf(std::vector<float>& buffer) {
	if (buffer.size() < 1000) {
		DBG("fail");
		return;
	}
	size_t windowSize = buffer.size();
	// default 50% overlap
	size_t overlapSize = windowSize / 2;
	std::vector<float> window(windowSize, 0.f);
	dsp::WindowingFunction<float>::fillWindowingTables(&window[0], windowSize + 1, dsp::WindowingFunction<float>::WindowingMethod::triangular, true);

	std::vector<float> out((int)std::max((float)buffer.size(), (float)2048), 0.f);
	for (int i = 0, j = windowSize / 2; i < windowSize; i++, j++) {
		j %= windowSize;
		out[i] = (buffer[i] * window[i] + buffer[j] * window[j]);
	}

	for (int i = 0; i < windowSize; i++) {
		buffer[i] = out[i];
	}
}

std::vector<float> SamplingUtil::dynamicRangeCompression(std::vector<float> buffer, int windowSize, double overlap) {
	while (buffer.size() % windowSize != 0) {
		windowSize++;
	}
	buffer.insert(buffer.begin(), buffer.begin(), buffer.begin() + windowSize / 2);
	buffer.insert(buffer.end(), buffer.end() - windowSize / 2, buffer.end());
	std::vector<float> out(buffer.size(), 0.f);
	float targetLevel = rms(buffer);
	std::vector<float> windowingFunction(windowSize, 0.f);
	dsp::WindowingFunction<float>::fillWindowingTables(&windowingFunction[0], windowSize, dsp::WindowingFunction<float>::WindowingMethod::hann);

	for (int i = 0; i < buffer.size() - windowSize - 1; i += windowSize / overlap) {
		std::vector<float> window = std::vector<float>(&buffer[i], &buffer[i + (int)windowSize]);
		float windowRms = rms(window) + 1e-6;
		float frameLevel = 20.f * log10(windowRms);
		float gainLinear = targetLevel / windowRms;
		
		for (int j = 0, k = i; j < windowSize; j++, k++) {
			out[k] += window[j] * gainLinear * windowingFunction[j];
		}
	}

	for (int i = 0; i < windowSize / 2; i++) {
		out[i] /= windowingFunction[i];
		out[out.size() - 1 - i] /= windowingFunction[(int)windowSize - 1 - i];
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

std::vector<float> SamplingUtil::timeScale(std::vector<float> buffer, int targetLength, size_t sampleRate, size_t windowSize) {
	const int N = windowSize;
	std::vector<std::future<pffft::AlignedVector<std::complex<double>>>> forwards;
	std::vector<std::future<pffft::AlignedVector<double>>> backwards;

	// setup some vars
	const double overlapFactor = pow(2, -2);
	const size_t overlapSize = windowSize * (1.0 - overlapFactor);
	targetLength += 2.0 * overlapSize; // account for windowed head and tail of signal
	const size_t synthesisHopSize = windowSize * overlapFactor; // >50% overlap
	const size_t nMarkers = ceil(((float)targetLength - windowSize) / (float)synthesisHopSize + 1.f);
	const size_t analysisHopSize = floor((float)(buffer.size() - windowSize) / (nMarkers - 1.f));
	if (analysisHopSize < 1) return buffer; // fallback to unforced method
	targetLength = (nMarkers - 1) * synthesisHopSize + windowSize; // update the target length with respect to rounding
	const float timeScaleFactor = (float)synthesisHopSize / (float)analysisHopSize;
	const float rms_input = rms(buffer);

	// setup storage
	std::vector<double> prevPhases((int)N / 2 + 1, 0.f),
		prevSynthPhases((int)N / 2 + 1, 0.f);
	std::vector<float> out(targetLength, 0.f);

	// setup constants
	std::vector<double> windowingFunction(windowSize, 0.0);
	dsp::WindowingFunction<double>::fillWindowingTables(&windowingFunction[0], windowSize + 1, dsp::WindowingFunction<double>::hann, false);
	std::vector<double> omegas(N / 2, 0.0);
	for (int i = 0; i < N / 2; i++) {
		omegas[i] = MathConstants<double>::twoPi * analysisHopSize * i / N;
	}

	// pre-processing variables
	bool firstTime = true;
	int analysisWindowStart = 0, synthWindowStart = 0, frame = 0;

	// generate all the FFT asynchronously
	while (analysisWindowStart + windowSize <= buffer.size()) {
		forwards.push_back(std::async(
			std::launch::async,
			forward,
			std::vector<float>(buffer.begin() + analysisWindowStart, buffer.begin() + analysisWindowStart + windowSize),
			N,
			windowingFunction
		));
		analysisWindowStart += analysisHopSize;
		frame++;
	}

	frame = 0;
	// go through each frame as they become available
	while (synthWindowStart + windowSize <= out.size()) {
		auto Y = forwards[frame].get();

		for (int i = 1; i < N / 2; i++) {
			double re = Y[i].real();
			double im = Y[i].imag();

			const double mag = sqrt(re * re + im * im);
			const double phase = atan2(im, re);

			double yunwrap = principleDetermination(phase - prevPhases[i] - omegas[i]);
			yunwrap += omegas[i];
			yunwrap *= timeScaleFactor;

			prevPhases[i] = phase;
			prevSynthPhases[i] = firstTime ? phase : prevSynthPhases[i] + yunwrap;
			firstTime = false;
			Y[i] = std::polar(mag, prevSynthPhases[i]);
		}

		// do inverse FFT in another thread
		backwards.push_back(std::async(
			std::launch::async,
			backward,
			Y,
			N,
			windowingFunction
		));
		synthWindowStart += synthesisHopSize;
		frame++;
	}
	
	synthWindowStart = 0;
	// overlap-add frames as they become available
	for (int i = 0; i < backwards.size(); i++) {
		auto Z = backwards[i].get();
		for (int i = 0, j = synthWindowStart; i < windowSize && j < out.size(); i++, j++) {
			out[j] += (float)(Z[i] / N);
		}
		synthWindowStart += synthesisHopSize;
	}

	// remove windowed head and tail
	out.erase(out.begin(), out.begin() + overlapSize);
	out.erase(out.end() - overlapSize, out.end());
	const float ampScalar = rms_input / rms(out);
	for (auto& val : out) {
		val *= ampScalar;
	}

	// apply low-pass filter to clean up stretched sample
	dsp::ProcessorDuplicator <dsp::IIR::Filter<float>, dsp::IIR::Coefficients <float>> filter;
	dsp::ProcessSpec spec;
	spec.sampleRate = 48000;
	spec.maximumBlockSize = out.size();
	spec.numChannels = 1;

	filter.prepare(spec);
	filter.reset();
	*filter.state = *dsp::IIR::Coefficients<float>::makeLowPass(48000, 1800, 1);
	AudioBuffer<float> buf = AudioBuffer<float>(1, out.size());
	buf.clear();
	buf.addFrom(0, 0, &out[0], out.size());
	dsp::AudioBlock<float> block(buf);
	filter.process(dsp::ProcessContextReplacing<float>(block));
	auto* ptr = buf.getReadPointer(0);
	return std::vector<float>(ptr, ptr + out.size());
}

float SamplingUtil::principleDetermination(double input) {
	while (input > MathConstants<double>::pi) {
		input -= MathConstants<double>::twoPi;
	}
	while (input < -MathConstants<double>::pi) {
		input += MathConstants<double>::twoPi;
	}
	return input;
}

pffft::AlignedVector<std::complex<double>> SamplingUtil::forward(
	std::vector<float> buf,
	size_t N,
	std::vector<double>& windowingFunction
) {
	pffft::Fft<double> fft(N);
	const size_t windowSize = buf.size();
	auto X = fft.valueVector();
	auto Y = fft.spectrumVector();

	// apply windowing function
	for (int i = 0; i < windowSize; i++) {
		X[i] = (double)buf[i] * windowingFunction[i];
	}
	for (int i = windowSize; i < N; i++) {
		X[i] = 0.0;
	}

	// fft the window
	fft.forward(X, Y);

	return Y;
}

pffft::AlignedVector<double> SamplingUtil::backward(
	pffft::AlignedVector<std::complex<double>> Y,
	size_t N,
	std::vector<double>& windowingFunction
) {
	pffft::Fft<double> fft(N);
	auto Z = fft.valueVector();
	fft.inverse(Y, Z);
	for (int i = 0; i < windowingFunction.size() && i < Z.size(); i++) {
		Z[i] *= windowingFunction[i];
	}

	return Z;
}