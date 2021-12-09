#pragma once

#include "synchronization/event.hpp"
#include "synchronization/mutex.hpp"

#include <functional>


namespace np
{
	class executor
	{
	public:
		executor() noexcept;

		template <typename F>
		void push(F&& fnc) noexcept;

		void start() noexcept;
		inline void stop() noexcept;

	private:
		np::event _ev;
		np::mutex _mutex;

		bool _running;
		std::atomic<uint32_t> _pending;
		moodycamel::ConcurrentQueue<std::function<void()>> _jobs;
	};


	template <typename F>
	void executor::push(F&& fnc) noexcept
	{
		_jobs.enqueue(std::forward<F>(fnc));
		_mutex.lock();
		++_pending;
		_ev.notify();
		_mutex.unlock();
	}

	inline void executor::stop() noexcept
	{
		_running = false;
	}
}