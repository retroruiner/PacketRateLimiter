#pragma once
#include <chrono>
#include <pcap.h>
class PacketLimiter
{
public:
	// Constructor to initialize the rate limiter
	PacketLimiter(double rateMbps);

	// Attempt to consume tokens for a packet of given size
	bool tryConsume(double packetSize);

private:
	double rateMbps_; // Processing rate in megabits per second
	double tokenBucketSize; // Maximum number of tokens in the bucket
	double tokens; // Current number of available tokens
	std::chrono::steady_clock::time_point lastUpdate; // Time of last token update

	// Replenish the number of tokens in the token bucket based on time elapsed
	void updateTokens();
};

