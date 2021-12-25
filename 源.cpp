#include<stdio.h>
#include<mysql.h>
#include<Windows.h>
#pragma comment (lib,"libmysql.lib")

bool init();
void wlecome();
void bookTicket();
void unSubscribe();
void queryFlightInfo(char goalCity[], int* flag,char stratCity[]);
float fullSeatRate(MYSQL_ROW mysqlRow);
bool delelteNameInfo(char name[]);

MYSQL *mysql = (MYSQL*)malloc(sizeof(MYSQL));
int main()
{
	bool systemFlag = true;
	int i;
	if(init())
	{
		system("cls");
		while (systemFlag)
		{
			wlecome();
			fflush(stdin);
			scanf("%d", &i);
			switch (i)
			{
				case 1:bookTicket(); break;
				case 2:unSubscribe(); break;
				case 0:systemFlag = false; break;
				default:printf("您输入的按键无效，请重新输入。\n");
					system("pause");
			}
			system("cls");
		}
		mysql_close(mysql);
	}
	system("pause");
	return 0;
}

bool init()
{
	mysql_init(mysql);
	printf("正在链接到数据库，请稍候......\n");
	if (!mysql_real_connect(mysql, "127.0.0.1", "root", "123456", "shiyan", 3306, NULL, 0))
	{
		printf("创建链接失败\n%s\n",mysql_error(mysql));
		return false;
	}
	if (mysql_query(mysql, "set names GBK"))
	{
		printf("设置字符集失败。\n"); 
		return false;
	}
	return true;
}

void wlecome()
{
	printf("******************************************************\n");
	printf("***             欢迎使用机票预订系统              ****\n");
	printf("******************************************************\n");
	printf("***输入1:订票                                     ****\n");
	printf("***输入2:退票                                     ****\n");
	printf("***输入0:退出系统                                 ****\n");
	printf("******************************************************\n");
}


void bookTicket()
{
	MYSQL_RES* mysqlResult;
	MYSQL_ROW resultRow;
	char name[9] = { '\0' }, sex[3] = {'\0'}, id[11] = { '\0' }, goalCity[9] = { '\0' },startCity[9] = {'\0'};
	char *table = (char*)"passenger", * nameInfo = (char*)malloc(sizeof(char)*100);
	nameInfo[0] = '\0';
	int resultFlag, column, row;
	system("cls");
	printf("请依次输入您的个人信息：\n");
	printf("姓名：");
	scanf("%s", name);
	printf("性别：");
	scanf("%s", sex);
	printf("身份证号：");
	scanf("%s", id);


	strcat(nameInfo, "(\"");
	strcat(nameInfo, name);
	strcat(nameInfo,"\",\"");
	strcat(nameInfo, id);
	strcat(nameInfo, "\",\"");
	strcat(nameInfo, sex);
	strcat(nameInfo, "\")");
	char* queryString = (char*)malloc(sizeof(char) * 100);
	queryString[0] = '\0';
	strcat(queryString, "insert into ");
	strcat(queryString, table);
	strcat(queryString, " values ");
	strcat(queryString, nameInfo);


	resultFlag = mysql_real_query(mysql, queryString,strlen(queryString));

	if (!resultFlag)
	{
		printf("新建个人信息成功。\n");
		printf("请输入您的始发地:");
		scanf("%s", startCity);
		printf("请输入您的目的地:");
		scanf("%s", goalCity);


		queryFlightInfo(goalCity, &resultFlag, startCity);
		if (resultFlag)
		{
			mysqlResult = mysql_store_result(mysql);
			row = (int)mysql_num_rows(mysqlResult);
			if (row == 0)
			{
				printf("\n暂无航班信息,将取消本次订票。\n");
				delelteNameInfo(name);
			}
			else
			{
				char banci[20];
				char* orderNumber = (char*)malloc(sizeof(char) * 100);
				char* queryString = (char*)malloc(sizeof(char) * 100);
				char* temp = (char*)malloc(sizeof(char) * 100);
				char* value = (char*)malloc(sizeof(char) * 100);
				orderNumber[0] = '\0';
				queryString[0] = '\0';
				temp[0] = '\0';
				value[0] = '\0';
				if (mysqlResult)
				{
					column = mysql_num_fields(mysqlResult);
					printf("班次   价格    始发地    目的地     起飞时间              舱位余量  舱位总量 满座率\n");
					for (int i = 1; i < row + 1; i++)
					{
						resultRow = mysql_fetch_row(mysqlResult);
						if (resultRow[5][0] == '0')continue;
						for (int j = 0; j < column; j++)
						{
							printf("%s      ", resultRow[j]);
						}
						printf("%.2f%%", fullSeatRate(resultRow)*100);
						printf("\n");
					}

					printf("请输入对应的班次订票：");
					scanf("%s", &banci);//"insert into bookinginformation values ("name","id","banci","seatnumber","oederNumber")

					strcat(temp, "select shippingSpaceResidue from flightInformation where banci = ");
					strcat(temp, "\"");
					strcat(temp, banci);
					strcat(temp, "\"");
					mysql_real_query(mysql, temp, strlen(temp));
					mysqlResult = mysql_store_result(mysql);
					resultRow = mysql_fetch_row(mysqlResult);

					strcat(orderNumber, banci);  //生成流水号
					strcat(orderNumber, resultRow[0]);


					strcat(queryString, "insert into bookinginformation values "); //向订单信息表中插入用户数据
					strcat(value, "(\"");
					strcat(value, name);
					strcat(value, "\",\"");
					strcat(value, id);
					strcat(value, "\",\"");
					strcat(value, banci);
					strcat(value, "\",\"");
					strcat(value, resultRow[0]);
					strcat(value, "\",\"");
					strcat(value, orderNumber);
					strcat(value, "\")");
					strcat(queryString, value);
					if (!mysql_real_query(mysql, queryString, strlen(queryString)))printf("创建订单成功！\n");
					else
					{
						printf("可能是哪里出错了。\n");
						delelteNameInfo(name);
					}
				}
			}
		}
		else printf("查询失败。\n");
	}
	else
	{
		printf("新建个人信息失败。\n%s\n即将返回主界面。\n", mysql_error(mysql));
	}
	system("pause");
}

bool delelteNameInfo(char name[])
{
	char* queryString = (char*)malloc(sizeof(char) * 100);
	queryString[0] = '\0';
	strcat(queryString, "delete from passenger where name = \"");
	strcat(queryString, name);
	strcat(queryString, "\"");
	if (!mysql_real_query(mysql, queryString, strlen(queryString)))
	{
		printf("已撤销您的个人信息。\n");
		return true;
	}
	else
	{
		printf("撤销个人信息失败。\n");
		return false;
	}
}


float fullSeatRate(MYSQL_ROW mysqlRow)
{
	float a = 0, b = 0;
	char *pa=mysqlRow[5],*pb = mysqlRow[6];
	while (*pa != '\0')
	{
		a= a*10 +( * pa - '0');
		pa++;
	}
	while (*pb != '\0')
	{
		b = b*10 + ( * pb - '0');
		pb++;
	} 1.0 - (a/b);
	return 1.0 - (a / b);
}


void unSubscribe()
{
	system("cls");
	MYSQL_RES* mysqlResult;
	MYSQL_ROW resultRow;
	char* queryString = (char*)malloc(sizeof(char) * 100);
	char name[9];
	queryString[0] = '\0';
	printf("请输入要取消订单的姓名:");
	scanf("%s", name);
	strcat(queryString, "delete from bookinginformation where name = \"");
	strcat(queryString, name);
	strcat(queryString, "\"");
	if (!mysql_real_query(mysql, queryString, strlen(queryString)))
	{
		if (mysql_affected_rows(mysql) == 0)printf("未查找到此人的订单。\n");
		else
		{
			delelteNameInfo(name);
			printf("删除订单成功！\n");
		}
	}
	else printf("删除失败,请联系相关管理员。\n");
	system("pause");
}


void queryFlightInfo(char goalCity[], int* flag,char startCity[])
{
	char* queryString = (char*)malloc(sizeof(char) * 100);
	queryString[0] = '\0';
	strcat(queryString, "select * from flightinformation where goalCity = \"");
	strcat(queryString, goalCity);
	strcat(queryString, "\" and startCity = \"");
	strcat(queryString, startCity);
	strcat(queryString, "\"");
	*flag = !mysql_real_query(mysql, queryString,strlen(queryString));
}
