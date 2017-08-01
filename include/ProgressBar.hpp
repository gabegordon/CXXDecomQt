#pragma once

#include <iostream>
#include <iomanip>
#include <cstring>

class BackEnd;

#define TOTAL_PERCENTAGE 100.0
#define CHARACTER_WIDTH_PERCENTAGE 4

class ProgressBar {
public:
    ProgressBar(const uint64_t& n_, const char* description_) :
        n{n_},
        desc_width{0},
        frequency_update{n_/100},
        tenth{n_/100},
        description{description_},
        firstRun{true}
    {
        unit_bar = "=";
        unit_space = " ";
        desc_width = std::strlen(description);
    }

    void Progressed(const uint64_t& idx_, BackEnd* backend);

private:
    uint64_t n;
    uint64_t desc_width;
    uint64_t frequency_update;
    uint64_t tenth;
    const char *description;
    const char *unit_bar;
    const char *unit_space;
    bool firstRun;
    int GetBarLength() const;
};
