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
	DATA data[QUEUELEN];   //队列数组 
	int head;      //队头 
	int tail;      //队尾 
};
/*******************队列的初始化*************************/
SQType *SQTypeInit()
{
	SQType * q;
	if (q = new SQType)    //申请队列的内存空间
	{
		q->head = 0;     //设置队首 
		q->tail = 0;     //设置队尾 
		return q;
	}
	else
	{
		return NULL;    //返回空 
	}
}
/*******************判断空队列*************************/
int SQTypeIsEmpty(SQType *q)
{
	return(q->head == q->tail);
}
/*******************判断满队列*************************/
int SQTypeIsFull(SQType *q)
{
	return(q->tail == QUEUELEN);
}
/*******************清空队列*************************/
void SQTypeClear(SQType *q)
{
	q->head = 0;
	q->tail = 0;
}
/*******************释放空间*************************/
void SQTypeFree(SQType *q)
{
	if (q != NULL) delete q;
}
/*******************入队列操作*************************/
int InSQType(SQType *q, DATA data)
{
	if (q->tail == QUEUELEN)
	{
		cout << "队列已满！操作失败！" << endl;
		return 0;
	}
	else
	{
		q->data[q->tail++] = data;      //将元素入队列 
		return 1;
	}
}
/*******************出队列操作*************************/
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
/*******************读结点数据*************************/
DATA * PeekSQType(SQType *q)
{
	if (SQTypeIsEmpty(q))
	{
		cout << "空队列" << endl;
		return NULL;
	}
	else
	{
		return &(q->data[q->head]);
	}

}
/*******************计算队列长度*************************/
int SQTypeLen(SQType *q)
{
	return(q->tail - q->head);
}
/*********************主函数******************************/
int main()
{
	SQType *stack;
	DATA data, *p;
	stack = SQTypeInit();     //执行初始化操作
	cout << "执行入队列操作：" << endl;
	cout << "输入姓名,年龄进行入队操作：" << endl;
	while (1)
	{
		cin >> data.name >> data.age;
		if (data.age == 0)
		{
			break;      //若输入为0，则退出 
		}
		else
		{
			InSQType(stack, data);
		}
	}
	cout << "进行出栈操作：" << endl;
	p = OutSQType(stack);
	cout << p->name << "," << p->age << endl;
	cout << "读取首结点数据：" << endl;
	p = PeekSQType(stack);
	cout << p->name << "," << p->age << endl;
	cout << "执行出栈操作：" << endl;
	while (1)
	{
		if (SQTypeIsEmpty(stack))
		{
			cout << "所有数据出栈完毕，栈为空！" << endl;
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