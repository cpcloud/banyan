#ifndef MIN_GAP_UPDATOR_HPP
#define MIN_GAP_UPDATOR_HPP

#include <functional>
#include <cmath>

template<
    typename Key>
struct _MinGapMetadata
{
    void 
    update(const Key & key, const _MinGapMetadata * l, const _MinGapMetadata * r)
    {
        min_ = l == NULL? key: l->min_;
        max_ = r == NULL? key: r->max_;  
        
        // Tmp Ami - below sehr ugly.
        min_gap = -1;
        if (l != NULL) 
            min_gap = minna_tzemach(
                minna_tzemach(min_gap, l->min_gap), 
                static_cast<Key>(std::fabs(static_cast<double>(key - l->max_))));
        if (r != NULL) 
            min_gap = minna_tzemach(minna_tzemach(
                min_gap, r->min_gap), 
                static_cast<Key>(std::fabs(static_cast<double>(key - r->min_))));
    }
    
    inline static Key
    minna_tzemach(Key lhs, Key rhs)
    {
        if (lhs < 0)
            return rhs;
        if (rhs < 0)
            return lhs;            
        return std::min<Key>(lhs, rhs);
    }
        
#ifdef BANYAN_DEBUG
    void
    trace(std::ostream & os) const
    {
        os << "< " << min_ << " " << max_ << " " << min_gap << " > ";
    }
#endif // #ifdef BANYAN_DEBUG

    Key min_, max_, min_gap;
};

#endif // #ifndef MIN_GAP_UPDATOR_HPP
