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
        // Первый элемент - не имя файла оригинального windres, значит, нам нужно добавить 'windres' в вектор аргументов, без пути
        // иначе - первый элемент оставляем, как есть
        windresExe = "windres";
        #if defined(_WIN32) || defined(WIN32)
        windresExe += ".exe"; // чтобы не запустился какой-нибудь левый батник, добавляем явно расширение
        #endif

        argsVec.insert(argsVec.begin(), windresExe);
    }

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

