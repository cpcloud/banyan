#ifndef RANK_UPDATOR_HPP
#define RANK_UPDATOR_HPP

struct _RankMetadata
{
    template<typename T>
    inline void 
    update(const T &, const _RankMetadata * l, const _RankMetadata * r)
    {
        rank = 1 + (l == NULL? 0: l->rank) + (r == NULL? 0: r->rank);
    }
    
#ifdef BANYAN_DEBUG
    void
    trace(std::ostream & os) const
    {
        os << "< " << rank << " > ";
    }
#endif // #ifdef BANYAN_DEBUG

    size_t rank;        
};

template<class Node>
const Node *
_rank_updator_kth(const Node * n, size_t k)
{
    while (true) {
        DBG_ASSERT(n != NULL);
        const Node * const l = n->l;
        const size_t left_k = l == NULL? 0: l->rank;
        if (left_k == k)
            return n;
        if (left_k > k)
            n = l;
        else {
            n = n->r;
            k = k - left_k - 1;
        }                                    
    }
}

template<class Node>
size_t
_rank_updator_order(const Node * n)
{
    DBG_ASSERT(n != NULL);
    size_t k = n->l == NULL? 0: n->l->rank;
    while (n->p != NULL) {
        DBG_ASSERT(n != NULL);
        
        if (n->p->r == n) 
            k += 1 + (n->p->l == NULL? 0: n->p->l->rank);
            
        n = n->p;                        
    }
    return k;    
}

#endif // #ifndef RANK_UPDATOR_HPP
