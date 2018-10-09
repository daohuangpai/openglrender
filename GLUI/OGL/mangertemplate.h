#pragma once

#include <map>
#include <boost/thread.hpp>
#include <memory>

#define MangerTemplateClassPtr(KEYTYPE,VALUETYPE,MangerTemplate)\
class MangerTemplate {\
public:\
	void set(KEYTYPE& key, std::shared_ptr<VALUETYPE>& value)\
	{\
		boost::unique_lock<boost::mutex> locker(m_Mutex);\
		m_list[key] = value;\
	}\
	bool get(KEYTYPE& key,std::shared_ptr<VALUETYPE>& value)\
	{\
		boost::unique_lock<boost::mutex> locker(m_Mutex);\
		std::map<KEYTYPE, std::shared_ptr<VALUETYPE>>::iterator itr = m_list.find(key);\
		if (itr != m_list.end()) {\
            value = itr->second;\
			return true;\
		}\
		return false;\
	}\
  void ClearUnUse()\
   {\
       boost::unique_lock<boost::mutex> locker(m_Mutex); \
	  for(std::map<KEYTYPE, std::shared_ptr<VALUETYPE>>::iterator itr = m_list.begin(); \
	    itr != m_list.end();itr++) {\
		   \
			   if(itr->second.unique()); \
                    m_list.erase(itr);\
	   }\
    }\
	virtual ~MangerTemplate()\
	{\
        boost::mutex m_Mutex;\
	}\
	static MangerTemplate* share##MangerTemplate()\
	{\
		if (!k_Manger.get()) {\
			k_Manger.reset(new MangerTemplate());\
		}\
		return k_Manger.get();\
	}\
private:\
	static std::auto_ptr<MangerTemplate> k_Manger;\
	boost::mutex m_Mutex;\
	std::map<KEYTYPE, std::shared_ptr<VALUETYPE>> m_list;\
};


#define MangerTemplateClassCPP(MangerTemplate)\
std::auto_ptr<MangerTemplate> MangerTemplate::k_Manger;