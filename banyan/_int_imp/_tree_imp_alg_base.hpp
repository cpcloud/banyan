#ifndef _TREE_IMP_ALG_BASE_HPP
#define _TREE_IMP_ALG_BASE_HPP

#include <Python.h>

#include "_tree_imp_mapping_base.hpp"
#include "_dsa/_red_black_tree.hpp"
#include "_dsa/_splay_tree.hpp"
#include "_dsa/_ov_tree.hpp"
#include "_dsa/_dbg.hpp"

class _RBTreeTag{};
class _SplayTreeTag{};
class _OVTreeTag{};
    
template<
    class Alg,
    typename T,
    bool Set,
    class Key_Extractor,
    class Metadata,
    class LT>
class _TreeImpAlgBase
{
    // Nothing.
};

template<
    class TreeT,
    bool Set>
class _NodeBaseTreeImpAlgBase : 
    public _TreeImpMappingBase<Set>
{
protected:
    typedef typename TreeT::KeyType KeyType;
    typedef typename TreeT::ValueType ValueType;

protected:
    _NodeBaseTreeImpAlgBase(
            typename TreeT::ValueType * b, typename TreeT::ValueType * e, 
            const typename TreeT::MetadataT & metadata, const typename TreeT::LTT & lt) :
        tree(b, e, metadata, lt),
        end_it(tree.end())
    {
        // Do nothing.  
    }    

    const typename TreeT::Iterator &
    end() const
    {
        return end_it;
    }
    
    void * 
    mem_begin(const KeyType * start, const KeyType * stop)
    {
        typename TreeT::Iterator it = start == NULL? 
            tree.begin() : 
            tree.lower_bound(*start);

        if (it.p == NULL || 
                (stop != NULL && 
                    !tree.less_than()(TreeT::KeyExtractorT::extract(it.p->val), *stop)))
            return NULL; 
        return it.p;
    }

    void * 
    mem_rbegin(const KeyType * start, const KeyType * stop)
    {
        typename TreeT::Iterator it;
        if (stop == NULL)
            it = tree.rbegin();
        else {
            it = tree.lower_bound(*stop);
            if (it.p != NULL && 
                    !tree.less_than()(TreeT::KeyExtractorT::extract(it.p->val), *stop))
                it.p = it.p->prev();                    
        }
        if (it.p == NULL || 
                (start != NULL && 
                    tree.less_than()(TreeT::KeyExtractorT::extract(it.p->val), *start)))
            return NULL;                        
        return it.p;
    }

    void * 
    mem_next(void * mem, const KeyType * stop)
    {
        DBG_ASSERT(mem != NULL);        
        
        typename TreeT::NodeT * const n =
            static_cast<typename TreeT::NodeT *>(static_cast<typename TreeT::NodeT *>(mem)->next());
            
        if (stop != NULL && n != NULL && 
                !tree.less_than()(TreeT::KeyExtractorT::extract(n->val), *stop))            
            return NULL;
        
        return n;
    }

    void * 
    mem_prev(void * mem, const KeyType * start)
    {
        DBG_ASSERT(mem != NULL);        
        
        typename TreeT::NodeT * const n =
            static_cast<typename TreeT::NodeT *>(static_cast<typename TreeT::NodeT *>(mem)->prev());
            
        if (start != NULL && n != NULL &&            
                tree.less_than()(TreeT::KeyExtractorT::extract(n->val), *start))            
            return NULL;
        
        return n;

    }
    
    const typename TreeT::ValueType &
    mem_value(void * mem)
    {
        DBG_ASSERT(mem != NULL);
        
        return static_cast<typename TreeT::NodeT *>(mem)->val;
    }

    virtual void *
    root_iter()
    {
        return tree.node_begin();
    }

    virtual void 
    delete_node_iter(void *)
    {
        // Do nothing.
    }

    virtual void *
    left_iter(void * node)
    {
        return reinterpret_cast<typename TreeT::NodeIterator>(node)->l;
    }

    virtual void *
    right_iter(void * node)
    {
        return reinterpret_cast<typename TreeT::NodeIterator>(node)->r;
    }

    const ValueType & 
    iter_value(void * node)
    {
        return reinterpret_cast<typename TreeT::NodeIterator>(node)->val;
    }
    
    typename TreeT::MetadataT &
    iter_internal_metadata(void * node)
    {
        return *reinterpret_cast<typename TreeT::NodeIterator>(node);
    }

protected:
    TreeT tree;
    
    typename TreeT::Iterator end_it;
};

template<
    typename T,
    bool Set,
    class Key_Extractor,
    class Metadata,
    class LT>
class _TreeImpAlgBase<
        _RBTreeTag,    
        T,
        Set,
        Key_Extractor,
        Metadata,
        LT> :        
    public _NodeBaseTreeImpAlgBase<        
        _RBTree<T, Key_Extractor, Metadata, LT, PyMemMallocAllocator<T> >,
        Set>
{
public:
    typedef _RBTree<T, Key_Extractor, Metadata, LT, PyMemMallocAllocator<T> > TreeT;
    
protected:
    _TreeImpAlgBase(
            T * b, T * e, const Metadata & metadata, const LT & lt) :
        _NodeBaseTreeImpAlgBase<        
                _RBTree<T, Key_Extractor, Metadata, LT, PyMemMallocAllocator<T> >, Set>(
            b, e, metadata, lt)
    {
        // Do nothing.
    }            
};

template<
    typename T,
    bool Set,
    class Key_Extractor,
    class Metadata,
    class LT>
class _TreeImpAlgBase<
        _SplayTreeTag,    
        T,
        Set,
        Key_Extractor,
        Metadata,
        LT> : 
    public _NodeBaseTreeImpAlgBase<        
        _SplayTree<T, Key_Extractor, Metadata, LT, PyMemMallocAllocator<T> >, Set>
{
public:
    typedef _SplayTree<T, Key_Extractor, Metadata, LT, PyMemMallocAllocator<T> > TreeT;
    
protected:
    _TreeImpAlgBase(
            T * b, T * e, const Metadata & metadata, const LT & lt) :
        _NodeBaseTreeImpAlgBase<        
                _SplayTree<T, Key_Extractor, Metadata, LT, PyMemMallocAllocator<T> >, Set>(
            b, e, metadata, lt)
    {
        // Do nothing.
    }            
};

template<
    typename T,
    bool Set,
    class Key_Extractor,
    class Metadata,
    class LT>
class _TreeImpAlgBase<
        _OVTreeTag,    
        T,
        Set,
        Key_Extractor,
        Metadata,
        LT> : 
    public _TreeImpMappingBase<Set>
{
public:
    typedef _OVTree<T, Key_Extractor, Metadata, LT, PyMemMallocAllocator<T> > TreeT;
    typedef typename TreeT::KeyType KeyType;
    typedef typename TreeT::ValueType ValueType;
    
protected:
    inline
    _TreeImpAlgBase(T * b, T * e, const Metadata & metadata, const LT & lt) :
        tree(b, e, metadata, lt)
    {
        // Do nothing.
    }    

    typename TreeT::Iterator
    end() 
    {
        return tree.end();
    }
    
    void * 
    mem_begin(const KeyType * start, const KeyType * stop)
    {
        typename TreeT::Iterator it = start == NULL? 
            tree.begin() : 
            tree.lower_bound(*start);

        if (it == tree.end() || 
                (stop != NULL && 
                    !tree.less_than()(TreeT::KeyExtractorT::extract(*it), *stop)))
            return NULL;                        
        return it;
    }

    void * 
    mem_rbegin(const KeyType * start, const KeyType * stop)
    {
        typename TreeT::Iterator it;
        if (stop == NULL)
            it = tree.rbegin();
        else {
            it = tree.lower_bound(*stop);
            if (it != tree.end() && 
                    !tree.less_than()(TreeT::KeyExtractorT::extract(*it), *stop))
                --it;
        }
        if (it == tree.end() || 
                (start != NULL && 
                    tree.less_than()(TreeT::KeyExtractorT::extract(*it), *start)))
            return NULL;                        
        return it;
    }

    void * 
    mem_next(void * mem, const KeyType * stop)
    {
        DBG_ASSERT(mem != NULL);        
        
        typename TreeT::Iterator it = static_cast<typename TreeT::Iterator>(mem) + 1;
        if (stop != NULL && it != tree.end() && 
                !tree.less_than()(TreeT::KeyExtractorT::extract(*it), *stop))            
            return NULL;
        
        return it == tree.end()? NULL: it;
    }

    void * 
    mem_prev(void * mem, const KeyType * start)
    {
        DBG_ASSERT(mem != NULL);        
        
        typename TreeT::Iterator it = static_cast<typename TreeT::Iterator>(mem) - 1;
        if (start != NULL && it != tree.rend() && 
                tree.less_than()(TreeT::KeyExtractorT::extract(*it), *start))            
            return NULL;
        
        return it == tree.rend()? NULL: it;
    }
    
    const typename TreeT::ValueType &
    mem_value(void * mem)
    {
        DBG_ASSERT(mem != NULL);

        return *static_cast<typename TreeT::Iterator>(mem);
    }

    virtual void *
    root_iter()
    {
        if (tree.size() == 0)
            return NULL;

        typename TreeT::NodeIterator * const it = alloc.allocate(1);
        if (it == NULL)
            throw std::bad_alloc();
        new (it) typename TreeT::NodeIterator(tree.node_begin());
            
        return it;                
    }

    virtual void 
    delete_node_iter(void * it)
    {
        alloc.deallocate(static_cast<typename TreeT::NodeIterator *>(it), 1);
    }

    virtual void *
    left_iter(void * it)
    {
        typename TreeT::NodeIterator left = reinterpret_cast<typename TreeT::NodeIterator *>(it)->left();
        if (left == tree.node_end())
            return NULL;

        typename TreeT::NodeIterator * const left_it = alloc.allocate(1);
        if (left_it == NULL)
            throw std::bad_alloc();
        new (left_it) typename TreeT::NodeIterator(left);

        return left_it;
    }

    virtual void *
    right_iter(void * it)
    {
        typename TreeT::NodeIterator right = reinterpret_cast<typename TreeT::NodeIterator *>(it)->right();
        if (right == tree.node_end())
            return NULL;

        typename TreeT::NodeIterator * const right_it = alloc.allocate(1);
        if (right_it == NULL)
            throw std::bad_alloc();
        new (right_it) typename TreeT::NodeIterator(right);

        return right_it;
    }

    const ValueType & 
    iter_value(void * it)
    {
        return tree.node_value(*reinterpret_cast<typename TreeT::NodeIterator *>(it));
    }
    
    Metadata &
    iter_internal_metadata(void * it)
    {
        return tree.node_metadata(*reinterpret_cast<typename TreeT::NodeIterator *>(it));
    }
    
    size_t
    iter_size(void * it)
    {
        return reinterpret_cast<typename TreeT::NodeIterator *>(it)->size();
    }

private:        
    typedef typename TreeT::NodeIterator NodeIteratorT;
    typedef PyMemMallocAllocator<NodeIteratorT> ItAllocT;
    
protected:
    TreeT tree;
    
    static ItAllocT alloc;
};

template<typename T, bool Set, class Key_Extractor, class Metadata, class LT>
typename _TreeImpAlgBase<_OVTreeTag, T, Set, Key_Extractor, Metadata, LT>::ItAllocT
    _TreeImpAlgBase<_OVTreeTag, T, Set, Key_Extractor, Metadata, LT>::alloc;
    
#endif // #ifndef _TREE_IMP_ALG_BASE_HPP

