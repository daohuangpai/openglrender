#pragma once
#include <vector>
#include <map>
#include <memory>
#include "lazytool.h"

namespace GLUI
{
	
	template<class T>
	class simpletree {
	public:
		virtual void addchild(T* child)
		{
			m_childrens.push_back(std::unique_ptr<T>());
			m_childrens.back().reset(child);
			m_childrens.back()->set_parent((T*)this);
		}

		virtual void removechild(T* child)
		{
			for (std::vector<std::unique_ptr<T>>::iterator itr = m_childrens.begin();
				itr != m_childrens.end(); itr++)
			{
				if (itr->get() == child) {
					m_childrens.erase(itr);
					break;
				}
			}
		}

		std::vector<std::unique_ptr<T>>& childs() { return m_childrens; }
		SET_GET_FUN(T*, parent)

	private:
		std::vector<std::unique_ptr<T>> m_childrens;
		T* m_parent;
	};

}