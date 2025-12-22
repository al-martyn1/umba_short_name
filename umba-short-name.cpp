#include <iostream>
#include <string>
#include <vector>


#if defined(_WIN32) || defined(WIN32)

    #include <windows.h>

#else

    #include <unistd.h>

#endif


using std::cout;
using std::cerr;


int main(int argc, char* argv[])
{
    std::string inputPath;

    if (argc>1)
    {
        if (argc==2 && std::string(argv[1])=="-c")
        {
            // Use current dir
            #if defined(_WIN32) || defined(WIN32)

            char buf[MAX_PATH+1] = {0};
            DWORD dwRes = GetCurrentDirectoryA( sizeof(buf), &buf[0] );
            if (dwRes)
            {
                inputPath.assign(&buf[0], std::size_t(dwRes));
            }

            #else

            char buf[2048];

            auto pRes = getcwd(&buf[0], sizeof(buf));
            if (pRes)
                inputPath = pRes;

            #endif
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

    // std::cout << "Input Path: " << inputPath << "\n";

#if defined(_WIN32) || defined(WIN32)

    std::vector<std::string> parts;
    std::string tmpPart;

    for(auto ch: inputPath)
    {
        if (ch=='/' || ch=='\\')
        {
            if (!tmpPart.empty())
                parts.push_back(tmpPart);
            tmpPart.clear();
        }
        else
        {
            tmpPart.append(1, ch);
        }
    }

    if (!tmpPart.empty())
        parts.push_back(tmpPart);

    std::string resFullPath;
    std::string resShortPath;
    bool bFailed = false;

    for(auto p: parts)
    {
        // cout << p << "\n";
        if (!resFullPath.empty())
            resFullPath.append(1, '\\');
        resFullPath.append(p);

        if (!bFailed)
        {
            char buf[MAX_PATH] = {0};
    
            DWORD dwRes = GetShortPathNameA(resFullPath.c_str(), &buf[0], sizeof(buf));
            if (!dwRes)
            {
                // cout << "failed\n";
                if (!resShortPath.empty())
                    resShortPath.append(1, '\\');
                resShortPath.append(p);
                bFailed = true;
            }
            else
            {
                resShortPath.assign(&buf[0], std::size_t(dwRes));
                //cout << resShortPath << "\n";
            }
        }
        else // Был сбой, дальше пойдут одни сбои
        {
            if (!resShortPath.empty())
                resShortPath.append(1, '\\');
            resShortPath.append(p);
        }

        // cout << resShortPath << "\n";
    }

    cout << resShortPath << "\n";

#else

    std::cout << inputPath << "\n";

#endif
    
    return 0;
}