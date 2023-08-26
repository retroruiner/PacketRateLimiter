#include "pch.h"
#include "CppUnitTest.h"
#include "../RateLimiter/PacketLimiter.h"
#include "../RateLimiter/PacketLimiter.cpp"
#include <thread>
#include <iostream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestLimiter
{
	TEST_CLASS(UnitTestLimiter)
	{
	public:
		
		TEST_METHOD(ConsumeMoreThanAvailable)
		{
			try {
				PacketLimiter limiter(10); // Create a limiter with a rate of 10 Mbps
				// Initial tokens should be the token bucket size
				Assert::IsTrue(limiter.tryConsume(1000000)); // Assuming packet size is 1 megabyte
				Assert::IsFalse(limiter.tryConsume(260000)); // Not enough tokens
				// Wait for some time for tokens to replenish
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				// After waiting, tokens should be available
				Assert::IsTrue(limiter.tryConsume(260000));
			}
			catch (std::invalid_argument) {
				std::cerr << "Invalid rate argument!" << std::endl;
			}
			
		}

		TEST_METHOD(InvalidRateTest) {
			// Wrapping the code under test in a lambda expression to be called by the Assert::ExpectException function
			auto InvalidRateConstructor = [] { 
				PacketLimiter limiter(-10); // Giving negative rate value
			};
			// Expects for exception to be thrown
			Assert::ExpectException<std::invalid_argument>(InvalidRateConstructor);
		}

		TEST_METHOD(InvalidPacketSizeTest) {
			//Wrapping the code under test in a lambda expression to be called by the Assert::ExpectException function
			auto ConsumeFunction = [] { 
				PacketLimiter limiter(10); 
				limiter.tryConsume(-10); // Giving negative packet size value
			};
			// Expects for exception to be thrown
			Assert::ExpectException<std::invalid_argument>(ConsumeFunction);
		}
	};
}
