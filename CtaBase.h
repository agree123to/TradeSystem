#pragma once

#include <string>
#include <memory>

#include "ctpapi/ThostFtdcUserApiDataType.h"
#include "Model//PublicStruct.h"

// ����ֹͣ��ǰ׺
const std::string STOPORDERPREFIX = "CtaStopOrder.";

enum OrderType
{
	CTAORDER_BUY = '0',		// ��
	CTAORDER_SELL = '1',	// ��ƽ
	CTAORDER_SHORT = '2',	// ����
	CTAORDER_COVER = '3',	// ��ƽ
};

// ����ֹͣ��״̬
enum StopStatus
{
	STOPORDER_WAITING = '0',	// �ȴ���
	STOPORDER_CANCELLED = '1',	// �ѳ���
	STOPORDER_TRIGGERED = '2'	// �Ѵ���
};

class StrategyBase;
struct StopOrder
{
	std::string vtSymbol;
	char orderType{ 0 };
	char direction{ 0 };
	char offset{ 0 };
	double price{ 0 };
	int volume{ 0 };

	// ��ֹͣ���Ĳ��Զ���
	std::shared_ptr<StrategyBase>	strategy;
	// ֹͣ���ı��ر�� 
	std::string stopOrderID;
	// ֹͣ��״̬
	std::string status;
};

struct CtaBarData
{
	std::string vtSymbol = std::string("");
	std::string symbol = std::string("");
	std::string exchange = std::string("");
	
	double open{ 0 };
	double high{ 0 };
	double low{ 0 };
	double close{ 0 };

	int volume{ 0 };
	int openInterest{ 0 };

	int date{ 0 };
	int time{ 0 };

	bool is_new{ true };
};

// pos�����ϲ�CPositionBuffer�ڳֲֻص�����������,������迼�����ֽ�����������Ҫdebug�鿴����ʵ�֣�
class CtaPositionBuffer
{
public:
	// ���³ֲ�����
	inline void updatePositionData(PositionInfo pos)
	{
		if (THOST_FTDC_PD_Long == pos.direction)
		{
			longPosition = pos.position;
			longYd = pos.ydPosition;
			longToday = longPosition - longYd;
		}
		else
		{
			shortPosition = pos.position;
			shortYd = pos.ydPosition;
			shortToday = shortPosition - shortYd;
		}
	}
	// ���³ɽ�����
	inline void updateTradeData(TradeInfo trade)
	{
		// ����
		if (THOST_FTDC_D_Buy == trade.direction)
		{
			// �෽���֣����Ӧ��ͷ�ĳֲֺͽ������
			if (THOST_FTDC_OF_Open == trade.offset)
			{
				longPosition += trade.volume;
				longToday += trade.volume;
			}
			// �෽ƽ�񣬶�Ӧ��ͷ�ĳֲֺͽ�ּ���
			else if (THOST_FTDC_OF_CloseToday == trade.offset)
			{
				shortPosition -= trade.volume;
				shortToday -= trade.volume;
			}
			// �෽ƽ�򣬶�Ӧ��ͷ�ĳֲֺ���ּ���
			else
			{
				shortPosition -= trade.volume;
				shortYd -= trade.volume;
			}
		}
		// ����
		else
		{
			// �շ����֣����Ӧ�շ��ĳֲֺͽ������
			if (THOST_FTDC_OF_Open == trade.offset)
			{
				shortPosition += trade.volume;
				shortToday += trade.volume;
			}
			// �շ�ƽ�񣬶�Ӧ��ͷ�ĳֲֺͽ�ּ���
			else if (THOST_FTDC_OF_CloseToday == trade.offset)
			{
				longPosition -= trade.volume;
				longToday -= trade.volume;
			}
			// �շ�ƽ�򣬶�Ӧ��ͷ�ĳֲֺ���ּ���
			else
			{
				longPosition -= trade.volume;
				longYd -= trade.volume;
			}
		}
	}

public:
	std::string vtSymbol;
	// ��ͷ
	int longPosition{ 0 };
	int longToday{ 0 };
	int longYd{ 0 };
	// ��ͷ
	int shortPosition{ 0 };
	int shortToday{ 0 };
	int shortYd{ 0 };
};