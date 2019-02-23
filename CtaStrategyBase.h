#pragma once

#include "CtaBase.h"
#include "TechIndicator.h"

// ���в������͵Ļ��࣬��ȡ�����ӿںͷ��������еĽ��ײ��Ծ���Ҫ�̳���
class CtaEngine;
class StrategyBase
{
public:
	virtual void onInit() = 0;
	virtual void onStart() = 0;
	virtual void onStop() = 0;

	virtual void onTick(QuoteInfo& quote) = 0;
	virtual void onBar(CtaBarData& trade) = 0;
	virtual void onOrder(OrderInfo& order) = 0;
	virtual void onTrade(TradeInfo& trade) = 0;
	
public:
	// ���캯��
	StrategyBase(CtaEngine* ce, std::string name, std::string symbol);

	// ��������
	int convert_time_str2int(const char* update_time);

	// ������أ�ί���µ�
	std::string buy(double price, int volume, bool stop = false);
	std::string sell(double price, int volume, bool stop = false);
	std::string short_(double price, int volume, bool stop = false);
	std::string cover(double price, int volume, bool stop = false);

	// ����
	void cancelOrder(std::string orderID);

private:
	std::string sendOrder(char order_type, double price, int volume, bool stop = false);

public:
	// ������ݿ�
	// ..

	// ���Ի�������
	std::string name;		// ����ʵ������
	std::string vtSymbol;	// ���׺�Լ���ݶ�ֻ����һ�ֺ�Լ

	// ���ԵĻ������������������
	bool inited{ false };	// �Ƿ�����˳�ʼ��
	bool trading{ false };	// �Ƿ���������
	int pos{ 0 };			// �ֲ����

	CtaEngine* ce{ nullptr };
};