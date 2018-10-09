#pragma once 
#include "../OGL/treestruct.h"
#include <map>
#include "Value.h"

namespace GLUI
{
	
//#pragma pack(push)
//#pragma pack(4)
//	struct BinaryHead
//	{
//		unsigned int type;
//		unsigned int size;
//	};
//#pragma pack(pop)
//	
//
//	DWORD write_binary(char* buffer,void* buf, Value::Tag_Value_type valuetype, unsigned int len)
//	{
//		BinaryHead* head = reinterpret_cast<BinaryHead*>(buffer);
//		head->type = valuetype;
//		head->size = len;
//		void* offset = buffer + sizeof(BinaryHead);
//		memcpy(offset, buf, head->size);
//		return sizeof(BinaryHead) + head->size;
//	}
//
//
//#define WRITE_FUN(datatype,valuetype)\
//    inline DWORD write_##datatype##(char* buffer, datatype v){return write_binary(buffer,reinterpret_cast<void*>(&v),valuetype,sizeof(datatype));}\
//    inline DWORD get_write_##datatype##_size(){return sizeof(BinaryHead) + sizeof(datatype);}
//
//	WRITE_FUN(int,Value::VAR_INT)
//	WRITE_FUN(char, Value::VAR_CHAR)
//	WRITE_FUN(wchar_t, Value::VAR_WCHAR)
//	WRITE_FUN(float, Value::VAR_FLOAT)
//	WRITE_FUN(double, Value::VAR_DOUBLE)
//	WRITE_FUN(bool, Value::VAR_BOOLEAN)
//	WRITE_FUN(long, Value::VAR_LONG)
//	WRITE_FUN(LONGLONG, Value::VAR_LONGLONG)
//
//	inline  DWORD write_buffer(char* buffer, void* buf, unsigned int len) {return write_binary(buffer, buf, Value::VAR_BUFFER, len);}
//	inline  DWORD get_write_buffer_size(unsigned int len) {return sizeof(BinaryHead) + sizeof(len);}
//	inline  DWORD write_chars(char* buffer, char* buf) { return write_binary(buffer, buf, Value::VAR_BUFFER, strlen(buf)); }
//	inline  DWORD get_write_buffer_size(char* buf) { return sizeof(BinaryHead) + strlen(buf); }
//	inline  DWORD write_wchars(char* buffer, wchar_t* buf) { return write_binary(buffer, buf, Value::VAR_BUFFER, wcslen(buf)*sizeof(wchar_t)); }
//	inline  DWORD get_write_wchars_size(wchar_t* buf) { return sizeof(BinaryHead) + wcslen(buf) * sizeof(wchar_t); }
	
	class ValueTree : public Value
	{
	public:
 
		ValueTree& operator[](std::string key)
		{
			std::shared_ptr<ValueTree>& ptr = m_list[key];
			if (ptr.get() == nullptr){
				ptr.reset(new ValueTree);
			}
			return *ptr.get();
		}
#define TREE_NOBUFFER_OPERATOR_FUN(TYPE,ENUMNAME,unvalue) \
       ValueTree& operator=(const TYPE v) \
	   {\
		unvalue = v;\
		m_type = ENUMNAME;\
		return *this;\
	   }\

		TREE_NOBUFFER_OPERATOR_FUN(LONGLONG, VAR_LONGLONG, lVal)
		TREE_NOBUFFER_OPERATOR_FUN(bool, VAR_BOOLEAN, bVal)
		TREE_NOBUFFER_OPERATOR_FUN(int, VAR_INT, iVal)
		TREE_NOBUFFER_OPERATOR_FUN(char, VAR_CHAR, cVal)
		TREE_NOBUFFER_OPERATOR_FUN(wchar_t, VAR_WCHAR, wVal)
		TREE_NOBUFFER_OPERATOR_FUN(float, VAR_FLOAT, fVal)
		TREE_NOBUFFER_OPERATOR_FUN(double, VAR_DOUBLE, dVal)
		TREE_NOBUFFER_OPERATOR_FUN(long, VAR_LONG, gVal)
		ValueTree& operator=(char* csval)
		{
			m_type = VAR_CHARS;
			int length = strlen(csval);
			malloc_incase(length + 1);
			csVal = (char*)m_data;
			memcpy(m_data, csval, length);
			csVal[length] = 0;
			return *this;
		}

		ValueTree& operator=(std::string& csval)
		{
			*this = (char*)csval.data();
			return *this;
		}


		ValueTree& operator=(const char* csval)
		{
			*this = (char*)csval;
			return *this;
		}

		ValueTree& operator=(wchar_t* csval)
		{
			m_type = VAR_WCHARS;
			int length = wcslen(csval);
			int bytesize = sizeof(wchar_t)*(length + 1);
			malloc_incase(bytesize);
			wsVal = (wchar_t*)m_data;
			memcpy(m_data, csval, bytesize);
			wsVal[length] = 0;
			return *this;
		}

		ValueTree& operator=(std::wstring& csval)
		{
			*this = (wchar_t*)csval.data();
			return *this;
		}


		ValueTree& operator=(const wchar_t* csval)
		{
			*this = (wchar_t*)csval;
			return *this;
		}

		ValueTree& operator=(ValueTree& value)
		{
			if (VAR_CHARS == value.m_type) {
				*this = value.csVal;
			}
			else if (VAR_BUFFER == value.m_type)
			{
				SetBuffer(value.AsBuffer(), value.GetSize());
			}
			else {
				memcpy(this, &value, sizeof(Value));
			}
			return *this;
		}

		bool operator==(ValueTree& value)
		{
			bool result = false;
			if (m_type == value.m_type) {
				switch (m_type)
				{
				case VAR_INT:
				{
					result = (AsInt() == value.AsInt());
				}
				break;
				case VAR_CHARS:
				{
					result = (strlen(AsString()) == strlen(value.AsString()));
					if (result) { result = (memcmp(AsString(), value.AsString(), strlen(AsString())) == 0); }
				}
				break;
				case VAR_CHAR:
				{
					result = (AsChar() == value.AsChar());
				}
				break;
				case VAR_FLOAT:
				{
					result = (AsFloat() == value.AsFloat());
				}
				break;
				case VAR_DOUBLE:
				{
					result = (AsDouble() == value.AsDouble());
				}
				break;
				case VAR_BOOLEAN:
				{
					result = (AsBool() == value.AsBool());
				}
				break;
				case VAR_LONGLONG:
				{
					LONGLONG other = value.AsInt64();
					result = (memcmp((char*)&lVal, (char*)&other, sizeof(LONGLONG)) == 0);
				}
				break;
				case VAR_LONG:
				{
					result = (AsLong() == value.AsLong());
				}
				case VAR_BUFFER:
				{
					if (GetSize() != value.GetSize())
						result = false;
					else
						result = (memcmp((char*)AsBuffer(), (char*)value.AsBuffer(), GetSize()) == 0);
				}
				}
			}
			return result;
		}

		void clearsetting() { m_list.clear(); }
	private:
		std::map<std::string, std::shared_ptr<ValueTree>> m_list;
	};

 

	typedef ValueTree Configure;


}