#ifndef _SPLAY_TREE_HPP
#define _SPLAY_TREE_HPP

#include <exception>

#include "_dbg.hpp"
#include "_node_based_binary_tree.hpp"
#include "_binary_tree_node.hpp"

template<
    typename T,
    class Key_Extractor,
    class Metadata = _NullMetadata,
    class LT = std::less<typename Key_Extractor::KeyType>,
    class Allocator = std::allocator<T> >
class _SplayTree :
    public _NodeBasedBinaryTree<
        T, 
        Key_Extractor, 
        Metadata, 
        LT, 
        Allocator, 
        Node<T, Key_Extractor, Metadata> >
{
public:
    typedef
        _SplayTree<
            T, 
            Key_Extractor, 
            Metadata, 
            LT, 
            Allocator>
        ThisT;        
    typedef 
        _NodeBasedBinaryTree<
            T, 
            Key_Extractor, 
            Metadata, 
            LT, 
            Allocator, 
            Node<T, Key_Extractor, Metadata> > 
        BaseT;
    typedef typename BaseT::Iterator Iterator;
    typedef typename BaseT::NodeT NodeT;

public:
    explicit
    _SplayTree();
    explicit
    _SplayTree(const LT & lt);
    explicit
    _SplayTree(T * b, T * e);
    explicit
    _SplayTree(T * b, T * e, const LT & lt);
    explicit
    _SplayTree(T * b, T * e, const Metadata & md, const LT & lt);

    virtual
    ~_SplayTree();

    std::pair<Iterator, bool>
    insert(const T & val);
    
    Iterator
    find(const typename Key_Extractor::KeyType & key);
    Iterator 
    lower_bound(const typename Key_Extractor::KeyType & key);
    
    T
    erase(const typename Key_Extractor::KeyType & key);
    T
    pop();
    
    void 
    join(ThisT & larger);
    void
    split(const typename Key_Extractor::KeyType & rb, ThisT & larger);

#ifdef BANYAN_DEBUG
    void assert_valid() const;
#endif // #ifdef BANYAN_DEBUG

protected:
    void
    remove(NodeT * m);

    void
    splay(NodeT * x);

private:
    void
    splay_it(NodeT * x);
};

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
_SplayTree<T, Key_Extractor, Metadata, LT, Allocator>::_SplayTree()
{
    DBG_ONLY(assert_valid();)
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
_SplayTree<T, Key_Extractor, Metadata, LT, Allocator>::_SplayTree(const LT & lt) :
    BaseT(lt)
{
    DBG_ONLY(assert_valid();)
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
_SplayTree<T, Key_Extractor, Metadata, LT, Allocator>::_SplayTree(T * b, T * e) :
    BaseT(b, e)
{
    DBG_ONLY(assert_valid();)
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
_SplayTree<T, Key_Extractor, Metadata, LT, Allocator>::_SplayTree(T * b, T * e, const LT & lt) :
    BaseT(b, e, lt)
{
    DBG_ONLY(assert_valid();)
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
_SplayTree<T, Key_Extractor, Metadata, LT, Allocator>::_SplayTree(
        T * b, T * e, const Metadata & md, const LT & lt) :
    BaseT(b, e, md, lt)
{
    DBG_ONLY(assert_valid();)
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
_SplayTree<T, Key_Extractor, Metadata, LT, Allocator>::~_SplayTree()
{
    // Do nothing.
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
std::pair<typename _SplayTree<T, Key_Extractor, Metadata, LT, Allocator>::Iterator, bool>
_SplayTree<T, Key_Extractor, Metadata, LT, Allocator>::insert(const T & val)
{
    if (BaseT::root == NULL) {
        NodeT * const n = BaseT::allocate();
        new (n) NodeT(val, BaseT::md);    
        BaseT::root = n;
        ++BaseT::n;
        return std::make_pair(Iterator(n), true);
    }
    
    NodeT * ins = BaseT::root;
    while (true)
        if (BaseT::lt(BaseT::extract(val), BaseT::extract(ins->val))) {
            if (ins->l == NULL) {
                NodeT * const n = BaseT::allocate();
                new (n) NodeT(val, BaseT::md);    
                ins->make_left_child(n);
                ++BaseT::n;
                n->fix_to_top();
                splay(n);
                return std::make_pair(Iterator(n), true);
            }
            ins = ins->l;
        }
        else if (!BaseT::lt(BaseT::extract(ins->val), BaseT::extract(val))) {
            splay(ins);
            return std::make_pair(ins, false);
        }
        else {
            if (ins->r == NULL) {
                NodeT * const n = BaseT::allocate();
                new (n) NodeT(val, BaseT::md);    
                ins->make_right_child(n);
                ++BaseT::n;
                n->fix_to_top();
                splay(n);
                return std::make_pair(Iterator(n), true);
            }
            ins = ins->r;
        }
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
typename _SplayTree<T, Key_Extractor, Metadata, LT, Allocator>::Iterator
_SplayTree<T, Key_Extractor, Metadata, LT, Allocator>::find(const typename Key_Extractor::KeyType & key)
{
    NodeT * n = BaseT::root;

    while (n != NULL)
        if (BaseT::lt(key, BaseT::extract(n->val)))
            n = n->l;
        else if (!BaseT::lt(BaseT::extract(n->val), key))
            break;
        else
            n = n->r;

    if (n != NULL)
        splay(n);
        
    return Iterator(n);        
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
typename _SplayTree<T, Key_Extractor, Metadata, LT, Allocator>::Iterator
_SplayTree<T, Key_Extractor, Metadata, LT, Allocator>::lower_bound(const typename Key_Extractor::KeyType & key)
{
    Iterator it = BaseT::lower_bound(key);
    
    if (it.p != NULL)
        splay(it.p);
        
    return it;        
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
void
_SplayTree<T, Key_Extractor, Metadata, LT, Allocator>::splay(NodeT * x)
{
    while (x->p != NULL)
        splay_it(x);
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
void
_SplayTree<T, Key_Extractor, Metadata, LT, Allocator>::splay_it(NodeT * x)
{
    NodeT * p = x->p;
    if (p == NULL) {
        DBG_ONLY(x->assert_valid(BaseT::lt);)
        return;
    }

    if (p == BaseT::root) {
        if (p->l == x)
            p->rotate_right();
        else
            p->rotate_left();
        BaseT::root = x;
        DBG_ONLY(x->assert_valid(BaseT::lt);)
        return;
    }

    NodeT * const pp = p->p;
    DBG_ASSERT(pp != NULL);
    NodeT * const ppp = pp->p;

    if (BaseT::root == pp) {
        DBG_ASSERT(ppp == NULL);
        BaseT::root = x;
        x->p = NULL;
    }
    else {
        x->p = ppp;
        if (pp == ppp->l)
            ppp->l = x;
        else
            ppp->r = x;
    }

    if (p->l == x) {
        if (pp->l == p) {
            pp->l = p->r;
            p->r = pp;
            p->l = x->r;
            x->r = p;
            p->p = x;
            pp->p = p;
            if (p->l != NULL)
                p->l->p = p;
            if (pp->l != NULL)
                pp->l->p = pp;
            pp->fix();
            p->fix();
            x->fix();
            DBG_ONLY(x->assert_valid(BaseT::lt));
            return;
        }

        pp->r = x->l;
        x->l = pp;
        p->l = x->r;
        x->r = p;
        pp->p = p->p = x;
        if (p->l != NULL)
            p->l->p = p;
        if (pp->r != NULL)
            pp->r->p = pp;
        pp->fix();
        p->fix();
        x->fix();
        DBG_ONLY(x->assert_valid(BaseT::lt);)
        return;
    }

    if (pp->r == p) {
        pp->r = p->l;
        p->l = pp;
        p->r = x->l;
        x->l = p;
        p->p = x;
        pp->p = p;
        if (p->r != NULL)
            p->r->p = p;
        if (pp->r != NULL)
            pp->r->p = pp;
        pp->fix();
        p->fix();
        x->fix();
        DBG_ONLY(x->assert_valid(BaseT::lt);)
        return;
    }

    pp->l = x->r;
    x->r = pp;
    p->r = x->l;
    x->l = p;
    pp->p = p->p = x;
    if (p->r != NULL)
        p->r->p = p;
    if (pp->l != NULL)
        pp->l->p = pp;
    pp->fix();
    p->fix();
    x->fix();
    DBG_ONLY(x->assert_valid(BaseT::lt);)
}

#ifdef BANYAN_DEBUG
template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
void
_SplayTree<T, Key_Extractor, Metadata, LT, Allocator>::assert_valid() const
{
    BaseT::assert_valid();
}
#endif // #ifdef BANYAN_DEBUG

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
T
_SplayTree<T, Key_Extractor, Metadata, LT, Allocator>::erase(const typename Key_Extractor::KeyType & key)
{
    NodeT * n = BaseT::root;

    while (n != NULL)
        if (BaseT::lt(key, BaseT::extract(n->val)))
            n = n->l;
        else if (!BaseT::lt(BaseT::extract(n->val), key))
            break;
        else
            n = n->r;

    if (n == NULL)
        throw std::logic_error("Key not found");
        
    const T erased_val = n->val;
    remove(n);                    
    BaseT::deallocate(n);            
    return erased_val;
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
void
_SplayTree<T, Key_Extractor, Metadata, LT, Allocator>::remove(NodeT * n)
{
    splay(n);
    DBG_ASSERT(BaseT::root == n);

    --BaseT::n;

    NodeT * const l = n->l;
    NodeT * const r = n->r;

    if (l == NULL) {
        BaseT::root = n->r;
        if(n->r != NULL)
            n->r->p = NULL;
        DBG_ONLY(assert_valid();)
        return;
    }
    if (r == NULL) {
        BaseT::root = n->l;
        if(n->l != NULL)
            n->l->p = NULL;
        DBG_ONLY(assert_valid();)
        return;
    }
    
    NodeT * const new_root = n->next();
    n->r->p = NULL;
    BaseT::root = n->r;
    splay(new_root);
    DBG_ASSERT(BaseT::root == new_root);
    DBG_ASSERT(BaseT::root->l == NULL);
    BaseT::root->l = l;
    l->p = BaseT::root;
    BaseT::root->fix();

    DBG_ONLY(assert_valid();)
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
T
_SplayTree<T, Key_Extractor, Metadata, LT, Allocator>::pop()
{
    DBG_ASSERT(BaseT::size() > 0);
    NodeT * const p = BaseT::root;
    const T popped_val = p->val;    
    remove(p);
    BaseT::deallocate(p);            
    return popped_val;
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
void
_SplayTree<T, Key_Extractor, Metadata, LT, Allocator>::join(ThisT & larger)
{
    if (BaseT::join(larger))
        return;
        
    splay(BaseT::rbegin().p);
    DBG_ASSERT(BaseT::root != NULL);
    DBG_ASSERT(larger.root != NULL);
    DBG_ASSERT(BaseT::root->r == NULL);        
    BaseT::root->r = larger.root;
    BaseT::root->r->p = BaseT::root;
    BaseT::root->fix();
    BaseT::n += larger.n;     
    larger.root = NULL;
    larger.n = 0;   

    DBG_ONLY(assert_valid();)
    DBG_ONLY(larger.assert_valid();)
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
void
_SplayTree<T, Key_Extractor, Metadata, LT, Allocator>::split(
    const typename Key_Extractor::KeyType & rb, ThisT & larger)
{
    larger.clear();

    Iterator r_it = lower_bound(rb);
    if (r_it == ThisT::end())
        return;
    
    splay(r_it.p);        
        
    larger.n = std::distance(r_it, BaseT::end());
    BaseT::n -= larger.n;
        
    larger.root = BaseT::root;
    BaseT::root = BaseT::root->l;            
    if (BaseT::root != NULL) 
        BaseT::root->p = NULL;
    larger.root->p = NULL;
    larger.root->l = NULL;
    
    if (BaseT::root != NULL)
        BaseT::root->fix();

    larger.root->fix();
    
    DBG_ONLY(assert_valid();)
    DBG_ONLY(larger.assert_valid();)
}

#endif // #ifndef _SPLAY_TREE_HPP

