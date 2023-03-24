//
// Created by T450s on 26/12/2022.
//

#ifndef AIFU_CLOCK_H
#define AIFU_CLOCK_H

#include <chrono>

#define MeasureNanoseconds(Start, End) std::chrono::duration_cast<std::chrono::nanoseconds>(End - Start).count();
#define HNow() std::chrono::high_resolution_clock::now()
typedef std::chrono::high_resolution_clock::time_point HPoint;

class Clock {
private:
    double Framerate;
    long long NanoDelay;

    long long TotalNanoElapsed;
    long long NanoElapsed;

    HPoint VeryBeginning;
    HPoint Beginning;

    int Ticks;

public:
    Clock(double framerate);
    double GetTimeElapsed() const { return (double)(NanoElapsed) / 1.0e9; }
    double GetTotalTimeElapsed() const { return (double)(TotalNanoElapsed) / 1.0e9; }
    int CurrentTick() const { return Ticks; }

    void SetFramerate(double framerate);

    void Tick();
};

#endif //AIFU_CLOCK_H
