#pragma once

#include <iostream>
#include <cstring>
#include <chrono>
#include <algorithm>
#include <vector>
#include <sstream>

class Timer
{
private:
    struct scope
    {
        const char* ID;
        int64_t runs = 1;
        int64_t totalTime = 0;

        float percent;
    };

    std::vector<scope*> list;

public:
    struct perf
    {
        const char* ID;
        Timer* t;

        perf(Timer* t, const char* id) : t(t), ID(id)
        {
            startTimepoint = std::chrono::high_resolution_clock::now();
        }
        ~perf()
        {
            auto end = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count();
            auto start = std::chrono::time_point_cast<std::chrono::microseconds>(startTimepoint).time_since_epoch().count();

            for (int i = 0; i < t->list.size(); ++i)
            {
                if (ID == t->list[i]->ID)
                {
                    ++t->list[i]->runs;
                    t->list[i]->totalTime += end - start;
                    return;
                }
            }
            scope* s = new scope;
            s->ID = ID;
            s->totalTime += end - start;
            t->list.push_back(s);
        }
    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> startTimepoint;
    };

    void print(std::vector<const char*> hide)
    {
        printf("                 ---   Performance   ---                 \n");
        printf("   %     Time(us)       Total(us)       Runs           ID\n");
        
        std::vector<scope*> scopes;
        scopes.reserve(list.size());
        for (int i = 0; i < list.size(); )
        {
            for (int h = 0; h < hide.size(); ++h)
            {
                if (strcmp(hide[h], list[i]->ID) == 0)
                {
                    goto Continue;
                }
            }
            scopes.push_back(list[i]);
        Continue:
            ++i;
        }

        int64_t overallTime = 0LL;
        for (int i = 0; i < scopes.size(); ++i)
        {
            overallTime += scopes[i]->totalTime;
        }
        for (int i = 0; i < scopes.size(); ++i)
        {
            scopes[i]->percent = (float)(scopes[i]->totalTime * 10000LL / overallTime) / 100.0f;
        }
        std::sort(scopes.begin(), scopes.end(), [](const scope* a, const scope* b) { return a->percent > b->percent; });

        for (int i = 0; i < scopes.size(); ++i)
        {
            std::stringstream ss;
            ss << ceilf(scopes[i]->percent * 100.0) / 100.0 << "%";

            printf("%-9s%-15lld%-16lld%-15lld%s\n", ss.str().c_str(), scopes[i]->totalTime / scopes[i]->runs, scopes[i]->totalTime, scopes[i]->runs, scopes[i]->ID);
        }
    }

    ~Timer()
    {
        for (int i = 0; i < list.size(); ++i)
        {
            delete list[i];
        }
    }
};