#pragma once
#include <vector>
#include <memory>
#include <functional>
#include <algorithm>
#include <thread>         // std::thread
#include <mutex>          // std::mutex, std::lock_guard
#include <map>
#include <boost/thread.hpp>

namespace GLUI {

	//�൱�ڰ�һ�����������ĺ�����ַ
	template<class Return, class Type, class... Args>
	inline std::function<Return(Args...)> bind_member(Type* instance, Return(Type::*method)(Args...))
	{
		return[=](Args&&... args) -> Return
		{
			return (instance->*method)(std::forward<Args>(args)...);
		};
	}

	class SlotBase {
	public:
		virtual void RemoveTable(void* ptr) = 0;
	};

	class ReceiveObject;

	template<class... Args>
	class Slot : public SlotBase{
	public:
		void run(Args&&... param)
		{
			boost::unique_lock<boost::mutex> locker(m_mtx);
			for (std::map<int, std::map<int, std::function<void(Args...)>>>::iterator funcmap = m_funs.begin();
				funcmap != m_funs.end(); funcmap++)
			{
				std::map<int, std::function<void(Args...)>>& funs = funcmap->second;
				for (std::map<int, std::function<void(Args...)>>::iterator itr = funs.begin();
					itr != funs.end(); itr++)
				{
					itr->second(std::forward<Args>(param)...);
				}
			}
		}

		template<class Type>
		void Insert(Type* ptr,void(Type::*method)(Args...))
		{
			boost::unique_lock<boost::mutex> locker(m_mtx);
			//ֱ��ȡ������ջ��ó�Ա������ַ��ֱ��ת���C++��֧��
			char* temp = reinterpret_cast<char*>(&ptr);
			char* methodid = temp + sizeof(Type*);//��������
			int class_ptr = reinterpret_cast<int>(ptr);
			int class_fun_ptr = *reinterpret_cast<int*>(methodid);
			m_funs[class_ptr][class_fun_ptr] = bind_member<void, Type, Args...>(ptr, method);
		}


		template<class Type>
		void Remove(Type* ptr,void(Type::*method)(Args...))
		{
			boost::unique_lock<boost::mutex> locker(m_mtx);
			int class_ptr = reinterpret_cast<int>(ptr);
			std::map<int, std::map<int, std::function<void(Args...)>>>::iterator funcmap = m_funs.find(class_ptr);
			if (funcmap != m_funs.end())
			{
				char* temp = reinterpret_cast<char*>(&ptr);
				char* methodid = temp + sizeof(Type*);
				int class_fun_ptr = *reinterpret_cast<int*>(methodid);
				std::map<int, std::function<void(Args...)>>& funs = funcmap->second;
				std::map<int, std::function<void(Args...)>>::iterator itr = funs.find(class_fun_ptr);
				if (itr != funs.end()) {
					//ɾ��С���е���
					funs.erase(itr);
					//�����ʱС�������������
					if (funs.size() == 0) {
						m_funs.erase(funcmap);
					}
				}
			}
		}

		virtual void RemoveTable(void* ptr) override
		{
			boost::unique_lock<boost::mutex> locker(m_mtx);
			int class_ptr = reinterpret_cast<int>(ptr);
			std::map<int, std::map<int, std::function<void(Args...)>>>::iterator funcmap = m_funs.find(class_ptr);
			if (funcmap != m_funs.end())
			{
				m_funs.erase(funcmap);
			}
		}
	private:
		boost::mutex m_mtx;
		std::map<int, std::map<int, std::function<void(Args...)>>> m_funs;
	};


	class ReceiveObject {
	public:
		virtual ~ReceiveObject()
		{
			boost::unique_lock<boost::mutex> locker(m_ReceiveObject_mtx);
			for (std::vector<std::weak_ptr<SlotBase>>::iterator itr = m_Receiveslotbases.begin();
				itr != m_Receiveslotbases.end(); itr++)
			{
				std::shared_ptr<SlotBase> sig = itr->lock();
				if (sig != nullptr) {
					sig->RemoveTable(this);
				}
			}
		}


		void InsertSlotBase(std::shared_ptr<SlotBase>& slot)
		{
			boost::unique_lock<boost::mutex> locker(m_ReceiveObject_mtx);
			for each (std::weak_ptr<SlotBase> var in m_Receiveslotbases) {
				if (var._Get() == slot.get())
					return;
			}
			m_Receiveslotbases.push_back(slot);
		}

		void removeSlotBase(std::shared_ptr<SlotBase>& slot)
		{
			boost::unique_lock<boost::mutex> locker(m_ReceiveObject_mtx);
			for (std::vector<std::weak_ptr<SlotBase>>::iterator itr = m_Receiveslotbases.begin();
				itr != m_Receiveslotbases.end(); itr++)
			{
				if ((*itr)._Get() == slot.get()) {
					m_Receiveslotbases.erase(itr);
					return;
				}
			 }
		}

	private:
		std::vector<std::weak_ptr<SlotBase>> m_Receiveslotbases;
		boost::mutex m_ReceiveObject_mtx;
	};

	class SendObject {
	public:
		//û�а�ȫ���Եģ��ڱ����Ӷ���ɾ��ʱ�����Զ��ڱ��������ɾ����������ɴ���
		template<class ScrType,class DstType,class... Args>
		void Connect(void(ScrType::*srcmethod)(Args...), DstType* dstptr,void(DstType::*dstmethod)(Args...))
		{
			boost::unique_lock<boost::mutex> locker(m_SendObject_mtx);
			char* temp = reinterpret_cast<char*>(&srcmethod);
			int srcfun = *reinterpret_cast<int*>(temp);
			std::map<int, std::shared_ptr<SlotBase>>::iterator itr = m_SendObjectfuns.find(srcfun);
			if (itr == m_SendObjectfuns.end()) {
				m_SendObjectfuns[srcfun] = std::shared_ptr<SlotBase>(new Slot<Args...>());
			}
			((Slot<Args...>*)m_SendObjectfuns[srcfun].get())->Insert(dstptr, dstmethod);
		}

		template<class ScrType, class DstType, class... Args>
		void DisConnect(void(ScrType::*srcmethod)(Args...), DstType* dstptr, void(DstType::*dstmethod)(Args...))
		{
			boost::unique_lock<boost::mutex> locker(m_SendObject_mtx);
			char* temp = reinterpret_cast<char*>(&srcmethod);
			int srcfun = *reinterpret_cast<int*>(temp);
			std::map<int, std::shared_ptr<SlotBase>>::iterator itr = m_SendObjectfuns.find(srcfun);
			if (itr != m_SendObjectfuns.end()) {
				((Slot<Args...>*)m_SendObjectfuns[srcfun].get())->Remove(dstptr, dstmethod);
			}
		}

		//�а�ȫ���Եģ��ڱ����Ӷ���ɾ��ʱ���Զ��ڱ��������ɾ�������Ӻ��ù��ı����������״̬��һ�������Ӷ����ͷ��ˣ����Զ��ӱ�����ɾ�����̰߳�ȫ
		template<class ScrType, class DstType, class... Args>
		void Connectsafe(void(ScrType::*srcmethod)(Args...), DstType* dstptr, void(DstType::*dstmethod)(Args...))
		{
			char* temp = reinterpret_cast<char*>(&srcmethod);
			int srcfun = *reinterpret_cast<int*>(temp);
			Connect(srcmethod, dstptr, dstmethod);
			ReceiveObject* robj = reinterpret_cast<ReceiveObject*>(dstptr);
			robj->InsertSlotBase(m_SendObjectfuns[srcfun]);
		}

		template<class ScrType, class DstType, class... Args>
		void DisConnectsafe(void(ScrType::*srcmethod)(Args...), DstType* dstptr, void(DstType::*dstmethod)(Args...))
		{
			char* temp = reinterpret_cast<char*>(&srcmethod);
			int srcfun = *reinterpret_cast<int*>(temp);
			DisConnect(srcmethod, dstptr, dstmethod);
			ReceiveObject* robj = reinterpret_cast<ReceiveObject*>(dstptr);
			robj->removeSlotBase(m_SendObjectfuns[srcfun]);
		}

		template<class ScrType, class... Args>
		void Run(void(ScrType::*srcmethod)(Args...), Args&&... param)
		{
			boost::unique_lock<boost::mutex> locker(m_SendObject_mtx);
			char* temp = reinterpret_cast<char*>(&srcmethod);
			int srcfun = *reinterpret_cast<int*>(temp);
			std::map<int, std::shared_ptr<SlotBase>>::iterator itr = m_SendObjectfuns.find(srcfun);
			if (itr != m_SendObjectfuns.end()) {
				((Slot<Args...>*)m_SendObjectfuns[srcfun].get())->run(std::forward<Args>(param)...);
			}
		}

	private:
		std::map<int, std::shared_ptr<SlotBase>> m_SendObjectfuns;
		boost::mutex m_SendObject_mtx;
	};


}