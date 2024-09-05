#pragma once
#include <chrono>

class Timer{
private:
    std::chrono::high_resolution_clock::time_point m_time_point;
public:
using ms = std::chrono::duration<float, std::milli>;
    Timer(){
        m_time_point = std::chrono::high_resolution_clock::now();
    }
    ~Timer() {};

    float delta(){
        float delta = std::chrono::duration_cast<ms>(
            std::chrono::high_resolution_clock::now() - m_time_point
        ).count();
        m_time_point = std::chrono::high_resolution_clock::now();
        return delta / 1000.f;
    }
};