#pragma once

#include <string>
#include <memory>

#define EVENT_LOG "eLog"                    //# ��־�¼���ͨ��ʹ��ĳ����������ֱ����ʾ
#define EVENT_TDLOGIN "eTdLogin"            //# ���׷�������¼�ɹ��¼�

#define EVENT_TIMER "eTimer"                //# ��ʱ���¼���ÿ��1�뷢��һ��

#define EVENT_CONTRACT  "eContract"         // # ��Լ��ѯ�ر��¼�
#define EVENT_INVESTOR  "eInvestor"         // # Ͷ���߲�ѯ�ر��¼�
#define EVENT_ACCOUNT   "eAccount"          // # �˻���ѯ�ر��¼�

#define EVENT_POSITION	  "ePosition"       // # �ֲֲ�ѯ�ر��¼�
#define EVENT_POSITION_UI "ePositionUI"     // # �ֲֲ�ѯ�ر��¼�������UI��

#define EVENT_TICK "eTick"					//# ���������¼�
#define EVENT_TICK_CONTRACT "eTick."		//# �ض���Լ�������¼�

#define EVENT_ORDER "eOrder"                //# ���������¼�
#define EVENT_ORDER_ORDERREF "eOrder."      //# �ض������ŵı����¼�

#define EVENT_TRADE "eTrade"                //# �ɽ������¼�
#define EVENT_TRADE_CONTRACT "eTrade."      //# �ض���Լ�ĳɽ��¼�

//////////////////////////////////////////////////////////////////////////
#define EVENT_STG_BEG "eBegStg"             //# CTA���Կ����¼�
#define EVENT_STG_END "eEngStg"				//# CTA���Թر��¼�

// ��Event����ÿһ���ص������У�data_�������ϵ�����ָ�룬��vnpy���ڻص������ڲ���char* dataת��Ϊ��Ҫ����������
// ���ڿ���ʹ��tc_malloc�����е�new��ȡ���ڴ���У��������ܵ�ͬʱ�����ڴ���Ƭ
struct Event
{
	Event() = default;
	Event(std::string type) : type_(type){}

	std::string type_;
	std::shared_ptr<char> data_;
};