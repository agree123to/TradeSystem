#include "MainEngine.h"

void MainEngine::me_login(QString userid, QString password, QString brokerid, QString mdAddress, QString tdAddress)
{
	// ��ʼ�����׺�����ӿ�
	ctpmd->ctp_md_init(mdAddress);
	ctptd->ctp_td_init(tdAddress, userid, password, brokerid);

	// ����������ӿڵ�¼
	while (true)
	{
		if (ctpmd->get_is_md_connect())
		{
			ctpmd->ctp_md_login();
			break;
		}
	}
	while (true)
	{
		if (ctptd->get_is_td_connect())
		{
			ctptd->ctp_td_login();
			break;
		}
	}

	// ����Event����
	ee->ee_begin();
	// ʱ�䴥�����ݶ���ѯ�ʽ�ͳֲֶ���
	register_event(EVENT_TIMER, ctptd, &CtpTdApi::ctp_td_query);

	// MainEngine��¼��ɱ�־
	me_is_login.store(true);
}

void MainEngine::me_logout()
{
	// �����¼��ɱ��Ϊtrue���ͷ���Դ
	if (me_is_login.load())
	{
		// ע��MD_API
		ctpmd->ctp_md_release();

		// ע��TD_API
		ctptd->ctp_td_logout();
		while (!ctptd->get_is_td_logout())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
		ctptd->ctp_td_release();

		// �ر�Event����
		ee->ee_stop();
	}
}

void MainEngine::me_subscribe(QString instrumentid)
{
	// �������� 
	ctpmd->ctp_md_subscribe(instrumentid);
	// ��ѯ����
	ctptd->ctp_td_getCommission(instrumentid);
}

bool MainEngine::me_get_is_login()
{
	return me_is_login.load();
}

QString MainEngine::me_sendDefaultOrder(orderCommonRequest& order_field)
{
	return ctptd->ctp_td_send_limitOrder(order_field.instrument, order_field.price, order_field.volume, order_field.direction, order_field.offset);
}

void MainEngine::me_cancelOrder(cancelCommonRequest& cancel_field)
{
	ctptd->ctp_td_cancelOrder(cancel_field.instrument, cancel_field.exchange, cancel_field.order_ref, cancel_field.front_id, cancel_field.session_id);
}

void MainEngine::me_strat_cta()
{
	Event begStgEvent(EVENT_STG_BEG);
	ee->putEvent(begStgEvent);
}

void MainEngine::me_stop_cta()
{
	// �˴���Qt�̵߳��ã������д������Դ���¼��ص��̴߳��ھ�����ϵ����workingStopOrderDict����ֱ�������¼�����
	Event endStgEvent(EVENT_STG_END);
	ee->putEvent(endStgEvent);
}

bool MainEngine::me_get_contract(QString vtSymbol, InstrumentInfo& contract)
{
	return de->de_get_contract(vtSymbol, contract);
}

bool MainEngine::me_get_order(QString ordID, OrderInfo& ordInfo)
{
	return de->de_get_order(ordID, ordInfo);
}

QMap <QString, CThostFtdcDepthMarketDataField> MainEngine::me_getLastMarketData() const
{
	return de->lastMarketDataSet;
}

QMap <QString, InstrumentInfo> MainEngine::me_getInstrumentInfo() const
{
	return de->allInstruments;
}

AccountInfo MainEngine::me_getAccountInfo() const
{
	return de->accountInfo;
}

QMap<QString, PositionInfo> MainEngine::me_getPositionInfo() const
{
	return de->allPosition;
}

QMap<QString, OrderInfo> MainEngine::me_getOrderInfo() const
{
	return de->allOrderDict;
}

QMap<QString, OrderInfo> MainEngine::me_getWorkingOrderInfo() const
{
	return de->workingOrderDict;
}