#ifndef _STRING_STREAM_H_
#define _STRING_STREAM_H_

#include <string>
#include <stdarg.h>
#define GG_VA_COPY(a, b) (a = b)
#include <vector>

template <typename T, size_t N>
char (&ArraySizeHelper(T (&array)[N]))[N];

#define arraysize(array) (sizeof(ArraySizeHelper(array)))

 namespace StringStream{
	 
	 template<class T>
	 T To_Num(std::string num);	//字符串转数字;

	 template<class T>
	 std::string To_String(T num) ;//数字转字符串;

	 template<class T>
	 void To_CharBuf(T num,char* buf) ;//数字转字符串存在char数组里;

	 inline int vsntprintf(char* buffer, size_t size,
		 const char* format, va_list arguments) ;

	 inline int vsntprintf(wchar_t* buffer, size_t size,
		 const wchar_t* format, va_list arguments) ;


	 template <class StringType>
	 static void StringAppendVT(StringType* dst,
		 const typename StringType::value_type* format,
		 va_list ap) ;
	 void StringAppendV(std::string* dst, const char* format, va_list ap) ;
	 void StringAppendV(std::wstring* dst, const wchar_t* format, va_list ap) ;

	 std::string StringPrintf(const char* format, ...); 
	 std::wstring StringPrintf(const wchar_t* format, ...) ;

	 void Trim(std::string &str);//去除空格;
	 void Trimc(std::string &str,std::string& c);//删除指定符号;
	 void TranToLow(std::string& str);//转小写;
	 void TranToUpper(std::string& str);//转大写;

     std::string&   replace_all(std::string&   str,const   std::string&   old_value,const   std::string&   new_value);  
     std::string&   replace_all_distinct(std::string&   str,const   std::string&   old_value,const   std::string&   new_value); 
     std::string&   replace_First(std::string&   str,const   std::string&   old_value,const   std::string&   new_value);

	 void ConvertGBKToUtf8(std::string& strGBK);//GBK转utf8;
	 void UTF8toGBK(std::string &strUTF8);  
	 std::wstring UTF8ToWide(const std::string &utf8);
	 std::string WideToUTF8(const std::wstring& wide);
	 void BIG5ToGBK(std::string& szBuf);
	 void GBKToBIG5(std::string &szBuf);
	 void GB2312ToGBK(std::string& szBuf);
	 void GBKToGB2312(std::string& szBuf);

	 std::string GetBetown(std::string& msg, std::string beginstr, std::string endstr);
     std::string GetBetownLast(std::string& msg, std::string beginstr, std::string endstr);
	 void Division(std::string& data,std::string divisionsign,std::vector<std::string>& datas);
 }
#endif