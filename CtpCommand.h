#pragma once

#include <qthread.h>
#include <qtimer.h>
#include <memory>
#include <thread>
#include <chrono>

#include "cpp_basictool/CppQueue.hpp"
#include "ctpapi/ThostFtdcTraderApi.h"

//��װctp����Ľӿ�
class CtpCommand
{
public:
	virtual ~CtpCommand()
	{
		api = nullptr;
	}
	virtual int execute() = 0;

protected:
	CtpCommand::CtpCommand(int& requestID, CThostFtdcTraderApi *api)
		:requestID(requestID)
	{
		this->api = api;
		requestID++;
	}

	int& requestID;
	CThostFtdcTraderApi* api;
};
/*��¼*/
class LoginCommand :public CtpCommand
{
public:
	LoginCommand::LoginCommand(CThostFtdcTraderApi *api, CThostFtdcReqUserLoginField& loginField, int &requestID)
		:CtpCommand(requestID, api)
	{
		memcpy_s(&(this->loginField), sizeof(CThostFtdcReqUserLoginField), &loginField, sizeof(CThostFtdcReqUserLoginField));
	}
	int LoginCommand::execute()
	{
		return api->ReqUserLogin(&loginField, requestID);
	}
private:
	CThostFtdcReqUserLoginField loginField;
};
/*�ǳ�*/
class LogoutCommand : public CtpCommand
{
public:
	LogoutCommand::LogoutCommand(CThostFtdcTraderApi *api, CThostFtdcUserLogoutField& logoutField, int &requestID)
		: CtpCommand(requestID, api)
	{
		memcpy_s(&(this->logoutField), sizeof(CThostFtdcUserLogoutField), &logoutField, sizeof(CThostFtdcUserLogoutField));
	}
	int LogoutCommand::execute()
	{
		return api->ReqUserLogout(&logoutField, requestID);
	}
private:
	CThostFtdcUserLogoutField logoutField;
};
/*ȷ�Ͻ�����*/
class ComfirmSettlementCommand :public CtpCommand
{
public:
	ComfirmSettlementCommand::ComfirmSettlementCommand(CThostFtdcTraderApi *api, CThostFtdcSettlementInfoConfirmField& comfirmField, int &requestID) 
		:CtpCommand(requestID, api)
	{
		memcpy_s(&(this->comfirmField), sizeof(CThostFtdcSettlementInfoConfirmField), &comfirmField, sizeof(CThostFtdcSettlementInfoConfirmField));
	}
	int ComfirmSettlementCommand::execute()
	{
		return api->ReqSettlementInfoConfirm(&comfirmField, requestID);
	}
private:
	CThostFtdcSettlementInfoConfirmField comfirmField;
};
/*��ѯ�ʽ�*/
class QueryFundCommand :public CtpCommand
{
public:
	QueryFundCommand::QueryFundCommand(CThostFtdcTraderApi *api, CThostFtdcQryTradingAccountField& accountField,int &requestID)
		:CtpCommand(requestID, api)
	{
		memcpy_s(&(this->accountField), sizeof(CThostFtdcQryTradingAccountField), &accountField, sizeof(CThostFtdcQryTradingAccountField));
	}
	int QueryFundCommand::execute()
	{
		return api->ReqQryTradingAccount(&accountField, requestID);
	}
private:
	CThostFtdcQryTradingAccountField accountField;
};
/*��ѯ�ֲ�*/
class QueryPositionCommand :public CtpCommand
{
public:
	QueryPositionCommand::QueryPositionCommand(CThostFtdcTraderApi *api, CThostFtdcQryInvestorPositionField& accountField,int &requestID) 
		:CtpCommand(requestID, api)
	{
		memcpy_s(&(this->accountField), sizeof(CThostFtdcQryInvestorPositionField), &accountField, sizeof(CThostFtdcQryInvestorPositionField));
	}
	int QueryPositionCommand::execute()
	{
		return api->ReqQryInvestorPosition(&accountField, requestID);
	}
private:
	CThostFtdcQryInvestorPositionField accountField;
};
/*��������*/
class InsertOrderCommand :public CtpCommand
{
public:
	InsertOrderCommand::InsertOrderCommand(CThostFtdcTraderApi *api, CThostFtdcInputOrderField& orderField,int &requestID) 
		:CtpCommand(requestID, api)
	{
		memcpy_s(&(this->orderField), sizeof(CThostFtdcInputOrderField), &orderField, sizeof(CThostFtdcInputOrderField));
	}
	int InsertOrderCommand::execute()
	{
		return api->ReqOrderInsert(&orderField, requestID);
	}
private:
	CThostFtdcInputOrderField orderField;
};
/*��������*/
class WithdrawOrderCommand :public CtpCommand
{
public:
	WithdrawOrderCommand::WithdrawOrderCommand(CThostFtdcTraderApi *api, CThostFtdcInputOrderActionField& orderField,int &requestID) 
		:CtpCommand(requestID, api)
	{
		memcpy_s(&(this->orderField), sizeof(CThostFtdcInputOrderActionField), &orderField, sizeof(CThostFtdcInputOrderActionField));
	}
	int WithdrawOrderCommand::execute()
	{
		return api->ReqOrderAction(&orderField, requestID);
	}
private:
	CThostFtdcInputOrderActionField orderField;
};
/*��Լ���ʲ�ѯ����*/
class InstrumentCommissionCommand :public CtpCommand
{
public:
	InstrumentCommissionCommand::InstrumentCommissionCommand(CThostFtdcTraderApi *api, CThostFtdcQryInstrumentCommissionRateField& comField, int &requestID)
		:CtpCommand(requestID, api)
	{
		memcpy_s(&(this->comField), sizeof(CThostFtdcQryInstrumentCommissionRateField), &comField, sizeof(CThostFtdcQryInstrumentCommissionRateField));
	}
	int InstrumentCommissionCommand::execute()
	{
		return api->ReqQryInstrumentCommissionRate(&comField, requestID);
	}
private:
	CThostFtdcQryInstrumentCommissionRateField comField;
};

//ָ�����
class CommandQueue :public QThread
{
	Q_OBJECT
public:

	CommandQueue(int time) :sleep_time(time) {}
	~CommandQueue() = default;

	void cmd_begin()
	{
		// �������п���
		cmd_active.store(true);

		// �����߳�
		thread_active.store(true);
		start();
	}

	void cmd_stop()
	{
		// �رն��п��أ����ȴ�����ִ�����
		cmd_active.store(false);
		while (!commandQueue.empty())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
		}

		// �˳��߳�
		thread_active.store(false);
		commandQueue.notify_all(); // ֪ͨ�ȴ��ڿն����ϵ��߳�

		quit();
		wait();
	}

	void CommandQueue::addCommand(std::shared_ptr<CtpCommand> newCommand)
	{
		if (cmd_active.load())
		{
			commandQueue.push_back(newCommand);
		}
	}

private:
	ConcurrentQueue<std::shared_ptr<CtpCommand>> commandQueue;

	int  sleep_time{ 0 };
	std::atomic<bool> thread_active{ false };
	std::atomic<bool> cmd_active{ false };

private:
	void CommandQueue::cut_in_command(std::shared_ptr<CtpCommand> newCommand)
	{
		if (cmd_active.load())
		{
			commandQueue.push_front(newCommand);
		}
	}

	void CommandQueue::run()
	{
		while (thread_active.load())
		{
			// ��ȡ����
			std::shared_ptr<CtpCommand> command;
			if (commandQueue.wait_and_pop(command, -1))
			{
				if (command->execute())
				{
					// ���Ժܳ�ʱ�䵼�¶��������󣬵���˳���ť�����ֲ�ѯ�����������²�ȥ�����³���ʼ���޷������˳��رգ�����Ҫ��һ����ʲôԭ��
					cut_in_command(command);
				}
	
				// ����ָ��ɹ�,��Ϣһ�루CTPҪ��
				std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
			}
		}
	}
};