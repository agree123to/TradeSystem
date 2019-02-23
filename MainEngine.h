#pragma once

#include <atomic>

#include "EventEngine.h"
#include "DataEngine.h"
#include "CtaEngine.h"

#include "CtpMdApi.h"
#include "CtpTdApi.h"

// ��ܶ���������,�����ܶ�������й������׺Ͳ�ѯ���ڲ�ʵ�ֲ�ͬ�ӿڵ�����ת��
class MainEngine
{
public:
	MainEngine()
	{
		ee = new EventEngine;
		de = new DataEngine;
		ce = std::move(std::shared_ptr<CtaEngine>(new CtaEngine(this, ee)));

		// ��de��ȥ��û�취��Ŀǰ���ǵ��¼��ص���֧�ֲ���
		ctpmd = new CtpMdApi(ee, de);
		ctptd = new CtpTdApi(ee, de);
	}

	template<typename T>
	void register_event(std::string type, T* pObj, void (T::*pMemberFunc)(Event))
	{
		ee->addEvent(type, pObj, pMemberFunc);
	}

	// ��¼
	void me_login(QString userid, QString password, QString brokerid, QString mdAddress, QString tdAddress);
	// �˳�
	void me_logout();
	// ��������
	void me_subscribe(QString instrumentid);

	// �û���¼���
	bool me_get_is_login();

	// ί�е�����
	QString me_sendDefaultOrder(orderCommonRequest& order_field);
	void me_cancelOrder(cancelCommonRequest& cancel_field);

	// CTA����
	void me_strat_cta();
	void me_stop_cta();

	// CTA�µ���ѯ��Լ��ί�е�
	bool me_get_contract(QString vtSymbol, InstrumentInfo& contract);
	bool me_get_order(QString ordID, OrderInfo& ordInfo);
	
	// ��ѯ�������û��������������Ϣ��ѯ,��Ҫ����UI����ˢ�£�����UI����Ҫȥ��
	AccountInfo me_getAccountInfo() const;
	QMap<QString, PositionInfo> me_getPositionInfo() const;
	QMap<QString, InstrumentInfo> me_getInstrumentInfo() const;
	QMap<QString, CThostFtdcDepthMarketDataField> me_getLastMarketData() const;
	QMap<QString, OrderInfo> me_getOrderInfo() const;
	QMap<QString, OrderInfo> me_getWorkingOrderInfo() const;

private:
	std::atomic<bool> me_is_login{ false };

	// ��չ����ģ��
	EventEngine* ee;
	DataEngine* de;
	std::shared_ptr<CtaEngine> ce;

	// Gateway�ӿ�
	CtpMdApi* ctpmd;
	CtpTdApi* ctptd;
};