#include "maintrade.h"
#include <QtWidgets/QApplication>
#include "MainEngine.h"

#include <vector>
#include <algorithm>
#include <iostream>

#include "TechIndicator.h"
#include "GlogWrapper.h"

// ȫ�������棬���ڿɸ���ɵ���
MainEngine* me = nullptr;


int main(int argc, char *argv[])
{
	// ��ʼ����־
	CGLog::get_glog()->init_log("./log/");

	//GLOG("Hello, world_info", CGLog::CGLog_INFO);
	//GLOG("Hello, world_warning", CGLog::CGLog_WARNING);
	//GLOG("Hello, world_error", CGLog::CGLog_ERROR);
	/* GLOG("Hello, world_fatal", CGLog::CGLog_FATAL);	// �����⣿ */

	// ��ʼ��talib
	TechIndicator::initialize();
	
	// ����������
	QApplication app(argc, argv);
	me = new MainEngine();

	MainTrade win;

	// ����QT�¼�ѭ��
	win.show();
	app.exec();

	if (me)
	{
		delete me;
		me = nullptr;
	}

	// �ر�talib
	TechIndicator::taShutdown();
	// �ر���־
	CGLog::get_glog()->release_log();

	return 0;
}
