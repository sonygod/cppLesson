#pragma once
#include <cppcoro/io_service.hpp>
#include <cppcoro/sync_wait.hpp>
#include <cppcoro/when_all.hpp>
#include <cppcoro/task.hpp>
#include  <cppcoro/schedule_on.hpp>
namespace cor = cppcoro;
namespace chr = std::chrono;
cor::io_service srvc;
template <typename T = void>
using async = cor::task<T>;

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include <cppcoro/sync_wait.hpp>


using std::chrono::high_resolution_clock;
using std::chrono::time_point;
using std::chrono::duration;

using namespace std::chrono_literals; // 1s
namespace asyncio
{

	template <typename Dur>
	cor::task<> sleep(Dur&& d)
	{
		co_await srvc.schedule_after(std::forward<Dur>(d));
	}

	cor::task<> sleep(int d)
	{
		co_await sleep(std::chrono::duration<int>(d));
	}

	template <typename A>
	auto run(A&& a)
	{
		return std::get<0>(cor::sync_wait(
			cor::when_all(
				[&]() -> cor::task<std::remove_reference_t<typename cor::awaitable_traits<A>::await_result_t>> {
			cor::io_work_scope scp(srvc);
			co_return co_await cor::schedule_on(srvc, std::forward<A>(a));
		}(),
			[&]() -> cor::task<> {
			srvc.process_events();
			co_return;
		}()
			)
		));
	}

	template <typename... Args>
	decltype(auto) gather(Args&&... args)
	{
		return cor::when_all(std::forward<Args>(args)...);
	}
}