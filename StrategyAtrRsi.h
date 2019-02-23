#pragma once

#include "CtaStrategyBase.h"

class CtaEngine;
class StrategyAtrRsi :public StrategyBase
{
public:
	StrategyAtrRsi(CtaEngine* ce, std::string name, std::string symbol);

	virtual void onInit() override;
	virtual void onStart() override;
	virtual void onStop() override;

	virtual void onTick(QuoteInfo& quote) override;
	virtual void onBar(CtaBarData& bar) override;
	virtual void onOrder(OrderInfo& order) override;
	virtual void onTrade(TradeInfo& trade) override;

private:
	// ���Բ���
	int atrLength{ 22 };			// ����ATRָ��Ĵ�����
	int atrMaLength{ 10 };			// ����ATR���ߵĴ�����
	int rsiLength{ 5 };				// ����RSI�Ĵ�����
	double rsiEntry{ 16 };			// RSI�Ŀ����ź�
	double trailingPercent{ 0.8 };	// �ٷֱ��ƶ�ֹ��
	int initDays{ 10 };				// ��ʼ���������õ�����

	// ���Ա���
	CtaBarData bar;					// K�߶���
	int barMinute{ 0 };				// K�ߵ�ǰ�ķ���

	int bufferSize{ 100 };			// ��Ҫ��������ݵĴ�С
	int bufferCount{ 0 };			// Ŀǰ�Ѿ������˵����ݵļ���
	std::vector<double> highArray;	// K����߼۵�����
	std::vector<double> lowArray;	// K����ͼ۵�����
	std::vector<double> closeArray;	// K�����̼۵�����

	int atrCount{ 0 };				// Ŀǰ�Ѿ������˵�ATR�ļ���
	std::vector<double> atrArray;	// ATRָ�������
	double atrValue{ 0.0 };			// ���µ�ATRָ����ֵ
	double atrMa{ 0 };				// ATR�ƶ�ƽ������ֵ

	double rsiValue{ 0.0 };			// RSIָ�����ֵ
	double rsiBuy{ 0.0 };			// RSI����ֵ
	double rsiSell{ 0.0 };			// RSI������ֵ
	double intraTradeHigh{ 0.0 };	// RSI�ƶ�ֹ���õĳֲ�������߼�
	double intraTradeLow{ 0.0 };	// �ƶ�ֹ���õĳֲ�������ͼ�

	std::vector<std::string> orderList;	// ����ί�д�����б�
};