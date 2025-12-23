/*! \file
    \brief Утилита, формирующая кортокие имена файлов и каталогов
 */

#include "short_name.h"

#include <iostream>
#include <string>
#include <vector>


int main(int argc, char* argv[])
{
    std::string inputPath;

    if (argc>1)
    {
        if (argc==2 && std::string(argv[1])=="-c")
        {
            umba::shortnames::getCurrentDirectory(inputPath);
        }
        else
        {
            for(int i=1; i<argc; ++i)
            {
                if (!inputPath.empty())
                    inputPath.append(1, ' ');
                inputPath.append(argv[i]);
    
                // Если у нас несколько частей, выводим их в cerr
                if (argc>2)
                   std::cerr << argv[i] << "\n";
            }
        }
    }
    else
    {
        std::getline(std::cin, inputPath);
    }

    std::string resShortPath = umba::shortnames::convertToShort(inputPath);
    std::cout << resShortPath << "\n";
    
    return 0;
}