/*! \file
    \brief Хелперы для коротких имён файлов и каталогов
 */

#pragma once

#if defined(_WIN32) || defined(WIN32)
    #include <windows.h>
    #include <shlwapi.h>
    // Shlwapi.lib
#else
    #include <unistd.h>
    #include <sys/stat.h>
#endif

#include <string>
#include <vector>
#include <iostream>

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
namespace umba {
namespace shortnames {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
inline
std::string getParentPath(const std::string &path, bool keepSep=false)
{
    auto pos = path.find_last_of("\\/");
    if (pos==path.npos)
        return std::string();
    return std::string(path, 0, pos + (keepSep?1u:0u));
}

//----------------------------------------------------------------------------
inline
std::vector<std::string> splitPath(const std::string &path, bool *pFoundLinuxSep=0)
{
    bool foundLinuxSep = false;

    std::vector<std::string> parts;
    std::string tmpPart;

    for(auto ch: path)
    {
        if (ch=='/' || ch=='\\')
        {
            if (ch=='/')
                foundLinuxSep = true;

            // if (tmpPart==".." &&  /* collapseDotDir &&  */ parts.size()>1)
            // {
            //     parts.pop_back();
            // }
            // else
            {
                if (!tmpPart.empty())
                    parts.push_back(tmpPart);
                tmpPart.clear();
            }
        }
        else
        {
            tmpPart.append(1, ch);
        }
    }

    if (!tmpPart.empty())
    {
        if (tmpPart==".." && parts.size()>1)
            parts.pop_back();
        else
            parts.push_back(tmpPart);
    }

    if (pFoundLinuxSep)
       *pFoundLinuxSep = foundLinuxSep;

    return parts;
}

//----------------------------------------------------------------------------
inline
std::vector<std::string> collapseParentRefs(const std::vector<std::string> &parts)
{
    std::vector<std::string> res;
    for(const auto &p : parts)
    {
        if (p==".." && res.size()>1)
            res.pop_back();
        else
            res.push_back(p);
    }

    return res;
}

//----------------------------------------------------------------------------
template<typename IterType>
std::string mergePath(IterType b, IterType e, char sep)
{
    std::string res;
    if (b==e)
       return res;

    res = *b;
    ++b;

    for(; b!=e; ++b)
    {
        res.append(1, sep);
        res.append(*b);
    }

    return res;
}

//----------------------------------------------------------------------------
inline
std::string mergePath(const std::vector<std::string> &parts, char sep)
{
    return mergePath(parts.begin(), parts.end(), sep);
}

//----------------------------------------------------------------------------
inline
std::string normalizePath(const std::string &p, char sep=0)
{
    bool foundLinuxSep = false;
    std::vector<std::string> parts = splitPath(p, &foundLinuxSep);
    
    if (sep==0 && foundLinuxSep)
        sep = '/';

    parts = collapseParentRefs(parts);

    return mergePath(parts, sep);
}

//----------------------------------------------------------------------------
inline
bool isPathDirOrFile(const std::string &p)
{
    #if defined(_WIN32) || defined(WIN32)

    // Путь, указанный соглашением об именовании (UNC), ограничен только файлом; То есть разрешено \server\share\file.
    // UNC-путь к серверу или общей папке сервера не разрешен; то есть \server или \server\share. 
    // Эта функция возвращает FALSE, если подключенный удаленный диск не работает.
    // DWORD GetLastError();

    std::string normalizedPath = normalizePath(p, '\\');
    BOOL bRes = ::PathFileExistsA(normalizedPath.c_str());
    // std::cerr << "!!!\n";
    // std::cerr << "p: " << p << "\n";
    // std::cerr << "n: " << normalizedPath << "\n";
    // std::cerr << "b: " << bRes << "\n";

    return bRes ? true : false;
    
    #else

    struct stat st;
    if (stat(path, &st) == -1)
        return false;

    return true;
    
    #endif

}

//----------------------------------------------------------------------------
inline
bool getCurrentDirectory(std::string &dirName)
{
    #if defined(_WIN32) || defined(WIN32)

    char buf[MAX_PATH+1] = {0};
    DWORD dwRes = GetCurrentDirectoryA( sizeof(buf), &buf[0] );
    if (dwRes)
    {
        dirName.assign(&buf[0], std::size_t(dwRes));
        return true;
    }

    return false;

    #else

    char buf[2048];

    auto pRes = getcwd(&buf[0], sizeof(buf));
    if (pRes)
    {
        dirName = pRes;
        return true;
    }

    return false;

    #endif

}

//----------------------------------------------------------------------------
inline
std::string convertToShort(const std::string &path)
{
    #if !(defined(_WIN32) || defined(WIN32))
    
    return path;

    #else

    // char sep = '\\';
    bool foundLinuxSep = false;
    std::vector<std::string> parts = splitPath(path, &foundLinuxSep);
    parts = collapseParentRefs(parts);
    char sep = foundLinuxSep ? '/' : '\\';

    std::string resFullPath;
    std::string resShortPath;
    //bool bFailed = false;

    //for(auto p: parts)
    for(auto it=parts.begin(); it!=parts.end(); ++it )
    {
        if (!resFullPath.empty())
            resFullPath.append(1, sep);
        resFullPath.append(*it);

        char buf[MAX_PATH] = {0};
    
        DWORD dwRes = GetShortPathNameA(resFullPath.c_str(), &buf[0], sizeof(buf));
        if (!dwRes)
        {
            // std::cerr << "!!! " << resShortPath << "\n";
            auto pathTail = mergePath(it, parts.end(), sep);
            if (!resShortPath.empty() && !pathTail.empty())
                resShortPath.append(1, sep);
            resShortPath.append(pathTail);
            return resShortPath;
        }
        else
        {
            resShortPath.assign(&buf[0], std::size_t(dwRes));
        }
        
    }

    return resShortPath;

    #endif
}

//----------------------------------------------------------------------------
inline
char charToUpper(char ch)
{
    if (ch>='a' && ch<='z')
        return ch-'a'+'A';
    return ch;
}

//----------------------------------------------------------------------------
inline
char charToLower(char ch)
{
    if (ch>='A' && ch<='Z')
        return ch-'A'+'a';
    return ch;
}

//----------------------------------------------------------------------------
inline
std::string toUpper(std::string str)
{
    for(auto &ch: str)
        ch = charToUpper(ch);
    return str;
}

//----------------------------------------------------------------------------
inline
std::string toLower(std::string str)
{
    for(auto &ch: str)
        ch = charToLower(ch);
    return str;
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

} // namespace shortnames
} // namespace umba

