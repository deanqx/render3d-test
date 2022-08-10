#include <iostream>
#include <chrono>
#include <thread>
#include "timer.h"

int main()
{
    Timer* t = new Timer;

    for (size_t i = 0; i < 2; ++i)
    {
        // current = nullptr
        {
            Timer::perf depth1(t, "Main1"); // header = current
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            {
                Timer::perf depth2(t, "Sub1"); // header = current
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                {
                    Timer::perf depth3(t, "SubSub1"); // header = current
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    {
                        Timer::perf depth4(t, "SubSubSub1"); // header = current
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    } // current = header
                } // current = header
                {
                    Timer::perf depth3(t, "SubSub2"); // header = current
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                } // current = header
            } // current = header
            {
                Timer::perf depth2(t, "Sub2"); // header = current
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            } // current = header
        } // current = header
        {
            Timer::perf depth1(t, "Main2"); // header = current
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        } // current = header
        // current = Absolute Header
    }

    t->print({});
}
