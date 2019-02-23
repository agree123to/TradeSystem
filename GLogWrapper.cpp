#include "GLogWrapper.h"

const CGLog*		 CGLog::google_log = new CGLog();
const CGLog::GcGlog  CGLog::gc_log;

void CGLog::set_default_setting(const std::string& log_dir)
{
#ifdef _DEBUG
	//������־·��  INFO WARNING ERROR FATAL 
	std::string sub_dir = log_dir + "info/";
	google::SetLogDestination(google::INFO, sub_dir.c_str());	
	sub_dir = log_dir + "warning/";
	google::SetLogDestination(google::WARNING, sub_dir.c_str());
	sub_dir = log_dir + "error/";
	google::SetLogDestination(google::ERROR, sub_dir.c_str());
	sub_dir = log_dir + "fatal/";
	google::SetLogDestination(google::FATAL, sub_dir.c_str());

	FLAGS_logtostderr = false;									//�Ƿ�������־����� stderr�������ļ�
	FLAGS_alsologtostderr = false;								//�Ƿ�ͬʱ����־������ļ���stderr
	FLAGS_minloglevel = google::INFO;							//��������� stderr �Ĳ�����Ϣ�������˴��󼶱�͸��ߴ��󼶱����־��Ϣ 
	FLAGS_stderrthreshold = google::INFO;						//���˽���־������ļ�֮�⣬�����˴��󼶱�͸��ߴ��󼶱����־ͬʱ����� stderr�����ֻ��ʹ�� -stderrthreshold=1 ����������ã�������ʹ�û�����������ʽ��������������������������������
	FLAGS_colorlogtostderr = false;								//������� stderr �ϵĴ�����־��ʾ��Ӧ����ɫ 
	FLAGS_max_log_size = 10;									//�����־�ļ���С10M.
	//vmodule(string, default = "");����						//���ļ����������ļ�����׺��֧��ͨ����������Զ�����־�Ŀ���������磺GLOG_vmodule=server=2,client=3 ��ʾ�ļ���Ϊserver.* ��ֻ���С�� 2 ����־���ļ���Ϊ client.* ��ֻ���С�� 3 ����־�����ͬʱʹ�� GLOG_v ѡ������� GLOG_v ѡ�
	//google::SetLogFilenameExtension("log_");
#else
	//������־·��  INFO WARNING ERROR FATAL 
	std::string sub_dir = log_dir + "info/";
	google::SetLogDestination(google::INFO, sub_dir.c_str());
	sub_dir = log_dir + "warning/";
	google::SetLogDestination(google::WARNING, sub_dir.c_str());
	sub_dir = log_dir + "error/";
	google::SetLogDestination(google::ERROR, sub_dir.c_str());
	sub_dir = log_dir + "fatal/";
	google::SetLogDestination(google::FATAL, sub_dir.c_str());

	FLAGS_logtostderr = false;
	FLAGS_alsologtostderr = false;
	FLAGS_minloglevel = google::INFO;
	FLAGS_stderrthreshold = google::ERROR;
	FLAGS_colorlogtostderr = false;
	FLAGS_max_log_size = 10;
	//vmodule(string, default = "")
#endif
}

void CGLog::print_log(char* out_msg, int severity)
{
	switch (severity)
	{
	case CGLog_INFO:
		LOG(INFO) << out_msg;
		break;
	case CGLog_WARNING:
		LOG(WARNING) << out_msg;
		break;
	case CGLog_ERROR:
		LOG(ERROR) << out_msg;
		break;
	case CGLog_FATAL:
		LOG(FATAL) << out_msg;
		break;
	default:
		LOG(INFO) << out_msg;
		break;
	}
}