#ifndef _DBG_HPP
#define _DBG_HPP

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <string>

/******************************************************************************
*******************************************************************************
******************************************************************************/
namespace detail
{

inline void dbg_log(const char file_name[], 
	unsigned int line_num, const std::string &r_msg, std::ostream & os)
{
	os << file_name << "::" << line_num << ": " << r_msg << "\n";
	// os << r_msg << "\n";
}

inline void dbg_warn(const char file_name[], 
	unsigned int line_num, const std::string &r_msg)
{
	dbg_log(file_name, line_num, r_msg, std::cerr);
}

inline void dbg_trace(const char file_name[], 
	unsigned int line_num, const std::string &r_msg)
{
	dbg_log(file_name, line_num, r_msg, std::cout);
}

inline void dbg_assert(const char file_name[], 
	unsigned int line_num, 
	bool cond, const char cond_str[])
{
	if(cond)
		return;

	const std::string assertion = 
		std::string("assertion ") + cond_str + " failed";

	dbg_warn(file_name, line_num, assertion);

	std::abort();
}

template<class Ex_T>
inline void dbg_warn_and_throw(const char file_name[], 
	unsigned int line_num, const std::string &r_msg, 
	const char ex_name[])
{
	const std::string throwing = 
		std::string("throwing ") + ex_name + " - " + r_msg;

	dbg_warn(file_name, line_num, throwing);

	throw Ex_T(r_msg);
}

/******************************************************************************
*******************************************************************************
******************************************************************************/
} // namespace detail

#define DBG_UNCONDITIONAL_ASSERT(COND) ::detail::dbg_assert(\
	__FILE__, __LINE__, (COND), #COND)

#ifdef BANYAN_DEBUG
#define DBG_ASSERT(COND) DBG_UNCONDITIONAL_ASSERT(COND)
#else // #ifdef BANYAN_DEBUG
#define DBG_ASSERT(COND)
#endif // #ifdef BANYAN_DEBUG

#define DBG_VERIFY(COND) DBG_UNCONDITIONAL_ASSERT(COND)

#ifdef BANYAN_DEBUG
#define DBG_ONLY(X) X
#else // #ifdef BANYAN_DEBUG
#define DBG_ONLY(X)  
#endif // #ifdef BANYAN_DEBUG

#define WARN_AND_THROW(MSG, EX) \
{ \
	std::ostringstream oss; \
	\
	oss << MSG; \
	\
	::detail::dbg_warn_and_throw<EX>( \
		__FILE__, __LINE__, oss.str(), #EX);  \
}

#define TRACE(MSG) \
{ \
	std::ostringstream oss; \
	\
	oss << MSG; \
	\
	::detail::dbg_trace( \
		__FILE__, __LINE__, oss.str());  \
}

#define WARN(MSG) \
{ \
	std::ostringstream oss; \
	\
	oss << MSG; \
	\
	::detail::dbg_warn( \
		__FILE__, __LINE__, oss.str());  \
}

#endif // #ifndef _DBG_HPP


