#include <vector>
#include <string>
#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

namespace getFiles
{
    static std::vector<std::string> filesInFolder(const std::string& folder, const std::string& type)
    {
        std::vector<std::string> names;
#ifdef _WIN32
        std::string search_path = folder + type;
        WIN32_FIND_DATAA fd;
        void* hFind = FindFirstFileA(search_path.c_str(), &fd);
        if(hFind != INVALID_HANDLE_VALUE) {
            do {
                // read all (real) files in current folder
                // , delete '!' read other 2 default folder . and ..
                if(! (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) {
                    std::string ws(fd.cFileName);
                    names.push_back(folder + "/" + ws);
                }
            }while(::FindNextFileA(hFind, &fd));
            ::FindClose(hFind);
        }
        return names;
#else
        DIR *dir;
        class dirent *ent;
        class stat st;

        dir = opendir(folder.c_str());
        while ((ent = readdir(dir)) != NULL) {
            const std::string file_name = ent->d_name;
            const std::string full_file_name = folder + "/" + file_name;

            if (file_name.back() != type.back())
                continue;

            if (stat(full_file_name.c_str(), &st) == -1)
                continue;

            const bool is_directory = (st.st_mode & S_IFDIR) != 0;

            if (is_directory)
                continue;

            names.push_back(full_file_name);
        }
        closedir(dir);
        return names;
#endif
    }
}
