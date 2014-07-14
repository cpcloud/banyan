#include <cstdlib>
#include <algorithm>
#include <vector>
#include <set>
#include <climits>

#include "_dbg.hpp"
#include "_key_extractor.hpp"
#include "_red_black_tree.hpp"
#include "_splay_tree.hpp"
#include "_ov_tree.hpp"
#include "_rank_updator.hpp"
#include "_min_gap_updator.hpp"

using namespace std;

typedef
    _KeyExtractor<int>
    SetKeyExtractor;

typedef
    _MinGapMetadata<
        int>
    MinGapMetadata;                                

static int 
rand_shuffle_rand(int i) 
{ 
    return rand() % i;
}

template<class Set>
void 
set_verify(Set & u, const set<int> & v)
{
    DBG_VERIFY(distance(u.begin(), u.end()) == v.size());    
    for (typename Set::Iterator it = u.begin(); it != u.end(); ++it)
        DBG_VERIFY(v.find(*it) != v.end());            
        
    set_verify_metadata(u, typename Set::MetadataT());        
}

template<class Set>
void 
set_verify_metadata(Set & u, _RankMetadata m)
{
    set_verify_metadata(u, m, u.node_begin());
}

template<class Set>
void 
set_verify_metadata(Set & u, _RankMetadata m, typename Set::NodeIterator it)
{
    if (it == u.node_end())
        return;

    typename Set::NodeIterator left = it->left();
    set_verify_metadata(u, m, left);
    typename Set::NodeIterator right = it->right();
    set_verify_metadata(u, m, right);
    DBG_VERIFY(it->rank == 
        1 + 
        (left == u.node_end()? 0: left->rank) + 
        (right == u.node_end()? 0: right->rank));
}

template<
    typename T,
    class Key_Extractor,
    class LT,
    class Allocator>
void 
set_verify_metadata(
    _OVTree<T, Key_Extractor, _RankMetadata, LT, Allocator> & u, 
    _RankMetadata m, 
    typename _OVTree<T, Key_Extractor, _RankMetadata, LT, Allocator>::NodeIterator it)
{
    if (it == u.node_end())
        return;
        
    typedef
        _OVTree<T, Key_Extractor, _RankMetadata, LT, Allocator>
        SetT;        

    typename SetT::NodeIterator left = it.left();
    set_verify_metadata(u, m, left);
    typename SetT::NodeIterator right = it.right();
    set_verify_metadata(u, m, right);
    DBG_VERIFY(u.node_metadata(it).rank == 
        1 + 
        (left == u.node_end()? 0: u.node_metadata(left).rank) + 
        (right == u.node_end()? 0: u.node_metadata(right).rank));
}

template<class Set>
void 
set_verify_metadata(Set & u, _NullMetadata)
{
    // Do nothing
}

template<class Set>
void 
set_verify_metadata(Set & u, MinGapMetadata)
{
    if (u.size() < 2 || u.size() == static_cast<size_t>(-1))
        return;                   
        
    const int min_gap = u.node_begin()->min_gap;        
    int mg = INT_MAX;
    for (typename Set::Iterator it = u.begin(); ; ++it) {
        typename Set::Iterator next_it = it;
        ++next_it;
        if (next_it == u.end())
            break;
        mg = std::min<int>(abs(*it - *next_it), mg);        
    }
    
    DBG_VERIFY(mg == min_gap);    
}

template<
    typename T,
    class Key_Extractor,
    class LT,
    class Allocator>
void 
set_verify_metadata(
    _OVTree<T, Key_Extractor, MinGapMetadata, LT, Allocator> & u, 
    MinGapMetadata)
{
    // Tmp Ami - complete.
}

template<class Set>
static void
set_lower_bound_test()
{
    Set u;
    
    u.insert(0);
    u.insert(2);
    u.insert(4);
    u.insert(6);
    
    DBG_VERIFY(*u.lower_bound(-1) == 0);
    DBG_VERIFY(*u.lower_bound(0) == 0);
    DBG_VERIFY(*u.lower_bound(1) == 2);
    DBG_VERIFY(*u.lower_bound(2) == 2);
    DBG_VERIFY(*u.lower_bound(3) == 4);
    DBG_VERIFY(*u.lower_bound(4) == 4);
    DBG_VERIFY(*u.lower_bound(5) == 6);
    DBG_VERIFY(*u.lower_bound(6) == 6);
    DBG_VERIFY(u.lower_bound(7) == u.end());
}

template<class Set>
static void
set_insert_test()
{
    Set u;
    set<int> v;

    for (size_t i = 0; i < 1000; ++i) {
        const size_t j = rand() % 10000;

        u.insert(j);
        v.insert(j);
        
        set_verify(u, v);
    }
}

template<class Set>
static void
set_items_create_test()
{
    set<int> v;
    vector<int> all;    
    const size_t num = 15;
    for (size_t i = 0; i < num; ++i) {
        const size_t j = rand() % num;
        
        if (find(all.begin(), all.end(), j) == all.end())
            all.push_back(j);        

        v.insert(j);
    }        
    
    sort(all.begin(), all.end());
    DBG_ASSERT(!all.empty());
    Set u(&all[0], &all[0] + all.size());
    
    set_verify(u, v);
}

template<class Set>
static void
set_insert_erase_test()
{
    Set u;
    set<int> v;
    vector<int> all;

    for (size_t i = 0; i < 120; ++i) {
        const size_t j = rand() % 10000;

        u.insert(j);
        v.insert(j);
        
        set_verify(u, v);
        
        if (find(all.begin(), all.end(), j) == all.end())
            all.push_back(j);        
    }

    random_shuffle(all.begin(), all.end(), rand_shuffle_rand);
    for (size_t i = 0; i < all.size(); ++i) {
        u.erase(all[i]);
        v.erase(all[i]);

        set_verify(u, v);
    }   
}

template<class Set>
static void
set_join_test()
{
    const size_t max_tree_size = 100;
    for (size_t i = 0; i < 1000; ++i) {
        Set ls;
        for (size_t i = 0; i < rand() % max_tree_size; ++i)
            ls.insert(i);
        Set rs;
        for (size_t i = 0; i < rand() % max_tree_size; ++i)
            rs.insert(ls.size() + i);
        ls.join(rs);        
        
        set_verify_metadata(ls, typename Set::MetadataT());        
        set_verify_metadata(rs, typename Set::MetadataT());        
    }        
}

template<class Set>
static void
set_split_test()
{
    for (size_t i = 0; i < 100; ++i) {
        Set s;
        const size_t num = rand() % 1000;
        for (size_t i = 0; i < num; ++i) 
            s.insert(rand() % 100);
            
        Set rs;
        const size_t p = rand() % 100;

        const set<int> l(s.begin(), s.lower_bound(p)), r(s.lower_bound(p), s.end());

        s.split(p, rs);

        set_verify(s, l);
        set_verify(rs, r);
    }        
}

template<class Set>
static void 
set_testsuite(int seed)
{
    srand(seed);

    set_insert_test<Set>();
    set_items_create_test<Set>();
    set_insert_erase_test<Set>();
    set_lower_bound_test<Set>();
    set_join_test<Set>(); 
    set_split_test<Set>(); 
}

int 
main(int argc, char * argv[])
{
    if (argc != 2) {
        cerr << "usage: test <seed>" << endl;
        return -1;
    }
    
    const int seed = atoi(argv[1]);
    TRACE("seed = " << seed);
        
    set_testsuite<_RBTree<int, SetKeyExtractor> >(seed); 
    set_testsuite<_RBTree<int, SetKeyExtractor, _RankMetadata> >(seed); 
    set_testsuite<_RBTree<int, SetKeyExtractor, MinGapMetadata> >(seed); 

    set_testsuite<_SplayTree<int, SetKeyExtractor> >(seed); 
    set_testsuite<_SplayTree<int, SetKeyExtractor, _RankMetadata> >(seed); 
    set_testsuite<_SplayTree<int, SetKeyExtractor, MinGapMetadata> >(seed);

    set_testsuite<_OVTree<int, SetKeyExtractor> >(seed); 
    set_testsuite<_OVTree<int, SetKeyExtractor, _RankMetadata> >(seed); 
    set_testsuite<_OVTree<int, SetKeyExtractor, MinGapMetadata> >(seed);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                

    return 0;
}



