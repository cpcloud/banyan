#ifndef _NODE_BASED_BINARY_TREE_HPP
#define _NODE_BASED_BINARY_TREE_HPP

#include <cstdlib>
#include <vector>
#include <memory>

#include "_dbg.hpp"
#include "_binary_tree_node.hpp"
#include "_binary_tree.hpp"

template<class Node_T>
class _NodeBasedBinaryTreeIterator :
    public std::iterator<std::forward_iterator_tag, typename Node_T::ValueType>
{
public:
    typedef typename Node_T::ValueType ValueType;

public:
    _NodeBasedBinaryTreeIterator(Node_T * p_ = NULL) :
        p(p_)
    {
        // Do nothing.
    }            
    
    ValueType & 
    operator*()
    {
        DBG_ASSERT(p != NULL);
        return static_cast<ValueType &>(const_cast<ValueType &>(p->val));
    }
    
    ValueType * 
    operator->()
    {
        DBG_ASSERT(p != NULL);
        return static_cast<ValueType *>(&const_cast<ValueType &>(p->val));
    }

    _NodeBasedBinaryTreeIterator<Node_T> &
    operator++()
    {
        DBG_ASSERT(p != NULL);
        p = p->next();
        return *this;
    }

    _NodeBasedBinaryTreeIterator<Node_T> 
    operator++(int)
    {
        DBG_ASSERT(p != NULL);
        _NodeBasedBinaryTreeIterator<Node_T> tmp(p);
        p = p->next();
        return tmp;
    }
    
    _NodeBasedBinaryTreeIterator<Node_T> &
    operator--()
    {
        DBG_ASSERT(p != NULL);
        p = p->prev();
        return *this;
    }

    _NodeBasedBinaryTreeIterator<Node_T> 
    operator--(int)
    {
        DBG_ASSERT(p != NULL);
        _NodeBasedBinaryTreeIterator<Node_T> tmp(p);
        p = p->prev();
        return tmp;
    }

    inline bool
    operator==(const _NodeBasedBinaryTreeIterator<Node_T> & other) const
    {
        return p == other.p;
    }

    inline bool
    operator!=(const _NodeBasedBinaryTreeIterator<Node_T> & other) const
    {
        return p != other.p;
    }

public:    
    Node_T * p;
};    

template<
    typename T,
    class Key_Extractor,
    class Metadata,
    class LT,
    class Allocator,
    class Alloc_Node_T>
class _NodeBasedBinaryTree :
    public _BinaryTree<T, Key_Extractor, Metadata, LT>
{
public:
    typedef T ValueType;
    typedef _NodeBasedBinaryTreeIterator<Node<T, Key_Extractor, Metadata> > Iterator;
    typedef Node<T, Key_Extractor, Metadata> * NodeIterator;
    typedef Node<T, Key_Extractor, Metadata> NodeT;
    typedef
        _NodeBasedBinaryTree<
            T,
            Key_Extractor,
            Metadata,
            LT,
            Allocator,
            Alloc_Node_T>
        ThisT;
    
public:
    explicit
    _NodeBasedBinaryTree();
    explicit
    _NodeBasedBinaryTree(T * b, T * e);
    explicit
    _NodeBasedBinaryTree(const LT & lt);
    explicit
    _NodeBasedBinaryTree(T * b, T * e, const LT & lt);
    explicit
    _NodeBasedBinaryTree(T * b, T * e, const Metadata & md, const LT & lt);
    
    void 
    swap(ThisT & other);

    virtual
    ~_NodeBasedBinaryTree() = 0;
    void
    clear();

    Iterator 
    find(const typename Key_Extractor::KeyType & key);
    Iterator 
    lower_bound(const typename Key_Extractor::KeyType & key);

    std::vector<T>
    erase(
        const typename Key_Extractor::KeyType & b,
        const typename Key_Extractor::KeyType & e);

    inline size_t 
    size() const;
    void
    restore_size(size_t size);
    
    Iterator
    begin();
    Iterator
    rbegin();
    Iterator
    end();
    Iterator
    rend();
    
    NodeIterator
    node_begin();
    NodeIterator
    node_end();

#ifdef BANYAN_DEBUG
    void
    assert_valid() const;
#endif // #ifdef BANYAN_DEBUG

protected:
    bool
    join(ThisT & larger);

    void
    swap(NodeT * u, NodeT * v);
    
    Alloc_Node_T *
    allocate();
    void 
    deallocate(Alloc_Node_T *p);

protected:
    NodeT * root;

    size_t n;
    
private:
    typedef
        _BinaryTree<T, Key_Extractor, Metadata, LT>    
        BaseT;
        
    typedef 
        typename Allocator::template rebind<Alloc_Node_T>::other
        AllocT;
        
private:        
    void
    rec_dealloc(Alloc_Node_T * n);

private:
    Alloc_Node_T *
    from_elems(T * b, T * e);
    
    static AllocT alloc;
};

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator, class Alloc_Node_T>
typename _NodeBasedBinaryTree<T, Key_Extractor, Metadata, LT, Allocator, Alloc_Node_T>::AllocT
    _NodeBasedBinaryTree<T, Key_Extractor, Metadata, LT, Allocator, Alloc_Node_T>::alloc;

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator, class Alloc_Node_T>
_NodeBasedBinaryTree<T, Key_Extractor, Metadata, LT, Allocator, Alloc_Node_T>::_NodeBasedBinaryTree() :
    root(NULL),
    n(0)
{
    DBG_ONLY(assert_valid();)
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator, class Alloc_Node_T>
_NodeBasedBinaryTree<T, Key_Extractor, Metadata, LT, Allocator, Alloc_Node_T>::_NodeBasedBinaryTree(
        const LT & lt) :
    BaseT(lt),
    root(NULL),
    n(0)
{
    DBG_ONLY(assert_valid();)
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator, class Alloc_Node_T>
_NodeBasedBinaryTree<T, Key_Extractor, Metadata, LT, Allocator, Alloc_Node_T>::_NodeBasedBinaryTree(
        T * b, T * e, const LT & lt) :
    BaseT(lt),
    root(from_elems(b, e)),
    n(std::distance(b, e))
{
    if (root != NULL)
        root->p = NULL;
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator, class Alloc_Node_T>
_NodeBasedBinaryTree<T, Key_Extractor, Metadata, LT, Allocator, Alloc_Node_T>::_NodeBasedBinaryTree(
        T * b, T * e, const Metadata & md, const LT & lt) :
    BaseT(md, lt),
    root(from_elems(b, e)),
    n(std::distance(b, e))
{
    if (root != NULL)
        root->p = NULL;
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator, class Alloc_Node_T>
_NodeBasedBinaryTree<T, Key_Extractor, Metadata, LT, Allocator, Alloc_Node_T>::
    _NodeBasedBinaryTree(T * b, T * e) :
    root(from_elems(b, e)),
    n(std::distance(b, e))
{
    if (root != NULL)
        root->p = NULL;
    DBG_ONLY(assert_valid();)
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator, class Alloc_Node_T>
Alloc_Node_T *
_NodeBasedBinaryTree<T, Key_Extractor, Metadata, LT, Allocator, Alloc_Node_T>::from_elems(T * b, T * e)
{
    DBG_ASSERT(b <= e);
    if (b == e)
        return NULL;

    T * const m = b + std::distance(b, e) / 2;
    Alloc_Node_T * const n = allocate();
    new (n) Alloc_Node_T(*m, BaseT::md);    
    n->l = from_elems(b, m);
    if (n->l != NULL)
        n->l->p = n;
    n->r = from_elems(m + 1, e);
    if (n->r != NULL)
        n->r->p = n;
    n->fix();
    return n;
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator, class Alloc_Node_T>
_NodeBasedBinaryTree<T, Key_Extractor, Metadata, LT, Allocator, Alloc_Node_T>::~_NodeBasedBinaryTree()
{
    clear();
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator, class Alloc_Node_T>
void
_NodeBasedBinaryTree<T, Key_Extractor, Metadata, LT, Allocator, Alloc_Node_T>::swap(ThisT & other)
{
    std::swap(n, other.n);
    std::swap(root, other.root);
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator, class Alloc_Node_T>
void
_NodeBasedBinaryTree<T, Key_Extractor, Metadata, LT, Allocator, Alloc_Node_T>::clear()
{
    rec_dealloc(static_cast<Alloc_Node_T *>(root));
    
    n = 0;
    
    root = NULL;
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator, class Alloc_Node_T>
void
_NodeBasedBinaryTree<T, Key_Extractor, Metadata, LT, Allocator, Alloc_Node_T>::rec_dealloc(Alloc_Node_T * n)
{
    if (n == NULL)
        return;
    if (n->l != NULL)
        rec_dealloc(static_cast<Alloc_Node_T *>(n->l));
    if (n->r != NULL)
        rec_dealloc(static_cast<Alloc_Node_T *>(n->r));            
    deallocate(n);            
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator, class Alloc_Node_T>
Alloc_Node_T *
_NodeBasedBinaryTree<T, Key_Extractor, Metadata, LT, Allocator, Alloc_Node_T>::allocate()
{
    Alloc_Node_T * const p = alloc.allocate(1, 0);
    if (p == NULL)
        throw std::bad_alloc();
    return p;        
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator, class Alloc_Node_T>
void 
_NodeBasedBinaryTree<T, Key_Extractor, Metadata, LT, Allocator, Alloc_Node_T>::deallocate(Alloc_Node_T *p)
{
    DBG_ASSERT(p != NULL);
    p->~Alloc_Node_T();
    alloc.deallocate(p, 1);
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator, class Alloc_Node_T>
void
_NodeBasedBinaryTree<T, Key_Extractor, Metadata, LT, Allocator, Alloc_Node_T>::swap(NodeT * u, NodeT * v)
{
    DBG_ASSERT(u != v);
    
    if (v->p == u) 
        std::swap(v, u);
    
    if (u->p == v) {
        if (v->p == NULL) {
            u->p = NULL;
            root = u;
        }
        else {
            u->p = v->p;
            if (v->p->l == v)
                v->p->l = u;
            else
                v->p->r = u;
            v->p = u;
        }
        
        if (v->l == u) {
            v->l = u->l;
            u->l = v;
            std::swap(u->r, v->r);            
        }
        else {
            DBG_ASSERT(v->r == u);
            v->r = u->r;
            u->r = v;
            std::swap(u->l, v->l);            
        }
    
        if (u->l != NULL)
            u->l->p = u;
        if (u->r != NULL)
            u->r->p = u;
        
        if (v->l != NULL)
            v->l->p = v;
        if (v->r != NULL)
            v->r->p = v;
                        
        return;   
    }      

    std::swap(u->l, v->l);
    std::swap(u->r, v->r);
    std::swap(u->p, v->p);

    if (u->l != NULL)
        u->l->p = u;
    if (u->r != NULL)
        u->r->p = u;    
    if (u->p == NULL)
        root = v;
    else {
        if (u->p->l == v)
            u->p->l = u;
        else
            u->p->r = u;
    }

    if (v->l != NULL)
        v->l->p = v;
    if (v->r != NULL)
        v->r->p = v;
    if (v->p == NULL)
        root = v;
    else {
        if (v->p->l == u)
            v->p->l = v;
        else
            v->p->r = v;
    }
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator, class Alloc_Node_T>
typename _NodeBasedBinaryTree<T, Key_Extractor, Metadata, LT, Allocator, Alloc_Node_T>::Iterator
_NodeBasedBinaryTree<T, Key_Extractor, Metadata, LT, Allocator, Alloc_Node_T>::find(
    const typename Key_Extractor::KeyType & key)
{
    NodeT * n = root, * pot = NULL;
    
    while (n != NULL) 
        if (BaseT::lt(key, BaseT::extract(n->val)))
            n = n->l;
        else {
            pot = n;
            n = n->r;
        }
        
    return pot != NULL && !BaseT::lt(BaseT::extract(pot->val), key)? 
        Iterator(pot) : 
        Iterator(NULL);
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator, class Alloc_Node_T>
typename _NodeBasedBinaryTree<T, Key_Extractor, Metadata, LT, Allocator, Alloc_Node_T>::Iterator
_NodeBasedBinaryTree<T, Key_Extractor, Metadata, LT, Allocator, Alloc_Node_T>::lower_bound(
    const typename Key_Extractor::KeyType & key)
{
    NodeT * n = root, * pot = NULL;
    
    while (n != NULL) 
        if (BaseT::lt(key, BaseT::extract(n->val)))
            n = n->l;
        else {
            pot = n;
            n = n->r;
        }
        
    if (pot == NULL)        
        return begin();
        
    return !BaseT::lt(BaseT::extract(pot->val), key)? 
        Iterator(pot) : 
        Iterator(pot->next());
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator, class Alloc_Node_T>
inline size_t
_NodeBasedBinaryTree<T, Key_Extractor, Metadata, LT, Allocator, Alloc_Node_T>::size() const
{
    return n;
}
template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator, class Alloc_Node_T>
typename _NodeBasedBinaryTree<T, Key_Extractor, Metadata, LT, Allocator, Alloc_Node_T>::NodeIterator
_NodeBasedBinaryTree<T, Key_Extractor, Metadata, LT, Allocator, Alloc_Node_T>::node_begin()
{
    return root;
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator, class Alloc_Node_T>
typename _NodeBasedBinaryTree<T, Key_Extractor, Metadata, LT, Allocator, Alloc_Node_T>::NodeIterator
_NodeBasedBinaryTree<T, Key_Extractor, Metadata, LT, Allocator, Alloc_Node_T>::node_end()
{
    return NULL;
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator, class Alloc_Node_T>
typename _NodeBasedBinaryTree<T, Key_Extractor, Metadata, LT, Allocator, Alloc_Node_T>::Iterator
_NodeBasedBinaryTree<T, Key_Extractor, Metadata, LT, Allocator, Alloc_Node_T>::begin()
{
    NodeT * p = root;
    if (p == NULL)
        return NULL;
    while (p->l != NULL)
        p = p->l;                
    return Iterator(p);
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator, class Alloc_Node_T>
typename _NodeBasedBinaryTree<T, Key_Extractor, Metadata, LT, Allocator, Alloc_Node_T>::Iterator
_NodeBasedBinaryTree<T, Key_Extractor, Metadata, LT, Allocator, Alloc_Node_T>::rbegin()
{
    NodeT * p = root;
    if (p == NULL)
        return NULL;
    while (p->r != NULL)
        p = p->r;                
    return Iterator(p);
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator, class Alloc_Node_T>
typename _NodeBasedBinaryTree<T, Key_Extractor, Metadata, LT, Allocator, Alloc_Node_T>::Iterator
_NodeBasedBinaryTree<T, Key_Extractor, Metadata, LT, Allocator, Alloc_Node_T>::end()
{
    return Iterator(NULL);
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator, class Alloc_Node_T>
typename _NodeBasedBinaryTree<T, Key_Extractor, Metadata, LT, Allocator, Alloc_Node_T>::Iterator
_NodeBasedBinaryTree<T, Key_Extractor, Metadata, LT, Allocator, Alloc_Node_T>::rend()
{
    return Iterator(NULL);
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator, class Alloc_Node_T>
void
_NodeBasedBinaryTree<T, Key_Extractor, Metadata, LT, Allocator, Alloc_Node_T>::restore_size(size_t size)
{
    DBG_ASSERT(n == static_cast<size_t>(-1) || 
        (size == 0 && root == NULL) || 
        (n == size && root->count() == n));
    n = size;
    DBG_ASSERT((root == NULL? 0 : root->count()) == n);
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator, class Alloc_Node_T>
bool
_NodeBasedBinaryTree<T, Key_Extractor, Metadata, LT, Allocator, Alloc_Node_T>::join(ThisT & larger)
{
    if (larger.root == NULL)
        return true;
        
    if (root == NULL) {
        swap(larger);
        
        DBG_ONLY(assert_valid();)
        DBG_ONLY(larger.assert_valid();)

        return true;
    }       
    
#ifdef BANYAN_DEBUG            
    DBG_ASSERT(BaseT::less_than()(
        Key_Extractor::extract(rbegin().p->val), 
        Key_Extractor::extract(larger.begin().p->val)));    
#endif // #ifdef BANYAN_DEBUG
    
    return false;         
}

#ifdef BANYAN_DEBUG
template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator, class Alloc_Node_T>
void
_NodeBasedBinaryTree<T, Key_Extractor, Metadata, LT, Allocator, Alloc_Node_T>::assert_valid() const
{
    if (root != NULL)
        root->assert_valid(BaseT::lt);

    if (n != static_cast<size_t>(-1))
        DBG_VERIFY((root == NULL? 0 : root->count()) == n);
    
    size_t count = 0;
    Iterator b = const_cast<ThisT *>(this)->begin();
    Iterator rb = const_cast<ThisT *>(this)->rbegin();
    while (b != const_cast<ThisT *>(this)->end()) {
        DBG_VERIFY(rb != const_cast<ThisT *>(this)->rend());
        ++b;
        --rb;
        ++count;
    }
    DBG_VERIFY(rb == const_cast<ThisT *>(this)->rend());
    DBG_VERIFY(n == static_cast<size_t>(-1) || count == n);
}
#endif // #ifdef BANYAN_DEBUG

#endif // #ifndef _NODE_BASED_BINARY_TREE_HPP

  
