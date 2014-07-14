#ifndef _BINARY_TREE_NODE_HPP
#define _BINARY_TREE_NODE_HPP

#include <cstdlib>

#include "_dbg.hpp"
#include "_binary_tree.hpp"

template<typename T, class Key_Extractor, class Metadata>
class Node :
    public Metadata
{
public:
    typedef Node<T, Key_Extractor, Metadata> ThisT;
    typedef T ValueType;

public:
    explicit
    Node(const T & val);    
    template<typename Metadata_>
    explicit 
    Node(const T & val, const Metadata_ & md);    
    explicit
    Node(const T & val, const _NullMetadata &);

    virtual
    ~Node();

#ifdef BANYAN_DEBUG
    template<class LT>
    void
    assert_valid(const LT & lt) const;

    size_t
    count() const;

    void
    trace(std::ostream & os, std::size_t i) const;
#endif // #ifdef BANYAN_DEBUG

    void
    rotate_left();
    void
    rotate_right();

    inline void
    make_left_child(ThisT * const other);
    inline void
    make_right_child(ThisT * const other);

    ThisT *
    next();
    ThisT *
    prev();

    ThisT *
    left();
    ThisT *
    right();

    inline void
    fix();
    inline void
    fix_to_top();

public:
    ThisT * l, * r, * p;
    const T val;

protected:
#ifdef BANYAN_DEBUG
    virtual void
    trace(std::ostream & os) const;
#endif // #ifdef BANYAN_DEBUG

private:
    template<class Metadata_>
    inline void
    fix(Metadata_ &);
    template<class Metadata_>
    inline void
    fix_to_top(Metadata_ &);

    inline void
    fix(_NullMetadata &);
    inline void
    fix_to_top(_NullMetadata &);
};

template<typename T, class Key_Extractor, class Metadata>
Node<T, Key_Extractor, Metadata>::Node(const T & val_) :
    l(NULL),
    r(NULL),
    p(NULL),
    val(val_)
{
    fix();
}

template<typename T, class Key_Extractor, class Metadata>
template<typename Metadata_>
Node<T, Key_Extractor, Metadata>::Node(const T & val_, const Metadata_ & md) :
    Metadata(md),
    l(NULL),
    r(NULL),
    p(NULL),
    val(val_)
{
    fix();
}


template<typename T, class Key_Extractor, class Metadata>
Node<T, Key_Extractor, Metadata>::Node(const T & val_, const _NullMetadata &) :
    l(NULL),
    r(NULL),
    p(NULL),
    val(val_)
{
    fix();
}

template<typename T, class Key_Extractor, class Metadata>
Node<T, Key_Extractor, Metadata>::~Node()
{
    // Do nothing.
}

#ifdef BANYAN_DEBUG
template<typename T, class Key_Extractor, class Metadata>
template<class LT>
void
Node<T, Key_Extractor, Metadata>::assert_valid(const LT & lt) const
{
    if (l != NULL) {
        l->assert_valid(lt);
        DBG_ASSERT(l->p == this);
        DBG_ASSERT(!lt(Key_Extractor::extract(val), Key_Extractor::extract(l->val)));
    }
    if (r != NULL) {
        r->assert_valid(lt);
        DBG_ASSERT(r->p == this);
        DBG_ASSERT(!lt(Key_Extractor::extract(r->val), Key_Extractor::extract(val)));
    }
}
#endif // #ifdef BANYAN_DEBUG

#ifdef BANYAN_DEBUG
template<typename T, class Key_Extractor, class Metadata>
void
Node<T, Key_Extractor, Metadata>::trace(std::ostream & os, std::size_t i) const
{
    if (r != NULL)     
        r->trace(os, i + 1);
    
    os << std::string(i, ' ') << this << " " << val << " ";
    if (l != NULL && l->p != this)
        os << "Lx ";
    if (r != NULL && r->p != this)
        os << "Rx ";
    Metadata::trace(os);
    trace(os);
    os << "\n";

    if (l != NULL)
        l->trace(os, i + 1);
}
#endif // #ifdef BANYAN_DEBUG

#ifdef BANYAN_DEBUG
template<typename T, class Key_Extractor, class Metadata>
void
Node<T, Key_Extractor, Metadata>::trace(std::ostream & os) const
{
    // Do nothing.
}
#endif // #ifdef BANYAN_DEBUG

#ifdef BANYAN_DEBUG
template<typename T, class Key_Extractor, class Metadata>
size_t
Node<T, Key_Extractor, Metadata>::count() const
{
    return 1 +
        (l == NULL? 0 : l->count()) +
        (r == NULL? 0 : r->count());
}
#endif // #ifdef BANYAN_DEBUG

template<typename T, class Key_Extractor, class Metadata>
void
Node<T, Key_Extractor, Metadata>::rotate_left() 
{
    DBG_ASSERT(r != NULL);
    ThisT * const p = this, * const b = r->l, * const q = r;
    DBG_ASSERT(p != NULL);
    DBG_ASSERT(q != NULL);

    ThisT * const parent = p->p;
    const bool is_l = parent != NULL && parent->l == p;

    p->make_right_child(b);
    q->make_left_child(p);

    if (parent != NULL)  {
        if (is_l)
            parent->make_left_child(q);
        else
            parent->make_right_child(q);
    }
    else
        q->p = NULL;
}

template<typename T, class Key_Extractor, class Metadata>
void
Node<T, Key_Extractor, Metadata>::rotate_right() 
{
    DBG_ASSERT(l != NULL);
    ThisT * const p_ = l, * const b = l->r, * const q = this;
    DBG_ASSERT(p_ != NULL);
    DBG_ASSERT(q != NULL);

    ThisT * const parent = q->p;
    const bool is_l = parent != NULL && parent->l == q;

    q->make_left_child(b);
    p_->make_right_child(q);

    if (parent != NULL)  {
        if (is_l)
            parent->make_left_child(p);
        else
            parent->make_right_child(p_);
    }
    else
        p_->p = NULL;
}

template<typename T, class Key_Extractor, class Metadata>
void
inline Node<T, Key_Extractor, Metadata>::make_left_child(ThisT * const other)
{
    l = other;
    if (other != NULL)
        other->p = this;
    fix();        
}

template<typename T, class Key_Extractor, class Metadata>
void
inline Node<T, Key_Extractor, Metadata>::make_right_child(ThisT * const other)
{
    r = other;
    if (other != NULL)
        other->p = this;
    fix();        
}

template<typename T, class Key_Extractor, class Metadata>
Node<T, Key_Extractor, Metadata> *
Node<T, Key_Extractor, Metadata>::next()
{
    ThisT * n = this;

    if (r != NULL) {
        n = r;
        while (n->l != NULL)
            n = n->l;
        return n;
    }

    if (n->p != NULL) {
        while (n->p != NULL && n->p->r == n)
            n = n->p;
        return n->p;
    }

    return NULL;
}

template<typename T, class Key_Extractor, class Metadata>
Node<T, Key_Extractor, Metadata> *
Node<T, Key_Extractor, Metadata>::prev()
{
    ThisT * n = this;

    if (l != NULL) {
        n = l;
        while (n->r != NULL)
            n = n->r;
        return n;
    }

    if (n->p != NULL) {
        while (n->p != NULL && n->p->l == n)
            n = n->p;
        return n->p;
    }

    return NULL;
}

template<typename T, class Key_Extractor, class Metadata>
Node<T, Key_Extractor, Metadata> *
Node<T, Key_Extractor, Metadata>::left()
{
    return l;
}

template<typename T, class Key_Extractor, class Metadata>
Node<T, Key_Extractor, Metadata> *
Node<T, Key_Extractor, Metadata>::right()
{
    return r;
}

template<typename T, class Key_Extractor, class Metadata>
inline void
Node<T, Key_Extractor, Metadata>::fix()
{
    fix((Metadata &)*this);
}

template<typename T, class Key_Extractor, class Metadata>
inline void
Node<T, Key_Extractor, Metadata>::fix_to_top()
{
    fix_to_top((Metadata &)*this);
}

template<typename T, class Key_Extractor, class Metadata>
template<class Metadata_>
inline void
Node<T, Key_Extractor, Metadata>::fix(Metadata_ &)
{
    DBG_ASSERT(l == NULL || l->p == this);
    DBG_ASSERT(r == NULL || r->p == this);
    Metadata::update(Key_Extractor::extract(val), l, r);
}

template<typename T, class Key_Extractor, class Metadata>
template<class Metadata_>
inline void
Node<T, Key_Extractor, Metadata>::fix_to_top(Metadata_ &)
{
    fix((Metadata &)*this);
    if (p != NULL)
        p->fix_to_top();
}

template<typename T, class Key_Extractor, class Metadata>
inline void
Node<T, Key_Extractor, Metadata>::fix(_NullMetadata &)
{
    // Do nothing.
}

template<typename T, class Key_Extractor, class Metadata>
inline void
Node<T, Key_Extractor, Metadata>::fix_to_top(_NullMetadata &)
{
    // Do nothing.
}

#endif // #ifndef _BINARY_TREE_NODE_HPP

