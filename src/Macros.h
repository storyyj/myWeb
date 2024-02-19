//定义一些宏,使类不能被拷贝和移动，但是这个cname是怎么用网上没查阅到有关信息
#pragma once
#include <cassert>
#include <stdexcept>
#define OS_LINUX

// Macros to disable copying and moving
#define DISALLOW_COPY(cname)                             \
  cname(const cname &) = delete;            /* NOLINT */ \
  cname &operator=(const cname &) = delete; /* NOLINT */

#define DISALLOW_MOVE(cname)                        \
  cname(cname &&) = delete;            /* NOLINT */ \
  cname &operator=(cname &&) = delete; /* NOLINT */

#define DISALLOW_COPY_AND_MOVE(cname) \
  DISALLOW_COPY(cname);               \
  DISALLOW_MOVE(cname);

/*表示第二个message只有在第一个expr表达式为0时才会被打印出来。
assert应该放在这种头文件守卫里才比较合理，需要调试的打印assert信息的时候才用
#ifndef NDEBUG
#define ASSERT...
#endif
*/
#define ASSERT(expr, message) assert((expr) && (message))

#define UNREACHABLE(message) throw std::logic_error(message)