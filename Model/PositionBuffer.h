#pragma once

#include "PublicStruct.h"
#include "..\ctpapi\ThostFtdcUserApiStruct.h"

class CPositionBuffer
{
public:
	void setPositionBuffer(CThostFtdcInvestorPositionField* data, QString gatewayName)
	{
		symbol = data->InstrumentID;
		direction = data->PosiDirection;

		pos_.symbol = (symbol);
		pos_.vtSymbol = (symbol);
		pos_.gatewayName = (gatewayName);
		pos_.direction = (direction);
		pos_.vtPositionName = (pos_.vtPositionName + QString(".") + QString(direction));
	}

	const PositionInfo& get_position()
	{
		return pos_;
	}

	// �������������棬���ظ��º�ĳֲ�����
	PositionInfo updateShfeBuffer(CThostFtdcInvestorPositionField* data, int size)
	{
		// ��ֺͽ�ֵ����ݸ����Ƿ���������¼��ģ������Ҫ�жϼ�������¼��Ӧ��λ
		// ��Ϊ����ֶ�TodayPosition���ܱ�Ϊ0����ȫ��ƽ�֣�����˷ֱ�������Ҫ��YdPosition�ֶ�
		if (data->YdPosition)
		{
			yd_postion = data->Position;
			yd_position_cost = data->PositionCost;
		}
		else
		{
			today_position = data->Position;
			today_position_cost = data->PositionCost;
		}

		// �ֲֵ���ֺͽ����Ӻ�Ϊ�ֲܳ�
		pos_.position = (today_position + yd_postion);
		pos_.ydPosition = (yd_postion);

		// �����ͷ���гֲ֣���ͨ����Ȩƽ����ʽ����ֲ־���
		if (today_position || yd_postion)
		{
			pos_.price = ((yd_position_cost + today_position_cost) / ((yd_postion + today_position) * size));
		}
		// ����۸�Ϊ0
		else
		{
			pos_.price = (0);
		}

		// ��ո���
		if (pos_.direction == '2')
		{
			pos_.directName = ("Long");
		}
		else
		{
			pos_.directName = ("Short");
		}

		return pos_;
	}

	// ���������������Ļ��棬���ظ��º�ĳֲ�����
	PositionInfo updateBuffer(CThostFtdcInvestorPositionField* data, int size)
	{
		// �����������������ֽ������ֻ�����ܲ�λ�����Ϊ0
		pos_.position = (data->Position);
		pos_.ydPosition = (0);

		if (data->Position)
		{
			pos_.price = (data->PositionCost / (data->Position * size));
		}
		else
		{
			pos_.price = (0);
		}

		// ��ո���
		if (pos_.direction == '2')
		{
			pos_.directName = ("Long");
		}
		else
		{
			pos_.directName = ("Short");
		}

		return pos_;
	}

	// �����yd_postion �� pos_.yd_position���ǵ�ǰ��֡��ص��е�YdPosition��������������ֺͽ���ж�
private:
	// ���뵱ǰһ�λص��������������ݣ����� ��� �� ���
	QString symbol;
	QChar direction{ 0 };
	int today_position{ 0 };
	int yd_postion{ 0 };
	double today_position_cost{ 0 };
	double yd_position_cost{ 0 };

	// ���ܵ�ǰ�ֲ����ݵĳֲ֣�����ʷ�ֲֻ��棬����һ�λص�����ʱ���¡�
	// ����Ǳ���ά���������ֲ�����
	PositionInfo pos_;
};