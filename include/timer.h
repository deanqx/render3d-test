#pragma once

#include <iostream>
#include <string>
#include <chrono>
#include <algorithm>
#include <vector>
#include <sstream>

class Timer
{
    std::vector<std::string> hide;
    struct scope
    {
        Timer* t;
        scope* header;
        std::vector<scope*> sub;
        int layer = 0;

        const std::string ID = "";
        int64_t totalTime = 0;
        int64_t runs = 0;

        float percent;

        scope(Timer* t, const std::string ID, scope* header) : t(t), ID(ID), header(header) {}
        ~scope()
        {
            for (int i = 0; i < sub.size(); ++i)
            {
                delete sub[i];
            }
        }

        // std::string getTree()
        // {
        //     std::stringstream id;
        //     if (header != nullptr)
        //         id << header->getTree() << "/";
        //     id << ID;

        //     return std::move(id.str());
        // }
        void print()
        {
            if (sub.size() > 0)
            {
                std::vector<scope*> temp;
                temp.reserve(sub.size());
                for (int i = 0; i < sub.size();)
                {
                    for (int h = 0; h < t->hide.size(); ++h)
                    {
                        if (t->hide[h] == sub[i]->ID)
                        {
                            goto Continue;
                        }
                    }
                    temp.push_back(sub[i]);

                Continue:
                    ++i;
                }
                sub.swap(temp);

                int64_t overallTime = 0LL;
                for (int i = 0; i < sub.size(); ++i)
                {
                    overallTime += sub[i]->totalTime;
                }
                for (int i = 0; i < sub.size(); ++i)
                {
                    sub[i]->percent = (float)(sub[i]->totalTime * 10000LL / overallTime) * 0.01f;
                }
                std::sort(sub.begin(), sub.end(), [](const scope* a, const scope* b) { return a->percent > b->percent; });

                for (int i = 0; i < sub.size(); ++i)
                {
                    // std::stringstream tree;
                    // for (int l = 2; l < sub[i]->layer; ++l)
                    //     tree << (char)179;
                    // if (sub[i]->layer > 1)
                    //     tree << (char)195;
                    std::stringstream tree;
                    for (int l = 2; l < sub[i]->layer; ++l)
                        tree << ' ';
                    if (sub[i]->layer > 1)
                        tree << (char)192;
                    

                    // tree << ceilf(sub[i]->percent * 100.0f) * 0.01f << '%';
                    tree << ceilf(sub[i]->percent * 100.0f) * 0.01f;

                    std::stringstream spaces;
                    for (int l = 1; l < sub[i]->layer; ++l)
                        spaces << "  ";
                    spaces << sub[i]->ID;

                    printf("%-15s%-15lld%-16lld%-15lld%s\n", tree.str().c_str(), sub[i]->totalTime / sub[i]->runs, sub[i]->totalTime, sub[i]->runs, spaces.str().c_str());
                    sub[i]->print();
                }
            }
        }
    };

    scope* current = new scope(this, "", nullptr); // (= Main header)

public:
    class perf
    {
        std::chrono::time_point<std::chrono::high_resolution_clock> startTimepoint;
        Timer* t;
        scope* local = nullptr;

    public:
        perf(Timer*& t, const std::string id) : t(t)
        {
            for (int i = 0; i < t->current->sub.size(); ++i)
            {
                if (t->current->sub[i]->ID == id)
                {
                    local = t->current->sub[i];
                    t->current = local;
                    goto Continue;
                }
            }

            local = new scope(t, id, t->current);
            t->current = local;
            local->layer = local->header->layer + 1;
            local->header->sub.push_back(local);

        Continue:
            startTimepoint = std::chrono::high_resolution_clock::now();
        }
        ~perf()
        {
            auto end = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count();
            auto start = std::chrono::time_point_cast<std::chrono::microseconds>(startTimepoint).time_since_epoch().count();

            local->totalTime += end - start;
            ++local->runs;
            t->current = local->header;
        }
    };

    void print(std::vector<std::string> hide)
    {
        printf("                 ---   Performance   ---                 \n");
        printf("   %           Time(us)       Total(us)       Runs           ID\n");

        current->print();
    }

    ~Timer()
    {
        delete current;
    }
};