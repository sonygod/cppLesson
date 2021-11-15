#include "Async1.h"

#include <future>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <fmt/core.h>
#include "nicer_syntax.hpp"
#include <cppcoro/generator.hpp>
#include <cppcoro/single_consumer_event.hpp>
#include <cppcoro/sync_wait.hpp>
#include <cppcoro/task.hpp>
#include <cppcoro/cancellation_token.hpp>
#include <cppcoro/cancellation_source.hpp>
#include <cppcoro/async_mutex.hpp>
#include <cppcoro/when_all.hpp>
#include <cppcoro/static_thread_pool.hpp>
#include <cppcoro/async_latch.hpp>
using namespace std::literals;
using namespace fmt;

auto getTimeSince(const time_point<high_resolution_clock>& start) {

	auto end = high_resolution_clock::now();
	duration<double> elapsed = end - start;
	return elapsed.count();

}

async<>  third(const time_point<high_resolution_clock>& start) {

	std::this_thread::sleep_for(1s);
	std::cout << "Third waited " << getTimeSince(start) << " seconds." << std::endl;

	co_return;                                                     // (4)

}

async<>  second(const time_point<high_resolution_clock>& start) {

	auto thi = third(start);                                       // (2)
	std::this_thread::sleep_for(1s);
	co_await thi;                                                  // (3)

	std::cout << "Second waited " << getTimeSince(start) << " seconds." << std::endl;

}

async<> first(const time_point<high_resolution_clock>& start) {

	auto sec = second(start);                                       // (2)
	std::this_thread::sleep_for(1s);
	co_await sec;                                                   // (3)

	std::cout << "First waited " << getTimeSince(start) << " seconds." << std::endl;

}

cppcoro::generator<char> hello() {
	co_yield 'h';
	co_yield 'e';
	co_yield 'l';
	co_yield 'l';
	co_yield 'o';
}


cppcoro::generator<int> gen(int count) {
	for (int i = 0; i < count; i++) {
		co_yield i;
	}
}


cppcoro::generator<const long long> fibonacci() {
	long long a = 0;
	long long b = 1;
	while (true) {
		co_yield b;                 // (2)
		auto tmp = a;
		a = b;
		b += tmp;
	}
}

cppcoro::single_consumer_event event;

cppcoro::task<> consumer() {

	auto start = std::chrono::high_resolution_clock::now();

	co_await event;  // suspended until some thread calls event.set()

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = end - start;
	std::cout << "Consumer waited " << elapsed.count() << " seconds." << std::endl;

	co_return;
}

void producer() {

	using namespace std::chrono_literals;
	std::this_thread::sleep_for(2s);

	event.set();  // resumes the consumer  

}
void testCancel() {
	std::cout << std::endl;

	cppcoro::cancellation_source canSource;
	cppcoro::cancellation_token canToken = canSource.token();  // (1)

	auto cancelSender = std::async([&canSource] {
		std::this_thread::sleep_for(5s);
		canSource.request_cancellation();                      // (3)
		std::cout << "canSource.request_cancellation() " << std::endl;
	});

	auto cancelReceiver = std::async([&canToken] {
		while (true) {
			std::cout << "Wait for cancellation request" << std::endl;
			std::this_thread::sleep_for(200ms);
			if (canToken.is_cancellation_requested()) return;  // (2)
		}
	});

	cancelSender.get(), cancelReceiver.get();

	std::cout << std::endl;
}

cppcoro::async_mutex mutex;

int sum;                                                                  // (2)

cppcoro::task<> addToSum(int num) {
	cppcoro::async_mutex_lock lockSum = co_await mutex.scoped_lock_async(); // (3)  
	sum += num;

}

void testAddSum()
{
	std::vector<std::thread> vec(10);                                       // (1)

	for (auto& thr : vec) {
		thr = std::thread([] { for (int n = 0; n < 10; ++n) cppcoro::sync_wait(addToSum(n)); });
	}

	for (auto& thr : vec) thr.join();

	std::cout << "sum: " << sum << std::endl;
}



cppcoro::task<std::string> getFirst() {
	std::this_thread::sleep_for(1s);                       // (3)
	co_return "First";
}

cppcoro::task<std::string> getSecond() {
	std::this_thread::sleep_for(1s);                      // (3)
	co_return "Second";
}

cppcoro::task<std::string> getThird() {
	std::this_thread::sleep_for(1s);                     // (3)
	co_return "Third";
}


cppcoro::task<> runAll() {
	// (2)
	auto[fir, sec, thi] = co_await cppcoro::when_all(getFirst(), getSecond(), getThird());

	std::cout << fir << " " << sec << " " << thi << std::endl;

}

void testWhenAll()
{

	auto start = std::chrono::steady_clock::now();

	cppcoro::sync_wait(runAll());                          // (1)

	std::cout << std::endl;

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = end - start;   // (4)
	std::cout << "Execution time " << elapsed.count() << " seconds." << std::endl;

}



template <typename Func>
cppcoro::task<std::string> runOnThreadPool(cppcoro::static_thread_pool& tp, Func func) {
	co_await tp.schedule();
	auto res = co_await func();
	co_return res;
}

cppcoro::task<> runAll(cppcoro::static_thread_pool& tp) {

	auto start = std::chrono::steady_clock::now();
	auto[fir, sec, thi] = co_await cppcoro::when_all(    // (3)
		runOnThreadPool(tp, getFirst),
		runOnThreadPool(tp, getSecond),
		runOnThreadPool(tp, getThird));

	std::cout << fir << " " << sec << " " << thi << std::endl;

	std::cout << std::endl;

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = end - start;    // (4)
	std::cout << "Execution time " << elapsed.count() << " seconds." << std::endl;

	std::cout << std::endl;

}

void testRunALl()
{

	
		cppcoro::static_thread_pool tp;                         // (1)
		cppcoro::sync_wait(runAll(tp));
	
}


cppcoro::task<> waitFor(cppcoro::async_latch& latch) {
	std::cout << "Before co_await" << std::endl;
	co_await latch;                              // (3)
	std::cout << "After co_await" << std::endl;
}

void async_latch()
{
	std::cout << std::endl;

	cppcoro::async_latch latch(3);              // (1)

												// (2)
	auto waiter = std::async([&latch] { cppcoro::sync_wait(waitFor(latch)); });

	auto counter1 = std::async([&latch] {       // (2)
		std::this_thread::sleep_for(2s);
		std::cout << "counter1: latch.count_down() " << std::endl;
		latch.count_down();
	});

	auto counter2 = std::async([&latch] {      // (2)
		std::this_thread::sleep_for(1s);
		std::cout << "counter2: latch.count_down(2) " << std::endl;
		latch.count_down(2);
	});

	waiter.get(), counter1.get(), counter2.get();

	std::cout << std::endl;
	
}
//https://www.modernescpp.com/index.php/c-20-coroutines-with-cppcoro
void mainAsync() {

	std::cout << std::endl;

	auto start = high_resolution_clock::now();
	cppcoro::sync_wait(first(start));                              // (1)

	std::cout << "Main waited " << getTimeSince(start) << " seconds." << std::endl;

	std::cout << std::endl;


	for (auto c : hello()) std::cout << c;

	for(auto index :gen(10))
	{
		fmt::print("\n index={0}", index);
	}

	auto g = gen(10);
	auto it = g.begin();

	print("\ncurrent index={}", *it);

	it++;

	print("2 current index={}", *it);
	//fmt::print()
	auto start2 = high_resolution_clock::now();
	//需要测试运行时间的程序段
	
	for (auto i : fibonacci()) {  // (1)
		if (i > 1'000'000'000'000) break;
		std::cout << i << " ";
	}
	//std::cout << "Main waited " << getTimeSince(start) << " seconds." << std::endl;

	print("\n spend time={}", getTimeSince(start2));



	auto con = std::async([] { cppcoro::sync_wait(consumer()); });  // (1)
	auto prod = std::async(producer);                              // (2)

	con.get(), prod.get();

	std::cout << std::endl;

	testCancel();
	testAddSum();

	testWhenAll();

	testRunALl();
}



void Async1::main()
{
	mainAsync();
}
