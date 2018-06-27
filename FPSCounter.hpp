#pragma once

#include <chrono>
#include <iostream>

/** A simple FPS counter, designed to output to a stream. Use like this:
 *
 *  auto& myostream = std::cout;
 *  FPSCounter counter;
 *  counter.start();
 *  while (...) {
 *  	// do frame
 *  	counter.addFrame();
 *  	counter.report(myostream);
 *  }
 */
class FPSCounter final {
	using clock = std::chrono::high_resolution_clock;

	const std::string pre;

	// Latest time the FPS were reported
	clock::time_point checkpoint;

	// Frames counted since checkpoint
	uint32_t frames = 0;

public:
	explicit FPSCounter(const std::string& prefix = "FPS")
		: pre(prefix)
	{}

	float reportPeriod = 1;

	void start();
	void addFrame();
	void report(std::ostream& stream = std::cout);
};
