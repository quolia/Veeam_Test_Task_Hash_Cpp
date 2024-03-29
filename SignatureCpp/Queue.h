#pragma once
#ifndef  _VHASHCPP_QUEUE_H_
#define _VHASHCPP_QUEUE_H_

#include <queue>
#include <mutex>
#include <condition_variable>
#include "Task.h"

namespace VHASHCPP
{
	using namespace std;

	// Simple queue with possibility to await an element.
	template <typename T>
	class awaitable_queue
	{
		mutex _lock;
		queue<T> _elements;
		condition_variable _cv;

	public:

		void add(T el)
		{
			unique_lock<mutex> lock(_lock);

			_elements.push(el);
			_cv.notify_one();
		}

		T wait_and_get()
		{
			unique_lock<mutex> lock(_lock);
			
			while (true)
			{
				if (_elements.empty())
				{
					_cv.wait(lock);
				}
				else
				{
					T el = _elements.front();
					_elements.pop();

					return el;
				}
			}
		}

		unsigned const size()
		{
			unique_lock<mutex> lock(_lock);
			return _elements.size();
		}

		void clear()
		{
			unique_lock<mutex> lock(_lock);

			while (!_elements.empty())
			{
				_elements.pop();
			}
		}
	};

	class task_queue
	{
		awaitable_queue<shared_ptr<hash_task>> _queue;

	public:
		
		void add(const shared_ptr<hash_task>& el)
		{
			_queue.add(el);
		}

		shared_ptr<hash_task> wait_and_get()
		{
			return _queue.wait_and_get();
		}

		unsigned const size()
		{
			return _queue.size();
		}

		void clear()
		{
			_queue.clear();
		}
	};
}

#endif // ! _VHASHCPP_QUEUE_H_