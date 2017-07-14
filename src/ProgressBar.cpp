#include "ProgressBar.h"
#include "backend.h"

void ProgressBar::SetStyle(const char* unit_bar_, const char* unit_space_)
{
    unit_bar = unit_bar_;
    unit_space = unit_space_;
}

int ProgressBar::GetBarLength() const
{
    // get console width and according adjust the length of the progress bar

    int bar_length = static_cast<int>((95 - desc_width - CHARACTER_WIDTH_PERCENTAGE) / 2.0);

    return bar_length;
}

void ProgressBar::Progressed(const uint64_t& idx_, BackEnd* backend)
{
    try
    {
        if (idx_ > n) throw idx_;
        // determines whether to update the progress bar from frequency_update
        if ((idx_ !=n) && (idx_ < frequency_update) && !firstRun) return;
        frequency_update += tenth;
        firstRun = false;
        // calculate the size of the progress bar
        int bar_size = GetBarLength();

        // calculate percentage of progress
        double progress_percent = idx_* TOTAL_PERCENTAGE / n;

        // calculate the percentage value of a unit bar
        double percent_per_unit_bar = TOTAL_PERCENTAGE / bar_size;

        // display progress bar
        std::stringstream out;
        out << " " << std::right << std::setw(12) << description << " [";

        for (int bar_length = 0; bar_length <= bar_size - 1; ++bar_length)
        {
            if (bar_length*percent_per_unit_bar < progress_percent)
            {
                out << unit_bar;
            }
            else
            {
                out << unit_space;
            }
        }

        out << "]" << std::setw(CHARACTER_WIDTH_PERCENTAGE + 1) << std::setprecision(1) << std::fixed << progress_percent << "%\r" << std::flush;
        backend->setProgress(out.str());
    }
    catch (unsigned long e)
    {
        std::cerr << "PROGRESS_BAR_EXCEPTION: _idx (" << e << ") went out of bounds, greater than n (" << n << ")." << std::endl << std::flush;
    }
}
