#pragma once

#include "ctpapi/ThostFtdcMdApi.h"
#pragma comment(lib,"ctpapi/thostmduserapi.lib") 

#include <atomic>

#include "EventEngine.h"
#include "DataEngine.h"

class CtpMdApi :public QObject, public CThostFtdcMdSpi
{
	Q_OBJECT

public:
	CtpMdApi(EventEngine *ee, DataEngine* de)
	{
		this->ee = ee;
		this->de = de;
	}

	bool get_is_md_connect();
	bool get_is_md_logout();

	void ctp_md_init(QString mdaddress);
	void ctp_md_release();

	void ctp_md_login();
	void ctp_md_logout();

	void ctp_md_subscribe(QString instrumendid);

private:
	CThostFtdcMdApi* MdApi;
	EventEngine* ee;
	DataEngine* de;

	int requestID{ 0 };
	std::atomic<bool>	is_md_connect{ false };
	std::atomic<bool>	is_md_logout{ false };

private:
	/****************************Spi�ص�����****************************************/
	//���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷���������
	void OnFrontConnected() override;
	///���ͻ����뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á���������������API���Զ��������ӣ��ͻ��˿ɲ�������
	void OnFrontDisconnected(int nReason) override;
	///��¼������Ӧ
	void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
	///�ǳ�������Ӧ
	void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
	//����ر���Ӧ
	void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData) override;
};