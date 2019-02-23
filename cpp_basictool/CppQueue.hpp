#pragma once

#include <atomic>
#include <deque>
#include <mutex>
#include <condition_variable>

///�̰߳�ȫ�Ķ���
template<typename Data>
class ConcurrentQueue
{
private:
	std::atomic<bool> is_active{ false };
	
	std::deque<Data> the_queue;
	std::mutex queue_mutex;
	std::condition_variable queue_cv;

public:
	// ֪ͨ���еȴ��ڶ����ϵ��߳�
	void notify_all()
	{
		std::unique_lock<std::mutex> lock(queue_mutex);	// �����Ƿ���Ҫ������
		queue_cv.notify_all();
	}

	//ͷ�˴����µ�����
	void push_front(Data const& data)
	{
		std::unique_lock<std::mutex> lock(queue_mutex);	
		the_queue.push_front(data);	
		queue_cv.notify_one();
	}

	//β�˴����µ�����
	void push_back(Data const& data)
	{
		std::unique_lock<std::mutex> lock(queue_mutex);
		the_queue.push_back(data);
		queue_cv.notify_one();
	}

	//�������Ƿ�Ϊ��
	bool empty()
	{
		std::unique_lock<std::mutex> lock(queue_mutex);
		return the_queue.empty();
	}

	//ȡ��
	bool wait_and_pop(Data& popped_value, int millsecond)
	{
		std::unique_lock<std::mutex> lock(queue_mutex);

		if (the_queue.empty())						//������Ϊ��ʱ
		{
			// 0�� �������ȴ�
			if (0 == millsecond)
			{
				return false;
			}
			// -1�� �����ȴ�
			else if (-1 == millsecond)
			{
				queue_cv.wait(lock);
			}
			// >0�� �������ȴ�
			else
			{
				if (std::cv_status::timeout == queue_cv.wait_for(lock, std::chrono::milliseconds(millsecond)))
				{
					return false;
				}
			}
		}
		if (the_queue.empty())
		{
			return false;
		}

		popped_value = the_queue.front();		//��ȡ�����еĵ�һ������
		the_queue.pop_front();					//ɾ��������

		return true;
	}
};