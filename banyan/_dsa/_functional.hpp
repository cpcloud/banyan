#ifndef _PYOBJECT_FUNCTIONAL_HPP
#define _PYOBJECT_FUNCTIONAL_HPP

#include <functional>

template<class LT>
class LTEq : 
    public std::binary_function<
        typename LT::first_argument_type,
        typename LT::second_argument_type,
        bool>
{
public:
    LTEq(const LT & lt_) : 
        lt(lt_)
    {
        // Do nothing.
    };       
    
    inline bool
    operator()(
        typename LT::first_argument_type lhs,
        typename LT::second_argument_type rhs) const
    {
        return !lt(lhs, rhs) && !lt(rhs, lhs);
    }        
    
private:
    LT lt;    
};

template<
    typename LHS_It, typename RHS_It, class LT>
bool disjoint(
    LHS_It lhs_b, LHS_It lhs_e,
    RHS_It rhs_b, RHS_It rhs_e,
    const LT & lt)
{
    if (lhs_b == lhs_e || rhs_b == rhs_e)
        return true;
        
    while (true) 
        if (lt(*lhs_b, *rhs_b)) {
            if (++lhs_b == lhs_e)
                return true;
        }
        else if (lt(*rhs_b, *lhs_b)) {
            if (++rhs_b == rhs_e)
                return true;
        }
        else
            return false;                       
}    

template<
    class LT>
struct _FirstLT :
    public std::binary_function<
        std::pair<typename LT::first_argument_type, PyObject *>,
        std::pair<typename LT::second_argument_type, PyObject *>,
        bool>
{
    _FirstLT(const LT & lt_ = LT()) :
        lt(lt_)
    {
        // Do nothing.
    }        

    inline bool
    operator()(
        const std::pair<typename LT::first_argument_type, PyObject *> & lhs,
        const std::pair<typename LT::second_argument_type, PyObject *> & rhs) const
    {
        return lt(lhs.first, rhs.first);
    }

private:
    LT lt;    
};    

#endif // #ifndef _PYOBJECT_FUNCTIONAL_HPP

