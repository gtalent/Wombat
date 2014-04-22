/*
 * Copyright 2013-2014 gtalent2@gmail.com
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef WOMBAT_CORE_THREADS_HPP
#define WOMBAT_CORE_THREADS_HPP

#include <functional>
#include <map>
#include <queue>

#ifdef WITH_SDL
#include <SDL.h>
#endif

#include "event.hpp"
#include "misc.hpp"
#include "types.hpp"

namespace wombat {
namespace core {

class Semaphore;
class TaskProcessor;

class TaskState {
	public:
		enum State {
			Running,
			Waiting,
			Done
		} state;

		/**
		 * Time (milliseconds) til the Task wants to run again.
		 */
		uint64 sleepDuration;

		/**
		 * Constructor
		 * @param state state of task, defaults to Waiting
		 */
		TaskState(State state = Waiting);

		/**
		 * Constructor
		 * State is set to Running.
		 * @param sleep time to sleep in milliseconds
		 */
		TaskState(uint64 sleep);
};

class Task {
	friend TaskProcessor;
	protected:
		TaskProcessor *m_taskProcessor;
	private:
		bool m_autoDelete;
	public:
		/**
		 * Constructor
		 */
		Task();

		virtual ~Task();

		virtual TaskState run(Event) = 0;

	protected:
		/**
		 * Sets whether or not the Task should be auto-deleted when it completes.
		 * @param autoDelete whether or not the Task should be auto-deleted when it completes
		 */
		void setAutoDelete(bool autoDelete);

		/**
		 * Indicates whether or not the Task should be auto-deleted when it completes.
		 * @return value indicating whether or not the Task should be auto-deleted when it completes
		 */
		bool autoDelete();
};

class FunctionTask: public Task {
	private:
		std::function<TaskState(Event)> m_task;

	public:
		/**
		 * Constructor
		 * @param func the function to be called as Task::run(Event)
		 */
		FunctionTask(std::function<TaskState(Event)> func);

		TaskState run(Event);
};

class Mutex {
	public:
#ifdef WITH_SDL
		SDL_mutex *m_mutex;
#endif

		/**
		 * Constructor
		 */
		Mutex();

		/**
		 * Destructor
		 */
		~Mutex();

		/**
		 * Locks this Mutex.
		 * @return 0 on success
		 */
		int lock();

		/**
		 * Unlocks this Mutex.
		 * @return 0 on success
		 */
		int unlock();

	private:
		Mutex(const Mutex&);
		Mutex &operator=(const Mutex&);
};

class Semaphore {
	public:
		class Post {
			friend Semaphore;
			friend TaskProcessor;

			protected:
				/**
				 * Used to specify the Task that received a message.
				 */
				Task *m_task;
				EventType m_reason;

			public:
				/**
				 * Constructor
				 * @param reason optional reason parameter, defaults to SemaphorePost
				 */
				Post(EventType reason = SemaphorePost);

				/**
				 *	Gets the reason for the wake up.
				 * @return the reason for the post
				 */
				EventType reason();

			protected:
				/**
				 * Gets the Task that the wake up is for.
				 * @return the Task that the wake up is for
				 */
				Task *task();
		};

	private:
		std::queue<Post> m_posts;
#ifdef WITH_SDL
		SDL_sem *m_semaphore;
#endif
		Mutex m_mutex;

	public:
		/**
		 * Constructor
		 */
		Semaphore();

		/**
		 * Destructor
		 */
		~Semaphore();

		/**
		 * Waits until there is a post to process.
		 * @return a Post with the reason for the wake up
		 */
		Post wait();

		/**
		 * Waits until there is a post to process or the given timeout has expired.
		 * @param timeout the desired timeout period in milliseconds
		 * @return a Post with the reason for the wake up
		 */
		Post wait(uint64 timeout);

		/**
		 * Posts the the Semaphore to wake up.
		 * @param wakeup optional parameter used to specify the reason for the wake up
		 */
		void post(Post wakeup = Post());

		/**
		 * Gets the oldest post available.
		 * @return the oldest post available
		 */
		Post popPost();

		/**
		 * Indicates whether or not there are any pending posts.
		 * @return indicator of whether or not there are any pending posts
		 */
		bool hasPosts();

	// disallow copying
	private:
		Semaphore(const Semaphore&);
		Semaphore &operator=(const Semaphore&);
};

void startThread(std::function<void()> f);

void sleep(uint64 ms);


template <typename T>
class Channel {
	private:
		Semaphore *m_sem;
		Mutex m_mutex;
		std::queue<T> m_msgs;

	public:
		/**
		 * Constructor
		 * @param sem the Semaphore for this Channel to listen on
		 */
		Channel(Semaphore *sem = new Semaphore()) {
			m_sem = new Semaphore();
		}

		/**
		 * Destructor
		 */
		~Channel() {
			if (m_sem) {
				delete m_sem;
			}
		}

	private:
		/**
		 *
		 * @return indicates whether or not a message was retrieved
		 */
		bool getMessage(T &msg) {
			bool retval = false;
			m_mutex.lock();
			if (!m_msgs.empty) {
				msg = m_msgs.front();
				m_msgs.pop();
				retval = true;
			}
			m_mutex.unlock();
			return retval;
		}

	public:
		/**
		 * Waits until a message is received, then discards the message.
		 * @return reason for the wake up
		 */
		EventType read() {
			auto reason = m_sem->wait().reason();
			if (reason == ChannelMessage) {
				m_mutex.lock();
				m_msgs.pop();
				m_mutex.unlock();
			}
			return reason;
		}

		/**
		 * Read will wait until a message is received, then write it to the given
		 * destination.
		 * @param msg reference to the message to write to
		 * @return reason for the wake up
		 */
		EventType read(T &msg) {
			while (1) {
				auto reason = m_sem->wait().reason();
				if (reason == ChannelMessage) {
					if (getMessage(msg)) {
						return reason;
					}
				} else {
					return reason;
				}
			}
		}

		/**
		 * Read will wait until a message is received, then write it to the given
		 * destination.
		 * @param msg reference to the message to write to
		 * @param timeout timeout duration to give up on
		 * @return reason for the wake up
		 */
		EventType read(T &msg, uint64 timeout) {
			auto startTime = core::time();
			auto currentTimeout = timeout;
			while (1) {
				auto reason = m_sem->wait(currentTimeout).reason();
				if (reason == ChannelMessage) {
					if (getMessage(msg)) {
						return reason;
					}
				} else {
					return reason;
				}
				currentTimeout = timeout - (time() - startTime);
			}
		}

		/**
		 * Writes the given message to the message queue and wakes up any thread
		 * waiting for a message.
		 * @param msg the message to write
		 */
		void write(T msg) {
			m_mutex.lock();
			m_msgs.push(msg);
			m_mutex.unlock();
			m_sem->post(ChannelMessage);
		}

	// disallow copying
	private:
		Channel(const Channel&);
		Channel &operator=(const Channel&);
};

class TaskProcessor {
	private:
		bool m_running;
		Mutex m_mutex;
		Semaphore m_sem;
		Channel<bool> m_done;
		std::vector<std::pair<Task*, uint64>> m_schedule;

	public:
		/**
		 * Adds the given Task to this TaskProcessor.
		 * @param task the lambda for Task to add to this TaskProcessor
		 * @param state the initial state of the Task, defaults to Running
		 */
		void addTask(std::function<TaskState(Event)> task, TaskState state = TaskState::Running);

		/**
		 * Adds the given Task to this TaskProcessor.
		 * @param task the Task to add to this TaskProcessor
		 * @param state the initial state of the Task, defaults to Running
		 */
		void addTask(Task *task, TaskState state = TaskState::Running);

		/**
		 * Starts the thread for this TaskProcessor.
		 */
		void start();

		/**
		 * Stop the thread for this TaskProcessor.
		 */
		void stop();

		/**
		 * Blocks until this TaskProcessor has stopped.
		 */
		void done();

	private:
		/**
		 * Pops the currently active Task if there is one. If there is no active
		 * Task, it returns null.
		 * @return the currently active Task, if there is one, null otherwise
		 */
		Task *popActiveTask();

		/**
		 * Gets the next Task to execute.
		 * @return the next task scheduled to execute
		 */
		std::pair<Task*, uint64> nextTask();

		/**
		 * Sets the state of the task and schedules it appropriately.
		 * @param task the Task to update the state of
		 * @param state the state to give the task
		 */
		void processTaskState(Task *task, TaskState state);

		/**
		 * Runs the given Task.
		 * @param task the Task to run
		 * @param event the Event that this Task is waking up to process
		 */
		void runTask(Task *task, Event event);
};

}
}

#endif
