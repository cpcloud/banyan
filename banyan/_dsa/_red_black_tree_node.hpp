#ifndef _RED_BLACK_TREE_NODE_HPP
#define _RED_BLACK_TREE_NODE_HPP

#include <cstdlib>

#include "_dbg.hpp"
#include "_binary_tree_node.hpp"

template<typename T, class Key_Extractor, class Metadata>
class RBNode :
    public Node<T, Key_Extractor, Metadata>
{
public:
    typedef RBNode<T, Key_Extractor, Metadata> ThisT;

    typedef Node<T, Key_Extractor, Metadata> BaseT;

    enum color
    {
        red,
        black
    };

public:
    explicit
    RBNode(const T & val, color clr);
    template<typename Metadata_>
    explicit 
    RBNode(const T & val, const Metadata_ & md);    
    explicit
    RBNode(const T & val, const _NullMetadata &);
    template<typename Metadata_>
    explicit 
    RBNode(const T & val, color clr, const Metadata_ & md);    
    explicit
    RBNode(const T & val, color clr, const _NullMetadata &);

    virtual
    ~RBNode();

    ThisT *
    next();

    static
    size_t black_height(const RBNode<T, Key_Extractor, Metadata> * p);

#ifdef BANYAN_DEBUG
    static
    void verify_black_height(const RBNode<T, Key_Extractor, Metadata> * p);

    template<class LT>
    void
    assert_valid(const LT & lt) const;
#endif // #ifdef BANYAN_DEBUG

public:
    char clr;
    
    ThisT * n;

protected:
#ifdef BANYAN_DEBUG
    virtual void
    trace(std::ostream & os) const;
#endif // #ifdef BANYAN_DEBUG
};

template<typename T, class Key_Extractor, class Metadata>
RBNode<T, Key_Extractor, Metadata>::RBNode(const T & val, color clr_) :
    BaseT(val),
    clr(clr_)
{
    // Do nothing.
}

template<typename T, class Key_Extractor, class Metadata>
template<class Metadata_>
RBNode<T, Key_Extractor, Metadata>::RBNode(const T & val, const Metadata_ & md) :
    BaseT(val, md)
{
    // Do nothing.
}

template<typename T, class Key_Extractor, class Metadata>
RBNode<T, Key_Extractor, Metadata>::RBNode(const T & val, const _NullMetadata &) :
    BaseT(val)
{
    // Do nothing.
}

template<typename T, class Key_Extractor, class Metadata>
template<class Metadata_>
RBNode<T, Key_Extractor, Metadata>::RBNode(const T & val, color clr_, const Metadata_ & md) :
    BaseT(val, md),
    clr(clr_)
{
    // Do nothing.
}

template<typename T, class Key_Extractor, class Metadata>
RBNode<T, Key_Extractor, Metadata>::RBNode(const T & val, color clr_, const _NullMetadata &) :
    BaseT(val),
    clr(clr_)
{
    // Do nothing.
}

template<typename T, class Key_Extractor, class Metadata>
RBNode<T, Key_Extractor, Metadata>::~RBNode()
{
    // Do nothing.
}

template<typename T, class Key_Extractor, class Metadata>
RBNode<T, Key_Extractor, Metadata> *
RBNode<T, Key_Extractor, Metadata>::next()
{
    return n;
}

template<typename T, class Key_Extractor, class Metadata>
size_t
RBNode<T, Key_Extractor, Metadata>::black_height(const RBNode<T, Key_Extractor, Metadata> * p) 
{
    if (p == NULL)
        return 1;
        
    const size_t child_black_height = black_height(static_cast<ThisT *>(p->l));
    
    return p->clr == black? 1 + child_black_height : child_black_height;
}

#ifdef BANYAN_DEBUG
template<typename T, class Key_Extractor, class Metadata>
template<class LT>
void
RBNode<T, Key_Extractor, Metadata>::assert_valid(const LT & lt) const
{
    BaseT::assert_valid(lt);
    verify_black_height(static_cast<const ThisT *>(this));    
    
    if (clr == red) {
        DBG_VERIFY(BaseT::l == NULL || static_cast<ThisT *>(BaseT::l)->clr == black);
        DBG_VERIFY(BaseT::r == NULL || static_cast<ThisT *>(BaseT::r)->clr == black);
    }

    if (BaseT::l != NULL)
        static_cast<const ThisT *>(BaseT::l)->assert_valid(lt);

    if (BaseT::r != NULL)
        static_cast<const ThisT *>(BaseT::r)->assert_valid(lt);

    const BaseT * const cn = const_cast<ThisT *>(this)->BaseT::next();    
    // Tmp Ami
    if (n != cn)
        WARN(this << " " << n << " " << cn);            
    DBG_ASSERT(n == cn);
}
#endif // #ifdef BANYAN_DEBUG

#ifdef BANYAN_DEBUG
template<typename T, class Key_Extractor, class Metadata>
void
RBNode<T, Key_Extractor, Metadata>::verify_black_height(const RBNode<T, Key_Extractor, Metadata> * p) 
{
    if (p == NULL)
        return;
        
    verify_black_height(static_cast<ThisT *>(p->l));        
    const size_t lh = black_height(static_cast<ThisT *>(p->l));
    verify_black_height(static_cast<ThisT *>(p->r));        
    const size_t rh = black_height(static_cast<ThisT *>(p->r));
    DBG_VERIFY(lh == rh);
}
#endif // #ifdef BANYAN_DEBUG

#ifdef BANYAN_DEBUG
template<typename T, class Key_Extractor, class Metadata>
void
RBNode<T, Key_Extractor, Metadata>::trace(std::ostream & os) const
{
    os << (clr == red? "red": "black");
}
#endif // #ifdef BANYAN_DEBUG

#endif // #ifndef _RED_BLACK_TREE_NODE_HPP

