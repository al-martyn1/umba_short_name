/*! \file
    \brief Утилита, формирующая кортокие имена файлов и каталогов
 */

#include "short_name.h"

#include <iostream>
#include <string>
#include <vector>

//
#include <stdio.h>
#include <stdlib.h>
#include <process.h>


#if defined(_WIN32) || defined(WIN32)

#else

    #include <unistd.h>

#endif



int main(int argc, char* argv[])
{

#if 0
    if (argc>1)
    {
        if (!umba::shortnames::isPathDirOrFile(argv[1]))
        {
            std::cout << argv[1] << " is not a file nor directory\n";

            #if defined(_WIN32) || defined(WIN32)
            std::cout << "Error: " << GetLastError() << "\n";
            #endif
        }
        else
        {
            std::cout << argv[1] << " is a file or directory\n";
        }
    }
#endif

    std::vector<std::string> argsVec;

    for(int i=1; i<argc; ++i)
        argsVec.push_back(argv[i]);

    if (argsVec.empty())
    {
        std::cerr << "missing arguments\n";
        return 1;
    }

    std::string windresExe = umba::shortnames::toLower(argsVec[0]);
    auto pos = windresExe.find_last_of("\\/");
    if (pos!=windresExe.npos)
    {
        windresExe = std::string(windresExe, pos+1);
    }

    if (windresExe!="windres" && windresExe!="windres.exe")
    {
        // Первый элемент - не имя файла оригинального windres, значит, нам нужно добавить его в вектор, без пути
        // иначе - не меняем
        windresExe = "windres";
        #if defined(_WIN32) || defined(WIN32)
        windresExe += ".exe";
        #endif

        argsVec.insert(argsVec.begin(), windresExe);
    }

    // std::cout << "WINDRES: " << windresExe << "\n";

    for(std::vector<std::string>::iterator it=std::next(argsVec.begin()); it!=argsVec.end(); ++it)
    {
        std::string parentPath = umba::shortnames::getParentPath(*it);

        if (parentPath.empty())
            continue;

        if (!umba::shortnames::isPathDirOrFile(parentPath))
            continue;
    
        *it = umba::shortnames::convertToShort(*it);
    }


    std::string exeName = argsVec[0];
    std::vector<const char *> execArgs;

    for(std::vector<std::string>::iterator it=argsVec.begin(); it!=argsVec.end(); ++it)
    {
        execArgs.push_back(it->c_str());
    }

    execArgs.push_back(0);


    #if defined(_WIN32) || defined(WIN32)

    return _execvp(exeName.c_str(), execArgs.data());

    #else

    return execvp(exeName.c_str(), execArgs.data());

    #endif
}

