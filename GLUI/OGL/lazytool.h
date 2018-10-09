#pragma once
#include <boost/log/trivial.hpp>

#define SHARE_PTR_AVAILABLE(ptr)\
        (ptr.get()&&ptr->is_available())


#define FAILED_REPORT_RETURN_FALSE(b,report)\
        if(!(b)){\
        BOOST_LOG_TRIVIAL(error) << report;\
		return false;\
		}


#define SET_GET_FUN(TYPE,value)\
        void set_##value##(TYPE v)\
		{ \
            m_##value = v;\
        }\
        TYPE get_##value##()\
		{ \
            return m_##value;\
        }\

