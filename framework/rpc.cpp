#include <iostream>
#include "rpc-function.hpp"
#include <thread>
#include <mutex>
#include <string>
#include <semaphore>

#define TOTAL_THREAD 100

std::counting_semaphore sem(TOTAL_THREAD);

std::mutex mtxError;
size_t error = 0;

int doReq(size_t loopCount) {
	for (size_t i = 0; i < loopCount; ++i)
	{
		int val1 = rand();
		int val2 = rand();
		MyType valPair(val1, val2);
		try {
			int retVal = rpc::add(valPair);
			if (val1 + val2 != retVal)
			{
				throw std::runtime_error(std::to_string(val1) + "+" + std::to_string(val2) + " returned " + std::to_string(retVal));
			}
		}
		catch (const std::exception& e) {
			std::unique_lock<std::mutex> lockErr(mtxError);
			std::cerr << "Error: " << e.what() << std::endl;
			++error;
			lockErr.unlock();
		}
	}
	sem.release();
	return 0;
}

int main(int argc, char* argv[]) {
	// example
	rpc::SetServer("127.0.0.1", 12345);
	srand(time(0));
	size_t totalThread = TOTAL_THREAD;
	size_t loopCount = 10;
	for (size_t i = 0; i < totalThread; ++i)
	{
		sem.acquire();
		std::thread newThread(doReq, loopCount);
		newThread.detach();
	}
	for (size_t i = 0; i < totalThread; ++i)
	{
		sem.acquire();
	}
	std::cout << "Error rate: " << (double)error / (double)(totalThread * loopCount) << std::endl;
	std::cin.get();
	return 0;
}
