#pragma once

#include <qobject.h>
#include <qthread.h>
#include <qtimer.h>

#include "CallBack.h"
#include "cpp_basictool/CppQueue.hpp"


class EventEngine: public QThread
{
	Q_OBJECT
public:
	void ee_begin();
	void ee_stop();

	void putEvent(Event event);

public:
	template<typename T>
	void addEvent(std::string type, T* pObj, void (T::*pMemberFunc)(Event));
	void removeEvent(std::string type);

private:
	QTimer* __timer{ nullptr };
	ConcurrentQueue<Event> __queue;

	std::atomic<bool> __active_put{ false };
	std::atomic<bool> __active_thread{ false };
	
	std::mutex handlers_mutex;
	std::map < std::string, std::list<std::shared_ptr<CallBack>>> __handlers;
	
private slots:
	void onTimer();

private:
	void run()Q_DECL_OVERRIDE;
	void process(Event event);
};


template<typename T>
void EventEngine::addEvent(std::string type, T* pObj, void (T::*pMemberFunc)(Event))
{
	std::lock_guard<std::mutex> lock(handlers_mutex);

	// �ж�event_type�Ƿ���ڣ����������ʼ��map
	if (__handlers.find(type) == __handlers.end())
	{
		std::list<std::shared_ptr<CallBack>> list = {};
		__handlers[type] = list;
	}

	// �ж� pObj + T::*pMemberFunc �Ƿ��Ѿ������� event_type �Ļص��б��У��Ѵ�����ֱ��return
	// ���޷��ж��Ƿ��Ѱ��� pObj + T::*pMemberFunc �� ��Ҫ����Ա�˹���֤

	// push_back CallBack
	__handlers[type].push_back(std::make_shared<CallBack>(pObj, pMemberFunc));
}