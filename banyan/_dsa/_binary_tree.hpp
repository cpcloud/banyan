#ifndef _BINARY_TREE_HPP
#define _BINARY_TREE_HPP

#include <cstdlib>
#include <vector>
#include <utility>
#include <memory>
#ifdef BANYAN_DEBUG
#include <iosfwd>
#endif // #ifdef BANYAN_DEBUG

#include "_dbg.hpp"

struct _NullMetadata
{
#ifdef BANYAN_DEBUG
    void assert_valid() const
    {
        // Do nothing.    
    }

    void
    trace(std::ostream & ) const
    {
        // Do nothing.
    }
#endif // #ifdef BANYAN_DEBUG
};

template<
    typename T,
    class Key_Extractor,
    class Metadata,
    class LT>
class _BinaryTree :
    public Key_Extractor
{
public:
    typedef T ValueType;
    typedef Key_Extractor KeyExtractorT;
    typedef typename Key_Extractor::KeyType KeyType;
    typedef LT LTT;
    typedef Metadata MetadataT;

public:
    _BinaryTree();
    _BinaryTree(const LT & lt);
    _BinaryTree(const Metadata & md, const LT & lt);
        
    LT & 
    less_than();

    Metadata & 
    meta();

    virtual
    ~_BinaryTree() = 0;
    
protected:
    Metadata md;
    LT lt;
};

template<typename T, class Key_Extractor, class Metadata, class LT>
_BinaryTree<T, Key_Extractor, Metadata, LT>::_BinaryTree()
{
    // Do nothing.
}

template<typename T, class Key_Extractor, class Metadata, class LT>
_BinaryTree<T, Key_Extractor, Metadata, LT>::_BinaryTree(const LT & lt_) :
    lt(lt_)
{
    // Do nothing.
}

template<typename T, class Key_Extractor, class Metadata, class LT>
_BinaryTree<T, Key_Extractor, Metadata, LT>::_BinaryTree(const Metadata & md_, const LT & lt_) :
    md(md_),
    lt(lt_)
{
    // Do nothing.
}

template<typename T, class Key_Extractor, class Metadata, class LT>
LT & 
_BinaryTree<T, Key_Extractor, Metadata, LT>::less_than() 
{
    return lt;
}

template<typename T, class Key_Extractor, class Metadata, class LT>
Metadata & 
_BinaryTree<T, Key_Extractor, Metadata, LT>::meta() 
{
    return md;
}

template<typename T, class Key_Extractor, class Metadata, class LT>
_BinaryTree<T, Key_Extractor, Metadata, LT>::~_BinaryTree()
{
    // Do nothing.
}

#endif // #ifndef _BINARY_TREE_HPP

