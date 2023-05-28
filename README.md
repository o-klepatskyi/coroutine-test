# Coroutine test

## TODO
- [x] Get the Debugger working with VS Code
- [x] Make simpler primitives for coroutines
- [x] Add unit tests for the coros to verify that they are working correctly and returning actual values
- [x] Create a thread_pool interface which simply spawns new threads, and integrate it into the working examples
- [ ] Improve unit tests even further, the coro use cases must be more detailed
- [ ] Ensure sub-coroutines correctly deallocate their memory before coros are resumed
- [ ] C++20 coroutines: [“The bug is in the C++ standard, not GCC.”](https://www.reddit.com/r/cpp/comments/xvoqcd/c20_coroutines_the_bug_is_in_the_c_standard_not/). Replicate using online compiler explorer, and create a unit test for this case in your project.
- [ ] Start working on a thread_pool interface