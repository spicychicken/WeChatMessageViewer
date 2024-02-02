#include "AsyncActions.h"
#include <future>

void AsyncActions::execute(std::function<void(void)> func, std::function<void(void)> done)
{
    // If the std::future obtained from std::async is not moved from or bound to a reference, 
    // the destructor of the std::future will block at the end of the full expression 
    // until the asynchronous operation completes,
    // essentially making code such as the following synchronous:
    // can not use std::async here
    std::thread([](auto func, auto done) {
        func();done();
    }, func, done).detach();
}