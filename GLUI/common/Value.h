#pragma once
#include "windows.h"
#include "StringStream.h"
#include "../OGL/util.h"
#include <assert.h>

namespace GLUI
{
class Value{
public:
	enum Tag_Value_type {
		VAR_INT = 1,//数值设为1开始、因为传递多参数时最后一个参数为0，要用这个0判断结束;
		VAR_CHARS,
		VAR_CHAR,
		VAR_WCHARS,
		VAR_WCHAR,
		VAR_FLOAT,
		VAR_DOUBLE,
		VAR_BOOLEAN,
		VAR_LONG,
		VAR_LONGLONG,
		VAR_BUFFER,
		VAR_NULL
	};

	Value() {}
	Value(int num){*this =  num;}
	Value(char* s){*this =  s;}
	Value(char c){*this =  c;}
	Value(float f){*this =  f;}
	Value(double d){*this =  d;}
	Value(bool b){*this =  b;}
	Value(LONGLONG ll){*this =  ll;}
	Value(long b){ *this = b; }
    Value(Value& other){*this = other;}
	Value(BYTE* s, DWORD size){ SetBuffer(s, size); }

	virtual ~Value(){ malloc_release(); }
	


	void SetBuffer(BYTE* s, DWORD size)
	{
		m_type = VAR_BUFFER;
		malloc_incase(size);
		if(s!=nullptr)
		   memcpy(m_data, s, size);
		cbuf = (BYTE*)m_data;
	}

	void malloc_incase(DWORD size)
	{
		int realsize = size;
		if (m_data == nullptr) {
			m_data = malloc(realsize);
			m_capacity = realsize;
		}
		else if (m_capacity < realsize) {
			while (m_capacity < realsize)
				m_capacity = m_capacity << 1;
			m_data = realloc(m_data, m_capacity);
		}
		m_size = size;
	}
	void malloc_release()
	{
		if (m_data != nullptr) {
			free(m_data);
		}
	}

#define NOBUFFER_OPERATOR_FUN(TYPE,ENUMNAME,unvalue) \
       Value& operator=(const TYPE v) \
	   {\
		unvalue = v;\
		m_type = ENUMNAME;\
		return *this;\
	   }\


	NOBUFFER_OPERATOR_FUN(LONGLONG, VAR_LONGLONG, lVal)
	NOBUFFER_OPERATOR_FUN(bool, VAR_BOOLEAN, bVal)
	NOBUFFER_OPERATOR_FUN(int, VAR_INT, iVal)
	NOBUFFER_OPERATOR_FUN(char, VAR_CHAR, cVal)
	NOBUFFER_OPERATOR_FUN(wchar_t, VAR_WCHAR, wVal)
	NOBUFFER_OPERATOR_FUN(float, VAR_FLOAT, fVal)
	NOBUFFER_OPERATOR_FUN(double, VAR_DOUBLE, dVal)
	NOBUFFER_OPERATOR_FUN(long, VAR_LONG, gVal)

	Value& operator=(char* csval)
	{
		m_type = VAR_CHARS;
		int length = strlen(csval);
		malloc_incase(length + 1);
		csVal = (char*)m_data;
		memcpy(m_data, csval, length);
		csVal[length] = 0;
		return *this;
	}

	Value& operator=(std::string& csval)
	{
		*this = (char*)csval.data();
		return *this;
	}


	Value& operator=(const char* csval)
	{
		*this = (char*)csval;
		return *this;
	}

	Value& operator=(wchar_t* csval)
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

	Value& operator=(std::wstring& csval)
	{
		*this = (wchar_t*)csval.data();
		return *this;
	}


	Value& operator=(const wchar_t* csval)
	{
		*this = (wchar_t*)csval;
		return *this;
	}

	Value& operator=(Value& value) 
	{
		 if(VAR_CHARS == value.m_type){
			 *this = value.csVal;
		 }
		 else if (VAR_BUFFER == value.m_type)
		 {
			 SetBuffer(value.AsBuffer(), value.GetSize());
		 }
		 else {
			 memcpy(this,&value,sizeof(Value));
		 }
		 return *this;
	}

	bool operator==(Value& value)
	{
		 bool result = false;
		 if(m_type == value.m_type){
			 switch(m_type)
			 {
			 case VAR_INT:
				  {
				  result = (AsInt() == value.AsInt());
				  }
				  break;
			 case VAR_CHARS:
				  {
				  result = (strlen(AsString()) == strlen(value.AsString()));
				  if(result){result = (memcmp(AsString(),value.AsString(),strlen(AsString())) == 0);} 
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
					  result = (memcmp((char*)&lVal,(char*)&other,sizeof(LONGLONG)) == 0);
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
 

	DWORD GetSize()
	{
		switch (m_type)
		{
		case VAR_INT:
		{
			return sizeof(int);
		}
			break;
		case VAR_CHARS:
		case VAR_BUFFER:
		{
			return m_size;
		}
			break;
		case VAR_CHAR:
		{
			return sizeof(char);
		}
			break;
		case VAR_FLOAT:
		{
			return sizeof(float);
		}
			break;
		case VAR_DOUBLE:
		{
			return sizeof(double);
		}
			break;
		case VAR_BOOLEAN:
		{
			return sizeof(bool);
		}
			break;
		case VAR_LONGLONG:
		{
			return sizeof(LONGLONG);
		}
			break;
		case VAR_LONG:
		{
			return sizeof(LONG);
		}
			break;
		}
		 
		return 0;
	}


	bool AsBool()
	{
		assert(m_type == VAR_BOOLEAN);
		return bVal;
	}

	int AsInt()
	{
		assert(m_type == VAR_INT);
		return iVal;
	}
	char* AsString()
	{
		assert(m_type == VAR_CHARS);
		return csVal;
	}

	char AsChar()
	{
		assert(m_type == VAR_CHAR);
		return cVal;
	}

	float AsFloat()
	{
		assert(m_type == VAR_FLOAT);
		return fVal;
	}

	double AsDouble()
	{
		assert(m_type == VAR_DOUBLE);
		return dVal;
	}

	LONGLONG AsInt64()
	{
		assert(m_type == VAR_LONGLONG);
		return lVal;
	}

	long AsLong()
	{
		assert(m_type == VAR_LONG);
		return gVal;
	}

	BYTE* AsBuffer()
	{
		assert(m_type == VAR_BUFFER);
		return cbuf;
	}



	bool IsBool(){return(m_type == VAR_BOOLEAN);}
	bool IsInt(){return (m_type == VAR_INT);}
	bool IsString(){return (m_type == VAR_CHARS);}
	bool IsChar(){return (m_type == VAR_CHAR);}
	bool IsFloat(){	return (m_type == VAR_FLOAT);}
	bool IsDouble(){return (m_type == VAR_DOUBLE);}
    bool IsInt64(){	return (m_type == VAR_LONGLONG);}
	bool IsLong(){	return (m_type == VAR_LONG);}
    bool IsBuffer(){return (m_type == VAR_BUFFER);}
	Tag_Value_type get_type(){return m_type;}


	std::string ToString()
    {
	     std::string result = "";
      	 switch(m_type)
		   {
			  case VAR_INT:
				  {
					  result = StringStream::StringPrintf("%d",AsInt()); 
				  }
				  break;
			  case VAR_CHARS:
				  {
					  result = AsString();
				  }
				  break;
			  case VAR_CHAR:
				  {
					  result = StringStream::StringPrintf("%d",AsChar());
				  }
				  break;
			  case VAR_FLOAT:
				  {
					  result = StringStream::StringPrintf("%f",AsFloat()); 
				  }
				  break;
			  case VAR_DOUBLE:
				  {
					 result = StringStream::StringPrintf("%lf",AsDouble());
				  }
				  break;
              case VAR_BOOLEAN:
                  {
                     result = AsBool()?"TRUE":"FALSE";
                  }
                  break;
              case VAR_LONGLONG:
                 {  
                     result = StringStream::StringPrintf("%I64d",AsInt64());
                 }
                 break;
			  case VAR_LONG:
			    {
				     result = StringStream::StringPrintf("%d", AsLong());
			    }
				 break;
			  case VAR_BUFFER:
			   {
				     result = "MBUFFER";
			   }
				break;
			}
	   return  result;
    }

protected:
	union
	{
		int iVal;
		wchar_t* wsVal;
		wchar_t wVal;
		char* csVal;
		char cVal;
		float fVal;
		double dVal;
		bool bVal;
		LONGLONG lVal;
		long gVal;
		BYTE* cbuf;
	};


	Tag_Value_type m_type = VAR_NULL;
	DWORD m_size;
	void* m_data = nullptr;
	unsigned int m_capacity = 0;//以byte为单位的个数
};
 
}
