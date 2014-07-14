#ifndef _KEY_EXTRACTOR_HPP
#define _KEY_EXTRACTOR_HPP

#include "_dbg.hpp"

// Tmp Ami - why is there only one key extractor in the file? Consider moving it somewhere"

template<typename T>
struct _KeyExtractor
{
    typedef T KeyType;
    typedef T ValueType;

    inline static const T &
    extract(const T & t)
    {
        return t;    
    }
};

#endif // #ifndef _KEY_EXTRACTOR_HPP

