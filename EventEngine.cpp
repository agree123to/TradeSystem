#include "EventEngine.h"

#include <thread>

void EventEngine::run()
{
	while (__active_thread.load())
	{
		// ��ȡ����
		Event evt;
		if (__queue.wait_and_pop(evt, -1))
		{
			process(evt);
		}
	}
}

void EventEngine::process(Event event)
{
	std::lock_guard<std::mutex> lock(handlers_mutex);

	if (__handlers.find(event.type_) != __handlers.end())
	{
		std::list<std::shared_ptr<CallBack>>& handlerList = __handlers[event.type_];
		std::for_each(handlerList.cbegin(), handlerList.cend(), [&event](const std::shared_ptr<CallBack>& func_ptr)
		{
			func_ptr->Execute(event);
		});
	}
}

void EventEngine::onTimer()
{
	putEvent(Event(EVENT_TIMER));
}

void EventEngine::ee_begin()
{
	// ������ʱ���߳�
	__timer = new QTimer(this);
	__timer->setSingleShot(false);
	connect(__timer, SIGNAL(timeout()), this, SLOT(onTimer()), Qt::DirectConnection);
	__timer->start(1000);

	// �򿪶��п���
	__active_put.store(true);

	// ���¼������߳�
	__active_thread.store(true);
	start();
}

void EventEngine::ee_stop()
{
	// �رն�ʱ���߳�
	__timer->stop();

	// �رն��п���
	__active_put.store(false);
	while (!__queue.empty())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	// �ر��¼������߳�
	__active_thread.store(false);
	__queue.notify_all(); // ֪ͨ�ȴ��ڿն����ϵ��߳�

	this->quit();
	this->wait();
}

void EventEngine::removeEvent(std::string type)
{
	// ��Ϊ�޷������ض� obj + obj_ordinary_function, ��Event��ϵ��type�����Ƴ�
	std::lock_guard<std::mutex> lock(handlers_mutex);

	// �ж�event_type�Ƿ���ڣ�������ɾ���¼�����
	auto handle_list = __handlers.find(type);
	if (handle_list != __handlers.end())
	{
		__handlers.erase(handle_list);
	}
}

void EventEngine::putEvent(Event event)
{
	if (__active_put.load())
	{
		__queue.push_back(event);
	}
}