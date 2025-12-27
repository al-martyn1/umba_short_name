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

inline
bool testWindresExist(std::string path, const std::string &windres)
{
    path.append(windres);
    #if defined(_WIN32) || defined(WIN32)
    path += ".exe"; // чтобы не запустился какой-нибудь левый батник, добавляем явно расширение
    #endif

    return umba::shortnames::isPathDirOrFile(path); // можем обосраться, если это каталог, но это было бы очень странно
}

inline
std::string findOriginalRenamedWindres(const std::string &path) // путь должен заканчиваться разделителем
{
    std::vector<std::string> prefixes = { "mingw-gcc-original"
                                        , "gcc-original"
                                        , "gcc"
                                        , "mingw-gnu-original"
                                        , "gnu-original"
                                        , "gnu"
                                        , "mingw-original"
                                        , "mingw-gcc"
                                        , "mingw-gnu"
                                        , "mingw"
                                        };

    for(auto prefix: prefixes)
    {
        if (testWindresExist(path, prefix + "-windres"))
        {
            std::string windres = path + prefix + "-windres";
            #if defined(_WIN32) || defined(WIN32)
            windres += ".exe"; // чтобы не запустился какой-нибудь левый батник, добавляем явно расширение
            #endif
            return windres;
        }
    }

    return std::string();
}


int main(int argc, char* argv[])
{

    std::vector<std::string> argsVec;

    for(int i=1; i<argc; ++i)
        argsVec.push_back(argv[i]);

    std::string orgWindres = findOriginalRenamedWindres(umba::shortnames::getParentPath(argv[0], true  /* keepSep */ ));
    if (orgWindres.empty())
    {
        std::cerr << "Original windres not found\n";
        return 1;
    }

    argsVec.insert(argsVec.begin(), orgWindres);


    // Для всех аргументов пытаемся извлечь родительский каталог
    // Это делается для того, что аргументом может быть имя файла, который является выходным и может не существовать
    // А путь будет создан системой сборки
    // Если получилось извлечь родительский путь, то проверяем его существование
    // Не работает для UNC имён
    // Если путь существует, то пытаемся сконвертировать исходный аргумент в короткий 8.3 формат.
    // Если встречается какая-то часть, которая не конвертируется, то весь остаток пути собирается без конвертирования
    // Под системами не Win32 никакая модификация не производится
    // В Win32 системах короткие пути могут быть откючены, тогда это не работает.
    // Но по умолчанию короткие пути включены.
    for(std::vector<std::string>::iterator it=std::next(argsVec.begin()); it!=argsVec.end(); ++it)
    {
        std::string parentPath = umba::shortnames::getParentPath(*it);

        if (parentPath.empty())
            continue;

        if (!umba::shortnames::isPathDirOrFile(parentPath))
            continue;
    
        *it = umba::shortnames::convertToShort(*it);
    }


    std::vector<const char *> execArgs;

    for(std::vector<std::string>::iterator it=argsVec.begin(); it!=argsVec.end(); ++it)
    {
        execArgs.push_back(it->c_str());
    }

    execArgs.push_back(0);


    #if defined(_WIN32) || defined(WIN32)

    return (int)_execvp(orgWindres.c_str(), execArgs.data());

    #else

    return (int)execvp(orgWindres.c_str(), execArgs.data());

    #endif
}

