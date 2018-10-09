#pragma once
#include "buffer.h"

//线程不安全
class BufferArray {
public:
	virtual unsigned int elementsize()  = 0;
	virtual bool is_available() = 0;
	virtual unsigned int bytesize() = 0;
	virtual unsigned int size()  = 0;
	virtual unsigned int bufferbegin() = 0;
	virtual std::shared_ptr<GLUI::buffer>& buffer() = 0;
};


//这个是连续块，即确信buffer中的数据确实也是按照T格式排列的
template <class T>
class SimpleBufferArray :public BufferArray{
public:
	virtual unsigned int elementsize() override{ return sizeof(T); }
	//这个只是一个中介工具，所以有buffer的起始偏移,避免一个数据被多次绑定，会循环mip就会出错;
	bool bindbuffer(std::shared_ptr<GLUI::buffer>& buffer, unsigned int begin, unsigned int size)
	{
		if (!buffer.get() || !buffer->is_available()) {
			BOOST_LOG_TRIVIAL(error) << "cannot bind to unable buffer";
			return false;
		}
		if (buffer->getsize() < begin + size * sizeof(T))
		{
			BOOST_LOG_TRIVIAL(error) << "array size error";
			return false;
		}

		m_begin = begin;
		if(m_buffer.get()!= buffer.get())
		   m_buffer = buffer;
		m_size = size;
		return true;
	}

	bool Update(T* data, unsigned int size, unsigned int index, bool huge)
	{
		//如果是完全填充的数据直接拷贝
		if (index + size > m_size) {
			BOOST_LOG_TRIVIAL(error) << "Update error,data offset out";
			return false;
		}

		return huge ? m_buffer->UpdateDataHuge((char*)data, m_begin + index * sizeof(T), size * sizeof(T))
				          : m_buffer->UpdateDataNotHuge((char*)data, m_begin + index * sizeof(T), size * sizeof(T));
	}

	bool Updatewithoffset(char* data, unsigned int width, unsigned int offset, unsigned int size, unsigned int index)
	{
		//如果是完全填充的数据直接拷贝
		if (index + size > m_size) {
			BOOST_LOG_TRIVIAL(error) << "Update error,data offset out";
			return false;
		}


		char* ptr = (char*)m_buffer->mapping();
		if (!ptr)
			return false;
		 
		for (int i = 0; i < size; i++)
		{
			memcpy(ptr + m_begin + i* sizeof(T), data  +offset+ i*width ,sizeof(T));
		}

		m_buffer->Unmapping();
		return true;
	}

	bool CopyOut(T* data, unsigned int size, unsigned int index, bool huge)
	{
		if (index + size > m_size) {
			BOOST_LOG_TRIVIAL(error) << "CopyOut error,data offset out";
			return false;
		}

		return huge ? m_buffer->CopyOutDataHuge((char*)data, m_begin + index * sizeof(T), size * sizeof(T))
			: m_buffer->CopyOutDataNotHuge((char*)data, m_begin + index * sizeof(T), size * sizeof(T));
	}


	bool AccessData(const std::function<bool(T*, int)> &AccessFuntion)
	{
		char* data = (char*)m_buffer->mapping();
		if (!data)
			return false;
		bool result = AccessFuntion((T*)(m_begin + data), m_size);
		m_buffer->Unmapping();
		return result;
	}


	static bool generatebuffer(unsigned int size, GLUI::buffer::USAGE_MODE usagemode, GLUI::buffer::TargetMode target,std::shared_ptr<GLUI::buffer>& buffer)
	{
		std::shared_ptr<GLUI::buffer> result(new GLUI::buffer());
		if (!result->generate(NULL, size * sizeof(T), usagemode, target))
			return false;
		buffer = result;
		return true;
	}

	void reset()
	{
		m_buffer = nullptr;
		m_size = m_begin = 0;
	}
	virtual unsigned int size() override { return m_size; }
	virtual unsigned int bytesize() override { return m_size * sizeof(T); }
	virtual bool is_available() override { return m_buffer.get() && m_buffer->is_available(); }
	virtual std::shared_ptr<GLUI::buffer>& buffer() override { return m_buffer; }
	unsigned int bufferbegin() override{ return m_begin; }
private:
	unsigned int m_size = 0;
	unsigned int m_begin = 0 ;
	std::shared_ptr<GLUI::buffer> m_buffer;
};


template <class T>
class uniformbuffer {
public:

	bool bindbuffer(std::shared_ptr<GLUI::buffer>& buffer)
	{
		if (!buffer.get() || !buffer->is_available()) {
			BOOST_LOG_TRIVIAL(error) << "uniformbuffer->bindbuffer : cannot bind to unable buffer";
			return false;
		}
		if (buffer->getsize() !=sizeof(T))
		{
			BOOST_LOG_TRIVIAL(error) << "uniformbuffer->bindbuffer : the buffer size is not Equal to class T";
			return false;
		}

		m_buffer = buffer;
		return true;
	}

	bool UpdateToGpu(T& cpudata)
	{
		if (!is_available()){
			BOOST_LOG_TRIVIAL(error) << "uniformbuffer->UpdateToGpu : the buffer not available";
			return false;
		}
		return m_buffer->UpdateDataNotHuge((char*)&cpudata, 0, sizeof(T));
	}

	bool UpdateTocpu(T& m_cpudata)
	{
		if (!is_available()) {
			BOOST_LOG_TRIVIAL(error) << "uniformbuffer->UpdateToGpu : the buffer not available";
			return false;
		}
		return m_buffer->CopyOutDataNotHuge((char*)&m_cpudata, 0, sizeof(T));
	}


	static bool generatebuffer(GLUI::buffer::USAGE_MODE usagemode, GLUI::buffer::TargetMode target, std::shared_ptr<GLUI::buffer>& buffer)
	{
		std::shared_ptr<GLUI::buffer> result(new GLUI::buffer());
		if (!result->generate(NULL, sizeof(T), usagemode, target))
			return false;
		buffer = result;
		return true;
	}

	bool is_available() { return m_buffer.get() && m_buffer->is_available(); }
	bool bind_glsl_layout_num(int num) {
		return is_available() && m_buffer->BindToBinding(m_buffer->get_target(),num);
	}

	std::shared_ptr<GLUI::buffer>& buffer() { return m_buffer; }
private:
	std::shared_ptr<GLUI::buffer> m_buffer;
};