#ifndef _OV_TREE_HPP
#define _OV_TREE_HPP

#include <exception>

#include "_binary_tree.hpp"

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif // #ifdef _MSC_VER

template<
    class Metadata,
    class Allocator>
class _OVTreeMetadataBase 
{
protected:
    typedef _OVTreeMetadataBase<Metadata, Allocator> ThisT;

protected:
    explicit
    _OVTreeMetadataBase(size_t size, const Metadata & md)
    {
        resize(size, md);
    }

    ~_OVTreeMetadataBase()
    {
        // Do nothing.
    }
    
    void 
    swap(ThisT & other) 
    {
        buf.swap(other.buf);
    }

    void
    resize(size_t size, const Metadata & md)
    {
        buf = BufT(size, md);
    }
    
    Metadata *
    first()
    {
        return buf.empty()? NULL: &buf[0];
    }

    const Metadata *
    first() const
    {
        return buf.empty()? NULL: &buf[0];
    }

#ifdef BANYAN_DEBUG    
    void 
    trace(std::ostream & os)
    {
        // Do nothing.               
    }   
#endif // #ifdef BANYAN_DEBUG

private:
    typedef 
        std::vector<
            Metadata, 
            typename Allocator::template rebind<Metadata>::other> 
        BufT;
                

private:
    BufT buf;
};

template<
    class Allocator>
class _OVTreeMetadataBase<
    _NullMetadata,
    Allocator>
{
protected:
    typedef _OVTreeMetadataBase<_NullMetadata, Allocator> ThisT;

protected:
    inline explicit
    _OVTreeMetadataBase(size_t)
    {
        // Do nothing.
    }

    inline explicit
    _OVTreeMetadataBase(size_t, const _NullMetadata &)
    {
        // Do nothing.
    }

    inline ~_OVTreeMetadataBase()
    {
        // Do nothing.
    }

    void 
    swap(ThisT &) 
    {
        // Do nothing.
    }

    inline void
    resize(size_t, const _NullMetadata &)
    {
        // Do nothing.
    }

    _NullMetadata *
    first()
    {
        return NULL;
    }

    const _NullMetadata *
    first() const
    {
        return NULL;
    }
};

template<
    typename T,
    class Metadata>
class _OVTreeNodeIterator
{
public:
    typedef 
        _OVTreeNodeIterator<
            T,
            Metadata>
        ThisT;            

public:
    explicit _OVTreeNodeIterator() :
        b(NULL), 
        mb(NULL),
        len(0)
    {
        // Do nothing.
    }

    explicit _OVTreeNodeIterator(T * b_, Metadata * mb_, size_t len_) :
        b(b_), 
        mb(mb_),
        len(len_)
    {    
        DBG_ASSERT((b != NULL) || (len == 0));
    }
    
    bool operator==(const _OVTreeNodeIterator & other) const
    {
        if (len != other.len)
            return false;
            
        DBG_ASSERT(len == 0 || ((b == other.b) == (mb == other.mb)));            
            
        return len == 0 || b == other.b;
    }

    bool operator!=(const _OVTreeNodeIterator & other) const
    {
        return !operator==(other);
    }
    
    ThisT
    left()
    {
        DBG_ASSERT(len > 0);
        
        return ThisT(b, mb, len >> 1);
    }        
            
    ThisT
    right()
    {
        DBG_ASSERT(len > 0);
        
        const size_t half = len >> 1;
        DBG_ASSERT(len >= half+ 1);
        return ThisT(b + half + 1, mb + half + 1, len - (half + 1));
    }        
    
    inline Metadata &
    metadata()
    {
        DBG_ASSERT(len > 0);
        return *(mb + (len >> 1));
    }
    
    inline T &
    value()
    {
        DBG_ASSERT(len > 0);
        return *(b + (len >> 1));
    }
    
    inline size_t
    size() const
    {
        return len;
    }
    
private:
    T * b;
    Metadata * mb;
    size_t len;
};

template<
    typename T>
struct _OVTreeNodeIterator<
    T,
    _NullMetadata>
{
    typedef 
        _OVTreeNodeIterator<
            T,
            _NullMetadata>
        ThisT;            

    explicit _OVTreeNodeIterator() :
        len(0)
    {
        // Do nothing.
    }

    explicit _OVTreeNodeIterator(T * b_, _NullMetadata * mb_, size_t len_) :
        b(b_), 
        len(len_)
    {
        DBG_ASSERT(mb_ == NULL);
    }
    
    bool operator==(const _OVTreeNodeIterator & other) const
    {
        if (len != other.len)
            return false;
            
        return len == 0 || b == other.b;
    }

    bool operator!=(const _OVTreeNodeIterator & other) const
    {
        return !operator==(other);
    }
    
    ThisT
    left()
    {
        DBG_ASSERT(len > 0);
        
        return ThisT(b, NULL, len >> 1);
    }        
            
    ThisT
    right()
    {
        DBG_ASSERT(len > 0);
        
        const size_t half = len >> 1;
        DBG_ASSERT(len >= half+ 1);
        return ThisT(b + half + 1, NULL, len - (half + 1));
    }        
    
    inline _NullMetadata &
    metadata()
    {
        DBG_ASSERT(false);
        _NullMetadata m;
        return m;
    }

    inline T &
    value()
    {
        DBG_ASSERT(len > 0);
        return *(b + (len >> 1));
    }

    inline size_t
    size() const
    {
        return len;
    }

    T * b;
    size_t len;
};

template<
    typename T,
    class Key_Extractor,
    class Metadata = _NullMetadata,
    class LT = std::less<typename Key_Extractor::KeyType>,
    class Allocator = std::allocator<T> >
class _OVTree :
    public _BinaryTree<T, Key_Extractor, Metadata, LT>,
    private _OVTreeMetadataBase<Metadata, Allocator>
{
private:
    typedef 
        std::vector<
            T, 
            typename Allocator::template rebind<T>::other> 
        BufT;
    typedef
        _OVTree<        
            T,
            Key_Extractor,
            Metadata,
            LT,
            Allocator>
        ThisT;
    
public:
    typedef T ValueType;
    typedef _OVTreeNodeIterator<T, Metadata> NodeIterator;
    typedef T * Iterator;

public:
    explicit
    _OVTree();
    explicit
    _OVTree(const LT & lt);
    explicit
    _OVTree(T * b, T * e);
    explicit
    _OVTree(T * b, T * e, const LT & lt);
    explicit
    _OVTree(T * b, T * e, const Metadata & md, const LT & lt);

    virtual
    ~_OVTree();
    void
    swap(ThisT & other);
    
    void
    clear();

    std::pair<Iterator, bool>
    insert(const T & val);
    
    Iterator
    find(const typename Key_Extractor::KeyType & key);
    Iterator
    lower_bound(const typename Key_Extractor::KeyType & key);
    
    T
    erase(const typename Key_Extractor::KeyType & key);
    T
    erase(Iterator m);
    T
    pop();
    
    inline size_t
    size() const;
    void
    restore_size(size_t size);

    inline Iterator
    begin();
    inline Iterator
    rbegin();
    inline Iterator
    end();
    inline Iterator
    rend();

    NodeIterator
    node_begin();
    NodeIterator
    node_end();

    inline Metadata &
    node_metadata(NodeIterator it);
    inline ValueType &
    node_value(NodeIterator it);

    void
    join(ThisT & larger);
    void
    split(const typename Key_Extractor::KeyType & rb, ThisT & larger);

#ifdef BANYAN_DEBUG
    void 
    assert_valid() const;
    
    void
    trace(std::ostream & os) const;
#endif // #ifdef BANYAN_DEBUG

protected:
    typedef _BinaryTree<T, Key_Extractor, Metadata, LT> BaseT;    
    typedef _OVTreeMetadataBase<Metadata, Allocator> MDBaseT;

private:
    typename BufT::iterator
    lower_bound(
        typename BufT::iterator b, typename BufT::iterator e, 
        const typename Key_Extractor::KeyType & key) const;
        
    template<class Metadata_>
    void
    fix(NodeIterator it, Metadata_ &);        
    void
    fix(NodeIterator it, _NullMetadata &);        
            
public:
    BufT buf;    
};

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
_OVTree<T, Key_Extractor, Metadata, LT, Allocator>::_OVTree() :
    MDBaseT(0, BaseT::md)
{
    DBG_ONLY(assert_valid();)
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
_OVTree<T, Key_Extractor, Metadata, LT, Allocator>::_OVTree(const LT & lt) :
    BaseT(lt),
    MDBaseT(0)
{
    DBG_ONLY(assert_valid();)
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
_OVTree<T, Key_Extractor, Metadata, LT, Allocator>::_OVTree(T * b, T * e) :
    MDBaseT(std::distance(b, e), BaseT::md),
    buf(b, e)
{
    fix(node_begin(), BaseT::md);
    
    DBG_ONLY(assert_valid();)
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
_OVTree<T, Key_Extractor, Metadata, LT, Allocator>::_OVTree(T * b, T * e, const LT & lt) :
    BaseT(lt),
    MDBaseT(std::distance(b, e)),
    buf(b, e)
{
    fix(node_begin(), BaseT::md);
    DBG_ONLY(assert_valid();)
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
_OVTree<T, Key_Extractor, Metadata, LT, Allocator>::_OVTree(
        T * b, T * e, const Metadata & md, const LT & lt) :
    BaseT(md, lt),
    MDBaseT(std::distance(b, e), md),
    buf(b, e)
{
    fix(node_begin(), BaseT::md);
    DBG_ONLY(assert_valid();)
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
_OVTree<T, Key_Extractor, Metadata, LT, Allocator>::~_OVTree()
{
    clear();
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
void
_OVTree<T, Key_Extractor, Metadata, LT, Allocator>::swap(ThisT & other)
{
    buf.swap(other.buf);
    MDBaseT::swap(other);
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
void
_OVTree<T, Key_Extractor, Metadata, LT, Allocator>::clear()
{
    buf.clear();

    MDBaseT::resize(0, BaseT::md);
    fix(node_begin(), BaseT::md);

    DBG_ONLY(assert_valid();)
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
std::pair<typename _OVTree<T, Key_Extractor, Metadata, LT, Allocator>::Iterator, bool>
_OVTree<T, Key_Extractor, Metadata, LT, Allocator>::insert(const T & val)
{
    typename BufT::iterator it = lower_bound(
        buf.begin(), 
        buf.end(), 
        BaseT::extract(val));
    if (it != buf.end() && !BaseT::lt(BaseT::extract(val), BaseT::extract(*it))) 
        return std::make_pair(&buf[0] + std::distance(buf.begin(), it), false);
    
    const size_t pos = std::distance(buf.begin(), it);        
    BufT new_buf(buf.size() + 1);
    std::copy(buf.begin(), it, &new_buf[0]);
    new_buf[pos] = val;
    std::copy(it, buf.end(), &new_buf[0] + pos + 1);     
    buf.swap(new_buf);   
    
    MDBaseT::resize(buf.size(), BaseT::md);
    fix(node_begin(), BaseT::md);

    DBG_ONLY(assert_valid();)
    
    return std::make_pair(&new_buf[0] + pos, true);
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
typename _OVTree<T, Key_Extractor, Metadata, LT, Allocator>::Iterator
_OVTree<T, Key_Extractor, Metadata, LT, Allocator>::find(const typename Key_Extractor::KeyType & key)
{
    typename BufT::iterator it = 
       lower_bound(buf.begin(), buf.end(), key);
    return it != buf.end() && !BaseT::lt(key, BaseT::extract(*it))?
        &(*it) : 
        end();
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
typename _OVTree<T, Key_Extractor, Metadata, LT, Allocator>::Iterator
_OVTree<T, Key_Extractor, Metadata, LT, Allocator>::lower_bound(
    const typename Key_Extractor::KeyType & key)
{
    typename BufT::iterator it = 
       lower_bound(buf.begin(), buf.end(), key);           
    return it != buf.end()?
        &(*it) : 
        end();
}

#ifdef BANYAN_DEBUG
template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
void
_OVTree<T, Key_Extractor, Metadata, LT, Allocator>::assert_valid() const
{
    for(size_t i = 1; i < buf.size(); ++i)
        DBG_VERIFY(!BaseT::lt(BaseT::extract(buf[i]), BaseT::extract(buf[i - 1])));
}
#endif // #ifdef BANYAN_DEBUG

#ifdef BANYAN_DEBUG
template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
void
_OVTree<T, Key_Extractor, Metadata, LT, Allocator>::trace(std::ostream & os) const
{
    for(size_t i = 0; i < buf.size(); ++i) {
        os << "(" << i << ") " << buf[i];
        const Metadata * const first_p = MDBaseT::first();
        if (first_p != NULL)
            first_p[i].trace(os);
        os << "\n";
    }
}
#endif // #ifdef BANYAN_DEBUG

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
T
_OVTree<T, Key_Extractor, Metadata, LT, Allocator>::erase(const typename Key_Extractor::KeyType & key)
{
    typename BufT::iterator it = lower_bound(buf.begin(), buf.end(), key);
    if (it == buf.end() || BaseT::lt(key, BaseT::extract(*it)))
        throw std::logic_error("Key not found");            

    return erase(&buf[0] + std::distance(buf.begin(), it));
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
T
_OVTree<T, Key_Extractor, Metadata, LT, Allocator>::pop()
{
    DBG_ASSERT(size() > 0);
    return erase(begin());
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
T
_OVTree<T, Key_Extractor, Metadata, LT, Allocator>::erase(Iterator it)
{
    DBG_ASSERT(0 <= std::distance(begin(), it) && 
        static_cast<size_t>(std::distance(begin(), it)) < size());
 
    T erased_val = *it;   
    
    BufT new_buf(buf.size() - 1);
    std::copy(&buf[0], it, &new_buf[0]);
    std::copy(it + 1, &buf[0] + buf.size(), &new_buf[0] + std::distance(&buf[0], it));     
    buf.swap(new_buf);         
    
    MDBaseT::resize(buf.size(), BaseT::md);
    fix(node_begin(), BaseT::md);

    DBG_ONLY(assert_valid();)
    
    return erased_val;
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
inline size_t
_OVTree<T, Key_Extractor, Metadata, LT, Allocator>::size() const
{
    return buf.size();
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
typename _OVTree<T, Key_Extractor, Metadata, LT, Allocator>::BufT::iterator
_OVTree<T, Key_Extractor, Metadata, LT, Allocator>::lower_bound(
    typename BufT::iterator b, typename BufT::iterator e, 
    const typename Key_Extractor::KeyType & key) const
{
    typename BufT::iterator b_ = b, e_ = e;    
    size_t l = std::distance(b, e);
    while (l > 0) {
        DBG_ASSERT(l <= static_cast<size_t>(std::distance(b, e)));
        const size_t half = l >> 1;
        typename BufT::iterator m = b_ + half;        
        DBG_ASSERT(b <= m && m < e);
        if (BaseT::lt(BaseT::extract(*m), key)) {
            b_ = m + 1;
            DBG_ASSERT(l >= half + 1);
            l -= half + 1;
        }
        else
            l = half;
    }

    return b_;
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
typename _OVTree<T, Key_Extractor, Metadata, LT, Allocator>::Iterator
_OVTree<T, Key_Extractor, Metadata, LT, Allocator>::begin()
{
    return buf.empty()? NULL : &buf[0];
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
typename _OVTree<T, Key_Extractor, Metadata, LT, Allocator>::Iterator
_OVTree<T, Key_Extractor, Metadata, LT, Allocator>::rbegin()
{
    return buf.empty()? NULL : &buf[0] + buf.size() - 1;
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
typename _OVTree<T, Key_Extractor, Metadata, LT, Allocator>::Iterator
_OVTree<T, Key_Extractor, Metadata, LT, Allocator>::end()
{
    return buf.empty()? NULL : &buf[0] + buf.size();
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
typename _OVTree<T, Key_Extractor, Metadata, LT, Allocator>::Iterator
_OVTree<T, Key_Extractor, Metadata, LT, Allocator>::rend()
{
    return buf.empty()? NULL : &buf[0] - 1;
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator> 
typename _OVTree<T, Key_Extractor, Metadata, LT, Allocator>::NodeIterator
_OVTree<T, Key_Extractor, Metadata, LT, Allocator>::node_begin()
{
    return NodeIterator(buf.empty()? NULL: &buf[0], MDBaseT::first(), buf.size());
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
typename _OVTree<T, Key_Extractor, Metadata, LT, Allocator>::NodeIterator
_OVTree<T, Key_Extractor, Metadata, LT, Allocator>::node_end()
{
    return NodeIterator(NULL, NULL, 0);
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
typename _OVTree<T, Key_Extractor, Metadata, LT, Allocator>::ValueType &
_OVTree<T, Key_Extractor, Metadata, LT, Allocator>::node_value(NodeIterator it)
{
    return it.value();
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
Metadata &
_OVTree<T, Key_Extractor, Metadata, LT, Allocator>::node_metadata(NodeIterator it)
{
    return it.metadata();
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
template<class Metadata_>
void
_OVTree<T, Key_Extractor, Metadata, LT, Allocator>::fix(NodeIterator it, Metadata_ &)
{
    if (it == node_end())
        return;
        
    NodeIterator left_it = it.left();        
    NodeIterator right_it = it.right();        
        
    fix(left_it, BaseT::md);        
    fix(right_it, BaseT::md);        
    
    Metadata_ * const l = left_it == node_end()? NULL: &node_metadata(left_it);
    Metadata_ * const r = right_it == node_end()? NULL: &node_metadata(right_it);    
    node_metadata(it).update(Key_Extractor::extract(node_value(it)), l, r);
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
void
_OVTree<T, Key_Extractor, Metadata, LT, Allocator>::fix(NodeIterator it, _NullMetadata &)
{
    // Do nothing.
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
void
_OVTree<T, Key_Extractor, Metadata, LT, Allocator>::join(ThisT & larger)
{
    BufT new_buf;
    new_buf.reserve(buf.size() + larger.buf.size());
    std::copy(buf.begin(), buf.end(), std::back_inserter(new_buf));
    std::copy(larger.buf.begin(), larger.buf.end(), std::back_inserter(new_buf));
    buf.swap(new_buf);   
    
    MDBaseT::resize(buf.size(), BaseT::md);
    fix(node_begin(), BaseT::md);

    DBG_ONLY(assert_valid();)
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
void
_OVTree<T, Key_Extractor, Metadata, LT, Allocator>::split(
    const typename Key_Extractor::KeyType & rb, ThisT & larger)
{
    larger.clear();

    Iterator r_it = lower_bound(rb);
        
    larger.buf.reserve(std::distance(r_it, end()));
    std::copy(r_it, end(), std::back_inserter(larger.buf));
    larger.MDBaseT::resize(larger.buf.size(), larger.md);
    larger.fix(larger.node_begin(), larger.md);
    
    buf.resize(std::distance(begin(), r_it));    
    MDBaseT::resize(buf.size(), BaseT::md);
    fix(node_begin(), BaseT::md);

    DBG_ONLY(assert_valid();)
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
void
_OVTree<T, Key_Extractor, Metadata, LT, Allocator>::restore_size(size_t)
{
    // Do nothing.
}

#endif // #ifndef _OV_TREE_HPP

