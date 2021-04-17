#include<stdio.h>
#include<stdlib.h>
#include<malloc.h>
#include<time.h>
#include<conio.h>
#include<windows.h>
#include<string.h>
#include<math.h>

/* 宏定义 */
#define Max_Form_Num 20  //每一个完整订单中的最大杯数
#define Max_User_Enter 1000  //用户输入的最大字符串长度
#define Max_Beverage_SourceNum 10 //每种饮品最多使用的原材料的种类数目
#define Total_Material_Num  18  //原材料的种类数

/* 全局变量 */
int Total_Form_Num = 0;   //导入订单链表数据的时候会进行更新赋值
int Total_Beverage_Num = 0;  //饮品的种类数目(导入饮品链表数据的时候会进行更新赋值)
int month[13][30];//不赋值会初始化为0吗？会。应该定义13行因为第0行不代表任何月数。
int season[5][30];//应该定义5行因为第0行不代表任何季节。
int year[2][30];

/* 结构体 */
//饮品(beverage)
typedef struct Beverage
{
    int id;  //饮品对应的id(从1开始)
    char name[20];  //饮品名
    int material_num;  //原材料的种类数量
    float price;  //价格
    char materials[Max_Beverage_SourceNum];  //原材料 (materials)
    struct Beverage* next;
}Beverage;

//原材料
typedef struct Material
{
    int id;  //原材料对应的id(从1开始)
    char name[20];  //原材料名
    float consume;  //每次使用消耗的原材料量(单位为kg)
    float inventory;  //该材料的库存量
    struct Material* next;
}Material;

//ID
typedef struct Form_ID  //每个完整订单的ID设为一个结构体，里面包含时间信息，方便后面检索与排序
{
    int id;   //流水号
    int year;  //以下为订单产生时的时间
    int month;
    int day;
    int hour;
    int min;
}Form_ID;

//单个订单
typedef struct Single_Form
{
    int beverage;  //饮品种类(为饮品种类的id)
    int ingredients;  //小料(ingredient)(12345分别为布丁,椰果,红豆,珍珠,不加)
    int temperature;  //温度 (1--加冰，2--常温，3--微热，4--烫)
    int drink_place;  //饮用地点
    float total_price;  //订单总价
}Single_Form;

//完整订单
typedef struct Complete_Form
{
    Single_Form forms[Max_Form_Num];
    int number;  //这一单中的份数
    Form_ID id;  //流水号(暂时规定与其中的单个订单的流水号相等)
    float total_price;  //订单总价
    int is_member;  //该订单的消费者是否是会员(0为非会员，1为会员)
    int pay_way;  //付款方式(1.现金  2.在线支付  3.会员卡支付)
    struct Complete_Form *next;
}Complete_Form;

//原材料的消耗量
typedef struct Consumption
{
    int id;  //对应原材料的id
    float consumption;  //总消耗量
    char name[20];  //原材料名
    struct Consumption* pre;  //前驱
    struct Consumption* next;  //后继
}Consumption;

/* 全局指针变量 */
Beverage* head_beverage;
Complete_Form* head_form;
Material* head_material;
Consumption* head_consumption;

//
void Default();  //欢迎界面
void Order();  //点餐 --暂定order无返回，可以改为返回一个指向该次完整订单的指针
void Guest();  //普通用户界面
void Admin();  //管理员界面
void Root();  //boss界面
Complete_Form * Create_FormLinkList();  //创建订单信息链表(从文件中导入所有订单信息),返回指向该链表哨位结点的指针
Complete_Form* Find_Form(int x,Complete_Form* h);  //找到h为哨位结点的订单链表中id.id为x的结点，返回指向该结点的指针
Beverage* Create_BeverageLinkList();  //创建饮品种类的链表(从文件中导入饮品种类信息),返回指向该链表哨位结点的指针
Beverage* Find_Beverage(int x,Beverage *h);  //找到id为x的饮品种类，返回指向其的指针
Material* Create_MaterialLinkList();  //创建原材料种类的链表(从文件中导入饮品种类信息),返回指向该链表哨位结点的指针
Material* Find_Material(int x,Material *h);  //找到id为x的原材料种类,返回指向其的指针
int Show_Beverages(Beverage *h);  //展示饮品种类列表
void Choose_FormNum(Complete_Form* cf);  //选择cf指向订单的杯数
void Choose_Beverage(Complete_Form* cf,int i);  //对cf指向订单的第i分进行选择饮品类型
void Choose_Ingredients(Complete_Form* cf,int i);  //对cf指向的订单的第i份进行选择辅料
void Choose_Temperature(Complete_Form* cf,int i);  //对cf指向的订单的第i份进行选择温度
void Choose_Drinkplace(Complete_Form* cf,int i);  //对cf指向的订单的第i份进行选择饮品地点
void Choose_Payway(Complete_Form* cf);  //对cf指向的订单选择支付方式(如果是会员订单，则增加选择会员卡支付)
void Get_CpTotalprice(Complete_Form* cf);  //计算整个订单的价格并为其total_price属性赋值,并返回整个订单的总价
void Output_Form(Complete_Form* cf);  //将cf指向的订单信息输出到文件
void Create_ID(Complete_Form* cf);  //对cf指向的订单的id根据当前的时间对其进行赋值
Material* Consume_Inventory(Complete_Form* cf);  //消耗cf指向订单中需要的原材料库存
void Add_Form(Complete_Form*cf,Complete_Form* h);  //将cf指向的订单添加到订单链表中
void Rewrite_FormData();  //将更新后的订单链表数据重新写回文件中
void Rewrite_BeverageData();  //将更新后的饮品链表数据重新写回文件中
void Rewrite_MaterialData();  //将更新后的原材料链表数据重新写回文件中

// void Clear_Price(Complete_Form* cf);  //对cf指向的订单的所有单个订单的价格清零

//公共函数
void toxy(int x,int y);//将光标移动到x，y坐标处
void HideCursor(int x);//隐藏光标
char *HidePassword();//隐藏密码
int Check_Number(char ch[],int maxnum,int maxlen);  //检查数字是否合规(合规且不超过maxnu返回对应的数字值，合规但超过max返回-1，不合规返回-2)
// int Number_Check(char ch[]);

//函数实现区
void toxy(int x,int y)//将光标移动到x，y坐标处
{
    COORD pos={x,y};
    HANDLE Out = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(Out,pos);
}

void HideCursor(int x)//隐藏光标 ,当x为0时，隐藏，为1时，显示
{
    CONSOLE_CURSOR_INFO cursor_info ={1,x};
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE),&cursor_info);
}

int Check_Number(char ch[],int maxnum,int maxlen)  //检查数字(正整数)是否合规(合规且不超过max返回对应的数字值，合规但超过max返回-1，不合规返回-2)
{  //0001  1s23  01s  s  都为不规范输入  0为合规输入
    int i;  //循环控制
    int len = strlen(ch);  //数组长度
    int sum=0;  //记录字符串数组对应的值
    for(i=0;ch[i]!='\0';i++)  //检查是否都是数字
    {
        int num;
        if(ch[i]<'0'||ch[i]>'9')
        {
            return -2;  //有非数字输入，不合规
        }
        num = ch[i]-'0';
        sum+=num*pow(10,len-1-i);
    }
    if(strlen(ch)>maxlen)
    {
        return -1;
    }
    if(ch[0]=='0'&&len>1)  //处理0001这类不合规输入
    {
        return -2;
    }
    if(sum<=maxnum)
    {
        return sum;
    }
    else
    {
        return -1;
    }
}

/* void Default()  //欢迎界面
{
    char t;  //用于选择
    do
    {
        HideCursor(0); //隐藏光标
        system("color 72");
        toxy(28,8);
        printf(" 欢迎来到时尚泡吧！请选择使用者身份");
        toxy(26,9);
        printf("-----------------------------");
        toxy(27,11);
        printf("1.管理员 2.用户 3.Boss");
        while(1)  //死循环为防止其他按键干扰
        {
            t=getch();  //不回显函数
            if(t=='1')  //如果按1，则进入登录界面
            Admin();
            else if(t=='2')  //如果按2，则进入注册界面
            Guest();
            else if(t=='3')
            Root();
        }  //如果既不是1也不是2和3，则循环输入
    }while(1);  //永远为真
} */

/* 已测试 */
Complete_Form* Create_FormLinkList()  //通过订单文件导入订单数据，并创建链表，返回指向链表哨位结点的指针
{
    Complete_Form *H,*p;  //头指针
    H = (Complete_Form*)malloc(sizeof(Complete_Form));  //为哨位结点申请内存
    H->next = NULL;
    p = H;
    FILE* fp;
    fp = fopen("..\\Form_Data.txt", "r");  //打开存放饮品数据的文件
    if(fp==NULL)  //错误提示
    {
        printf("Can't open the file:%s","Form_data.txt");
    }
    else
    {
        while(!feof(fp))
        {
            int x;  //存储多余字符
            Complete_Form *r;
            r = (Complete_Form*)malloc(sizeof(Complete_Form));  //为新结点申请内存
            fscanf(fp,"%d,%d-%d-%d-%d:%d,%d",&r->id.id,&r->id.year,&r->id.month,&r->id.day,&r->id.hour,&r->id.min,&r->number);  //读入数据并且存放到新结点中
            for(int i=0;i<r->number;i++)
            {
                fscanf(fp,"%d,%d,%d,%d,%d,%f",&x,&r->forms[i].beverage,&r->forms[i].ingredients,&r->forms[i].temperature,&r->forms[i].drink_place,&r->forms[i].total_price);
            }
            Total_Form_Num ++;
            fscanf(fp,"%f,%d",&r->total_price,&r->pay_way);
            r->next = NULL;
            p->next = r;
            p = r;
        }
        fclose(fp);
    }
    return H;
}

Complete_Form* Find_Form(int x,Complete_Form* h)  //找到h为哨位结点的订单链表中id.id为x的结点，返回指向该结点的指针
{
    Complete_Form *p=h;
    while(p->next!=NULL&&p->next->id.id!=x)
    {
        p=p->next;
    }
    return p->next;
}

Beverage* Create_BeverageLinkList()  //读入beverageData.txt中的数据创建用于存储饮品种类的链表，返回该链表的头指针（头指针无数据域）
{
    Beverage *H,*p;  //头指针
    H = (Beverage*)malloc(sizeof(Beverage));
    H->next = NULL;
    p = H;  //利用p来进行生成链表，H用来返回(p表示链表的末尾结点)
    FILE* fp;
    fp = fopen("..\\beverageData2.txt", "r");  //打开存放饮品数据的文件
    if(fp==NULL)  //错误提示
    {
        printf("Can't open the file!");
    }


    else
    {
        while(!feof(fp))
        {
            Beverage *r;
            r = (Beverage*)malloc(sizeof(Beverage));  //为新结点申请内存
            fscanf(fp,"%d,%d,%[^,],%f,",&r->id,&r->material_num,r->name,&r->price);  //读入数据并且存放到新结点中
            Total_Beverage_Num += 1;
            for(int i=0;i<r->material_num;i++)
            {
                fscanf(fp,"%d,",&r->materials[i]);
            }
            r->next = NULL;
            p->next = r;
            p = r;
        }
        fclose(fp);
    }
    return H;
}

Beverage* Find_Beverage(int x,Beverage *h)  //找到h为哨位结点的链表中id为x的结点，返回指向该结点的指针
{
    Beverage *p=h;
    while(p->next!=NULL&&p->next->id!=x)
    {
        p=p->next;
    }
    return p->next;
}

Material* Create_MaterialLinkList()  //创建原材料种类的链表(从文件中导入饮品种类信息),返回指向该链表哨位结点的指针
{
    Material *H,*p;  //头指针
    H = (Material*)malloc(sizeof(Material));
    H->next = NULL;
    p = H;  //利用p来进行生成链表，H用来返回(p表示链表的末尾结点)
    FILE* fp;
    fp = fopen("..\\materialData.txt", "r");  //打开存放饮品数据的文件
    if(fp==NULL)  //错误提示
    {
        printf("Can't open the file!");
    }
    else
    {
        while(!feof(fp))
        {
            Material *r;
            r = (Material*)malloc(sizeof(Material));  //为新结点申请内存
            fscanf(fp,"%d,%[^,],%f,%f",&r->id,r->name,&r->consume,&r->inventory);  //读入数据并且存放到新结点中
            r->next = NULL;
            p->next = r;
            p = r;
        }
        fclose(fp);
    }
    return H;
}

Material* Find_Material(int x,Material* h)  //找到h为哨位结点的链表中id为x的结点，返回指向该结点的指针
{
    Material *p=h;
    while(p->next!=NULL&&p->next->id!=x)
    {
        p=p->next;
    }
    return p->next;
}

int Show_Beverages(Beverage *h)  //展示饮品类型(将h指向的链表中存放的饮品信息输出到屏幕),返回链表的长度，即饮品类别的数量
{
    int num=0;
    if(h->next==NULL)
    {
        printf("暂无饮品...请联系管理员进行添加");
    }
    else
    {
        printf("\t\t饮品id\t饮品名\t饮品价格(?)\n");
        while(h->next!=NULL)
        {
            printf("\t\t  %d\t%s\t%.1f\n",h->next->id,h->next->name,h->next->price);
            h=h->next;
            num++;
        }
    }
    return num;
}

void Order()
{
    Complete_Form* CF;  //创建一个指向完整订单结构的指针,方便包装多个函数对其进行修改
    CF = (Complete_Form*)malloc(sizeof(Complete_Form));
    CF->next = NULL;  //初始化指针域为空
    Create_ID(CF);
    Choose_FormNum(CF);
    int i = 1;  //计数器，表示当前为第i杯
    // while(i<=CF->number)
    // {
    Choose_Beverage(CF,i);
    //     i++;
    // }
    // Choose_Payway(CF);
    return;
}

void Choose_FormNum(Complete_Form* cf)  //选择cf指向订单的杯数,返回杯数
{
    system("cls");  //清屏
    toxy(12,6);
    HideCursor(1);
    printf("输入杯数(一次最多可点%d杯):",Max_Form_Num);
    char ch[Max_User_Enter];
    fflush(stdin);
    scanf("%s",ch);
    fflush(stdin);
    int x = Check_Number(ch,20,3);
    if(x>0)
    {
        cf->number = x;
        HideCursor(0);
        return;
    }
    else if(x==-1)
    {
        printf("\t\t输入数字超过最大量");
        Choose_FormNum(cf);  //递归直至输入合规
    }
    else if(x==-2)
    {
        printf("\t\t输入内容不合规，请输入数字");
        Choose_FormNum(cf);  //递归直至输入合规
    }
    return;

//     if(Check_PositiveInt(x)>0)
//     {
//         cf->number = x;
//         HideCursor(0);
//         return;
//     }
//     else
//     {
//         toxy(12,6);
//         printf("输入不合规，请1s后重新输入");
//         //Sleep(1000);  //暂停1秒显示提示信息fgfgfgfgcvcv
//         Choose_FormNum(cf);  //递归直至输入合规
//     }
//     return;
// }
}

void Choose_Beverage(Complete_Form* cf,int i)
{
    system("cls");
    int c;  //存储数字合规检查的返回值
    int Beverage_Num;  //保存目前饮品种类数
    char ch[Max_User_Enter];  //保存用户输入
    toxy(12,6);
    printf("请选择第%d杯饮品:\n",i);
    Beverage_Num = Show_Beverages(head_beverage);
    printf("\t\t输入0重新选择杯数\n");
    printf("\t\t输入饮品的id进行选择(按回车确认)：");
    fflush(stdin);
    scanf("%s",&ch);
    fflush(stdin);
    c = Check_Number(ch,Beverage_Num,3);
    if(c>0)
    {
        Beverage *q;  //找到饮品中id为c的饮品种类，用q指向
        q = Find_Beverage(c,head_beverage);
        cf->forms[i-1].beverage = q->id;  //为订单的饮品种类赋值
        cf->forms[i-1].total_price = q->price;  //为订单的价格赋初值，后面需要加上小料的价格
        Choose_Ingredients(cf,i);  //选择小料
        return;
    }
    else if(c==0)
    {
        Order();
        return;
    }
    else if(c==-1)
    {
        printf("\t\t输入数字超过饮品的数量,请1s后重新选择");
        //Sleep(1000);  //暂停1秒显示提示信息
        Choose_Beverage(cf,i);  //重新输入
    }
    else
    {
        printf("\t\t输入内容不合规，请1s后输入饮品对应的id");
        //Sleep(1000);  //暂停1秒显示提示信息
        Choose_Beverage(cf,i);  //重新输入
    }
    return;
}

void Choose_Ingredients(Complete_Form* cf,int i)
{
    system("cls");
    char t;
    toxy(12,6);
    HideCursor(0);
    printf("请选择第%d杯需要添加的小料\n",i);
    printf("\t\t1.布丁--1元\n");
    printf("\t\t2.椰果--1元\n");
    printf("\t\t3.红豆--2元\n");
    printf("\t\t4.珍珠--2元\n");
    printf("\t\t5.不添加小料\n");
    printf("\t\t0.重新选择杯数\n");
    fflush(stdin);
    t = getch();
    fflush(stdin);
    switch (t)
    {
        case '1':
            cf->forms[i-1].ingredients = 1;  //记录该订单添加的小料
            cf->forms[i-1].total_price += 1.0;  //订单总价加上该小料的价格
            break;
        case '2':
            cf->forms[i-1].ingredients = 2;
            cf->forms[i-1].total_price += 1.0;
            break;
        case '3':
            cf->forms[i-1].ingredients = 3;
            cf->forms[i-1].total_price += 2.0;
            break;
        case '4':
            cf->forms[i-1].ingredients = 4;
            cf->forms[i-1].total_price += 2.0;
            break;
        case '5':
            cf->forms[i-1].ingredients = 5;
            break;
        case '0':
            Order();
            return;
        default:
            printf("\t\t输入不合规,请重新输入");
            //Sleep(1000);  //暂停1秒显示提示信息
            Choose_Ingredients(cf,i);  //递归直至输入合规
            return;
    }
    Choose_Temperature(cf,i);
    return;
}

void Choose_Temperature(Complete_Form* cf,int i)
{
    system("cls");
    char t;
    toxy(12,6);
    HideCursor(0);
    printf("请选择第%d杯的饮用温度\n",i);
    printf("\t\t1.加冰\n");
    printf("\t\t2.常温\n");
    printf("\t\t3.微热\n");
    printf("\t\t4.烫\n");
    printf("\t\t0.重新选择杯数");
    fflush(stdin);
    t = getch();
    fflush(stdin);
    switch (t)
    {
        case '1':
            cf->forms[i-1].temperature = 1;
            break;
        case '2':
            cf->forms[i-1].temperature = 2;
            break;
        case '3':
            cf->forms[i-1].temperature = 3;
            break;
        case '4':
            cf->forms[i-1].temperature = 4;
            break;
        case '0':
            Order();
            return;
        default:
            printf("\t\t输入不合规,请重新输入");
            //Sleep(1000);  //暂停1秒显示提示信息
            Choose_Temperature(cf,i);  //递归直至输入合规
            return;
    }
    Choose_Drinkplace(cf,i);
    return;
}

void Choose_Drinkplace(Complete_Form* cf,int i)
{
    system("cls");
    char t;
    toxy(12,6);
    HideCursor(0);
    printf("请选择第%d杯的饮用地点\n",i);
    printf("\t\t1.堂食\n");
    printf("\t\t2.外带(打包)\n");
    printf("\t\t0.重新选择杯数\n");
    fflush(stdin);
    t = getch();
    fflush(stdin);
    switch (t)
    {
        case '1':
            cf->forms[i-1].drink_place = 1;
            break;
        case '2':
            cf->forms[i-1].drink_place = 2;
            break;
        case '0':
            Order();  //重新选择杯数
            return;
        default:
            printf("\t\t输入不合规,请重新输入");
            //Sleep(1000);  //暂停1秒显示提示信息
            Choose_Drinkplace(cf,i);  //递归直至输入合规
            return;
    }
    if(i<cf->number)
    {
        Choose_Beverage(cf,i+1);
        return;
    }
    else
    {
        Choose_Payway(cf);
        return;
    }
}

void Get_CpTotalprice(Complete_Form* cf)  //计算整个订单的价格并为其total_price属性赋值
{
    float sum=0;
    int i;
    for(i=0;i<cf->number;i++)
    {
        sum+=cf->forms[i].total_price;
    }
    cf->total_price = sum;
    return;
}

void Choose_Payway(Complete_Form* cf)
{
    system("cls");
    char t;
    toxy(12,6);
    HideCursor(0);
    Get_CpTotalprice(cf);
    printf("订单总价为:%.1f元,请选择支付方式\n",cf->total_price);
    printf("\t\t1.现金支付\n");
    printf("\t\t2.在线支付\n");
    if(cf->is_member>0)
    {
        printf("\t\t3.会员卡支付(85折)\n");
    }
    fflush(stdin);
    t = getch();
    fflush(stdin);
    if(cf->is_member>0)
    {
        switch (t)
        {
            case '1':
                cf->pay_way = 1;
                printf("\t\t谢谢惠顾！！！");
                //Sleep(1000);  //暂停1秒显示提示信息
                break;
            case '2':
                cf->pay_way = 2;
                printf("\t\t谢谢惠顾！！！");
                //Sleep(1000);  //暂停1秒显示提示信息
                break;
            default:
                printf("\t\t输入不合规,请重新输入");
                //Sleep(1000);  //暂停1秒显示提示信息
                Choose_Payway(cf);
                return;
        }
    }
    else
    {
        switch (t)
        {
            case '1':
                cf->pay_way = 1;
                printf("\t\t谢谢惠顾！！！");
                Sleep(1000);  //暂停1秒显示提示信息
                break;
            case '2':
                cf->pay_way = 2;
                printf("\t\t谢谢惠顾！！！");
                Sleep(1000);  //暂停1秒显示提示信息
                break;
            case '3':
                cf->pay_way = 3;
                printf("\t\t会员卡支付完毕");
                break;
            default:
                printf("\t\t输入不合规,请重新输入");
                //Sleep(1000);  //暂停1秒显示提示信息
                Choose_Payway(cf);
                return;
        }
    }
    Material* ci_return = Consume_Inventory(cf);
    if(ci_return==NULL)  //根据该订单信息对库存进行“消耗”
    {                       //若库存足够
        Add_Form(cf,head_form);  //将该订单结点添加到订单链表中
    }
    else  //原材料库存不足，给出提示，重新点单
    {
        printf("%s库存不足，请联系管理员进行添加,两秒后自动返回点单页面",ci_return->name);
        Sleep(2000);
        Order(); //返回点单页面
        return;
    }
    // Out_put(cf);  //订单所有信息赋值完毕,输出到文件(持续化存储)
    return;
}

void Create_ID(Complete_Form* cf)  //对cf指向的订单的id根据当前的时间对其进行赋值
{
    time_t timep;
    struct tm *p;
    time (&timep);
    p=gmtime(&timep);
    cf->id.id = Total_Form_Num+1;
    Total_Form_Num++;
    cf->id.year = 1900+p->tm_year;
    cf->id.month = 1+p->tm_mon;
    cf->id.day = p->tm_mday;
    cf->id.hour = 8+p->tm_hour;
    cf->id.min = p->tm_min;
    return;
}

void Add_Form(Complete_Form*cf,Complete_Form* h)  //将cf指向的订单添加到订单链表h的末尾
{
    Complete_Form * p;
    p = h;  //哨位结点指针
    while(p->next!=NULL)  //找到尾结点
    {
        p = p->next;
    }
    p->next = cf;
    return;
}

Material* Consume_Inventory(Complete_Form* cf)  //消耗cf指向订单中需要的原材料库存，若库存不足，则进行提示，自动取消该次订单，返回不足的原材料对应的结构体指针;若能正常消耗，返回null
{
    int beverage_id;  //饮品id
    int material_id;  //原材料id
    Beverage* beverage_struct;  //饮品结构体
    Material* material_struct;  //原材料结构体
    for(int i=0;i<cf->number;i++)  //遍历所有单个订单
    {
        beverage_id = cf->forms[i].beverage;  //找到这个订单的饮品对应的id
        beverage_struct = Find_Beverage(beverage_id,head_beverage);  //通过id找到对应的饮品结构体
        for(int j=0;j<beverage_struct->material_num;j++)  //遍历该饮品结构体的原材料种类，并进行“消耗”
        {
            material_id = beverage_struct->materials[j];  //找到该饮品的原材料对应的id
            material_struct = Find_Material(material_id,head_material);  //通过原材料对应的id找到对应的结构体
            if(material_struct->inventory >= material_struct->consume)  //若库存足够
            {
                material_struct->inventory -= material_struct->consume;  //该材料的库存量减去该材料的单次使用量即为“消耗”
            }
            else  //库存不足
            {
                return material_struct;
            }
        }
    }
    return NULL;
}

void Rewrite_FormData()  //将更新后的订单链表数据重新写回文件中
{
    if(head_form->next==NULL)
    {
        return;
    }
    Complete_Form* cf = head_form->next;
    FILE * fp;
    int i;  //循环控制
    fp = fopen("..\\Form_Data.txt","w");  //以附加方式打开文件
    while(cf!=NULL)  //输出全部
    {
        fprintf(fp,"%d,%d-%d-%d-%d:%d,%d\n",cf->id.id,cf->id.year,cf->id.month,cf->id.day,cf->id.hour,cf->id.min,cf->number);
        for(i=0;i<cf->number;i++)
        {
            fprintf(fp,"%d,%d,%d,%d,%d,%.1f\n",i+1,cf->forms[i].beverage,cf->forms[i].ingredients,cf->forms[i].temperature,cf->forms[i].drink_place,cf->forms[i].total_price);
        }
        fprintf(fp,"%.1f,%d",cf->total_price,cf->pay_way);
        if(cf->next!=NULL)  //最后一行不含换行符
        {
            fprintf(fp,"\n");
        }
        //文件写入格式为       订单号,年-月-日-时:分,该订单的份数
        //                    第1份,饮品类型,小料,饮用温度,饮用地点,这一份的总价
        //                    第2份,饮品类型,小料,饮用温度,饮用地点,这一份的总价
        //                    订单总价,付款方式
        cf=cf->next;
    }
    fclose(fp);
    return;
}

void Rewrite_BeverageData()  //将更新后的饮品链表数据重新写回文件中
{
    if(head_beverage->next==NULL)
    {
        return;
    }
    Beverage* bever = head_beverage->next;
    FILE* fp;
    int i;
    fp = fopen("..\\beverageData.txt","w");
    while(bever!=NULL)
    {
        fprintf(fp,"%d,%d,%s,%.1f,",bever->id,bever->material_num,bever->name,bever->price);
        for(i=0;i<bever->material_num-1;i++)
        {
            fprintf(fp,"%d,",bever->materials[i]);
        }
        fprintf(fp,"%d",bever->materials[i]);
        if(bever->next!=NULL)
        {
            fprintf(fp,"\n");
        }
        bever = bever->next;
    }
    fclose(fp);
    return;
}

void Rewrite_MaterialData()  //将更新后的原材料链表数据重新写回文件中
{
    if(head_material->next==NULL)
    {
        return;
    }
    Material * mat = head_material->next;
    FILE* fp;
    int i;
    fp = fopen("..\\materialData.txt","w");
    while(mat!=NULL)
    {
        fprintf(fp,"%d,%s,%.2f,%.2f",mat->id,mat->name,mat->consume,mat->inventory);
        if(mat->next!=NULL)
        {
            fprintf(fp,"\n");
        }
        mat = mat->next;
    }
    fclose(fp);
    return;
}

// void Clear_Price(Complete_Form* cf)  //对cf指向的订单的所有单个订单的价格清零
// {
//     int i;
//     for(i=0;i<Max_Form_Num;i++)
//     {
//         cf->forms[i].total_price = 0.0;
//     }
//     return;
// }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//库存管理区
void Inventory_Manage();  //库存管理页面
void Add_Inventory();  //添加库存
int Show_Inventory();  //查看现有库存的剩余量,返回库存种类数目
void Show_InventoryConsumption();  //查看库存消耗
void Inventory_GetAdded(Material *m);  //根据用户的选择来添加库存
void Show_Inventory_ByConsumption();  //根据原材料消耗量对原材料的消耗进行排序展示
void ChangeShowWay(Consumption *head,Consumption *tail,int status);  //根据status的值来选择是按消耗量从小到大，还是按消耗量从大到小来显示原材料的总消耗量(status=1时候从小到大,0时从大到小)
void Show_Inventory_ById();  //根据原材料的id展示各材料的消耗量
float Get_TotalConsumption(int x);  //检索所有订单信息,返回原材料id为x的原材料的总消耗量
Consumption* Create_ConsumptionLinkList();  //创建原材料消耗的双向链表，创建时即进行插入排序(消耗量从大到小)
Consumption* Find_Consumption(int x,Consumption* h);  //在原材料消耗的双向链表中找到id为x的结点并返回指向其的指针

void Inventory_Manage()  //库存管理页面
{
    system("cls");  //清屏
    char t;  //存储用户选择
    toxy(12,6);
    printf("请选择需要进行的操作:\n");
    printf("\t\t1.增加库存\n");
    printf("\t\t2.查看库存消耗\n");
    printf("\t\t0.返回上一级");
    fflush(stdin);
    t = getch();
    fflush(stdin);
    switch (t)
    {
        case '1':
            Add_Inventory();
            break;
        case '2':
            Show_InventoryConsumption();
            break;
            // case '0':

            //     break;
        default:
            printf("\t\t输入不合规，请重新输入");  //递归直至输入合规
            Inventory_Manage();
            break;
    }
    return;
}

void Add_Inventory()  //添加库存
{
    int inventory_num;  //库存种类数量
    int enter_id;  //用户输入的id
    char enter_choose;  //用户选择添加的量
    Material* operate_material;  //进行添加操作的原材料结构体对应的指针(id对应的原材料结构体)
    system("cls");  //清屏
    toxy(12,6);
    printf("请选择要添加的原材料种类:\n");
    inventory_num = Show_Inventory();
    char ch[Max_User_Enter];
    printf("\t\t请输入饮品对应的id进行选择:");
    fflush(stdin);
    scanf("%s",ch);
    fflush(stdin);
    int x = Check_Number(ch,inventory_num,3);  //用户输入判断
    if(x>0)
    {
        enter_id = x;
        HideCursor(0);
    }
    else if(x==-1)
    {
        printf("\t\t输入数字超过最大量");
        Add_Inventory();  //递归直至输入合规
        return;
    }
    else if(x==-2)
    {
        printf("\t\t输入内容不合规，请输入数字");
        Add_Inventory();  //递归直至输入合规
        return;
    }
    operate_material = Find_Material(enter_id,head_material);
    Inventory_GetAdded(operate_material);
    return;
}

int Show_Inventory()
{
    int max_id;
    if(head_material->next==NULL)
    {
        return 0;
    }
    Material *m;
    m = head_material->next;
    printf("\t\tID  \t 原材料名\t剩余库存量\n");
    while(m!=NULL)
    {
        printf("\t\t%d\t  %s  \t%.2f\n",m->id,m->name,m->inventory);
        max_id = m->id;
        m = m->next;
    }
    return max_id;
}

void Show_InventoryConsumption()  //查看库存消耗
{
    char t;
    system("cls");  //清屏
    toxy(12,6);
    HideCursor(0);
    printf("请选择查看库存消耗的关键词:\n");
    printf("\t\t1.按消耗量进行排序\n");
    printf("\t\t2.按原材料id进行排序\n");
    printf("\t\t0.返回上一级\n");
    fflush(stdin);
    t = getch();  //保存用户选择
    fflush(stdin);
    head_consumption = Create_ConsumptionLinkList();  //在用户选择要查看库存消耗之后创建一个各类材料消耗量的双向链表
    switch(t)  //根据用户选择进行执行对应操作
    {
        case '1':
            Show_Inventory_ByConsumption();
            break;
        case '2':
            Show_Inventory_ById();
            break;
        case '0':
            Inventory_Manage();
            return;
        default:
            printf("\t\t输入不合规，请重新输入");
            Show_InventoryConsumption(); //递归直至输入合规
            return;
    }
    return;
}

void Inventory_GetAdded(Material *m)  //用户选择添加的量，返回用户的选择(1---1kg，2---加至半满，3---加满)
{
    char t;
    system("cls");
    toxy(12,6);
    printf("请选择添加的量:\n");
    printf("\t\t1.添加至一半(5kg)\n");
    printf("\t\t2.添加至满(10kg)\n");
    printf("\t\t0.返回上一级\n");
    fflush(stdin);
    t = getch();
    fflush(stdin);
    switch(t)
    {
        case '1':
            m->inventory = 5.0;
            printf("\t\t%s库存已添加至一半!!!",m->name);
            Sleep(1000);
            break;
        case '2':
            m->inventory = 10.0;
            printf("\t\t%s库存已添加至满!!!",m->name);
            Sleep(1000);
            break;
        case '0':
            Add_Inventory();
            return;
        default:
            printf("\t\t输入不合规，请重新输入");
            Inventory_GetAdded(m);  //递归直至输入正确
            return;
    }
    return;
}

void ChangeShowWay(Consumption *head,Consumption *tail,int status)  //根据status的值来选择是按消耗量从小到大，还是按消耗量从大到小来显示原材料的总消耗量(status=1时候从小到大,0时从大到小)
{
    system("cls");
    toxy(12,6);
    Consumption *con;
    int i = 1; //计数器
    printf("原材料的消耗量为:\n");
    if(status == 1)
    {
        con = head;
        while(con!=NULL)
        {
            printf("\t\t%d.%s\t%.2f\n",i,con->name,con->consumption);
            con = con->next;
            i++;
        }
        printf("\t\t按0返回上一级\n");
        printf("\t\t按1切换排序方式\n");
    }
    else
    {
        con = tail;
        while(con!=NULL)
        {
            printf("\t\t%d.%s\t%.2f\n",i,con->name,con->consumption);
            con = con->pre;
            i++;
        }
        printf("\t\t按0返回上一级\n");
        printf("\t\t按1切换排序方式\n");
    }
    return;
}
Consumption* SortLinkList(Consumption *h)  //创建一个以h为头结点(非哨位结点)的双向链表的按消耗量排序后的副本(从小到大),返回该副本的头结点
{
    if(h == NULL)
    {
        return NULL;
    }
    Consumption *soldier;  //创建哨位结点
    soldier = (Consumption *)malloc(sizeof(Consumption));
    soldier->pre = NULL;
    soldier->next = NULL;
    Consumption *m;  //创建副本新结点
    m = (Consumption *)malloc(sizeof(Consumption));
    m->consumption = h->consumption;  //新结点赋值
    m->id = h->id;
    strcpy(m->name,h->name);
    m->pre = soldier;  //将h结点的副本m接在新链表的哨位结点后面
    m->next = NULL;
    soldier->next = m;
    while(h->next!=NULL)  //插入排序
    {
        Consumption *q;  //下一个结点的副本
        Consumption *n;  //与m组成双指针来进行插入排序
        m = soldier;
        n = soldier->next;
        q = (Consumption *)malloc(sizeof(Consumption));
        q->id = h->next->id;
        q->consumption = h->next->consumption;
        strcpy(q->name,h->next->name);
        q->next = NULL;
        q->pre = NULL;
        while(q->consumption > n->consumption&&n!=NULL)  //循环为新结点q找到合适位置
        {
            m = m->next;
            n = n->next;
        }
        m->next = q;  //将q插在m与n之间
        q->pre = m;
        q->next = n;
        if(n!=NULL)
        {
            n->pre = q;
        }
        h = h->next;
    }
    return soldier->next;
}

void Show_Inventory_ByConsumption()  //根据原材料消耗量对原材料的消耗进行排序展示
{
    char t;
    int i;  //循环控制
    int status;  //排序方法标志
    Consumption *head,*tail;  //头结点，尾结点
    head = SortLinkList(head_consumption->next);
    tail = head;
    while(tail->next!=NULL)  //给尾结点赋值
    {
        tail = tail->next;
    }
    status = 1;
    while(1)
    {
        ChangeShowWay(head,tail,status);  //默认按消耗量从小到大来显示
        fflush(stdin);  //用户输入
        t = getch();
        fflush(stdin);
        switch (t)
        {
            case '1':
                if(status == 1)
                {
                    status = 0;
                }
                else
                {
                    status = 1;
                }
                break;
            case '0':
                Show_InventoryConsumption();
                return;
            default:
                printf("\n\t\t输入不合规");
                break;
        }
    }
}

void Show_Inventory_ById()  //根据原材料的id展示各材料的消耗量
{
    char t;
    Consumption* con;
    system("cls");  //清屏
    toxy(12,6);
    HideCursor(0);
    printf("原材料名\t消耗量(kg)\n");
    con = head_consumption->next;
    while(con!=NULL)
    {
        printf("\t\t%d.%s\t%.2f\n",con->id,con->name,con->consumption);
        con = con->next;
    }
    printf("\t\t按0返回上一级\n");
    while(1)
    {
        fflush(stdin);
        t = getch();
        fflush(stdin);
        if(t=='0')
        {
            Show_InventoryConsumption();
            return;
        }
    }
}

Consumption* Find_Consumption(int x,Consumption* h)  //在原材料消耗的双向链表中找到id为x的结点并返回指向其的指针
{
    Consumption *p=h;
    while(p->next!=NULL&&p->next->id!=x)
    {
        p=p->next;
    }
    return p->next;
}

Consumption* Create_ConsumptionLinkList()  //创建原材料消耗的双向链表,按原材料id进行排序,返回双向链表的哨位结点指针
{
    Consumption* h;  //声明变量
    Consumption* p,*q;
    Complete_Form* cf;
    Beverage* bever;
    Material* mater;
    int i,j,k,m;  //循环控制变量
    h = (Consumption*)malloc(sizeof(Consumption));  //h为哨位结点，无数据域
    h->id = 0;
    h->next = NULL;
    h->pre = NULL;
    q = h;  //用q进行创建双向链表，h用于返回
    for(i=0;i<Total_Material_Num;i++)
    {
        p = (Consumption*)malloc(sizeof(Consumption));  //申请栈区空间
        p->id = i+1;  //为新结点数据域赋值
        strcpy(p->name,Find_Material(i+1,head_material)->name);
        p->consumption = 0.0;
        p->next = NULL;
        p->pre = q;
        q->next = p;  //将新结点连接至链表
        q = p;
    }
    for(j=0;j<Total_Form_Num;j++)  //遍历订单链表对原材料消耗链表进行赋值
    {
        cf = head_form->next;
        for(k=0;k<cf->number;k++)  //遍历完整订单的每单个订单
        {
            bever = Find_Beverage(cf->forms[k].beverage,head_beverage);  //找到每个订单的饮品类型
            for(m=0;m<bever->material_num;m++)  // 1.饮品原材料的消耗量
            {
                mater = Find_Material(bever->materials[m],head_material);  //找到该饮品类型消耗的原材料，用mater指向
                p = Find_Consumption(mater->id,h);  //找到该原材料对应的消耗量结点
                p->consumption += mater->consume;  //将该类原材料每次的消耗量加在其consumption属性上，完成赋值
            }
            switch(cf->forms[k].ingredients)  // 2.添加的小料的原材料消耗量
            {
                case 1:  //布丁的消耗量
                    mater = Find_Material(16,head_material);
                    p = Find_Consumption(16,h);
                    p->consumption += mater->consume;
                    break;
                case 2:  //椰果的消耗量
                    mater = Find_Material(18,head_material);
                    p = Find_Consumption(18,h);
                    p->consumption += mater->consume;
                    break;
                case 3:  //红豆的消耗量
                    mater = Find_Material(15,head_material);
                    p = Find_Consumption(15,h);
                    p->consumption += mater->consume;
                    break;
                case 4:  //珍珠的消耗量
                    mater = Find_Material(17,head_material);
                    p = Find_Consumption(17,h);
                    p->consumption += mater->consume;
                    break;
                default:
                    break;
            }
        }
    }
    return h;
}

//////////////////////////////////////////////////////////////////////
//王烁棋
void ShowMonthSales(int a);
void ShowSeasonSales(int a);
void ShowYearSales(int a);
void Check_Month_Sales();//读取head_form链表里面的结点里的数据。数组保存相应ID的销量，比如a[1]代表第一个饮料的销量，但是这里有一个bug就是读到了这个数组的第10个元素为2，但是已经删掉了这个 ，那么就不展示。
void Check_Season_Sales();
void Check_Year_Sales();
void ShowSale();
void AddGoods();
void DeleteGoods();
void ManageGoods();
void ChangeGoods();//增删商品种类。

void ShowMonthSales(int a)
{
    printf("%d月份各种饮品的销量如下：\n",a);
    printf("---------------------------\n");

    for(int j=0;j<Total_Beverage_Num;j++)//先遍历数组，如果销量为0则不输出（即数组里对应下标的数值=0，如果为0输出的话那么多余的空间就全输出了，有30个空间呢），如果查找这个id发现没有这个名字，也不输出。
    {
        //printf("id为%d的4月销量为：%d\n",j+1,month[4][j+1]);//接下来找id对应的名字。
        Beverage*d=Find_Beverage(j+1,head_beverage);//返回的是指向这个结构体的指针
        printf("%s的%d月销量为：%d\n",d->name,a,month[a][j+1]);
    }
}

void ShowSeasonSales(int a)
{
    int b[3];
    switch(a)
    {
        case 1:strcat(b,"春季");break;
        case 2:strcat(b,"夏季");break;
        case 3:strcat(b,"秋季");break;
        case 4:strcat(b,"冬季");break;
    }

    printf("%s各种饮品的销量如下：\n",b);
    printf("---------------------------\n");

    for(int j=0;j<Total_Beverage_Num;j++)//先遍历数组，如果销量为0则不输出（即数组里对应下标的数值=0，如果为0输出的话那么多余的空间就全输出了，有30个空间呢），如果查找这个id发现没有这个名字，也不输出。
    {
        //printf("id为%d的4月销量为：%d\n",j+1,month[4][j+1]);//接下来找id对应的名字。
        Beverage*d=Find_Beverage(j+1,head_beverage);//返回的是指向这个结构体的指针
        printf("%s的%s销量为：%d\n",d->name,b,season[a][j+1]);
    }
}

void ShowYearSales(int a)
{

    printf("本年各种饮品的销量如下：\n");
    printf("---------------------------\n");

    for(int j=0;j<Total_Beverage_Num;j++)//先遍历数组，如果销量为0则不输出（即数组里对应下标的数值=0，如果为0输出的话那么多余的空间就全输出了，有30个空间呢），如果查找这个id发现没有这个名字，也不输出。
    {
        //printf("id为%d的4月销量为：%d\n",j+1,month[4][j+1]);//接下来找id对应的名字。
        Beverage*d=Find_Beverage(j+1,head_beverage);//返回的是指向这个结构体的指针
        printf("%s的本年销量为：%d\n",d->name,year[a][j+1]);
    }
}

void Check_Month_Sales()//读取head_form链表里面的结点里的数据。数组保存相应ID的销量，比如a[1]代表第一个饮料的销量，但是这里有一个bug就是读到了这个数组的第10个元素为2，但是已经删掉了这个 ，那么就不展示。
{
    char a[3];
    int b;
    int line,row;//line为行，row为列

    Complete_Form *c=head_form;

    while(c->next!=NULL)//设计思路：遍历form链表的所有结点，在月的二维数组中，把结点里面的日期的月份作为行，遍历form结点里的单个订单结构体数组，在数组里面找id，在二维数组列里的对应id地方进行++计数。
    {
        c=c->next;

        for(int i=0;i<c->number;i++)
        {
            row=c->forms[i].beverage;
            line=c->id.month;
            month[line][row]++;//line表示月份，row表示商品id。但是商品id是从1开始的，所以month[line][0]是什么都没有
        }
    }//这个写入二维数组应该在check函数里面写还是在主函数里面写比较好。

//    for(int k=1;k<13;k++)//双重for循环遍历二维数组（1-12月）
//    {
//        for(int l=0;l<30;l++)
//        {
//            printf("%d",month[k][l]);
//        }
//        printf("\n");
//    }
//    system("pause");

    while(1)
    {
        system("CLS");
        printf("请输入你要查看的月份：(0退出)\n");
        fflush(stdin);
        scanf("%s",a);
        fflush(stdin);
        system("CLS");

        if(strcmp(a,"0")==0)
        {
            main();
        }

        b=Check_Number(a,12,2);


        if(b==-1||b==-2)
        {
            printf("请输入数字0-12！\n");
            sleep(2);
            system("CLS");
            Check_Month_Sales();
        }
        else
        {
            ShowMonthSales(b);
            printf("--------------------------\n");
            printf("\n请按任意键继续！\n");
            getch();
        }
    }

}

void Check_Season_Sales()
{
    char a[2];
    int b;
    int line,row;//line为行，row为列

    Complete_Form *c=head_form;

    while(c->next!=NULL)//设计思路：遍历form链表的所有结点，在月的二维数组中，把结点里面的日期的月份作为行，遍历form结点里的单个订单结构体数组，在数组里面找id，在二维数组列里的对应id地方进行++计数。
    {
        c=c->next;

        for(int i=0;i<c->number;i++)
        {
            row=c->forms[i].beverage;
            line=c->id.month;
            switch(line)
            {
                case 1:season[1][row]++;break;
                case 2:season[1][row]++;break;
                case 3:season[1][row]++;break;
                case 4:season[2][row]++;break;
                case 5:season[2][row]++;break;
                case 6:season[2][row]++;break;
                case 7:season[3][row]++;break;
                case 8:season[3][row]++;break;
                case 9:season[3][row]++;break;
                case 10:season[4][row]++;break;
                case 11:season[4][row]++;break;
                case 12:season[4][row]++;break;
            }
        }
    }//这个写入二维数组应该在check函数里面写还是在主函数里面写比较好?


    while(1)
    {
        system("CLS");
        printf("请输入你要查看的季度（0返回）：\n1.春季\t2.夏季\t3.秋季\t4.冬季\n");

        //system("pause");
        fflush(stdin);
        scanf("%s",a);
        fflush(stdin);
        system("CLS");

        if(strcmp(a,"0")==0)
        {
            main();
        }

        b=Check_Number(a,4,1);


        if(b==-1||b==-2)
        {
            printf("请输入数字0-4！\n");
            sleep(3);
            system("CLS");

            Check_Season_Sales();
        }
        else
        {
            ShowSeasonSales(b);
            printf("--------------------------\n");
            printf("\n请按任意键继续！\n");
            getch();
        }
    }
}

void Check_Year_Sales()
{
    char a[2];
    int b;
    int line,row;//line为行，row为列

    Complete_Form *c=head_form;

    while(c->next!=NULL)//设计思路：遍历form链表的所有结点，在月的二维数组中，把结点里面的日期的月份作为行，遍历form结点里的单个订单结构体数组，在数组里面找id，在二维数组列里的对应id地方进行++计数。
    {
        c=c->next;

        for(int i=0;i<c->number;i++)
        {
            row=c->forms[i].beverage;
            line=c->id.month;
            switch(line)
            {
                case 1:year[1][row]++;break;
                case 2:year[1][row]++;break;
                case 3:year[1][row]++;break;
                case 4:year[1][row]++;break;
                case 5:year[1][row]++;break;
                case 6:year[1][row]++;break;
                case 7:year[1][row]++;break;
                case 8:year[1][row]++;break;
                case 9:year[1][row]++;break;
                case 10:year[1][row]++;break;
                case 11:year[1][row]++;break;
                case 12:year[1][row]++;break;
            }
        }
    }//这个写入二维数组应该在check函数里面写还是在主函数里面写比较好?


    while(1)
    {
        system("CLS");
        printf("请输入1查看本年度的饮品（0返回）！\n");

        fflush(stdin);
        scanf("%s",a);
        fflush(stdin);
        system("CLS");

        if(strcmp(a,"0")==0)
        {
            main();
        }

        b=Check_Number(a,1,1);


        if(b==-1||b==-2)
        {
            printf("请输入数字0-1！\n");//发现一个bug，输入错误之后要按两下以上0才可以退出。
            sleep(2);
            system("CLS");
            Check_Year_Sales();//它是调用了之后return到上一个Check_Year_Sales()
        }
        else
        {
            ShowYearSales(b);
            printf("--------------------------\n");
            printf("\n请按任意键继续！\n");
            getch();
        }
    }
}

void ModifyPrice()//调整商品价格函数设计思路：1输入商品名字2查找2.1找到就显示价格并且提示改为多少钱2.2没找到就提示没找到是否继续找3继续：调用本函数3.1不继续：返回商品管理菜单
{
    char a[21];
    Beverage *b;
    b=head_beverage;
    char c,d;//用来判断是否的

    system("CLS");
    printf("欢迎来到修改价格界面！(0返回)\n");
    printf("---------------------------\n");
    printf("请输入修改价格的商品名字：\n");//测试：鍙板紡濂惰尪

    fflush(stdin);
    scanf("%20s",a);
    fflush(stdin);

    if(strcmp(a,"0")==0)
    {
        ManageGoods();
    }

    //之后开始遍历
    while(b->next!=NULL&&strcmp(b->name,a)!=0)
    {
        b=b->next;
    }
    if(strcmp(b->name,a)==0)
    {
        printf("你想把%s的价格从%f改为：\n",b->name,b->price);

        fflush(stdin);
        scanf("%f",&b->price);
        fflush(stdin);

        printf("修改成功！\n是否继续？\n1.是  2.否\n");

        while(1)
        {
            fflush(stdin);
            scanf("%c",&c);
            fflush(stdin);

            switch(c)
            {
                case '1':ModifyPrice();
                case '2':printf("即将退出到商品管理菜单！\n");Sleep(2);ManageGoods();
                default :printf("请输入1或2！\n");break;
            }
        }
    }
    else
    {
        system("CLS");
        printf("找不到商品%s！是否继续\n1.是  2.否\n",a);


        while(1)
        {
            fflush(stdin);
            scanf("%c",&d);
            fflush(stdin);

            switch(d)
            {
                case '1':ModifyPrice();
                case '2':printf("即将退出到商品管理菜单！\n");Sleep(2);ManageGoods();
                default :printf("请输入1或2！\n");break;
            }
        }
    }

}

void AddGoods()
{
    char a[21];//用来储存输入的名字
    char c;//c用来选择的
    Beverage*b,*d,*e;//b用来查重，d用来存储信息,e用来连接链表。（存储信息的指针需要开辟空间，而查重和连接链表的不需要）
    e=b=Create_BeverageLinkList();

    printf("欢迎来到添加商品界面！(0返回)\n");
    printf("-----------------------------\n");
    printf("请输入商品名称：\n");//之后要遍历看一看是否有一样的名称的饮品。有就提示有了，没有就提示新商品。

    fflush(stdin);
    scanf("%20s",a);
    fflush(stdin);

    if(strcmp(a,"0")==0)
    {
        ChangeGoods();
    }

    while(b->next!=NULL&&strcmp(a,b->name)!=0)//拿“鍙板紡濂惰尪”测试，因为是乱码
    {
        b=b->next;
    }
    if(strcmp(a,b->name)==0)
    {
        printf("已经有此商品了！不需要重复添加！");
        Sleep(2);
    }
    else//没有此商品
    {
        while(1)
        {
            printf("此商品还未添加！\n你想添加吗？\n1.是\t2.否\n");

            fflush(stdin);
            scanf("%c",&c);//printf("测试！\n");
            fflush(stdin);

            if(c=='1')
            {
                system("CLS");
                d=(Beverage*)malloc(sizeof(Beverage));

                strcpy(d->name,a);//把输入的名字存储到a数组中，然后赋值给d里面的name
                printf("请输入商品%s的定价：");

                fflush(stdin);
                scanf("%f",&d->price);//之后把d绑定到链表里面（id自动生成）
                fflush(stdin);

                d->id=++Total_Beverage_Num;
                d->next=NULL;
                e->next=d;
                e=d;//链表加上以后最后在main函数中调用重写就行了。

                printf("添加成功！\n");//之后要生成一个id并且要写入到文件里面：旺爷是说我先把这个信息写到这个全局链表里面，然后再用这个链表写入到文件里面。
                //这个ID号怎么自动生成？->当把新的结点添加到全局链表变量的之前，要把指针移到链表末尾，把指针后移一位，就++，最后就把那个++的数变成id号。
                Sleep(2);
                break;
            }
            else if(c=='2')//未完：除此之外还要检查输入是否是除了1和2之外的数。
            {
                system("CLS");
                printf("即将退出到主界面！\n");
                Sleep(2);
                ManageGoods();
            }
            else
            {
                printf("请输入1或者2!\n");
                Sleep(2);
                system("CLS");
            }
        }
    }
}


void DeleteGoods()//功能设计：1输入删除商品的名字2检查是否有此商品3.1没有则提示没有此商品3.2有则提示由此商品，是否删除4.1是的话提示删除成功4.2否的话提示即将返回上级
{
    char a[20];//存储商品名称。
    Beverage*b,*c;//b用来遍历找到目标，c用来指向前一个。
    int temp=1;//用来记录是第几个结点找到此商品的，temp-1也用于移动到被删除结点的前一个。
    char d,e;//用于选择

    c=head_beverage;
    b=c->next;
    printf("欢迎来到删除界面！(0返回)\n");
    printf("------------------------\n");
    printf("请输入删除商品的名称：");//测试案例：鍙板紡濂惰尪

    fflush(stdin);
    scanf("%s",a);
    fflush(stdin);

    if(strcmp(a,"0")==0)//想做一个0返回，但是会有bug
    {
        ChangeGoods();
    }

    while(b!=NULL&&strcmp(b->name,a)!=0)//这里就是遍历全局变量里的那个链表了。
    {
        b=b->next;//printf("temp=%d\n",temp);
        temp++;
    }

    if(b!=NULL)//这个是找到了的情况。不知为何换成if(strcmp(b->name,a)==0)判断就无法执行else?但是还是能正常执行找到商品
    {
        while(1)
        {
            system("CLS");
            printf("找到了此商品，是否删除？\n1.是\t2.否\n");//这里其实可以把“此”换成山沟名字。

            fflush(stdin);
            scanf("%c",&d);
            fflush(stdin);

            if(d=='1')
            {
                system("CLS");
                //删除链表操作
                for(int i=1;i<temp;i++)
                {
                    c=c->next;
                }
                c->next=b->next;
                free(b);

                printf("删除成功！即将返回删除界面。\n");
                Sleep(2);

                break;
            }
            else if(d=='2')//未完：除此之外还要检查输入是否是除了1和2之外的数。
            {
                system("CLS");
                printf("即将退出到上级界面！\n");
                Sleep(2);
                DeleteGoods();
            }
            else
            {
                printf("请输入1或者2!\n");
                Sleep(2);
                system("CLS");
            }
        }
    }
    else
    {
        system("CLS");
        printf("未找到此商品。是否还需要查找？\n1.是 2.否\n");

        fflush(stdin);
        scanf("%c",&e);
        fflush(stdin);

        while(1)
        {
            if(e=='1')
            {
                system("CLS");
                DeleteGoods();
            }
            else if(e=='2')
            {
                printf("即将退出到商品管理菜单界面！\n");
                Sleep(2);
                ManageGoods();
            }
            else
            {
                printf("请输入1或者2!\n");
                Sleep(2);
                system("CLS");
            }
        }
    }
}

void ChangeGoods()//增删商品种类。
{
    char a;//用来判断
    while(1)
    {
        system("cls");
        printf("增删商品种类界面：\n");
        printf("--------------------\n");
        printf("1.增添商品。\n");
        printf("2.删除商品。\n");
        printf("0.退出至上级。\n");

        fflush(stdin);
        scanf("%c",&a);
        fflush(stdin);

        if(a=='0')
            ManageGoods();

        system("cls");
        switch(a)
        {
            case '1':AddGoods();break;//switch也是循环来的，break只能跳出一个循环
            case '2':DeleteGoods();break;
            default: printf("请输入0或1或2！\n");Sleep(2);break;
        }

    }
}

void ShowSale()
{
    int c;
    while(1)
    {

        system("CLS");
        printf("欢迎来到查看销量界面！\n");
        printf("----------------------\n");
        printf("1.查看月销量。\n");
        printf("2.查看季度销量。\n");
        printf("3.查看年销量。\n");
        printf("（请输入数字1-3）\n");
        fflush(stdin);
        c = getch();
        fflush(stdin);

        switch(c){
            case 27: return;//27表示esc键。
            case '1':
                system("CLS");
                Check_Month_Sales();//printf("ces!!");
                break;
            case '2':
                system("CLS");
                Check_Season_Sales();
                break;
            case '3':system("CLS");Check_Year_Sales();break;
            default:continue;
        }
    }
}

void ManageGoods()
{
    char a;
    while(1)
    {
        system("CLS");
        printf("商品管理菜单\n");
        printf("--------------\n");
        printf("1.调整商品价格\n");
        printf("2.增删商品种类\n");
        printf("--------------\n");
        printf("键盘输入1 or 2\n");

        fflush(stdin);
        scanf("%c",&a);
        fflush(stdin);

        system("CLS");
        switch(a)
        {
            case '1':ModifyPrice();break;
            case '2':ChangeGoods();break;
            default:break;
        }
    }
}

int main()
{
    setbuf(stdout,NULL);
    head_beverage = Create_BeverageLinkList();
    head_form = Create_FormLinkList();
    head_material = Create_MaterialLinkList();
//    Order();
//    ShowSale();
//    Inventory_Manage();
//    ManageGoods()
    Rewrite_BeverageData();
    Rewrite_FormData();
    Rewrite_MaterialData();
    return 0;
}
