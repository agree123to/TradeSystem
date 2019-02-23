#pragma once

#include <QtWidgets/QMainWindow>

#include "ui_maintrade.h"

#include "MainEngine.h"

#include "TableView/tableView_Account.h"
#include "TableView/tableView_Quote.h"
#include "TableView/tableView_Position.h"
#include "TableView/tableView_InstrumentInfo.h"
#include "TableView/tableView_Order.h"

#include "qt_basictool/CompleteLineEdit.h"

extern MainEngine* me;

class MainTrade : public QMainWindow
{
	Q_OBJECT

public:
	MainTrade(QWidget *parent = 0);

	bool eventFilter(QObject *, QEvent *) override;

private:
	Ui::MainTradeClass ui;

	// ������ʱȫ��ˢ�£����ڿ��Ż�Ϊֻ�����¼����͵���һ����¼
	TableView_Account* tableView_account;
	TableView_Position* tableView_position;
	TableView_Quote* tableView_quote;
	TableView_InstrumentInfo* tableView_instrumentInfo;
	TableView_Order* tableView_order;

	// �����༭��
	CompleteLineEdit* order_contract_edit;

private Q_SLOTS:
	void OnInstrmentLineTextSubscribe();
	void OnSendOrderClicked();
	void OnCancelAll();

	void OnStartCTA();
	void OnStopCTA();
private:
	bool getSendOrderRequest(orderCommonRequest& order_field);
};
