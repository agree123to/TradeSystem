#pragma once

#include <map>
#include <list>
#include <memory>

#include "EventBase.h"
#include "CtaBase.h"

// ǰ����������ֹѭ������ 
class MainEngine;
class EventEngine;

class CtaEngine
{
public:
	CtaEngine(MainEngine* me, EventEngine* ee);
	~CtaEngine();

	// ע���¼��ص�
	void registerEvent();

	std::string sendOrder(const std::string& vtSymbol, char order_type, double price, int volume, std::shared_ptr<StrategyBase> stg);
	void cancelOrder(const std::string& order_id);
	std::string sendStopOrder(const std::string& vtSymbol, char order_type, double price, int volume, std::shared_ptr<StrategyBase> stg);
	void cancelStopOrder(const std::string& stop_id);

	// ���Լ������ʼ��
	void processStartStrategy(Event ev);
	void processStopStrategy(Event ev);

	// �¼��ص�������
	void procecssStopOrderEvent(const QuoteInfo& quote);
	void procecssTickEvent(Event ev);
	void processOrderEvent(Event ev);
	void processTradeEvent(Event ev);
	void processPositionEvent(Event ev);

private:
	void loadStrategy();
	void initStrategy(const std::string& stg_name);
	void startStrategy(const std::string& stg_name);
	void stopStrategy(const std::string& stg_name);

private:
	// ��ǰ����
	int todayDate{ 0 };
	int stopOrderCount{ 0 };

	// ����ʵ��Ӧ����ʼ��
	// keyΪ�������ƣ�valueΪ����ʵ��
	std::map<std::string, std::shared_ptr<StrategyBase>> strategyDict;
	// ���ڿ��ܶ��strategy����ͬһ��vtSymbol�����keyΪvtSymbol,valueΪ�����������strategy�����list,
	// �������ý������ڹ۲��Ƿ��в��Զ�ĳ��Լ����Ȥ�����������������Ȥ��������������
	std::map<std::string, std::list<std::shared_ptr<StrategyBase>>> tickStrategyDict;
	// keyΪOrderID��valueΪ����ʵ��(���Ե�CTP�ķ���)
	std::map<std::string, std::shared_ptr<StrategyBase>> orderStrategyDict;

	// keyΪstopOrderID��valueΪstopOrder����(���Ե���ֹͣ��)
	std::map<std::string, StopOrder> stopOrderDict;
	std::map<std::string, StopOrder> workingStopOrderDict;

	// keyΪvtSymbol��valueΪPositionBuffer����
	std::map<std::string, CtaPositionBuffer> posBufferDict;
private:
	MainEngine* me{ nullptr };
	EventEngine* ee{ nullptr };
};
