#pragma once

#include <WinSock2.h>

#include "mysql/include/mysql.h"
#pragma comment(lib,"mysql/lib/libmysql.lib")

#include <map>
#include <vector>
#include <sstream>

// ���ݿ�����
struct DBConf
{
	std::string _host;
	std::string _user;
	std::string _password;
	std::string _database;
	std::string _charset;
	int _port{ 0 };
	// �ͻ��˱�ʶ
	int _flag{ 0 };

	// ��ȡ���ݿ�����
	void loadFromMap(const std::map<std::string, std::string>& mpParam)
	{
		// ������������ʱ�����뱨��?
		std::map<std::string, std::string> mpTmp = mpParam;

		_host = mpTmp["dbhost"];
		_user = mpTmp["dbuser"];
		_password	= mpTmp["dbpass"];
		_database	= mpTmp["dbname"];
		_charset	= mpTmp["charset"];
		_port = atoi(mpTmp["dbport"].c_str());
		_flag = 0;

		if (0 == mpTmp["dbport"].compare(""))
		{
			_port = 3306;
		}
	}
};


/*
* MySQL���ݿ������
* ���̰߳�ȫ��ͨ��һ���߳�һ��MySQL����
* ����insert/update�����и��õĺ�����װ����֤SQLע��
* DB_INT��ʾ��װsql���ʱ������""��ת��
* DB_STR��ʾ��װsql���ʱ��  ��""��ת��
*/
class CMySQL
{
public:
	CMySQL();
	CMySQL(const std::string& host, const std::string& user, const std::string& pass, const std::string& databd, const std::string& charset, int port, int flag);
	CMySQL(const DBConf& dbconf);
	~CMySQL();

	// ���ο�������͸�ֵ����
	CMySQL(const CMySQL& mysql) = delete;
	CMySQL& operator = (const CMySQL& mysql) = delete;

	void init(const std::string& host, const std::string& user = "", const std::string& pass = "", const std::string& databd = "", const std::string& charset = "", int port = 0, int flag = 0);
	void init(const DBConf& dbconf);

	void connect();
	void disconnect();

	// ��ȡ���ݿ�ָ��
	MYSQL* get_mysql();
	// ��ȡ���ݿ����
	std::string get_variables(const std::string& name);
	// �ַ�ת�壬��Ҫ���ӵ����ݿ⣬���ǵ����ַ���
	std::string real_escape_string(const std::string& from);
	// ���ºͲ�������
	void execute(const std::string& sql);

	// mysql��һ����¼
	class MySqlRecord
	{
	public:
		MySqlRecord(const std::map<std::string, std::string>& record);
		// ��ȡ��¼�е�ĳ���ֶ�
		const std::string& operator[](const std::string& field);
	protected:
		const std::map<std::string, std::string>& _record;
	};
	// ��ѯ������mysql����
	class MySqlData
	{
	public:
		// ������������
		std::vector<std::map<std::string, std::string>>& data();
		// ��ȡ��¼����
		size_t size();
		// ��ȡĳһ����¼
		MySqlRecord operator[](size_t idx);

	protected:
		std::vector<std::map<std::string, std::string>> _data;
	};

	// sql���
	MySqlData query_record(std::string& sql);

	// �����ֶ�����
	enum FT
	{
		DB_INT,
		DB_STR
	};

	// ���ݼ�¼
	// map������һ����������ڲ���pair�ڵ��keyĬ��������keyΨһ
	// ��pairֻ���ڲ���ʾ��ֵ�Ե�Ԫ��<�ֶ��������ͣ�ֵ>
	using RECORD_DATA = std::map<std::string, const std::pair<FT, std::string>>;

	// ��ȡ��ѯ���������
	size_t get_record_count(const std::string& table_name, const std::string& condition = "");
	// ��ȡsql���ؽ��������
	size_t get_record_count(const std::string& condition = "");
	// ���ڼ�¼
	bool exist_record(const std::string& condition);
	// ��ȡ�ֶ����ֵ
	int get_max_value(const std::string& table_name, const std::string& field_name, const std::string& condition);
	// ��ȡ������ID
	long last_insert_id();

	// ��������
	size_t update_record(const std::string& table_name, const RECORD_DATA& columns, const std::string& condition);
	size_t insert_record(const std::string& table_name, const RECORD_DATA& columns);
	size_t replace_record(const std::string& table_name, const RECORD_DATA& columns);
	size_t update_record(const std::string& table_name, const std::string& condition = "");

	// ����sql���
	std::string build_insert_sql(const std::string& table_name, const RECORD_DATA& columns);
	std::string build_replace_sql(const std::string& table_name, const RECORD_DATA& columns);
	std::string build_update_sql(const std::string& table_name, const RECORD_DATA& columns, const std::string& condition);

	std::string get_last_sql();
	size_t get_affected_rows();

	//////////////////////////////////////////////////////////////////////////
	// �ַ�ת�壬�������ַ�������һ�����գ�
	static void build_ostringstream(const RECORD_DATA& columns, std::ostringstream& column_names, std::ostringstream& column_values);
	static std::string escape_string_nosafe(const std::string& from);
	static std::string build_insert_sql_nosafe(const std::string& table_name, const RECORD_DATA& columns);
	static std::string build_replace_sql_nosafe(const std::string& table_name, const RECORD_DATA& columns);
	static std::string build_update_sql_nosafe(const std::string& table_name, const RECORD_DATA& columns, const std::string& condition);

private:
	// ���ݿ�ָ��
	MYSQL* _ptrMysql{ nullptr };
	// ���ݿ�����
	DBConf	_dbConf;
	// �Ƿ�����
	bool _isConnected{ false };
	// ���ִ�е�sql
	std::string _lastSql;
};