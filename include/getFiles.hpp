#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;
struct recursive_directory_range
{
    typedef fs::recursive_directory_iterator iterator;
recursive_directory_range(fs::path p) : p_(p) {}

    iterator begin() { return fs::recursive_directory_iterator(p_); }
    iterator end() { return fs::recursive_directory_iterator(); }

    fs::path p_;
};

namespace getFiles
{
    /**
     * Gets filenames of all files in a directory/subdirs with matching extension.
     *
     * @param folder Folder to look in
     * @param type File extension to look for
     * @return Vector of matching filenames
     */
    static std::vector<std::string> filesInDirectory(const std::string& folder, const std::string& type)
    {
        std::vector<std::string> names;
        fs::path rootfolder(folder);
        for (auto file : recursive_directory_range(rootfolder))
        {
            if (!fs::is_directory(file))
            {
                if (file.path().extension().string() == type)
                {
                    names.emplace_back(file.path().string());
                }
            }
        }
        return names;
    }
}
