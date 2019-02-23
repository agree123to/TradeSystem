#pragma once

#include <QtWidgets/QLineEdit>
#include <QStringList> 

class QListView;  
class QStringListModel;  
class QModelIndex;  

class CompleteLineEdit : public QLineEdit 
{
    Q_OBJECT  
public:  
    CompleteLineEdit(QWidget *parent = 0);
	
public slots:
	// ��̬����ʾ����б�  
    void setCompleter(const QString &text);
	// �������б��е��ʹ�ô����Զ��������ĵ���
    void completeText(const QModelIndex &index);

protected:  
    virtual void keyPressEvent(QKeyEvent *e);  
    virtual void focusOutEvent(QFocusEvent *e);  

private:  
    QStringList words; // ��������б�ĵ���  
    QListView* listView; // ����б�  
    QStringListModel* model; // ����б��model  
};