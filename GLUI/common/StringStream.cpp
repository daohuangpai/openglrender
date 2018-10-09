#include "StringStream.h"
#include <algorithm>
#include <wtypes.h>
#pragma warning(disable:4996)

namespace StringStream{

	template<class T>
	T To_Num(std::string num)
	{
		stringstream stream;
		T result;
		stream<<num;
		stream>>result;
		return result;
	}


	template<class T>
	std::string To_String(T num)
	{
		stringstream stream;
		std::string result;
		stream<<num;
		stream>>result;
		return result;
	}


	template<class T>
	void To_CharBuf(T num,char* buf)
	{
		stringstream stream;
		stream<<num;
		stream>>buf;
		return result;
	}

	inline int vsntprintf(char* buffer, size_t size,
		const char* format, va_list arguments) {
			int length = _vsprintf_p(buffer, size, format, arguments);
			if (length < 0) {
				if (size > 0)
					buffer[0] = 0;
				return _vscprintf_p(format, arguments);
			}
			return length;
	}

	inline int vsntprintf(wchar_t* buffer, size_t size,
		const wchar_t* format, va_list arguments) {

			int length = _vswprintf_p(buffer, size, format, arguments);
			if (length < 0) {
				if (size > 0)
					buffer[0] = 0;
				return _vscwprintf_p(format, arguments);
			}
			return length;
	}



	template <class StringType>
	static void StringAppendVT(StringType* dst,
		const typename StringType::value_type* format,
		va_list ap) {
			typename StringType::value_type stack_buf[1024];
			va_list ap_copy;
			GG_VA_COPY(ap_copy, ap);
			int result = vsntprintf(stack_buf, arraysize(stack_buf), format, ap_copy);
			va_end(ap_copy);
			if (result >= 0 && result < static_cast<int>(arraysize(stack_buf))) {
				// It fit.
				dst->append(stack_buf, result);
				return;
			}
	}

	void StringAppendV(std::string* dst, const char* format, va_list ap) {
		StringAppendVT(dst, format, ap);
	}

	void StringAppendV(std::wstring* dst, const wchar_t* format, va_list ap) {
		StringAppendVT(dst, format, ap);
	}

	std::string StringPrintf(const char* format, ...) {
		va_list ap;
		va_start(ap, format);
		std::string result;
		StringAppendV(&result, format, ap);
		va_end(ap);
		return result;
	}

	std::wstring StringPrintf(const wchar_t* format, ...) {
		va_list ap;
		va_start(ap, format);
		std::wstring result;
		StringAppendV(&result, format, ap);
		va_end(ap);
		return result;
	}

	void Trimc(std::string &str,std::string& c)
	{
		if(str.compare(c) == 0){
			str = "";
		}else if(str!=""){
			int s=str.find_first_not_of(c);
			int e=str.find_last_not_of(c);
			str=str.substr(s,e-s+1);
		}
	}

	void Trim(std::string &str)
	{
		if(str!=""){
			int s=str.find_first_not_of(" \t");
			int e=str.find_last_not_of(" \t");
			str=str.substr(s,e-s+1);
		}
	}
	//转小写;
	void TranToLow(std::string& str)
	{
		transform(str.begin(),str.end(),str.begin(),tolower); 
	}
	//转大写;
	void TranToUpper(std::string& str)
	{
		transform(str.begin(), str.end(), str.begin(), toupper);
	}
	 //"12212"),"12","21"  ->  22211 
	std::string&   replace_all(std::string&   str,const   std::string&   old_value,const   std::string&   new_value)     
	{     
		while(true)   {     
			std::string::size_type   pos(0);     
			if(   (pos=str.find(old_value))!=std::string::npos   )     
				str.replace(pos,old_value.length(),new_value);     
			else   break;     
		}     
		return   str;     
	}  
	//"12212"),"12","21"  ->  21221 
	std::string&   replace_all_distinct(std::string&   str,const   std::string&   old_value,const   std::string&   new_value)     
	{     
		for(std::string::size_type   pos(0);   pos!=std::string::npos;   pos+=new_value.length())   {     
			if(   (pos=str.find(old_value,pos))!=std::string::npos   )     
				str.replace(pos,old_value.length(),new_value);     
			else   break;     
		}     
		return   str;     
	} 

	std::string&   replace_First(std::string&   str,const   std::string&   old_value,const   std::string&   new_value)
	{
		for(std::string::size_type   pos(0);   pos!=std::string::npos;   pos+=new_value.length())   {     
			if(   (pos=str.find(old_value,pos))!=std::string::npos   ){
				   str.replace(pos,old_value.length(),new_value);  
				   break;
	    	}else   break;	   	    
		}     
		return   str;   
	}

	void ConvertGBKToUtf8(std::string& strGBK)
	{
		int len = MultiByteToWideChar(CP_ACP, 0, strGBK.data(), -1, NULL, 0);
		unsigned short * wszUtf8 = new unsigned short[len + 1];
		memset(wszUtf8, 0, len * 2 + 2);
		MultiByteToWideChar(CP_ACP, 0, strGBK.data(), -1, (LPWSTR)wszUtf8, len);

		len = WideCharToMultiByte(CP_UTF8, 0, (LPCWCH)wszUtf8, -1, NULL, 0, NULL, NULL);
		char *szUtf8 = new char[len + 1];
		memset(szUtf8, 0, len + 1);
		WideCharToMultiByte(CP_UTF8, 0, (LPCWCH)wszUtf8, -1, szUtf8, len, NULL, NULL);

		strGBK = szUtf8;
		delete[] szUtf8;
		delete[] wszUtf8;
	}
	//UTF8转ANSI  
	void UTF8toGBK(std::string &strUTF8)  
	{  
		//获取转换为多字节后需要的缓冲区大小，创建多字节缓冲区  
		UINT nLen = MultiByteToWideChar(CP_UTF8,NULL,strUTF8.data(),-1,NULL,NULL);  
		WCHAR *wszBuffer = new WCHAR[nLen+1];  
		nLen = MultiByteToWideChar(CP_UTF8,NULL,strUTF8.data(),-1,wszBuffer,nLen);  
		wszBuffer[nLen] = 0;  

		nLen = WideCharToMultiByte(936,NULL,wszBuffer,-1,NULL,NULL,NULL,NULL);  
		CHAR *szBuffer = new CHAR[nLen+1];  
		nLen = WideCharToMultiByte(936,NULL,wszBuffer,-1,szBuffer,nLen,NULL,NULL);  
		szBuffer[nLen] = 0;  

		strUTF8 = szBuffer;  
		//清理内存  
		delete []szBuffer;  
		delete []wszBuffer;  
	}  

	// Big5 => GBK：
	// い地チ㎝瓣 --> 中華人民共和國
	void BIG52GBK(char *szBuf)
	{
		if(!strcmp(szBuf, ""))
			   return;
		   int nStrLen = strlen(szBuf);
			 wchar_t *pws = new wchar_t[nStrLen + 1];
			   __try
				 {
					   int nReturn = MultiByteToWideChar(950, 0, szBuf, nStrLen, pws, nStrLen + 1);
						  BOOL bValue = false;
							 nReturn = WideCharToMultiByte(936, 0, pws, nReturn, szBuf, nStrLen + 1, "?", &bValue);
							    szBuf[nReturn] = 0;
				 }
				 __finally
				   {
						 delete[] pws;
				   }
	}
	void BIG5ToGBK(std::string& szBuf)
	{
		 BIG52GBK((char*)szBuf.data());
	}
	//---------------------------------------------------------------------------
	// GBK => Big5
	// 中華人民共和國 --> い地チ㎝瓣
	void GBK2BIG5(char *szBuf)
	{
		 if(!strcmp(szBuf, ""))
			   return ;
		   int nStrLen = strlen(szBuf);
			 wchar_t *pws = new wchar_t[nStrLen + 1];
			   __try
				 {
					   MultiByteToWideChar(936, 0, szBuf, nStrLen, pws, nStrLen + 1);
						  BOOL bValue = false;
							 WideCharToMultiByte(950, 0, pws, nStrLen, szBuf, nStrLen + 1, "?", &bValue);
							    szBuf[nStrLen] = 0;
				 }
				 __finally
				   {
						 delete[] pws;
				   }
	}
	void GBKToBIG5(std::string &szBuf)
	{
		 GBK2BIG5((char*)szBuf.data());
	}
	//----------------------------------------------------------------------------
	// GB2312 => GBK
	// 中华人民共和国 --> 中華人民共和國
	void GB2GBK(char *szBuf)
	{
		 if(!strcmp(szBuf, ""))
			   return;
		   int nStrLen = strlen(szBuf);
			 WORD wLCID = MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), SORT_CHINESE_PRC);
			   int nReturn = LCMapStringA(wLCID, LCMAP_TRADITIONAL_CHINESE, szBuf, nStrLen, NULL, 0);
				 if(!nReturn)
					   return;
				   char *pcBuf = new char[nReturn + 1];
					 __try
					   {
							 wLCID = MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), SORT_CHINESE_PRC);
							    LCMapStringA(wLCID, LCMAP_TRADITIONAL_CHINESE, szBuf, nReturn, pcBuf, nReturn + 1);
								   strncpy(szBuf, pcBuf, nReturn);
					   }
					   __finally
						 {
							   delete[] pcBuf;
						 }
	}

	void GB2312ToGBK(std::string& szBuf)
	{
		 GB2GBK((char*)szBuf.data());
	}
	//---------------------------------------------------------------------------
	// GBK =〉GB2312
	// 中華人民共和國 --> 中华人民共和国
	void GBK2GB(char *szBuf)
	{
		 if(!strcmp(szBuf, ""))
			   return;
		   int nStrLen = strlen(szBuf);
			 WORD wLCID = MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), SORT_CHINESE_BIG5);
			   int nReturn = LCMapStringA(wLCID, LCMAP_SIMPLIFIED_CHINESE, szBuf, nStrLen, NULL, 0);
				 if(!nReturn)
					   return;
				   char *pcBuf = new char[nReturn + 1];
					 __try
					   {
							 wLCID = MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), SORT_CHINESE_BIG5);
							    LCMapStringA(wLCID, LCMAP_SIMPLIFIED_CHINESE, szBuf, nReturn, pcBuf, nReturn + 1);
								   strncpy(szBuf, pcBuf, nReturn);
					   }
					   __finally
						 {
							   delete []pcBuf;
						 }
	}

	void GBKToGB2312(std::string& szBuf)
	{
		 GBK2GB((char*)szBuf.data());
	}

	std::wstring UTF8ToWide(const std::string &utf8) {
		if (utf8.length() == 0) {
			return std::wstring();
		}
		int charcount = MultiByteToWideChar(CP_ACP, 0, utf8.c_str(), -1, NULL, 0);

		if (charcount == 0) {
			return std::wstring();
		}
		wchar_t* buf = new wchar_t[charcount];
		MultiByteToWideChar(CP_ACP, 0, utf8.c_str(), -1, buf, charcount);
		std::wstring result(buf);
		delete[] buf;
		return result;
	}

	std::string WideToUTF8(const std::wstring& wide) {
		int wide_length = static_cast<int>(wide.length());
		if (wide_length == 0)
			return std::string();

		// Compute the length of the buffer we'll need.
		int charcount = WideCharToMultiByte(CP_ACP, 0, wide.data(), wide_length,
			NULL, 0, NULL, NULL);
		if (charcount == 0)
			return std::string();

		std::string mb;
		mb.resize(charcount);
		WideCharToMultiByte(CP_ACP, 0, wide.data(), wide_length,
			&mb[0], charcount, NULL, NULL);

		return mb;
	}

	std::string GetBetown(std::string& msg, std::string beginstr, std::string endstr)
	{
		std::string result = "";
		int begin = msg.find(beginstr);
		int end = msg.find(endstr);
		int oversize = strlen(beginstr.data());
		if ((begin != std::string::npos)
			&& (end != std::string::npos)){
				result = msg.substr(begin + oversize, end - begin - oversize);
		}
		return result;
	}

	std::string GetBetownLast(std::string& msg, std::string beginstr, std::string endstr)
	{
		std::string result = "";
		int begin = msg.find(beginstr);
		int end = msg.find_last_of(endstr);
		int oversize = strlen(beginstr.data());
		if ((begin != std::string::npos)
			&& (end != std::string::npos)){
				result = msg.substr(begin + oversize, end - begin - oversize);
		}
		return result;
	}

	//必须是类似"type1 INTEGER,type2 TEXT,type3 char(1),type4 FLOAT,type5 DOUBLE"，就是不是以，结尾的;
	void Division(std::string& data,std::string divisionsign,std::vector<std::string>& datas)
	{
		 datas.clear();
		 int offset = divisionsign.size();
		 int pos;
		 int begin = 0;
		 do
		 {
			   pos = data.find(divisionsign,begin);
			   if (pos == std::string::npos) break;
			   std::string buf = data.substr(begin,pos - begin);
			   datas.push_back(buf);
			   begin=pos+offset;	     
		 }while((begin<data.size())&&(pos!=std::string::npos));
	}
}