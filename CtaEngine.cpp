#include "CtaEngine.h"

#include "MainEngine.h"
#include "EventEngine.h"
#include "StrategyAtrRsi.h"
#include "TechIndicator.h"
#include "GLogWrapper.h"

CtaEngine::CtaEngine(MainEngine* me, EventEngine* ee)
: me(me)
, ee(ee)
{
	// ע���¼�����
	registerEvent();
}

CtaEngine::~CtaEngine()
{
	
}

void CtaEngine::loadStrategy()
{
	// ��������ʵ��
	std::shared_ptr<StrategyBase> AtrRsi = std::move(std::shared_ptr<StrategyBase>(new StrategyAtrRsi(this, "AtrRsi", "rb1701")));
	// ��ȡ������
	strategyDict["AtrRsi"] = AtrRsi;
	// ����Tickӳ���ϵ
	tickStrategyDict["rb1701"].push_back(AtrRsi);
	// ���ĺ�Լ
	me->me_subscribe("rb1701");
}

void CtaEngine::initStrategy(const std::string& stg_name)
{
	auto stg = strategyDict.find(stg_name);
	if (stg != strategyDict.end())
	{
		stg->second->inited = true;
		stg->second->onInit();
	}
}

void CtaEngine::startStrategy(const std::string& stg_name)
{
	auto stg = strategyDict.find(stg_name);
	if (stg != strategyDict.end())
	{
		stg->second->trading = true;
		stg->second->onStart();
	}
}

void CtaEngine::stopStrategy(const std::string& stg_name)
{
	auto stg = strategyDict.find(stg_name);
	if (stg != strategyDict.end())
	{
		if (stg->second->trading)
		{
			stg->second->trading = false;
			stg->second->onStop();
		}

		// �Ըò��Է����������޼۵����г���
		for_each(orderStrategyDict.begin(), orderStrategyDict.end(), [this, &stg_name](std::map<std::string, std::shared_ptr<StrategyBase>>::reference map)
		{
			if (map.second->name == stg_name)
			{
				cancelOrder(map.first);
			}
		});
	
		// �Ըò��Է��������б���ֹͣ������
		/* ��ѭ���ڣ�cancelStopOrder����erase������ʹfor_each������ʧЧ
		for_each(workingStopOrderDict.begin(), workingStopOrderDict.end(), [this, &stg_name](std::map<std::string, StopOrder>::reference map)
		{
			if (map.second.strategy->name == stg_name)
			{
	
				cancelStopOrder(map.first);
			}
		});
		*/
		for (auto it = workingStopOrderDict.begin(); it != workingStopOrderDict.end();)
		{
			if (it->second.strategy->name == stg_name)
			{
				// Ĭ��workingStopOrderDict���еģ�stopOrderDict����
				stopOrderDict[it->first].status = StopStatus::STOPORDER_CANCELLED;
				workingStopOrderDict.erase(it++);
			}
			else
			{
				++it;
			}
		}

	}
}

void CtaEngine::registerEvent()
{
	me->register_event(EVENT_STG_BEG, this, &CtaEngine::processStartStrategy);
	me->register_event(EVENT_STG_END, this, &CtaEngine::processStopStrategy);

	me->register_event(EVENT_TICK, this, &CtaEngine::procecssTickEvent);
	me->register_event(EVENT_ORDER, this, &CtaEngine::processOrderEvent);
	me->register_event(EVENT_TRADE, this, &CtaEngine::processTradeEvent);
	me->register_event(EVENT_POSITION, this, &CtaEngine::processPositionEvent);
}

std::string CtaEngine::sendOrder(const std::string& vtSymbol, char order_type, double price, int volume, std::shared_ptr<StrategyBase> stg)
{
	InstrumentInfo contract;
	if (!me->me_get_contract(vtSymbol.c_str(), contract))
	{
		// error contract
	}

	// �������
	orderCommonRequest req;
	strncpy_s(req.instrument, vtSymbol.c_str(), sizeof(req.instrument));
	req.price = price;
	req.volume = volume;

	if (OrderType::CTAORDER_BUY == order_type)
	{
		req.direction = THOST_FTDC_D_Buy;
		req.offset = THOST_FTDC_OF_Open;
	}
	else if (OrderType::CTAORDER_SELL == order_type)
	{
		req.direction = THOST_FTDC_D_Sell;

		// ����������ƽ��ƽ��
		if (contract.exchangeId.compare("SHFE"))
		{
			req.offset = THOST_FTDC_OF_Close;
		}
		else
		{
			// ������
			CtaPositionBuffer pos_buf; // �ֲֻ���
			if (posBufferDict.find(vtSymbol) == posBufferDict.end())
			{
				// ���治���ڣ�Ĭ��ƽ��
				req.offset = THOST_FTDC_OF_Close;
			}
			else
			{
				pos_buf = posBufferDict[vtSymbol];
				if (pos_buf.longToday)
				{
					// �н��ʱ����ƽ��
					req.offset = THOST_FTDC_OF_CloseToday;
				}
				else
				{
					req.offset = THOST_FTDC_OF_Close;
				}
			}
		}
	}
	else if (OrderType::CTAORDER_SHORT == order_type)
	{
		req.direction = THOST_FTDC_D_Sell;
		req.offset = THOST_FTDC_OF_Open;
	}
	else if (OrderType::CTAORDER_COVER == order_type)
	{
		req.direction = THOST_FTDC_D_Buy;
		
		// ����������ƽ��ƽ��
		if (contract.exchangeId.compare("SHFE"))
		{
			req.offset = THOST_FTDC_OF_Close;
		}
		else
		{
			// ������
			CtaPositionBuffer pos_buf; // �ֲֻ���
			if (posBufferDict.find(vtSymbol) == posBufferDict.end())
			{
				// ���治���ڣ�Ĭ��ƽ��
				req.offset = THOST_FTDC_OF_Close;
			}
			else
			{
				pos_buf = posBufferDict[vtSymbol];
				if (pos_buf.shortToday)
				{
					// �н��ʱ����ƽ��
					req.offset = THOST_FTDC_OF_CloseToday;
				}
				else
				{
					req.offset = THOST_FTDC_OF_Close;
				}
			}
		}
	}
	// ����
	std::string orderID = me->me_sendDefaultOrder(req).toStdString();
	// ����ί�е��Ͳ��Լ�Ķ�Ӧ��ϵ
	orderStrategyDict[orderID] = stg;

	// ��ӡ��־������ί��
	// .....

	return orderID;
}

void CtaEngine::cancelOrder(const std::string& order_id)
{
	OrderInfo order;
	if (!me->me_get_order(order_id.c_str(), order))
	{
		// error contract
		return;
	}

	// �����ѯ�ɹ�
	QChar status = order.status;
	if (status != THOST_FTDC_OST_AllTraded && status != THOST_FTDC_OST_Canceled)
	{
		cancelCommonRequest req;
		strncpy_s(req.instrument, order.symbol.toStdString().c_str(), sizeof(req.instrument));
		strncpy_s(req.exchange, order.exchange.toStdString().c_str(), sizeof(req.exchange));
		strncpy_s(req.order_ref, order.orderID.toStdString().c_str(), sizeof(req.order_ref));
		req.front_id = order.frontID;
		req.session_id = order.sessionID;
		me->me_cancelOrder(req);
	}
}

std::string CtaEngine::sendStopOrder(const std::string& vtSymbol, char order_type, double price, int volume, std::shared_ptr<StrategyBase> stg)
{
	// ����ֹͣ��Ψһ���
	char stopOrderRef[13] = { 0 };
	_snprintf_s(stopOrderRef, sizeof(stopOrderRef), _TRUNCATE, "%012d", ++stopOrderCount);
	std::string stopOrderID = STOPORDERPREFIX + stopOrderRef;

	//////////////////////////////////////////////////////////////////////////
	StopOrder so;
	so.vtSymbol = vtSymbol;
	so.orderType = order_type;
	so.price = price;
	so.volume = volume;
	so.strategy = stg;
	so.stopOrderID = stopOrderID;
	so.status = StopStatus::STOPORDER_WAITING;
	if (OrderType::CTAORDER_BUY == order_type)
	{
		so.direction = THOST_FTDC_D_Buy;
		so.offset = THOST_FTDC_OF_Open;
	}
	else if (OrderType::CTAORDER_SELL == order_type)
	{
		so.direction = THOST_FTDC_D_Sell;
		so.offset = THOST_FTDC_OF_Close;
	}
	else if (OrderType::CTAORDER_SHORT == order_type)
	{
		so.direction = THOST_FTDC_D_Sell;
		so.offset = THOST_FTDC_OF_Open;
	}
	else if (OrderType::CTAORDER_COVER == order_type)
	{
		so.direction = THOST_FTDC_D_Buy;
		so.offset = THOST_FTDC_OF_Close;
	}
	// ����stopOrder�����ֵ���
	if (stopOrderDict.find(stopOrderID) == stopOrderDict.end())
	{
		stopOrderDict.insert(std::make_pair(stopOrderID, so));
	}
	else
	{
		// stop_id error
	}

	if (workingStopOrderDict.find(stopOrderID) == workingStopOrderDict.end())
	{
		workingStopOrderDict.insert(std::make_pair(stopOrderID, so));
	}
	else
	{
		// stop_id error
	}

	return stopOrderID;
}

void CtaEngine::cancelStopOrder(const std::string& stop_id)
{
	// ����ֹͣ��
	auto stop_it = workingStopOrderDict.find(stop_id);
	if (stop_it != workingStopOrderDict.end())
	{
		// ���ں��� ����������ʧЧ
		char log_msg[512] = { 0 };
		_snprintf_s(log_msg, sizeof(log_msg), _TRUNCATE, "Stop: Canceled: orderID: %s  ", stop_it->first.c_str());
		GLOG(log_msg, CGLog::CGLog_INFO);

		// Ĭ��workingStopOrderDict���еģ�stopOrderDict����
		stopOrderDict[stop_id].status = StopStatus::STOPORDER_CANCELLED;
		workingStopOrderDict.erase(stop_it);
	}
}

void CtaEngine::processStartStrategy(Event ev)
{
	loadStrategy();
	initStrategy("AtrRsi");
	startStrategy("AtrRsi");
}

void CtaEngine::processStopStrategy(Event ev)
{
	stopStrategy("AtrRsi");
}

void CtaEngine::procecssStopOrderEvent(const QuoteInfo& quote)
{
	std::string vtSymbol = quote.symbol.toStdString();

	for (auto stopOrder = workingStopOrderDict.begin(); stopOrder != workingStopOrderDict.end(); )
	{
		StopOrder& order = stopOrder->second;
		bool longTriggered = false;
		bool shortTriggered = false;

		if (order.vtSymbol == vtSymbol)
		{
			longTriggered = (order.direction == THOST_FTDC_D_Buy) && (quote.lastPrice >= order.price);		// ��ͷֹͣ��������
			shortTriggered = (order.direction == THOST_FTDC_D_Sell) && (quote.lastPrice <= order.price);	// ��ͷֹͣ��������

			double price = 0.0;
			if (longTriggered || shortTriggered)
			{
				if (order.direction == THOST_FTDC_D_Buy)
				{
					price = quote.upperLimit; // �м�׷��
				}
				else if (order.direction == THOST_FTDC_D_Sell)
				{
					price = quote.lowerLimit; // �м�׷��
				}
				else
				{
					// error
				}

				char log_msg[512] = { 0 };
				_snprintf_s(log_msg, sizeof(log_msg), _TRUNCATE, "Stop: MarketTrigger: orderID: %s  ", stopOrder->first.c_str());
				GLOG(log_msg, CGLog::CGLog_INFO);

				// �޸ı���ֹͣ��״̬
				stopOrderDict[stopOrder->first].status = StopStatus::STOPORDER_TRIGGERED;
				sendOrder(vtSymbol, order.orderType, price, order.volume, order.strategy);
				workingStopOrderDict.erase(stopOrder++);
			}
			else
			{
				++stopOrder;
			}
		}
		else
		{
			++stopOrder;
		}
	}
}

void CtaEngine::procecssTickEvent(Event ev)
{
	QuoteInfo* quote = (QuoteInfo*)ev.data_.get();
	std::string vtSymbol = quote->vtSymbol.toStdString();

	// ���ȼ���Ƿ��в��Խ��׸ú�Լ
	if (tickStrategyDict.find(vtSymbol) != tickStrategyDict.end())
	{
		// �յ�tick������ȴ�����ֹͣ��������Ƿ�Ҫ����������
		// ���Ȳ鿴�Ƿ�֮ǰ�в���ί����ֹͣ�������ȴ���
		procecssStopOrderEvent(*quote);

		// ������͵�����ʵ���У�����ÿ��tick��������ѯ��������ָ�ꡣ����������Ż��ɣ�ÿ�����Կ���һ���̣߳���tick�ַ������Զ��У�
		std::list < std::shared_ptr<StrategyBase>>& stgList = tickStrategyDict[vtSymbol];
		for_each(stgList.begin(), stgList.end(), [quote](std::shared_ptr<StrategyBase>& stg)
		{
			stg->onTick(*quote);
		});
	}
}

void CtaEngine::processOrderEvent(Event ev)
{
	OrderInfo* order = (OrderInfo*)ev.data_.get();

	// ���ȼ���ί�����ĸ����Դ���
	auto map_it = orderStrategyDict.find(order->orderID.toStdString());
	if (map_it != orderStrategyDict.end())
	{
		map_it->second->onOrder(*order);
	}
}

// ��bug
void CtaEngine::processTradeEvent(Event ev)
{
	TradeInfo* trade = (TradeInfo*)ev.data_.get();

	// ����ֻά�����Է����ĳֲ�
	//////////////////////////////////////////////////////////////////////////
	auto map_it = orderStrategyDict.find(trade->orderID.toStdString());
	if (map_it != orderStrategyDict.end())
	{
		auto stg = map_it->second;
		// ������Գֲ�
		if (THOST_FTDC_D_Buy == trade->direction)
		{
			stg->pos += trade->volume;
		}
		else
		{
			stg->pos -= trade->volume;
		}

		stg->onTrade(*trade);
	}
	//////////////////////////////////////////////////////////////////////////

	// ���µ�Ŀ����ʲô��posBufferDict�ƺ������⣬����Ҫ��������
	//////////////////////////////////////////////////////////////////////////
	// ���³ֲֻ�������
	std::string symbol = trade->vtSymbol.toStdString();
	if (tickStrategyDict.find(symbol) != tickStrategyDict.end())
	{
		if (posBufferDict.find(symbol) == posBufferDict.end())
		{
			CtaPositionBuffer pos_buf;
			pos_buf.vtSymbol = symbol;
			posBufferDict.insert(make_pair(symbol, pos_buf));
		}
		CtaPositionBuffer& pb = posBufferDict[symbol];
		pb.updateTradeData(*trade);

		char log_msg[512] = { 0 };
		_snprintf_s(log_msg, sizeof(log_msg), _TRUNCATE, "pB: longPos: %d, longToday: %d, longYd: %d, shortPos: %d, shortToday: %d, shortYd: %d", 
			pb.longPosition, pb.longToday, pb.longYd, pb.shortPosition, pb.shortToday, pb.shortYd);
		GLOG(log_msg, CGLog::CGLog_INFO);
	}
}

void CtaEngine::processPositionEvent(Event ev)
{
	// ���³ֲ���������
	PositionInfo* pos = (PositionInfo*)ev.data_.get();

	std::string symbol = pos->vtSymbol.toStdString();
	if (tickStrategyDict.find(symbol) != tickStrategyDict.end())
	{
		if (posBufferDict.find(symbol) == posBufferDict.end())
		{
			CtaPositionBuffer pos_buf;
			pos_buf.vtSymbol = symbol;
			posBufferDict.insert(make_pair(symbol, pos_buf));
		}
		posBufferDict[symbol].updatePositionData(*pos);
	}
}