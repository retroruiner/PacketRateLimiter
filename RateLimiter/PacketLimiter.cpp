#include <pcap.h>
#include <chrono>
#include <stdexcept>
#include "PacketLimiter.h"
#include <iostream>


// Constructor to initialize the rate limiter
PacketLimiter::PacketLimiter(double rateMbps) : rateMbps_(rateMbps) {
	if (rateMbps_ > 0) {
		// Calculate the maximum number of tokens in the token bucket
		// Converts the rate from Mbps to bytes per second
		tokenBucketSize = rateMbps_ * 1e6 / 8;
		tokens = tokenBucketSize;
		// Record the current time as the last replenishment time
		lastUpdate = std::chrono::steady_clock::now();
	}
	else {
		// Throws exception if the enetered rate is negative
		throw std::invalid_argument("Invalid rate!");
	}
}

// Attempt to consume tokens for a packet of given size
bool PacketLimiter::tryConsume(double packetSize) {
	if (packetSize >= 0) {
		// Update the number of tokens in the token bucket
		updateTokens();
		// Check if there are enough tokens to consume for the packet
		if (tokens > packetSize) {
			// Consume tokens and return true
			tokens -= packetSize;
			return true;
		}
		return false;
	}
	else {
		// Throws exception if the packet size is negative
		throw std::invalid_argument("Invalid packet size!");
	}
}

// Replenish the number of tokens in the token bucket based on time elapsed
void PacketLimiter::updateTokens() {
	// Current time
	auto currentTime = std::chrono::steady_clock::now();
	// Calculate how much time has passed since the last replenishment
	auto timeELapsed = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - lastUpdate).count();

	// Ensure the token count does not exceed the token bucket size
	tokens = min(tokens + rateMbps_ * timeELapsed / 8, tokenBucketSize);

	// Update the last update time
	lastUpdate = currentTime;
}


