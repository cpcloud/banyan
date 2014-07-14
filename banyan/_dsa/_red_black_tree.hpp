#ifndef _RED_BLACK_TREE_HPP
#define _RED_BLACK_TREE_HPP

#include <exception>
#include <vector>

#include "_node_based_binary_tree.hpp"
#include "_red_black_tree_node.hpp"

template<
    typename T,
    class Key_Extractor,
    class Metadata = _NullMetadata,
    class LT = std::less<typename Key_Extractor::KeyType>,
    class Allocator = std::allocator<T> >
class _RBTree :
    public _NodeBasedBinaryTree<
        T, 
        Key_Extractor, 
        Metadata, 
        LT, 
        Allocator,
        RBNode<T, Key_Extractor, Metadata> >
{
public:
    typedef 
        _NodeBasedBinaryTree<
            T, 
            Key_Extractor, 
            Metadata, 
            LT, 
            Allocator, 
            RBNode<T, Key_Extractor, Metadata> > 
        BaseT;
    typedef RBNode<T, Key_Extractor, Metadata> NodeT;
    typedef typename BaseT::Iterator Iterator;
    typedef _RBTree<T, Key_Extractor, Metadata, LT, Allocator> ThisT;

public:
    explicit
    _RBTree();
    explicit
    _RBTree(const LT & lt);
    explicit
    _RBTree(T * b, T * e);
    explicit
    _RBTree(T * b, T * e, const LT & lt);
    explicit
    _RBTree(T * b, T * e, const Metadata & md, const LT & lt);

    virtual
    ~_RBTree();

    std::pair<Iterator, bool>
    insert(const T & val);
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
    // Tmp Ami - doesn't compile outside on MSVC, for some reason.
    static inline bool
    is_black(const typename BaseT::NodeT * p)
    {
        return p == NULL || static_cast<const NodeT *>(p)->clr == NodeT::black;
    }
    
private:
    typedef std::vector<NodeT *, typename Allocator::template rebind<Metadata>::other> NodePtrVecT;    

private:
    void
    ins_fixup(NodeT * n);

    NodeT *
    ins_fixup_it(NodeT * n);

    void
    remove(NodeT * m);

    void
    rmv_fixup(NodeT *p, NodeT * s);
    
    void
    init_elem_nodes(NodeT * p, size_t num_elems);
    void
    init_elem_nodes(NodeT * p, size_t num_elems, NodePtrVecT & nodes);
    
    void 
    join(NodeT * x, ThisT & r);

    void
    split_join(NodeT * p, ThisT & larger, bool left);
};

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
_RBTree<T, Key_Extractor, Metadata, LT, Allocator>::_RBTree()
{
    DBG_ONLY(assert_valid();)
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
_RBTree<T, Key_Extractor, Metadata, LT, Allocator>::_RBTree(const LT & lt) :
    BaseT(lt)
{
    DBG_ONLY(assert_valid();)
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
_RBTree<T, Key_Extractor, Metadata, LT, Allocator>::_RBTree(T * b, T * e) :
    BaseT(b, e)
{
    DBG_ASSERT(BaseT::n != static_cast<size_t>(-1));  
    init_elem_nodes(static_cast<NodeT *>(BaseT::root), BaseT::n);
    DBG_ONLY(assert_valid();)
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
_RBTree<T, Key_Extractor, Metadata, LT, Allocator>::_RBTree(T * b, T * e, const LT & lt) :
    BaseT(b, e, lt)
{
    DBG_ASSERT(BaseT::n != static_cast<size_t>(-1));  
    init_elem_nodes(static_cast<NodeT *>(BaseT::root), BaseT::n);
    DBG_ONLY(assert_valid();)
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
_RBTree<T, Key_Extractor, Metadata, LT, Allocator>::_RBTree(
        T * b, T * e, const Metadata & md, const LT & lt) :
    BaseT(b, e, md, lt)
{
    DBG_ASSERT(BaseT::n != static_cast<size_t>(-1));  
    init_elem_nodes(static_cast<NodeT *>(BaseT::root), BaseT::n);
    DBG_ONLY(assert_valid();)
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
void
_RBTree<T, Key_Extractor, Metadata, LT, Allocator>::init_elem_nodes(
    NodeT * p, size_t num_elems)
{    
    NodePtrVecT nodes;
    nodes.reserve(BaseT::n);
    init_elem_nodes(static_cast<NodeT *>(BaseT::root), BaseT::n, nodes);
    DBG_ASSERT(nodes.size() == BaseT::n);
    if (BaseT::root != NULL)
        static_cast<NodeT *>(BaseT::root)->clr = NodeT::black;
    if (nodes.empty())        
        return;
    for (size_t i = 0; i < nodes.size() - 1; ++i) {
        DBG_ASSERT(!BaseT::less_than()(
            BaseT::extract(nodes[i + 1]->val),
            BaseT::extract(nodes[i]->val)));
        nodes[i]->n = nodes[i + 1];        
    }        
    (*nodes.rbegin())->n = NULL;    
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
void
_RBTree<T, Key_Extractor, Metadata, LT, Allocator>::init_elem_nodes(
    NodeT * p, size_t num_elems, NodePtrVecT & nodes)
{
    if (p == NULL)
        return;
    
    DBG_ASSERT(num_elems > 0);
    init_elem_nodes(static_cast<NodeT *>(p->l), num_elems >> 1, nodes);
    nodes.push_back(p);
    init_elem_nodes(static_cast<NodeT *>(p->r), num_elems >> 1, nodes);

    p->clr = num_elems == 1? NodeT::red : NodeT::black;
        
    DBG_ASSERT(p->clr == NodeT::black || is_black(p->l));        
    DBG_ASSERT(p->clr == NodeT::black || is_black(p->r));        
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
_RBTree<T, Key_Extractor, Metadata, LT, Allocator>::~_RBTree()
{
    // Do nothing.
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
std::pair<typename _RBTree<T, Key_Extractor, Metadata, LT, Allocator>::Iterator, bool>
_RBTree<T, Key_Extractor, Metadata, LT, Allocator>::insert(const T & val)
{
    typename BaseT::NodeT * n = BaseT::root;
    typename BaseT::NodeT * pot = NULL;
    typename BaseT::NodeT * prev = NULL;
    while (n != NULL) {
        prev = n;
        if (BaseT::lt(BaseT::extract(val), BaseT::extract(n->val)))
            n = n->l;
        else {
            pot = n;
            n = n->r;
        }
    }

    if (pot != NULL && !BaseT::lt(BaseT::extract(pot->val), BaseT::extract(val)))
        return std::make_pair(Iterator(pot), false);

    NodeT * const nw = BaseT::allocate();
    new (nw) NodeT(val, NodeT::red, BaseT::md);     
    
    if (pot != NULL) {
        nw->n = static_cast<NodeT *>(pot)->n;
        static_cast<NodeT *>(pot)->n = nw;
    }
    else
        nw->n = static_cast<NodeT *>(prev);

    if (prev == NULL) {
        DBG_ASSERT(BaseT::root == NULL);
        BaseT::root = nw;
        static_cast<NodeT *>(BaseT::root)->clr = NodeT::black;
        DBG_ASSERT(BaseT::n != static_cast<size_t>(-1));  
        ++BaseT::n;
        nw->n = NULL;
        DBG_ONLY(assert_valid(););
        return std::make_pair(Iterator(nw), true);
    }

    if (BaseT::lt(BaseT::extract(val), BaseT::extract(prev->val))) {
        DBG_ASSERT(prev->l == NULL);
        prev->make_left_child(nw);
        prev->fix_to_top();
    }
    else {
        DBG_ASSERT(prev->r == NULL);
        prev->make_right_child(nw);
        prev->fix_to_top();
    }

    ++BaseT::n;

    static_cast<NodeT *>(BaseT::root)->clr = NodeT::black;

    ins_fixup(nw);

    DBG_ONLY(assert_valid(););

    return std::make_pair(Iterator(nw), true);
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
void
_RBTree<T, Key_Extractor, Metadata, LT, Allocator>::ins_fixup(NodeT * n)
{
    DBG_ONLY(n->assert_valid(BaseT::lt);)
    while ((n = ins_fixup_it(n)) != NULL)
        DBG_ONLY(n->assert_valid(BaseT::lt));
    DBG_ONLY(static_cast<NodeT *>(BaseT::root)->assert_valid(BaseT::lt);)
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
typename _RBTree<T, Key_Extractor, Metadata, LT, Allocator>::NodeT *
_RBTree<T, Key_Extractor, Metadata, LT, Allocator>::ins_fixup_it(NodeT * n)
{
    DBG_ASSERT(n->clr == NodeT::red);

    NodeT * p_ = static_cast<NodeT *>(n->p);

    if (p_ == NULL) {
        DBG_VERIFY(static_cast<NodeT *>(BaseT::root) == n);
        n->clr = NodeT::black;
        DBG_ONLY(n->assert_valid(BaseT::lt);)
        return NULL;
    }

    if (p_->clr == NodeT::black) {
        DBG_ONLY(n->assert_valid(BaseT::lt);)
        p_->fix_to_top();
        return NULL;
    }

    NodeT * const g = static_cast<NodeT *>(p_->p);
    DBG_ASSERT(g != NULL);
    NodeT * const u = static_cast<NodeT *>(g->l == p_? g->r : g->l);

    if (u != NULL && u->clr == NodeT::red) {
        p_->clr = u->clr = NodeT::black;
        g->clr = NodeT::red;
        DBG_ONLY(g->assert_valid(BaseT::lt);)
        return g;
    }

    if (g->l == p_) {
        if (p_->r == n) {
            p_->rotate_left();
            std::swap(p_, n);
        }

        g->rotate_right();

        p_->clr = NodeT::black;
        n->clr = g->clr = NodeT::red;
    }
    else {
        if (p_->l == n) {
            p_->rotate_right();
            std::swap(p_, n);
        }

        g->rotate_left();

        p_->clr = NodeT::black;
        n->clr = g->clr = NodeT::red;
    }

    if (p_->p == NULL)
        BaseT::root = p_;

    DBG_ONLY(p_->assert_valid(BaseT::lt);)

    return NULL;
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
T
_RBTree<T, Key_Extractor, Metadata, LT, Allocator>::pop()
{
    DBG_ASSERT(BaseT::size() > 0);
    NodeT * const p = static_cast<NodeT *>(BaseT::begin().p);
    const T popped_val = p->val;    
    remove(p);
    BaseT::deallocate(p);            
    return popped_val;
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
T
_RBTree<T, Key_Extractor, Metadata, LT, Allocator>::erase(const typename Key_Extractor::KeyType & key)
{
    NodeT * n = static_cast<NodeT *>(BaseT::root);
    NodeT * pot = NULL;
    while (n != NULL)
        if (BaseT::lt(key, BaseT::extract(n->val)))
            n = static_cast<NodeT *>(n->l);
        else {
            pot = n;
            n = static_cast<NodeT *>(n->r);
        }

    if (pot == NULL || BaseT::lt(BaseT::extract(pot->val), key))
        throw std::logic_error("Key not found");
        
    NodeT * const prev = static_cast<NodeT *>(pot->prev());        
        
    if (pot->l != NULL && pot->r != NULL) {    
        NodeT * const next = static_cast<NodeT *>(pot->n);
        DBG_ASSERT(next->l == NULL);
        BaseT::swap(pot, next);
        std::swap(pot->clr, next->clr);
        DBG_ASSERT(pot->l == NULL);
    }        
    
    if (prev != NULL)
        prev->n = pot->n;

    const T erased_val = pot->val;
    remove(pot);
    BaseT::deallocate(pot);

    DBG_ONLY(assert_valid();)
    
    return erased_val;
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
void
_RBTree<T, Key_Extractor, Metadata, LT, Allocator>::remove(NodeT * m)
{
    if (BaseT::n != static_cast<size_t>(-1))
        --BaseT::n;

    if(BaseT::root->l == NULL && BaseT::root->r == NULL) {
        BaseT::root = NULL;
        DBG_ONLY(assert_valid();)
        return;
    }

    DBG_ASSERT(m->l == NULL || m->r == NULL);
    NodeT * const c = static_cast<NodeT *>(m->l == NULL? m->r : m->l);

    if (m->p == NULL) {
        DBG_ASSERT(BaseT::root == m);
        BaseT::root = c;
        if (c != NULL) {
            c->p = NULL;
            c->clr = NodeT::black;
        }
        DBG_ONLY(assert_valid();)
        return;
    }
    bool was_left = true;
    if (m->p->l == m)
        m->p->l = c;
    else {
        was_left = false;
        m->p->r = c;
    }
    if (c != NULL)
        c->p = m->p;
    m->p->fix_to_top();
    DBG_ONLY(BaseT::assert_valid();)

    if (m->clr == NodeT::red) {
        DBG_ASSERT(is_black(c));
        DBG_ONLY(assert_valid());
        return;
    }

    if(c != NULL && c->clr == NodeT::red) {
        c->clr = NodeT::black;
        DBG_ONLY(assert_valid());
        return;
    }
    
    static_cast<NodeT *>(BaseT::root)->clr = NodeT::black;

    DBG_ASSERT(m->p != NULL);
    DBG_ASSERT(m->l == NULL && m->r == NULL);
    if (was_left) {
        m->p->l = NULL;
        rmv_fixup(static_cast<NodeT *>(m->p), static_cast<NodeT *>(m->p->r));
    }
    else {
        m->p->r = NULL;
        rmv_fixup(static_cast<NodeT *>(m->p), static_cast<NodeT *>(m->p->l));
    }
    
    DBG_ONLY(assert_valid();)
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
void
_RBTree<T, Key_Extractor, Metadata, LT, Allocator>::rmv_fixup(NodeT *p, NodeT * s)
{
    DBG_ONLY(BaseT::assert_valid();)
    DBG_ASSERT(p != NULL);
    DBG_ASSERT(p->l == s || p->r == s);
    DBG_ASSERT(s == NULL || s->p == p);
#ifdef BANYAN_DEBUG
    if (p->l == s) {
        DBG_ASSERT(NodeT::black_height(s) - 1 == 
            NodeT::black_height(static_cast<NodeT *>(p->r)));    
    }
    else {
        DBG_ASSERT(NodeT::black_height(s) - 1== 
            NodeT::black_height(static_cast<NodeT *>(p->l)));    
    }
    
    if (s != NULL)
        s->assert_valid(BaseT::lt);
#endif // #ifdef BANYAN_DEBUG

    if (!is_black(s)) {
        DBG_ASSERT(p->clr == NodeT::black);
        p->clr = NodeT::red;
        s->clr = NodeT::black;
        if (p->r == s) {
            p->rotate_left();
            if (s->p == NULL)
                BaseT::root = s;
            s = static_cast<NodeT *>(p->r);
        }
        else {
            p->rotate_right();
            if (s->p == NULL)
                BaseT::root = s;
            s = static_cast<NodeT *>(p->l);
        }
    }
    else if (is_black(p) && is_black(s->l) && is_black(s->r)) {
        DBG_ASSERT(s != NULL);
        s->clr = NodeT::red;
        DBG_ONLY(p->assert_valid(BaseT::lt);)
        NodeT * const pp = static_cast<NodeT *>(p->p);
        if (pp == NULL) 
            return;
        rmv_fixup(
            static_cast<NodeT *>(pp), 
            static_cast<NodeT *>(pp->l == p? pp->r : pp->l));
        return;
    }
    
    if(p->clr == NodeT::red && 
            s != NULL && s->clr == NodeT::black && 
            is_black(s->l) && is_black(s->r)) {
        p->clr = NodeT::black;
        if (s != NULL)
            s->clr = NodeT::red;
        DBG_ONLY(p->assert_valid(BaseT::lt);)
        return;
    }
    
    DBG_ASSERT(is_black(s) && (!is_black(s->l) || !is_black(s->r)));
    
    if (p->r == s && !is_black(s->l) && is_black(s->r)) {
        s->rotate_right();
        std::swap(s->clr, static_cast<NodeT *>(s->p)->clr);
        DBG_ONLY(s->assert_valid(BaseT::lt);)
        s = static_cast<NodeT *>(s->p);
    }
    else if (p->l == s && !is_black(s->r) && is_black(s->l)) {
        s->rotate_left();
        std::swap(s->clr, static_cast<NodeT *>(s->p)->clr);
        DBG_ONLY(s->assert_valid(BaseT::lt);)
        s = static_cast<NodeT *>(s->p);
    }
        
    DBG_ASSERT(is_black(s) && (!is_black(s->l) || !is_black(s->r)));
    
    if (p->r == s) {
        DBG_ASSERT(!is_black(s->r));
        p->rotate_left();
        std::swap(p->clr, s->clr);
        DBG_ASSERT(s->r != NULL);
        ((NodeT *)s->r)->clr = NodeT::black;
    }
    else {
        DBG_ASSERT(!is_black(s->l));
        p->rotate_right();
        std::swap(p->clr, s->clr);
        DBG_ASSERT(s->l != NULL);
        ((NodeT *)s->l)->clr = NodeT::black;
    }
    DBG_ONLY(s->assert_valid(BaseT::lt);)
    if (s->p == NULL)
        BaseT::root = s;
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
void
_RBTree<T, Key_Extractor, Metadata, LT, Allocator>::join(ThisT & larger)
{
    if (BaseT::join(larger))
        return;
        
    NodeT * const x = static_cast<NodeT *>(larger.begin().p);                    
    larger.remove(x);
    join(x, larger); 
    
    DBG_ONLY(assert_valid();)     
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
void 
_RBTree<T, Key_Extractor, Metadata, LT, Allocator>::join(NodeT * x, ThisT & larger)
{    
    DBG_ASSERT(x != NULL);
    x->clr = NodeT::red;
    
    DBG_ONLY(assert_valid());
    DBG_ONLY(larger.assert_valid());
    
    if (BaseT::root == NULL && larger.root == NULL) {
        BaseT::root = x;
        x->clr = NodeT::black;
        x->l = x->r = x->p = NULL;
        x->n  = NULL;
        if (BaseT::n != static_cast<size_t>(-1))
            BaseT::n = 1;    
        DBG_ONLY(assert_valid();)
        DBG_ONLY(larger.assert_valid();)
        return;
    }    
    if (larger.root == NULL) {
        NodeT * p = static_cast<NodeT *>(BaseT::root);
        while (p->r != NULL)
            p = static_cast<NodeT *>(p->r);
        DBG_ASSERT(BaseT::less_than()(Key_Extractor::extract(p->val), Key_Extractor::extract(x->val)));
        p->r = x;
        x->p = p;
        x->l = x->r = NULL;
        DBG_ASSERT(p->n == NULL);
        p->n = x;
        x->n = NULL;
        x->fix_to_top();
        ins_fixup(x);
        if (BaseT::n != static_cast<size_t>(-1))
            ++BaseT::n;
        DBG_ONLY(assert_valid();)
        DBG_ONLY(larger.assert_valid();)
        return;            
    }             
    if (BaseT::root == NULL) {
        NodeT * p = static_cast<NodeT *>(larger.root);
        while (p->l != NULL)
            p = static_cast<NodeT *>(p->l);
        DBG_ASSERT(BaseT::less_than()(Key_Extractor::extract(x->val), Key_Extractor::extract(p->val)));
        p->l = x;
        x->p = p;
        x->l = x->r = NULL;
        x->n = p;
        x->fix_to_top();
        larger.ins_fixup(x);
        if (larger.n != static_cast<size_t>(-1))
            ++larger.n;
        BaseT::swap(larger);
        DBG_ONLY(assert_valid();)
        DBG_ONLY(larger.assert_valid();)
        return;            
    }            

    const size_t lbh = NodeT::black_height(static_cast<NodeT *>(BaseT::root));
    const size_t rbh = NodeT::black_height(static_cast<NodeT *>(larger.root));    

    if (lbh >= rbh) {                               
        NodeT * p = static_cast<NodeT *>(BaseT::root);         
        size_t bh = lbh;
        while (true) {            
            if (is_black(p)) 
                if (bh-- == rbh)
                    break;
            p = static_cast<NodeT *>(p->r);                
        }
        DBG_ASSERT(p != NULL);
        
        NodeT * x_prev = p;
        while (x_prev->r != NULL)
            x_prev = static_cast<NodeT *>(x_prev->r);
        x_prev->n = x;        
        
        x->p = p->p;            
        x->n = static_cast<NodeT *>(larger.begin().p);
        if (p->p == NULL) 
            BaseT::root = x;
        else {
            p->p->r = x;
            NodeT * pp_next = p;
            while (pp_next->l != NULL)
                pp_next = static_cast<NodeT *>(pp_next->l);
            static_cast<NodeT *>(p->p)->n = static_cast<NodeT *>(pp_next);      
        }            
        
        DBG_ASSERT(p != NULL && is_black(p));        
        x->l = p;
        x->l->p = x;
        DBG_ASSERT(larger.root != NULL && is_black(larger.root));
        x->r = larger.root;
        x->r->p = x;
        x->fix_to_top();
        
        if (BaseT::n != static_cast<size_t>(-1) && larger.n != static_cast<size_t>(-1))
            BaseT::n += larger.n + 1;
        else 
            BaseT::n = larger.n = static_cast<size_t>(-1);            
    
        ins_fixup(x);
        
        larger.root = NULL;
        larger.n = 0;        

        DBG_ONLY(assert_valid(););
        return;
    }

    NodeT * p = static_cast<NodeT *>(larger.root);         
    size_t bh = rbh;
    while (true) {
        if (is_black(p)) 
            if (bh-- == lbh)
                break;
        p = static_cast<NodeT *>(p->l);                
    }
    DBG_ASSERT(p != NULL && is_black(p));        

    static_cast<NodeT *>(BaseT::rbegin().p)->n = static_cast<NodeT *>(x);    
    x->p = p->p;
    
    if (p->p == NULL) 
        larger.root = x;
    else 
        p->p->l = x;
    
    x->r = p;    
    NodeT * x_next = p;
    while (x_next->l != NULL)
        x_next = static_cast<NodeT *>(x_next->l);
    x->n = x_next;
    x->r->p = x;
    DBG_ASSERT(BaseT::root != NULL && is_black(BaseT::root));
    x->l = BaseT::root;
    x->l->p = x;
    x->fix_to_top();
    
    if (larger.n != static_cast<size_t>(-1) && BaseT::n != static_cast<size_t>(-1))
        larger.n += BaseT::n + 1;

    larger.ins_fixup(x);
    
    BaseT::root = NULL;
    if (BaseT::n != static_cast<size_t>(-1))
        BaseT::n = 0;    
    BaseT::swap(larger);

    DBG_ONLY(assert_valid();)
    DBG_ONLY(larger.assert_valid();)
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
void
_RBTree<T, Key_Extractor, Metadata, LT, Allocator>::split(
    const typename Key_Extractor::KeyType & rb, ThisT & larger)
{
    DBG_ONLY(assert_valid();)
    DBG_ONLY(larger.assert_valid();)
    larger.clear();

    Iterator r_it = BaseT::lower_bound(rb);
    if (r_it == ThisT::end())
        return;

    BaseT::n = larger.n = static_cast<size_t>(-1);        
        
    NodeT * const p = static_cast<NodeT *>(r_it.p);
    
    BaseT::root = p->l;
    p->l = NULL;
    p->fix_to_top();
    if (BaseT::root != NULL) {
        BaseT::root->p = NULL;
        static_cast<NodeT *>(BaseT::root)->clr = NodeT::black;
    } 
    NodeT * const last = static_cast<NodeT *>(BaseT::rbegin().p);
    if (last != NULL)
        last->n = NULL;
    
    split_join(p, larger, true);

    DBG_ONLY(assert_valid();)
    DBG_ONLY(larger.assert_valid();)
}

template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
void
_RBTree<T, Key_Extractor, Metadata, LT, Allocator>::split_join(
    NodeT * p, ThisT & larger, bool left)
{
    if (p == NULL)
        return;
        
    DBG_ONLY(assert_valid();)
    DBG_ONLY(larger.assert_valid();)
    DBG_ASSERT(p->l == NULL || p->l->p == p);
    DBG_ASSERT(p->r == NULL || p->r->p == p);
    
    NodeT * const pp = static_cast<NodeT *>(p->p);
    const bool p_left = pp == NULL || p == pp->l;
    if (pp != NULL) {
        *(p_left? &pp->l: &pp->r) = NULL;
        pp->fix();
    }

    if (left) {
        ThisT tmp(NULL, NULL, BaseT::meta(), BaseT::less_than());        
        tmp.root = p->r;
        tmp.n = static_cast<size_t>(-1);
        if (tmp.root != NULL) {
            tmp.root->p = NULL;
            static_cast<NodeT *>(tmp.root)->clr = NodeT::black;
            if (tmp.root->r == NULL)
                static_cast<NodeT *>(tmp.root)->n = NULL;
            static_cast<NodeT *>(tmp.rbegin().p)->n = NULL;
        }            
        p->r = NULL;  
        p->fix();
        larger.join(p, tmp); 
        larger.n = static_cast<size_t>(-1);
    }
    else {
        ThisT tmp(NULL, NULL, BaseT::meta(), BaseT::less_than());        
        tmp.root = p->l;
        tmp.n = static_cast<size_t>(-1);
        if (tmp.root != NULL) {
            tmp.root->p = NULL;
            static_cast<NodeT *>(tmp.root)->clr = NodeT::black;
            static_cast<NodeT *>(tmp.rbegin().p)->n = NULL;
        }            
        p->l = NULL;  
        p->fix();
        tmp.join(p, *this);
        std::swap(BaseT::root, tmp.root);
        BaseT::n = static_cast<size_t>(-1);        
    }
    
    DBG_ONLY(assert_valid();)
    DBG_ONLY(larger.assert_valid();)

    split_join(pp, larger, p_left);
}

#ifdef BANYAN_DEBUG
template<typename T, class Key_Extractor, class Metadata, class LT, class Allocator>
void
_RBTree<T, Key_Extractor, Metadata, LT, Allocator>::assert_valid() const
{
    BaseT::assert_valid();
    if (BaseT::root != NULL) {
        static_cast<const NodeT *>(BaseT::root)->assert_valid(BaseT::lt);
        DBG_VERIFY(static_cast<const NodeT *>(BaseT::root)->clr == NodeT::black);
    }        
}
#endif // #ifdef BANYAN_DEBUG

#endif // #ifndef _RED_BLACK_TREE_HPP

