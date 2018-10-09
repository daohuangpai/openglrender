#pragma once
#include "windows.h"
#include <assert.h>

template<class T>
class  dataarray {
public:

	dataarray() {}
	dataarray(int size) { init(size); }
	virtual ~dataarray() { malloc_release(); }

	void init(unsigned int size)
	{
		int realsize = size * sizeof(T);
		if (m_data == nullptr) {
			m_data = (T*)malloc(realsize);
			m_capacity = realsize;
		}
		else if (m_capacity < realsize) {
			while (m_capacity < realsize)
				m_capacity = m_capacity << 1;
			m_data = (T*)realloc((void*)m_data, m_capacity);
		}
		m_size = size;
	}

	T& operator[](unsigned int index)
	{
		assert(index < m_size);
		return m_data[index];
	}

	T* get_data() { return m_data; }
	unsigned int get_size() { return m_size; }
	unsigned int get_byte_size() { return m_size*sizeof(T); }

	void insert(T* data, int size)
	{
		init((size + m_size) * sizeof(T));
		void* ptr = (void*)m_data[m_size];
		memcpy(ptr, data, size * sizeof(T));
	}
private:


	void malloc_release()
	{
		if (m_data != nullptr) {
			free(m_data);
		}
	}
public://用于配置，面元素多速度要快，直接取指针速度最快，运算符重载速度下降几十倍
	T* m_data = nullptr;
private:
	unsigned int m_size = 0;
	unsigned int m_capacity = 0;
};

template<class T>
class  dataarray2D{
public:
	dataarray2D() {}
	dataarray2D(unsigned int width, unsigned int height) { init(width, height); }
	void init(unsigned int width,unsigned int height)
	{
		m_width = width;
		m_height = height;
		m_data.init(width*height);
	}
	T* get_data() { return m_data.get_data(); }
	T* operator[](unsigned int index)
	{
		assert(index < m_height);
		return &m_data[index*m_width];
	}
	unsigned int width() { return m_width; }
	unsigned int height() { return m_height; }
private:
	unsigned int m_width = 0;
	unsigned int m_height = 0;
	dataarray<T> m_data;
};