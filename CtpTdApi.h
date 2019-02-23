#pragma once

#include "ctpapi/ThostFtdcTraderApi.h"
#pragma comment(lib,"ctpapi/thosttraderapi.lib") 

#include <atomic>

#include "CtpCommand.h"
#include "EventEngine.h"
#include "DataEngine.h"

class CtpTdApi :public QObject, public CThostFtdcTraderSpi 
{
	Q_OBJECT
public:
	CtpTdApi(EventEngine* ee, DataEngine* de) : QueryQueue(1000), TradeQueue(1)
	{
		this->ee = ee;
		this->de = de;
	}

public:
	// ��ȡ���Ӻ͵�¼״̬
	bool get_is_td_connect();
	bool get_is_td_logout();

	// API��ʼ�����ͷ�
	void ctp_td_init(QString tdAddress, QString userid, QString password, QString brokerid);
	void ctp_td_release();

	// ��ѯˢ��ҳ��
	void ctp_td_query(Event ev);

	// �û���¼�ǳ�
	void ctp_td_login();
	void ctp_td_logout();

	// ����API������Ϣ��ѯ
	void ctp_td_getSettlement();
	void ctp_td_getInstrument();
	void ctp_td_getCommission(QString ins_id);
	void ctp_td_getAccount();
	void ctp_td_getPosition();

	// ί�е�����������
	QString ctp_td_send_limitOrder(TThostFtdcInstrumentIDType instrumentid, TThostFtdcPriceType price, TThostFtdcVolumeType volume, TThostFtdcDirectionType direction, TThostFtdcOffsetFlagType offset);
	QString ctp_td_send_marketOrder(TThostFtdcInstrumentIDType instrumentid, TThostFtdcVolumeType volume, TThostFtdcDirectionType direction, TThostFtdcOffsetFlagType offset);
	void ctp_td_cancelOrder(TThostFtdcInstrumentIDType instrumentID, TThostFtdcExchangeIDType exchangeID, TThostFtdcOrderRefType orderID, TThostFtdcFrontIDType frontID, TThostFtdcSessionIDType sessionID);

private:
	// �ж��Ƿ������Ϣ
	bool is_error_rsp(CThostFtdcRspInfoField *pRspInfo);
	// ί�е������ֶ���д���µ�
	QString ctp_td_order_insert(CThostFtdcInputOrderField& order_fields);
	// ί�е�״̬����
	void ctp_td_order_update(OrderInfo& order);

private:
	// �¼��������ѯ/���׶���
	EventEngine* ee;
	DataEngine* de;

	CommandQueue QueryQueue;
	CommandQueue TradeQueue;

	//����API
	CThostFtdcTraderApi* TdApi;

	// �û�������Ϣ
	user_trade_info usr_td_info;
	
	//�Ƿ�����/�ɽ���/�ǳ�
	std::atomic <bool>	is_td_connect{ false };
	std::atomic <bool>  is_td_tradable{ false };
	std::atomic<bool>	is_td_logout{ false };

	// ��ѯ�¼�����
	int query_count{ 0 };
	int query_trgger{ 3 };
	int query_function_index{ 0 };

private:
	/****************************Spi�ص�����****************************************/
	///���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
	void OnFrontConnected() override;
	///���ͻ����뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á�
	void OnFrontDisconnected(int nReason);
	///��¼������Ӧ
	void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
	///�ǳ�������Ӧ
	void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
	//Ͷ���߽�����ȷ����Ӧ
	void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
	///����¼��������Ӧ(������ͨ��)
	void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
	///��������������Ӧ(������ͨ��)
	void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
	///����֪ͨ
	void OnRtnOrder(CThostFtdcOrderField *pOrder) override;
	///�ɽ�֪ͨ
	void OnRtnTrade(CThostFtdcTradeField *pTrade) override;
	///�����ѯͶ���ֲ߳���Ӧ
	void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
	///�����ѯ�ʽ��˻���Ӧ
	void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
	///�����ѯ��Լ����������Ӧ
	void OnRspQryInstrumentCommissionRate(CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
	///�����ѯ��Լ��Ӧ
	void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
};