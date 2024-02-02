#ifndef _ASYNC_ACTIONS_H_
#define _ASYNC_ACTIONS_H_

#include <functional>

namespace AsyncActions
{
    void execute(std::function<void(void)> func, std::function<void(void)> done);
}

#endif