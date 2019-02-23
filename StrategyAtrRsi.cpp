#include "StrategyAtrRsi.h"

#include <QDebug>

#include "CtaEngine.h"
#include "GLogWrapper.h"

StrategyAtrRsi::StrategyAtrRsi(CtaEngine* ce, std::string name, std::string symbol)
: StrategyBase(ce, name, symbol)
{
	// ���Թ���
	//////////////////////////////////////////////////////////////////////////
	atrLength = 5;
	atrMaLength = 5;
	rsiLength = 5;
	rsiEntry = 10;
	trailingPercent = 0.8;


	bufferSize = 10;
	//////////////////////////////////////////////////////////////////////////


	// ��ʼ�����Բ���
	highArray.reserve(bufferSize);
	lowArray.reserve(bufferSize);
	closeArray.reserve(bufferSize);
}

void StrategyAtrRsi::onInit()
{
	// ��ʼ��RSI�볡��ֵ
	rsiBuy = 50 + rsiEntry;
	rsiSell = 50 - rsiEntry;

	/*
	// ������ʷ���ݣ������ûطż���ķ�ʽ��ʼ��������ֵ
	std::vector<CtaBarData> hist_bars;
	loadBar(initDays, hist_bars);
	for_each(hist_bars.begin(), hist_bars.end(), [](const CtaBarData& bar)
	{

	});
	*/

	// putEvent();
}

void StrategyAtrRsi::onStart()
{
	// putEvent();
}

void StrategyAtrRsi::onStop()
{
	// putEvent();
}

void StrategyAtrRsi::onTick(QuoteInfo& quote)
{
	// ����K��
	int tick_time = convert_time_str2int(quote.time.toStdString().c_str());
	int tickMinute = tick_time / 100;
	if (tickMinute != barMinute)
	{
		if (!bar.is_new)
		{
			// ����ʱ���;�bar,��ʼ����bar
			onBar(bar);
		}

		// ��ֵ���캯�����þ�bar
		bar = CtaBarData();
		bar.vtSymbol = quote.vtSymbol.toStdString();
		bar.symbol = quote.symbol.toStdString();
		//bar.exchange = 
		
		// ��ʼ��OHLC
		bar.open = quote.lastPrice;
		bar.high = quote.lastPrice;
		bar.low = quote.lastPrice;
		bar.close = quote.lastPrice;

		bar.time = tick_time;	// K�ߵ�ʱ����Ϊ��һ��Tick��ʱ��
		barMinute = tickMinute; // ���µ�ǰ�ķ���
	}
	else
	{
		double last_price = quote.lastPrice;
		bar.high = (bar.high > last_price) ? bar.high : last_price;
		bar.low = (bar.low < last_price) ? bar.low : last_price;
		bar.close = last_price;
		bar.is_new = false;
	}
}

void StrategyAtrRsi::onBar(CtaBarData& bar)
{
	// ����֮ǰ��������δ�ɽ���ί�У������޼۵���ֹͣ���������޼۵�����ֻ��CtaEngine��ά���ɣ���
	// ��Bar����ʱ��ǰһ��Bar������ֹͣ����������
	for_each(orderList.begin(), orderList.end(), [this](const std::string& orderID)
	{
		cancelOrder(orderID);
	});
	orderList.clear();

	// ����K������
	if (bufferCount == bufferSize)
	{
		highArray.erase(highArray.begin());
		lowArray.erase(lowArray.begin());
		closeArray.erase(closeArray.begin());
		bufferCount--;
	}

	highArray.push_back(bar.high);
	lowArray.push_back(bar.low);
	closeArray.push_back(bar.close);
	bufferCount++;

	// �����ۻ�ԭʼK��
	if (bufferCount < bufferSize)
	{
		return;
	}

	// ����ָ����ֵ

	// ATR
	int atr_size = -1;
	std::vector<double> atr_vec;
	if (!TechIndicator::ATR(highArray, lowArray, closeArray, atrLength, atr_size, atr_vec))
	{
		// ������־
		return;
	}
	double atrValue = atr_vec[atr_size-1];

	if (atrCount == bufferSize)
	{
		atrArray.erase(atrArray.begin());
		atrCount--;
	}

	atrArray.push_back(atrValue); 
	atrCount++;

	// �����ۻ�atr����
	if (atrCount < bufferSize)
	{
		return;
	}

	// MA
	int atr_ma_size = -1;
	std::vector<double> atr_ma_vec;
	if (!TechIndicator::MA(atrArray, atrMaLength, atr_ma_size, atr_ma_vec))
	{
		// ������־
		return;
	}
	// ATR�ľ���
	double atrMa = atr_ma_vec[atr_ma_size - 1];

	// RSI
	int rsi_size = -1;
	std::vector<double> rsi_vec;
	if (!TechIndicator::RSI(closeArray, rsiLength, rsi_size, rsi_vec))
	{
		// ������־
		return;
	}

	// ���Ժ���ָ��
	double rsiValue = rsi_vec[rsi_size-1];

	// �ж��Ƿ�Ҫ���н���
	//char sz_log[256] = { 0 };
	//sprintf_s(sz_log, "ATR: %.4f; ATR_MA: %.4f; RSI: %.4f", atrValue, atrMa, rsiValue);
	//qDebug() << sz_log;


	// ��ǰ�޲�λ
	if (0 == pos)
	{
		// �ֲ������ڵĸߵͼۣ��޲�λʱ��ʼ��
		intraTradeHigh = bar.high;
		intraTradeLow = bar.low;

		// ATR��ֵ�ϴ����ƶ�ƽ���ߣ�˵����������ڲ����Ӵ�
		// ���������Ƶĸ��ʽϴ��ʺ�CTA����
		if (atrValue > atrMa)
		{
			// ʹ��RSIָ�����������ʱ�����ڳ��������ۻ���������Ϊ�����ź�
			if (rsiValue > rsiBuy)
			{
				// ����Ϊ�˱�֤�ɽ���ѡ�񳬼�5����ָ�����µ�
				buy(bar.close+5, 1);

				char log_msg[512] = { 0 };
				_snprintf_s(log_msg, sizeof(log_msg), _TRUNCATE, "Open: BUY: rsiValue: %lf, atrMa: %lf, rsiValue: %lf ... close: %lf  ", atrValue, atrMa, rsiValue, bar.close+5);
				GLOG(log_msg, CGLog::CGLog_INFO);
			}
			else if (rsiValue < rsiSell)
			{
				short_(bar.close-5, 1);

				char log_msg[512] = { 0 };
				_snprintf_s(log_msg, sizeof(log_msg), _TRUNCATE, "Open: SHORT: rsiValue: %lf, atrMa: %lf, rsiValue: %lf ... close: %lf  ", atrValue, atrMa, rsiValue, bar.close-5);
				GLOG(log_msg, CGLog::CGLog_INFO);
			}
		}
	}
	// ���ж�ͷ��ע��ѧϰ�˴�ֹͣ�����߼�����ʵֹӯҲ��ĳ�������ϵ�ֹ�𣡣�����
	else if (pos >= 1)
	{
		// �����ͷ�������ڵ���߼ۣ��Լ�������ͼ�
		intraTradeHigh = (intraTradeHigh > bar.high) ? intraTradeHigh : bar.high;
		intraTradeLow = bar.low;
		// �����ͷ�ƶ�ֹ��: ֹ��ٷֱȲ�������۸�䶯���ƶ�
		double longStop = intraTradeHigh * (1 - trailingPercent / 100.);

		// ��������ֹ��ί�У����Ұ�ί�кż�¼���������ں�������(���Ϸ���ֹ��ί��)
		std::string orderID = sell(longStop, 1, true);
		orderList.push_back(orderID);	// orderList ֻ��OnBar��push_back��clear

		char log_msg[512] = { 0 };
		_snprintf_s(log_msg, sizeof(log_msg), _TRUNCATE, "Stop: SELL: intraTradeHigh: %lf, longStop: %lf, orderID: %s  ", intraTradeHigh, longStop, orderID.c_str());
		GLOG(log_msg, CGLog::CGLog_INFO);
	}
	// ���п�ͷ
	else if (pos <= -1)
	{
		intraTradeLow = (intraTradeLow < bar.low) ? intraTradeLow : bar.low;
		intraTradeHigh = bar.high;
		// �����ͷ�ƶ�ֹ��: ֹ��ٷֱȲ�������۸�䶯���ƶ�
		double shortStop = intraTradeLow * (1 + trailingPercent / 100.);
		// �����ͷ�ƶ�ֹ��: ֹ��ٷֱȲ�������۸�䶯���ƶ�
		std::string orderID = cover(shortStop, 1, true);
		orderList.push_back(orderID);  // orderList ֻ��OnBar��push_back��clear

		char log_msg[512] = { 0 };
		_snprintf_s(log_msg, sizeof(log_msg), _TRUNCATE, "Stop: COVER: intraTradeLow: %lf, shortStop: %lf, orderID: %s  ", intraTradeLow, shortStop, orderID.c_str());
		GLOG(log_msg, CGLog::CGLog_INFO);
	}

	// putEvent();
}

void StrategyAtrRsi::onOrder(OrderInfo& order)
{
	
}

void StrategyAtrRsi::onTrade(TradeInfo& trade)
{
	char log_msg[512] = { 0 };
	_snprintf_s(log_msg, sizeof(log_msg), _TRUNCATE, "onTrade: orderID: %s, volume: %d, tradeTime: %s, stgPos: %d  ", trade.orderID.toStdString().c_str(), trade.volume, trade.tradeTime.toStdString().c_str(), pos);
	GLOG(log_msg, CGLog::CGLog_INFO);
}

