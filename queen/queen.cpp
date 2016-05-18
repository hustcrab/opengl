#include<iostream>
#include<string>
using namespace std;
#define QUEUELEN 10
struct DATA{
	string name;
	int age;
};
struct SQType
{
	DATA data[QUEUELEN];   //�������� 
	int head;      //��ͷ 
	int tail;      //��β 
};
/*******************���еĳ�ʼ��*************************/
SQType *SQTypeInit()
{
	SQType * q;
	if (q = new SQType)    //������е��ڴ�ռ�
	{
		q->head = 0;     //���ö��� 
		q->tail = 0;     //���ö�β 
		return q;
	}
	else
	{
		return NULL;    //���ؿ� 
	}
}
/*******************�жϿն���*************************/
int SQTypeIsEmpty(SQType *q)
{
	return(q->head == q->tail);
}
/*******************�ж�������*************************/
int SQTypeIsFull(SQType *q)
{
	return(q->tail == QUEUELEN);
}
/*******************��ն���*************************/
void SQTypeClear(SQType *q)
{
	q->head = 0;
	q->tail = 0;
}
/*******************�ͷſռ�*************************/
void SQTypeFree(SQType *q)
{
	if (q != NULL) delete q;
}
/*******************����в���*************************/
int InSQType(SQType *q, DATA data)
{
	if (q->tail == QUEUELEN)
	{
		cout << "��������������ʧ�ܣ�" << endl;
		return 0;
	}
	else
	{
		q->data[q->tail++] = data;      //��Ԫ������� 
		return 1;
	}
}
/*******************�����в���*************************/
DATA *OutSQType(SQType *q)
{
	if (q->tail == q->head)
	{
		return NULL;
	}
	else
	{
		return &(q->data[q->head++]);
	}
}
/*******************���������*************************/
DATA * PeekSQType(SQType *q)
{
	if (SQTypeIsEmpty(q))
	{
		cout << "�ն���" << endl;
		return NULL;
	}
	else
	{
		return &(q->data[q->head]);
	}

}
/*******************������г���*************************/
int SQTypeLen(SQType *q)
{
	return(q->tail - q->head);
}
/*********************������******************************/
int main()
{
	SQType *stack;
	DATA data, *p;
	stack = SQTypeInit();     //ִ�г�ʼ������
	cout << "ִ������в�����" << endl;
	cout << "��������,���������Ӳ�����" << endl;
	while (1)
	{
		cin >> data.name >> data.age;
		if (data.age == 0)
		{
			break;      //������Ϊ0�����˳� 
		}
		else
		{
			InSQType(stack, data);
		}
	}
	cout << "���г�ջ������" << endl;
	p = OutSQType(stack);
	cout << p->name << "," << p->age << endl;
	cout << "��ȡ�׽�����ݣ�" << endl;
	p = PeekSQType(stack);
	cout << p->name << "," << p->age << endl;
	cout << "ִ�г�ջ������" << endl;
	while (1)
	{
		if (SQTypeIsEmpty(stack))
		{
			cout << "�������ݳ�ջ��ϣ�ջΪ�գ�" << endl;
			break;
		}
		else
		{
			p = OutSQType(stack);
			cout << p->name << "," << p->age << endl;
		}
	}
	SQTypeFree(stack);
	return 0;
}