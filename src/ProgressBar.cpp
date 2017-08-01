#include "ProgressBar.hpp"
#include "backend.hpp"

/**
 * Get width of console window based on character width.
 *
 * @return Length of console window.
 */
int ProgressBar::GetBarLength() const
{
    int bar_length = static_cast<int>((95 - desc_width - CHARACTER_WIDTH_PERCENTAGE) / 2.0);

    return bar_length;
}

/**
 * Progress the bar. And write back the progressbar string to Qt.
 *
 * @param idx_ Amount to progress.
 * @param backend Qt backend.
 */
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
    catch (uint64_t e)
    {
        std::cerr << "PROGRESS_BAR_EXCEPTION: _idx (" << e << ") went out of bounds, greater than n (" << n << ")." << std::endl << std::flush;
    }
}
