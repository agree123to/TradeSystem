#pragma once

#include <QString>
#include <QDateTime>

//��¼�˻��Ļ�����Ϣ���û�ID�������˺ţ��˺����룬�����̴��룬ǰ�û���ַ
struct AccountID
{
public:
	AccountID& operator=(const AccountID&) = delete;

public:
	QString apiName;
	QString investorID;
	QString password;
	QString brokerID;
	QString frontAddress;
	int connectionStatus;
};

// �û��ʺ��ʽ��˻���Ϣ
struct AccountInfo
{
	// vnpy�ʺ����
	QString id;
	QString vtId;
	QString gatewayName;

	// vnpy��ֵ���
	double preBalance;
	double balance;
	double available;
	double commission;
	double margin;
	double close_profit;
	double position_profit;
};

//��Լ��Ϣ��id�����֡���������š���󽻸��ա���֤���ʡ���Լ��������֤���ʡ������ѡ���С�䶯��λ�������Ƿ���Խ���
struct InstrumentInfo
{
public:
	//Ϊ�˷���set�����У����� < �����
	bool operator < (const InstrumentInfo &instr) { return id < instr.id; };

public:
	QString id;
	QString name;
	QString exchangeId;
	QDate deadline;

	double marginRate;
	int multiplier;
	double openCommission;
	double closeCommission;
	double closeTodayCommission;
	double minimumUnit;
	bool tradable;

	bool has_subsribed{ false };
};

// ί�е���Ϣ
struct OrderInfo
{
	// ���������
	QString symbol;
	QString vtSymbol;
	QString exchange;

	QString orderID;
	QString vtOrderID;

	// �������
	QChar direction;
	QChar offset;
	double price;
	int totalVolume;
	int tradeVolume;
	QChar status;

	QString orderTime;
	QString cancelTime;

	int frontID;
	int sessionID;

	QString gatewayName;
};

// �ɽ���Ϣ
struct TradeInfo
{
	// ���������
	QString symbol;
	QString vtSymbol;
	QString exchange;

	QString tradeID;
	QString vtTradeID;

	QString orderID;
	QString vtOrderID;

	// �ɽ����
	QChar direction;
	QChar offset;
	double price;
	int volume;
	QString tradeTime;

	QString gatewayName;
};

// �ֲ���Ϣ
struct PositionInfo
{
	// ���ǵ��ֽ׶δ󲿷�CTP�е�ExchangeID�ֶη��صĶ��ǿ�ֵ, vtSymbolֱ��ʹ��symbol
	QString symbol;
	QString vtSymbol;

	QString directName;
	QChar direction{ 0 };
	int position{ 0 };

	int frozen{ 0 };
	double price{ 0 };

	QString vtPositionName;
	int ydPosition{ 0 };

	QString gatewayName;
};

// Tick����
struct QuoteInfo
{
	QString symbol;
	QString vtSymbol;

	int volume;
	int openInterest;

	double openPrice;
	double highPrice;
	double lowPrice;
	double lastPrice;

	double bidPrice1;
	int bidVolume1;
	double askPrice1;
	int askVolume1;

	double upperLimit;
	double lowerLimit;

	double preClosePrice;

	QString time;
	QString gatewayName;
};