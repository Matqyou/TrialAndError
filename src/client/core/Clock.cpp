//
// Created by T450s on 26/12/2022.
//

#include "Clock.h"
#include <stdexcept>

Clock::Clock() {
    Framerate = 1.0;
	InitialFramerate = 60.0;
	IdleFramerate = 10.0;
	NanoDelay = 1e9;
    TotalNanoElapsed = 0;
    NanoElapsed = 0;
    VeryBeginning = HNow();
    Beginning = VeryBeginning;
    Ticks = 0;

//    SetFramerate(framerate);
	ResetFramerate();
}

void Clock::Tick() {
    HPoint Ending = HNow();
    NanoElapsed = MeasureNanoseconds(Beginning, Ending);
    while (NanoElapsed < NanoDelay) {
        Ending = HNow();
        NanoElapsed = MeasureNanoseconds(Beginning, Ending);
    }
    TotalNanoElapsed = MeasureNanoseconds(VeryBeginning, Ending);
    Beginning = HNow();
    Ticks++;
}

void Clock::ResetFramerate()
{
	SetFramerate(InitialFramerate);
}

void Clock::SetIdleFramerate()
{
	SetFramerate(IdleFramerate);
}

void Clock::SetFramerate(double framerate) {
    if (framerate <= 0.0)
        throw std::invalid_argument("Invalid clock framerate was set");

    Framerate = framerate;
    NanoDelay = (long long)(1e9 / framerate);
}

void Clock::SetNewBaseFramerate(double new_base)
{
	InitialFramerate = new_base;
}

void Clock::SetNewIdleFramerate(double new_idle)
{
	IdleFramerate = new_idle;
}

NonBlockingClock::NonBlockingClock()
    : Clock() {

}

bool NonBlockingClock::TimePassed() {
    HPoint Ending = HNow();
    NanoElapsed = MeasureNanoseconds(Beginning, Ending);
    bool passed = (NanoElapsed >= NanoDelay);
    if (passed) {
        TotalNanoElapsed = MeasureNanoseconds(VeryBeginning, Ending);
        Beginning = HNow();
        Ticks++;
    }
    return passed;
}