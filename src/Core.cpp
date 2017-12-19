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

// Used to figure out our platform
#include "conf.h"

#include <cstdlib>
#include <iostream>
#include <thread>

#ifdef ACS_LINUX
#   include <getopt.h>
#elif defined(ACS_WIN32)
#   include <time.h>
#   include "getopt.h"
#endif
#include "Event.h"

void foo()
{
    std::cout << "hello from foo\n";
}

void done()
{
    std::cout << "Done!\n";
}

struct bar
{
    void hello()
        { std::cout << "Hello from bar::hello\n";
		bar b;
        auto now = std::chrono::system_clock::now();
        Events::add(std::bind(&bar::hello, b), now + std::chrono::seconds(6));}
};

int main(int argc, char *argv[]) {
    int next_option, use_drift = 0, use_race = 0, use_practice = 0;

    // We should have Usage: %s and then pass argv[0] to get executable name
    const char *usage = "Usage: acStarter [-h] [-d] [-p] [-r] [-q] [-v]\n"
			"  -h    --help               Display this usage information\n"
			"  -d    --drift              Read from drift configuration\n"
			"  -p    --practice           Read from practice configuration\n"
			"  -r    --race               Read from race configuration (default)\n"
			"  -q    --quiet              Don't display logging to console\n"
			"  -v    --version            Display version\n";

    const struct option long_options[] = {
    	{ "help",       0, NULL,   'h' },
    	{ "drift",      0, NULL,   'd' },
    	{ "practice",   0, NULL,   'p' },
    	{ "race",       0, NULL,   'r' },
    	{ "quiet",      0, NULL,   'q' },
    	{ "version",    0, NULL,   'v' },
    	{ 0,            0, 0,       0  }
    };

    while ((next_option = getopt_long(argc, argv, "hdprqv", long_options, NULL)) != -1) {
        switch(next_option) {
            case 'h':
                std::cout << usage;
                std::exit(EXIT_SUCCESS);
            break;

            case 'v':
            break;

            default:
                std::cout << usage;
                std::exit(EXIT_SUCCESS);
            break;
        }
    }

    auto now = std::chrono::system_clock::now();
    bar b;

    Events::add(foo, now + std::chrono::seconds(2));
    Events::add(std::bind(&bar::hello, b), now + std::chrono::seconds(4));
    Events::add(done, now + std::chrono::seconds(6));

    std::cout << "Starting stuff" << std::endl;

    while (true) {
        Events::timer();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return EXIT_SUCCESS;
}
