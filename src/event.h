/*
 * acStarter - A simple server manager for Assetto Corsa.
 * Copyright (c) 2014-2017 Turncoat Tony
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <functional>
#include <queue>
#include <chrono>
#ifdef ACS_LINUX
#   include <sys/time.h>
#elif defined(ACS_WIN32) // We have to include <Windows.h> before this file.
#   include <time.h>  // for `time_t` and `struct timeval`
#endif

namespace Events {
    struct Event {
        typedef std::function<void()> callback_type;
        typedef std::chrono::time_point<std::chrono::system_clock> time_type;

        Event(const callback_type &callback, const time_type &when) : callback_(callback), when_(when) {
        }
        void operator()() const { callback_(); }

        callback_type       callback_;
        time_type           when_;
    };

    struct event_less {
        bool operator()(const Event &first, const Event &second) {
            return (second.when_ < first.when_);
        }
    };

    std::priority_queue<Event, std::vector<Event>, event_less> event_queue;

    void add(const Event::callback_type &callback, const time_t &when) {
        auto real_when = std::chrono::system_clock::from_time_t(when);

        event_queue.emplace(callback, real_when);
    }

    void add(const Event::callback_type &callback, const timeval &when) {
        auto real_when = std::chrono::system_clock::from_time_t(when.tv_sec) + std::chrono::microseconds(when.tv_usec);

        event_queue.emplace(callback, real_when);
    }

    void add(const Event::callback_type &callback, const std::chrono::time_point<std::chrono::system_clock> &when) {
        event_queue.emplace(callback, when);
    }

    void timer() {
        Event::time_type now = std::chrono::system_clock::now();

        while (!event_queue.empty() && (event_queue.top().when_ < now)) {
            event_queue.top()();
            event_queue.pop();
        }
    }
}
