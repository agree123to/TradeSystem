#include "CtpTdApi.h"
#include <QtDebug>
#include <qdir.h>
#include <string>

#include "CtpCommand.h"

void CtpTdApi::OnFrontConnected()
{
	is_td_connect.store(true);
}

void CtpTdApi::OnFrontDisconnected(int nReason)
{
	is_td_connect.store(false);
}

void CtpTdApi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!is_error_rsp(pRspInfo) && pRspUserLogin)
	{
		// ��¼�ɹ�
		qDebug() << "TD Succeed With Login";

		usr_td_info.front_id = pRspUserLogin->FrontID;
		usr_td_info.session_id = pRspUserLogin->SessionID;
		usr_td_info.maxOrderRef = atoi(pRspUserLogin->MaxOrderRef);
		strncpy_s(usr_td_info.trading_day, pRspUserLogin->TradingDay, sizeof(usr_td_info.trading_day));

		//ȷ�Ͻ�����
		ctp_td_getSettlement();
	}
	else
	{
		abort();
	}
}

void CtpTdApi::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!is_error_rsp(pRspInfo) && pUserLogout)
	{
		// �˳����
		is_td_logout.store(true);

		// �ǳ��ɹ�(����qDebug�ĵط��������־���ģ��)
		qDebug() << "TD  Succeed With  Logout";
	}
	else
	{
		abort();
	}
}

void CtpTdApi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!is_error_rsp(pRspInfo) && pSettlementInfoConfirm)
	{
		// ���㵥ȷ���Ժ�ſɽ���
		is_td_tradable.store(true);

		// ���㵥ȷ�ϳɹ�(����qDebug�ĵط��������־���ģ��)
		qDebug() << "TD  Succeed With  SettlementConfirm";

		ctp_td_getInstrument();//��ȡ���к�Լ��Ϣ
	}
	else
	{
	}
}
void CtpTdApi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

void CtpTdApi::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

void CtpTdApi::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	//��ȡ��󱨵�����
	usr_td_info.maxOrderRef = std::max(usr_td_info.maxOrderRef, (int)pOrder->OrderRef);

	// ί�е������¼�
	Event orderEvent(EVENT_ORDER);
	orderEvent.data_ = std::move(std::shared_ptr<char>((char*)(new OrderInfo)));

	OrderInfo* order = (OrderInfo*)orderEvent.data_.get();
	order->gatewayName = ("CTP");
	order->symbol = (pOrder->InstrumentID);
	order->exchange = (pOrder->ExchangeID);
	order->vtSymbol = (order->symbol);

	order->orderID = (pOrder->OrderRef);
	order->direction = (pOrder->Direction);
	order->offset = (pOrder->CombOffsetFlag[0]);
	order->status = (pOrder->OrderStatus);

	order->price = (pOrder->LimitPrice);
	order->totalVolume = (pOrder->VolumeTotalOriginal);
	order->tradeVolume = (pOrder->VolumeTraded);
	order->orderTime = (pOrder->InsertTime);
	order->cancelTime = (pOrder->CancelTime);
	order->frontID = (pOrder->FrontID);
	order->sessionID = (pOrder->SessionID);

	// CTP�ı�����һ����ά����Ҫ����frontID, sessionID, orderID�����ֶ�
	// ���ڱ��ӿ�����У��Ѿ�������CTP��OrderRef�������ԣ������ظ�
	// Ψһ���ܳ���OrderRef�ظ�������Ƕദ��¼���ڷǳ��ӽ���ʱ���ڣ�����ͬʱ������
	// ���ǵ�VtTrader��Ӧ�ó�������Ϊ����������ṹ������
	order->vtOrderID = (order->gatewayName + QString(".") + order->orderID);

	// ί�е�����
	ctp_td_order_update(*order);

	// ���Ͷ������������δ���ڣ�Ӧ���ڱ���ά��ί�е��Ķ���״̬
	ee->putEvent(orderEvent);
}

void CtpTdApi::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
	// �ɽ������¼�
	Event tradeEvent(EVENT_TRADE);
	tradeEvent.data_ = std::move(std::shared_ptr<char>((char*)(new TradeInfo)));

	TradeInfo* trade = (TradeInfo*)tradeEvent.data_.get();
	trade->gatewayName = ("CTP");
	trade->symbol = (pTrade->InstrumentID);
	trade->exchange = (pTrade->ExchangeID);
	trade->vtSymbol = (trade->symbol);

	trade->tradeID = (pTrade->TradeID);
	trade->vtTradeID = (trade->gatewayName + QString(".") + trade->tradeID);
	trade->orderID = (pTrade->OrderRef);
	trade->vtOrderID = (trade->gatewayName + QString(".") + trade->tradeID);
	trade->direction = (pTrade->Direction);
	trade->offset = (pTrade->OffsetFlag);
	trade->price = (pTrade->Price);
	trade->volume = (pTrade->Volume);
	trade->tradeTime = (pTrade->TradeTime);

	// ���ͳɽ��������
	ee->putEvent(tradeEvent);
}

// �������ֲַ� 4�� ��¼���أ�				���򣬶�񣬿��򣬿ս�
// ֣���������������н����� 2�� ��¼���أ�	����񡢿����

// �óֲֻص��������Ὣ�ѱ�δ�ɽ���ί�е�������ָ��ί���ڳֲ��еĶ�����Ϣ

// CPositionBuffer�ǰ��ֲַ������֣��ڲ���_pos����ĳ��Լĳ����ĳֲ֣�
// ��˾���Ҫ���������ֲּ�¼�ϲ�������
void CtpTdApi::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	// ���سֲ�	
	if (!is_error_rsp(pRspInfo) && pInvestorPosition)
	{
		// ��ȡ����
		QMap<QString, CPositionBuffer>& allPosition_buffer = de->allPosition_buffer;
		QMap <QString, InstrumentInfo>& allInstruments = de->allInstruments;
		QMap<QString, PositionInfo>& allPosition = de->allPosition;

		// �ֲֲ�ѯ
		QString pos_name = QString(pInvestorPosition->InstrumentID) + QString(".") + QString(QChar(pInvestorPosition->PosiDirection));
		if (allPosition_buffer.find(pos_name) == allPosition_buffer.end())
		{
			CPositionBuffer temp_buffer;
			temp_buffer.setPositionBuffer(pInvestorPosition, QString("CTP"));
			allPosition_buffer.insert(pos_name, temp_buffer);	// ����
		}

		// ���³ֲֻ���,���ͳֲֻص��¼�
		Event posEvent(EVENT_POSITION);
		posEvent.data_ = std::move(std::shared_ptr<char>((char*)(new PositionInfo)));

		// PositionInfo �Ѿ��ǻ��ܺ�Ľ������
		PositionInfo* pos = (PositionInfo*)posEvent.data_.get();
		QString exchange_name = allInstruments[pInvestorPosition->InstrumentID].exchangeId;
		int size = allInstruments[pInvestorPosition->InstrumentID].multiplier;

		CPositionBuffer& pos_buffer = allPosition_buffer[pos_name];
		if (0 == exchange_name.compare("SHFE"))
		{
			// ֻ���������Ŵ��ڽ�ֺ���ָ���
			*pos = pos_buffer.updateShfeBuffer(pInvestorPosition, size);
			allPosition[pos_name] = *pos;
		}
		else
		{
			// ��������������ָ���
			*pos = pos_buffer.updateBuffer(pInvestorPosition, size);
			allPosition[pos_name] = *pos;
		}
		
		// ��ҹ���س��򣬾�˵����ֲֻ����
		ee->putEvent(posEvent);

		// ���¼�������UI
		if (bIsLast)
		{
			ee->putEvent(Event(EVENT_POSITION_UI));
		}
	}
}

void CtpTdApi::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//�����˻���Ϣ
	if (!is_error_rsp(pRspInfo) && pTradingAccount)
	{
		// ��ȡ����
		AccountInfo& accountInfo = de->accountInfo;

		// �˻����
		accountInfo.gatewayName = ("CTP");
		accountInfo.id = (pTradingAccount->AccountID);
		accountInfo.vtId = (accountInfo.gatewayName + QString(".") + accountInfo.id);

		// ��ֵ���
		accountInfo.preBalance = (pTradingAccount->PreBalance);
		accountInfo.available = (pTradingAccount->Available);
		accountInfo.commission = (pTradingAccount->Commission);
		accountInfo.margin = (pTradingAccount->CurrMargin);
		accountInfo.close_profit = (pTradingAccount->CloseProfit);
		accountInfo.position_profit = (pTradingAccount->PositionProfit);

		// �����balance�Ϳ����е��˻���ȷ���Ƿ�һ������Ҫ����
		auto data = pTradingAccount;
		double balance = data->PreBalance - data->PreCredit - data->PreMortgage + data->Mortgage -
			data->Withdraw + data->Deposit + data->CashIn - data->Commission + data->CloseProfit + data->PositionProfit;
		accountInfo.balance = (balance);
		
		if (bIsLast)
		{
			ee->putEvent(Event(EVENT_ACCOUNT));
		}
	}
}

void CtpTdApi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	if (!is_error_rsp(pRspInfo) && pInstrument)
	{
		// ��ȡ����
		QMap <QString, InstrumentInfo>&	allInstruments = de->allInstruments;

		// ��������
		QString id = pInstrument->InstrumentID;
		if (allInstruments.find(id) != allInstruments.end())
		{
			//�������
		}
		else
		{
			// ��Լ��Ϣtable
			InstrumentInfo instrumentInfo;
			instrumentInfo.id = id;
			allInstruments.insert(id, instrumentInfo);
		}

		//д���ڴ������
		InstrumentInfo& info = allInstruments[id];
		info.name = QString::fromLocal8Bit(pInstrument->InstrumentName);
		info.exchangeId = QString(pInstrument->ExchangeID);
		info.deadline = QDate::fromString(QString(pInstrument->ExpireDate), "yyyyMMdd");
		info.marginRate = pInstrument->LongMarginRatio;
		info.multiplier = pInstrument->VolumeMultiple;
		info.minimumUnit = pInstrument->PriceTick;

		if (bIsLast)
		{
			ee->putEvent(Event(EVENT_CONTRACT));
		}
	}
}

///�����ѯ��Լ����������Ӧ
void CtpTdApi::OnRspQryInstrumentCommissionRate(CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	if (!is_error_rsp(pRspInfo) && pInstrumentCommissionRate)
	{
		// ��ȡ����
		QMap <QString, InstrumentInfo>&	allInstruments = de->allInstruments;

		// �˴�ֻ���غ�Լ����
		QString id = pInstrumentCommissionRate->InstrumentID;
		// ����Լ�����и����͵ĺ�Լȡ��
		QStringList id_list;
		foreach(auto inst, allInstruments)
		{
			if (inst.id.contains(id))
			{
				id_list << inst.id;
			}
		}

		// ͳһ��ֵ����
		foreach(auto inst, id_list)
		{
			if (allInstruments.find(inst) != allInstruments.end())
			{
				//�������
			}
			else
			{
				InstrumentInfo instrumentInfo;
				instrumentInfo.id = inst;
				allInstruments.insert(inst, instrumentInfo);
			}

			//д���ڴ������
			InstrumentInfo& info = allInstruments[inst];
			double &oc = pInstrumentCommissionRate->OpenRatioByVolume;
			double &oc_rate = pInstrumentCommissionRate->OpenRatioByMoney;
			double &cc = pInstrumentCommissionRate->OpenRatioByVolume;
			double &cc_rate = pInstrumentCommissionRate->CloseRatioByMoney;
			double &today_cc = pInstrumentCommissionRate->CloseTodayRatioByVolume;
			double &today_cc_rate = pInstrumentCommissionRate->CloseTodayRatioByMoney;
			info.openCommission = (oc > oc_rate ? oc : oc_rate);
			info.closeCommission = (cc > cc_rate ? cc : cc_rate);
			info.closeTodayCommission = (today_cc > today_cc_rate ? today_cc : today_cc_rate);
		}
		
		if (bIsLast)
		{
			ee->putEvent(Event(EVENT_CONTRACT));
		}
	}
}

bool CtpTdApi::get_is_td_connect()
{
	return is_td_connect.load();
}

bool CtpTdApi::get_is_td_logout()
{
	return is_td_logout.load();
}

void CtpTdApi::ctp_td_init(QString tdAddress, QString userid, QString password, QString brokerid)
{
	// ���������Ϣ
	strncpy_s(usr_td_info.brokerid, brokerid.toStdString().c_str(), sizeof(usr_td_info.brokerid));
	strncpy_s(usr_td_info.userid, userid.toStdString().c_str(), sizeof(usr_td_info.userid));
	strncpy_s(usr_td_info.password, password.toStdString().c_str(), sizeof(usr_td_info.password));

	QString con_path = "conn_file/" + userid + "/td/";

	QDir temp;
	if (!temp.exists(con_path))
	{
		bool n = temp.mkpath(con_path);
	}

	char frontaddress[512] = { 0 };
	strncpy_s(frontaddress, tdAddress.toStdString().c_str(), sizeof(frontaddress));

	TdApi = CThostFtdcTraderApi::CreateFtdcTraderApi(con_path.toStdString().c_str());
	TdApi->RegisterSpi(this);

	//���Ĺ�������˽����
	TdApi->SubscribePublicTopic(THOST_TERT_RESTART);
	TdApi->SubscribePrivateTopic(THOST_TERT_RESTART);

	//ע�Ტ����ǰ�û�
	TdApi->RegisterFront(frontaddress);
	TdApi->Init();

	//�����������
	QueryQueue.cmd_begin();
	TradeQueue.cmd_begin();
}

void CtpTdApi::ctp_td_release()
{
	//����������
	QueryQueue.cmd_stop();
	TradeQueue.cmd_stop();

	if (TdApi != nullptr)
	{
		TdApi->RegisterSpi(nullptr);
		TdApi->Release();
		TdApi = nullptr;
	}
}

// ÿ6��ֱ��һ���ʽ�ͳֲ�
void CtpTdApi::ctp_td_query(Event ev)
{
	query_count += 1;

	if (query_count > query_trgger)
	{
		query_count = 0;

		if (0 == query_function_index)
		{
			ctp_td_getAccount();
			query_function_index += 1;
		}
		else
		{
			ctp_td_getPosition();
			query_function_index = 0;
		}
	}
}

void CtpTdApi::ctp_td_login()
{
	/*���ӳɹ���ʼ��¼*/
	CThostFtdcReqUserLoginField loginField = { 0 };
	strncpy_s(loginField.BrokerID, usr_td_info.brokerid, sizeof(loginField.BrokerID));
	strncpy_s(loginField.UserID, usr_td_info.userid, sizeof(loginField.UserID));
	strncpy_s(loginField.Password, usr_td_info.password, sizeof(loginField.Password));

	//��ָ��ŵ�����β��,�������ָ���ִ�з�������
	std::shared_ptr<CtpCommand> command = std::make_shared<LoginCommand>(TdApi, loginField, usr_td_info.requestID);
	QueryQueue.addCommand(command);
}

void CtpTdApi::ctp_td_logout()
{
	CThostFtdcUserLogoutField logoutField = { 0 };
	strncpy_s(logoutField.BrokerID, usr_td_info.brokerid, sizeof(logoutField.BrokerID));
	strncpy_s(logoutField.UserID, usr_td_info.userid, sizeof(logoutField.BrokerID));

	std::shared_ptr<CtpCommand> command = std::make_shared<LogoutCommand>(TdApi, logoutField, usr_td_info.requestID);
	QueryQueue.addCommand(command);
}

void CtpTdApi::ctp_td_getSettlement()
{
	CThostFtdcSettlementInfoConfirmField comfirmField = { 0 };
	strncpy_s(comfirmField.BrokerID, usr_td_info.brokerid, sizeof(comfirmField.BrokerID));
	strncpy_s(comfirmField.InvestorID, usr_td_info.userid, sizeof(comfirmField.BrokerID));

	std::shared_ptr<CtpCommand> command = std::make_shared<ComfirmSettlementCommand>(TdApi, comfirmField, usr_td_info.requestID);
	QueryQueue.addCommand(command);
}
void CtpTdApi::ctp_td_getInstrument()
{
	//��ѯ��Լ������Ϣ
	CThostFtdcQryInstrumentField qre_instr = { 0 };
	strncpy_s(qre_instr.ExchangeID, "", sizeof(qre_instr.ExchangeID));
	usr_td_info.requestID++;
	TdApi->ReqQryInstrument(&qre_instr, usr_td_info.requestID);
	std::this_thread::sleep_for(std::chrono::milliseconds(1100));	//�������ƣ�ÿ�β�ѯ���1��

	// ��ѯ��Լ���ʣ��ⲿ�������⣬ʼ�շ���-2��ֻ���ں�Լ��ѯ��ɺ󣬼�last==trueʱ��ѯ���ʡ�
	// ͬʱ����Լ����ֻ������Լ��ѯ����ÿ�β�ѯ����Ҫ˯1�룬�϶������ڳ�������ʱȫ����ѯ��ֻ���ڲ����м��ؽ��׺�Լ����
	// ctp_td_getCommission();
}

void CtpTdApi::ctp_td_getCommission(QString ins_id)
{
	//��ѯ��Լ������
	CThostFtdcQryInstrumentCommissionRateField qry_com = { 0 };
	strncpy_s(qry_com.BrokerID, usr_td_info.brokerid, sizeof(qry_com.BrokerID));
	strncpy_s(qry_com.InvestorID, usr_td_info.userid, sizeof(qry_com.InvestorID));
	strncpy_s(qry_com.InstrumentID, ins_id.toStdString().c_str(), sizeof(qry_com.InstrumentID));

	std::shared_ptr<CtpCommand> command = std::make_shared<InstrumentCommissionCommand>(TdApi, qry_com, usr_td_info.requestID);
	QueryQueue.addCommand(command);
}

void CtpTdApi::ctp_td_getAccount()
{
	CThostFtdcQryTradingAccountField accountField = { 0 };
	strncpy_s(accountField.BrokerID, usr_td_info.brokerid, sizeof(accountField.BrokerID));
	strncpy_s(accountField.InvestorID, usr_td_info.userid, sizeof(accountField.BrokerID));

	std::shared_ptr<CtpCommand> command = std::make_shared<QueryFundCommand>(TdApi, accountField, usr_td_info.requestID);
	QueryQueue.addCommand(command);
}

void CtpTdApi::ctp_td_getPosition()
{
	CThostFtdcQryInvestorPositionField accountField = { 0 };
	strncpy_s(accountField.BrokerID, usr_td_info.brokerid, sizeof(accountField.BrokerID));
	strncpy_s(accountField.InvestorID, usr_td_info.userid, sizeof(accountField.BrokerID));
	
	std::shared_ptr<CtpCommand> command = std::make_shared<QueryPositionCommand>(TdApi, accountField, usr_td_info.requestID);
	QueryQueue.addCommand(command);
}
QString CtpTdApi::ctp_td_send_limitOrder(TThostFtdcInstrumentIDType instrumentid, TThostFtdcPriceType price, TThostFtdcVolumeType volume,
	TThostFtdcDirectionType direction, TThostFtdcOffsetFlagType offset)
{
	CThostFtdcInputOrderField orderField = { 0 };
	
	strncpy_s(orderField.InstrumentID, instrumentid, sizeof(orderField.InstrumentID));
	orderField.OrderPriceType = THOST_FTDC_OPT_LimitPrice;		//�޼�
	orderField.Direction = direction;
	orderField.CombOffsetFlag[0] = offset;
	orderField.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;	//Ͷ�� 
	orderField.LimitPrice = price;
	orderField.VolumeTotalOriginal = volume;		//����
	orderField.TimeCondition = THOST_FTDC_TC_GFD;				//������Ч '3'
	orderField.VolumeCondition = THOST_FTDC_VC_AV;				//�κ����� '1'
	orderField.ContingentCondition = THOST_FTDC_CC_Immediately;	//��������'1'

	return ctp_td_order_insert(orderField);
}

QString CtpTdApi::ctp_td_send_marketOrder(TThostFtdcInstrumentIDType instrumentid, TThostFtdcVolumeType volume, TThostFtdcDirectionType direction, TThostFtdcOffsetFlagType offset)
{
	CThostFtdcInputOrderField orderField = { 0 };

	strncpy_s(orderField.InstrumentID, instrumentid, sizeof(orderField.InstrumentID));
	orderField.OrderPriceType = THOST_FTDC_OPT_AnyPrice;		//�м�
	orderField.Direction = direction;
	orderField.CombOffsetFlag[0] = offset;
	orderField.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;	//Ͷ�� 
	orderField.VolumeTotalOriginal = volume;		//����
	orderField.TimeCondition = THOST_FTDC_TC_IOC;				//������� '1'
	orderField.VolumeCondition = THOST_FTDC_VC_AV;				//�κ����� '1'
	orderField.ContingentCondition = THOST_FTDC_CC_Immediately;	//��������'1'

	return ctp_td_order_insert(orderField);
}

// ���鶼������������
//InstrumentID + FrontID + SessionID + OrderRef
//ExchangeID + TraderID + OrderLocalID
//ExchangeID + OrderSysID
void CtpTdApi::ctp_td_cancelOrder(TThostFtdcInstrumentIDType instrumentID, TThostFtdcExchangeIDType exchangeID, TThostFtdcOrderRefType orderID, TThostFtdcFrontIDType frontID, TThostFtdcSessionIDType sessionID)
{
	//���ó�����Ϣ
	CThostFtdcInputOrderActionField orderField = { 0 };
	strncpy_s(orderField.InstrumentID, instrumentID, sizeof(orderField.InstrumentID));
	strncpy_s(orderField.ExchangeID, exchangeID, sizeof(orderField.BrokerID));
	strncpy_s(orderField.OrderRef, orderID, sizeof(orderField.OrderRef));
	orderField.FrontID = frontID;
	orderField.SessionID = sessionID;

	orderField.ActionFlag = THOST_FTDC_AF_Delete;	//ɾ������ '0'
	strncpy_s(orderField.BrokerID, usr_td_info.brokerid, sizeof(orderField.BrokerID));
	strncpy_s(orderField.InvestorID, usr_td_info.userid, sizeof(orderField.InvestorID));
	
	std::shared_ptr<CtpCommand> command = std::make_shared<WithdrawOrderCommand>(TdApi, orderField, usr_td_info.requestID);
	TradeQueue.addCommand(command);
}

//////////////////////////////////////////////////////////////////////////

bool CtpTdApi::is_error_rsp(CThostFtdcRspInfoField *pRspInfo)
{
	return (pRspInfo && (pRspInfo->ErrorID != 0));
}

QString CtpTdApi::ctp_td_order_insert(CThostFtdcInputOrderField& orderField)
{
	if (!is_td_tradable.load())
	{
		// ���㵥δȷ��(����qDebug�ĵط��������־���ģ��)
		qDebug() << "Miss Settlement Confirmation";
		return "Error";
	}

	strncpy_s(orderField.BrokerID, usr_td_info.brokerid, sizeof(orderField.BrokerID));
	strncpy_s(orderField.InvestorID, usr_td_info.userid, sizeof(orderField.InvestorID));

	_snprintf_s(orderField.OrderRef, sizeof(orderField.OrderRef), _TRUNCATE, "%012d", ++usr_td_info.maxOrderRef);

	orderField.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;	//��ǿƽ '0'
	orderField.IsAutoSuspend = 0;
	orderField.UserForceClose = 0;

	// ����һ�����񵽶���
	std::shared_ptr<CtpCommand> command = std::make_shared<InsertOrderCommand>(TdApi, orderField, usr_td_info.requestID);
	TradeQueue.addCommand(command);

	return orderField.OrderRef;
}

void CtpTdApi::ctp_td_order_update(OrderInfo& order)
{
	// ��ȡ����
	QMap<QString, OrderInfo>& allOrderDict = de->allOrderDict;
	QMap<QString, OrderInfo>& workingOrderDict = de->workingOrderDict;

	// �����������ί��
	QString ordID = order.orderID;
	if (allOrderDict.find(ordID) != allOrderDict.end())
	{
		// �������
		allOrderDict[ordID] = order;
	}
	else
	{
		// ���������
		allOrderDict.insert(ordID, order);
	}
	// �޸Ļί��
	QChar status = order.status;
	auto working_find_iter = workingOrderDict.find(ordID);
	if (working_find_iter != workingOrderDict.end())
	{
		if (status == THOST_FTDC_OST_AllTraded || status == THOST_FTDC_OST_Canceled)
		{
			workingOrderDict.erase(working_find_iter);
		}
	}
	else
	{
		workingOrderDict.insert(ordID, order);
	}
}