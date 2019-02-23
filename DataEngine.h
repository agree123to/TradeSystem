#pragma once


#include <QMap>
#include <QStringList> 

#include "Model/PublicStruct.h"
#include "Model/PositionBuffer.h"

struct user_trade_info
{
	// ��¼�ʺ���Ϣ
	TThostFtdcBrokerIDType	brokerid;
	TThostFtdcUserIDType	userid;
	TThostFtdcPasswordType	password;
	// ������
	TThostFtdcDateType trading_day;

	//������
	int requestID{ 0 };
	//���ص���󱨵�����
	int maxOrderRef{ 0 };

	// �Ự���
	int front_id{ 0 };
	int session_id{ 0 };

	user_trade_info()
	{
		memset(trading_day, 0x00, sizeof(trading_day));
	}
};

struct orderCommonRequest
{
	char instrument[31];
	double price;
	int volume;
	char direction;
	char offset;

	orderCommonRequest()
	{
		memset(this, 0x00, sizeof(orderCommonRequest));
	}
};

struct cancelCommonRequest
{
	char instrument[31];
	char exchange[9];

	// CTP, LTS���
	char order_ref[13];
	int session_id;
	int front_id;

	cancelCommonRequest()
	{
		memset(this, 0x00, sizeof(cancelCommonRequest));
	}
};

// �洢ģ�鹫������
class DataEngine: public QObject
{
public:
	bool de_get_contract(QString vtSymbol, InstrumentInfo& contract);
	bool de_get_order(QString ordID, OrderInfo& ordInfo);
	

public:
	/***MD Interface***/
	// Tick�ص���tick��Ҫ����һ�£�
	QMap <QString, CThostFtdcDepthMarketDataField> lastMarketDataSet;

	/***TD Interface***/
	// �˻���Ϣ
	AccountInfo accountInfo;
	// �ֲֲ�ѯ���棨������ֺͽ�֣�<key: vtSymbol.�ֲַ���>��������ά���������ֲ�����
	QMap<QString, CPositionBuffer>	allPosition_buffer;
	// �ֲֲ�ѯ <key: vtSymbol.�ֲַ���>���������û��ֲ��б�չʾ�ĳֱֲ���
	QMap<QString, PositionInfo>		allPosition;
	// ������к�Լ��Ϣ����UI����չʾ
	QMap <QString, InstrumentInfo>	allInstruments;
	// ���׿����������ί��
	QMap<QString, OrderInfo>		allOrderDict;
	// ���׻ί��
	QMap<QString, OrderInfo>		workingOrderDict;
};