//
// Created by tangwenxin on 2021/5/3.
//
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
#define Normal_Type setColor(4, 15);//白底红字
#define Warning_Type setColor(2, 15);//白底绿字
#define Normal_Color "color f4"
#define Lstrict 30 //文字距左上限
#define Upstrict 10 //文字的高度上限
#define w 3 //文字水平偏置值
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

//每个会员的ID设为一个结构体，里面包含时间信息，方便后面检索与排序
typedef struct ID_VIP
{
    char id[20];//200105301526
    int year;
    int month;
    int day;
    int hour;
    int min;
}IDVIP;

//vip的结构体
typedef struct vip {
    IDVIP id;
    char name[25];
    char tel[25];
    int position;//1 服务员 2 店长 3 清洁工 4 实习生
    float restmoney;
    char password[25];
    int loss;           // 0 for loss 1 for existing
    int orders[200];  //订单数组，内置订单id
    struct vip *next;
}VIP;
//由于员工的数据结构跟会员很相似，所以这里继承了会员的数据结构
typedef struct vip STAFF;

/* 全局变量 */
int Total_Form_Num = 0;   //导入订单链表数据的时候会进行更新赋值
int Total_Beverage_Num = 0;  //饮品的种类数目(导入饮品链表数据的时候会进行更新赋值)
int flag_come_back;  //全局变量 退出 不生成结果
int auto_back;  //全局变量 作为一系列动作完成,连续回退的标志
int month[13][30];  //不赋值会初始化为0吗？会。应该定义13行因为第0行不代表任何月数。
int season[5][30];  //应该定义5行因为第0行不代表任何季节。
int year[2][30];
int judgeFormOk;
int ExplicitFlag = 1;  //解决错误输入导致的闪烁问题
char bosspassword[21] = "11";
// 实现分页功能的全局变量
int now = 0;
int all=0;
int page = 0;
int index_vip=1;
int single;
int rest;

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
Beverage* head_beverage;  //饮品种类信息链表
Complete_Form* head_form;  //订单信息链表
Material* head_material;  //原材料信息链表
Consumption* head_consumption;  //原材料消耗信息链表
STAFF *head_staff;  //员工信息链表
VIP *head_vip;  //vip信息链表
VIP *node_VIP;  //当前点单的VIP对应的结点(如果当前点单的非vip,则结点值为NULL)
STAFF *node_STAFF;  //当前进行操作的店员对应的结点(自助点单模式时即无店员时值为NULL)

//
void Default();  //欢迎界面
void INIT();  //初始化，从文件中读入数据创建各种链表
void SAVE();  //保存程序执行过程中对数据链表的改动，并重新写入文件
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
Material* Consume_Inventory(Complete_Form* cf,int i);  //消耗cf指向订单中第i杯饮品需要的原材料库存
void Add_Form(Complete_Form*cf,Complete_Form* h);  //将cf指向的订单添加到订单链表中
void Inventory_Manage();  //库存管理页面
void Add_Inventory();  //添加库存
int Show_Inventory();  //查看现有库存的剩余量,返回库存种类数目
void Show_InventoryConsumption();  //查看库存消耗
void Inventory_GetAdded(Material *m);  //根据用户的选择来添加库存
void Show_Inventory_ByConsumption();  //根据原材料消耗量对原材料的消耗进行排序展示
void ChangeShowWay(Consumption *head,Consumption *tail,int status);  //根据status的值来选择是按消耗量从小到大，还是按消耗量从大到小来显示原材料的总消耗量(status=1时候从小到大,0时从大到小)
void Show_Inventory_ById();  //根据原材料的id展示各材料的消耗量
Consumption* Create_ConsumptionLinkList();  //创建原材料消耗的双向链表，创建时即进行插入排序(消耗量从大到小)
Consumption* Find_Consumption(int x,Consumption* h);  //在原材料消耗的双向链表中找到id为x的结点并返回指向其的指针
void Rewrite_FormData();  //将更新后的订单链表数据重新写回文件中
void Rewrite_BeverageData();  //将更新后的饮品链表数据重新写回文件中
void Rewrite_MaterialData();  //将更新后的原材料链表数据重新写回文件中
VIP *VIPReadFile();
void VIPInputFile(VIP *VIPlisthead);
void ClearArr_int(int *a, int size, int value);
void ClearArr_char(char *a, int size, char value);
void VIP__init__(VIP *node);
struct tm *GetNowTime();
int Register_tel(VIP *node,VIP *head);
int Register_recharge(VIP *node);
int VIPCheck_money(char *money);
int C_INTtransf(char *money);
Complete_Form *Find_Form(int x, Complete_Form *h);
void ConsumeRecord(VIP *node, Complete_Form *head);
void Add_VIPOrders(VIP *node, Complete_Form *cf);
void UnchangeWindowSize();
int inputYorN();
void V_order_char(int version);
void V_order_float(int version);
void F_order_char(int version);
void F_order_float(int version);
int pagef(VIP *head,int begin,int end);//实现分页功能
int VIPShowList(VIP *head);  //显示所有会员信息，返回会员个数
void VIPdetele(VIP *head);  //删除会员
VIP *Exedelete(VIP *head, int index);  //会员删除步骤--执行会员删除
void ModandShowDetail(VIP *head);  //VIPadminoperation中引用--修改会员信息
void ShowVIPadmin(VIP *node);  //显示单一会员详细信息
VIP *Find_Vip(VIP *head, int index);  //寻找vip
/*
员工文件的读取格式：
_index,name,tel,money,password,loss(bool),position,(if(position)=1:orderlist....)
index,name,tel,money,password,loss(bool),position,(if(position)=1:orderlist....)
(下划线'_'代表空格)
*/
STAFF *STAFFReadFile();
void STAFF__init__(STAFF *node);  //员工结点初始化，返回无哨兵结点
/*
员工文件的写入格式：
_index,name,tel,money,password,loss(bool),position,(if(position)=1:orderlist....)
index,name,tel,money,password,loss(bool),position,(if(position)=1:orderlist....)
(下划线'_'代表空格)
*/
void STAFFInputFile(STAFF *head);
int STAFFPosition(STAFF *node);  //选择员工的职位
int STAFFmoney(VIP *node);  //选择员工的工资
STAFF *STAFFadd(STAFF *head);  //添加员工
int STAFFShowList(STAFF *head);  //显示员工列表
void ShowSTAFFadmin(STAFF *node);  //显示单个员工的
void ModandShowstaffDetail(STAFF *head,int version); //对员工的操作和修改
void STAFFdetele(STAFF *head);  //员工删除
void VIPadminoperation();  //admin对会员的操作（API）
void STAFFadminoperation();  //root对员工的操作界面 （API）
VIP *VIPRegister(VIP *VIPlisthead);
VIP *VIPlogin(VIP *VIPheadlist);
void STAFFoperationUSER(STAFF *node);
void VIPoperationUSER(VIP *node);
void Default();
void ROOT();
int RootLogin();
//公共函数
void toxy(int x,int y);//将光标移动到x，y坐标处
void HideCursor(int x);//隐藏光标
char *HidePassword();//隐藏密码
int Check_Number(char ch[],int maxnum,int maxlen);  //检查数字是否合规(合规且不超过maxnu返回对应的数字值，合规但超过max返回-1，不合规返回-2)
void color(int x);  //设置字体颜色

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
        return -1;  //输出数字超过最大值
    }
}

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
        printf("Can't open the file:beverageData2.txt");
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
    int x1=22,x2=34,x3=Lstrict,y=13;  //光标所在的位置坐标
    if(h->next==NULL)
    {
        toxy(x1,y);
        printf("暂无饮品...请联系管理员进行添加");
    }
    else
    {
        toxy(x1,y);
        printf("饮品id");
        toxy(x2,y);
        printf("饮品名称");
        toxy(x3,y);
        printf("饮品价格(?)");
        x1+=2;
        while(h->next!=NULL)
        {
            y+=1;
            toxy(x1,y);
            printf("%d",h->next->id);
            toxy(x2,y);
            printf("%s",h->next->name);
            toxy(x3,y);
            printf("%.1f",h->next->price);
            h=h->next;
            num++;
        }
        y+=1;
        toxy(x1,y);
        printf("输入0重新选择杯数");
        y+=1;
        toxy(x1,y);
        printf("输入饮品的id进行选择(按回车确认)：");
    }
    return num;
}

void Order()
{
    Complete_Form* CF;
    CF = (Complete_Form*)malloc(sizeof(Complete_Form));  //创建订单CF
    if(node_VIP!=NULL)  //当前用户为会员
    {
        CF->is_member = 1;
    }
    CF->next = NULL;  //初始化指针域为空
    Create_ID(CF);
    Choose_FormNum(CF);
    int i = 1;  //计数器，表示当前为第i杯
    Choose_Beverage(CF,i);
    if((node_VIP!=NULL)&&judgeFormOk){
        Add_VIPOrders(node_VIP, CF);
    }
    if((node_STAFF != NULL)&&judgeFormOk){
        Add_VIPOrders(node_STAFF, CF);
    }
    judgeFormOk = 0;
    return;
}

void Choose_FormNum(Complete_Form* cf)  //选择cf对应订单的杯数
{
    system(Normal_Color);;  //设置背景颜色与字体颜色
    system("cls");  //清屏
    HideCursor(1);  //显示光标
    toxy(28,8);
    printf("     时尚泡吧点单系统");
    toxy(22,9);
    printf("-------------------------------------");
    if(node_VIP!=NULL)  //如果点单的为会员，发出问候
    {
        toxy(28,10);
        if(cf->id.hour<=12)
        {
            printf("上午好,%s",node_VIP->name);
        }
        else if(cf->id.hour>12&&cf->id.hour<=17)
        {
            printf("下午好,%s",node_VIP->name);
        }
        else
        {
            printf("晚上好,%s",node_VIP->name);
        }
    }
    toxy(28,11);
    printf("请输入此单的杯数:");
    toxy(25,13);
    printf("提示：");
    toxy(25,14);
    printf("一次最多点%d杯(按回车确认)",Max_Form_Num);
    char ch[Max_User_Enter];
    toxy(45,11);
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
        toxy(25,15);
        HideCursor(0);
        printf("输入数字超过最大量,请1s后重新输入");
        Sleep(1000);
        Choose_FormNum(cf);  //递归直至输入合规
    }
    else if(x==-2||x==0)
    {
        HideCursor(0);
        toxy(25,15);
        printf("输入内容不合规，请1s后重新输入数字");
        Sleep(1000);
        Choose_FormNum(cf);  //递归直至输入合规
    }
    return;
}

void Choose_Beverage(Complete_Form* cf,int i)
{
    system(Normal_Color);;  //设置背景颜色与字体颜色
    system("cls");  //清屏
    HideCursor(1);  //显示光标
    int c;  //存储数字合规检查的返回值
    int Beverage_Num;  //保存目前饮品种类数
    char ch[Max_User_Enter];  //保存用户输入
    toxy(28,8);
    printf("     时尚泡吧点单系统");
    toxy(22,9);
    printf("-------------------------------------");
    toxy(27,11);
    printf("请选择第%d杯饮品:\n",i);
    Beverage_Num = Show_Beverages(head_beverage);
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
        Sleep(1000);  //暂停1秒显示提示信息
        Choose_Beverage(cf,i);  //重新输入
    }
    else
    {
        printf("\t\t输入内容不合规，请1s后重新输入饮品对应的id");
        Sleep(1000);  //暂停1秒显示提示信息
        Choose_Beverage(cf,i);  //重新输入
    }
    return;
}
/*

对应的颜色码表：

1. 0 = 黑色       8 = 灰色

1 = 蓝色       9 = 淡蓝色

2 = 绿色       10 = 淡绿色

3 = 浅绿色     11 = 淡浅绿色

4 = 红色       12 = 淡红色

5 = 紫色       13 = 淡紫色

6 = 黄色       14 = 淡黄色

7 = 白色       15 = 亮白色

*/
void setColor(unsigned short ForeColor,unsigned short BackGroundColor)

{

    HANDLE handle=GetStdHandle(STD_OUTPUT_HANDLE);//获取当前窗口句柄

    SetConsoleTextAttribute(handle,ForeColor+BackGroundColor*0x10);//设置颜色

}

void Choose_Ingredients(Complete_Form* cf,int i)
{
    system(Normal_Color);;  //设置背景颜色与字体颜色
    system("cls");  //清屏
    HideCursor(0);  //隐藏光标
    char t;  //读入选择对应的字符
    toxy(28,8);
    printf("     时尚泡吧点单系统");
    toxy(22,9);
    printf("-------------------------------------");
    toxy(26,11);
    printf("请选择第%d杯需要添加的小料",i);
    toxy(27,13);
    printf("1.布丁--1元");
    toxy(27,14);
    printf("2.椰果--1元");
    toxy(27,15);
    printf("3.红豆--2元");
    toxy(27,16);
    printf("4.珍珠--2元");
    toxy(27,17);
    printf("5.不添加小料");
    toxy(27,18);
    printf("0.重新选择杯数");
    toxy(27,19);
    printf("Esc.返回主界面");
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
        case 27:
            Default();
            return;
        default:
            toxy(25,21);
            printf("请根据对应标号进行选择");
            Choose_Ingredients(cf,i);  //递归直至输入合规
            return;
    }
    Material* ci_return = Consume_Inventory(cf,i);  //根据该订单信息对库存进行“消耗”
    if(ci_return==NULL)  //若库存足够
    {
        Choose_Temperature(cf,i);  //选择饮品温度
        return;
    }
    else  //原材料库存不足，给出提示，重新点单
    {
        toxy(23,21);
        printf("%s库存不足，请联系管理员进行添加",ci_return->name);
        toxy(23,22);
        printf("两秒后自动返回点单页面");
        Sleep(2000);
        Order(); //返回点单页面
        return;
    }
}

void Choose_Temperature(Complete_Form* cf,int i)
{
    system(Normal_Color);;  //设置背景颜色与字体颜色
    system("cls");  //清屏
    HideCursor(0);  //隐藏光标
    char t;
    toxy(28,8);
    printf("     时尚泡吧点单系统");
    toxy(22,9);
    printf("-------------------------------------");
    toxy(26,11);
    printf("请选择第%d杯的饮用温度",i);
    toxy(27,13);
    printf("1.加冰");
    toxy(27,14);
    printf("2.常温");
    toxy(27,15);
    printf("3.微热");
    toxy(27,16);
    printf("4.热");
    toxy(27,17);
    printf("0.重新选择杯数");
    toxy(27,18);
    printf("Esc.返回主界面");
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
        case 27:
            Default();
            return;
        default:
            toxy(25,20);
            printf("输入不合规,请重新输入");
            Choose_Temperature(cf,i);  //递归直至输入合规
            return;
    }
    Choose_Drinkplace(cf,i);
    return;
}

void Choose_Drinkplace(Complete_Form* cf,int i)
{
    system(Normal_Color);;  //设置背景颜色与字体颜色
    system("cls");  //清屏
    HideCursor(0);  //隐藏光标
    char t;
    toxy(28,8);
    printf("     时尚泡吧点单系统");
    toxy(22,9);
    printf("-------------------------------------");
    toxy(26,11);
    printf("请选择第%d杯的饮用地点",i);
    toxy(27,13);
    printf("1.堂食");
    toxy(27,14);
    printf("2.外带(打包)");
    toxy(27,15);
    printf("0.重新选择杯数");
    toxy(27,16);
    printf("Esc.返回主界面");
//    printf("请选择第%d杯的饮用地点\n",i);
//    printf("\t\t按下esc键返回主页面\n");
//    printf("\t\t1.堂食\n");
//    printf("\t\t2.外带(打包)\n");
//    printf("\t\t0.重新选择杯数\n");
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
        case 27:
            Default();
            return;
        default:
            toxy(25,18);
            printf("输入不合规,请重新输入");
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
    system(Normal_Color);;  //设置背景颜色与字体颜色
    system("cls");  //清屏
    HideCursor(0);  //隐藏光标
    char t;
    Get_CpTotalprice(cf);  //计算当前订单价格
    toxy(28,8);
    printf("     时尚泡吧点单系统");
    toxy(22,9);
    printf("-------------------------------------");
    toxy(26,11);
    printf("订单总价为:%.1f元,请选择支付方式",cf->total_price);
    toxy(27,13);
    printf("1.现金支付");
    toxy(27,14);
    printf("2.在线支付");
//    printf("订单总价为:%.1f元,请选择支付方式\n",cf->total_price);
//    printf("\t\t按下esc键返回主页面\n");
//    printf("\t\t1.现金支付\n");
//    printf("\t\t2.在线支付\n");
    if(cf->is_member>0)
    {
        toxy(27,15);
        printf("3.会员卡支付(85折)");
        toxy(27,16);
        printf("Esc.返回主界面");
    }
    else
    {
        toxy(27,15);
        printf("Esc.返回主界面");
    }
    fflush(stdin);
    t = getch();
    fflush(stdin);
    if(cf->is_member==0)
    {
        switch (t)
        {
            case '1':
                cf->pay_way = 1;
                toxy(25,18);
                printf("谢谢惠顾！！！");
                judgeFormOk = 1;
                Sleep(1000);  //暂停1秒显示提示信息
                break;
            case '2':
                cf->pay_way = 2;
                toxy(25,18);
                printf("谢谢惠顾！！！");
                judgeFormOk = 1;
                Sleep(1000);  //暂停1秒显示提示信息
                break;
            case 27:
                Default();
                return;
            default:
                toxy(25,18);
                printf("输入不合规,请重新输入");
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
                toxy(25,18);
                printf("谢谢惠顾！！！");
                judgeFormOk = 1;
                Sleep(1000);  //暂停1秒显示提示信息
                break;
            case '2':
                cf->pay_way = 2;
                toxy(25,18);
                printf("谢谢惠顾！！！");
                judgeFormOk = 1;
                Sleep(1000);  //暂停1秒显示提示信息
                break;
            case '3':
                cf->pay_way = 3;
                if(node_VIP->restmoney>cf->total_price)
                {
                    node_VIP->restmoney -= cf->total_price*0.85;
                    toxy(24,18);
                    printf("会员卡支付完毕,您还剩余%.2f元",node_VIP->restmoney);
                    toxy(25,18);
                    printf("谢谢惠顾!!!");
                    judgeFormOk = 1;
                    Sleep(1000);
                }
                else
                {
                    toxy(24,18);
                    printf("会员卡余额不足,支付失败");
                    Sleep(1000);
                    return;  //支付失败不会添加到订单列表
                }
                break;
            case 27:
                Default();
                return;
            default:
                toxy(25,18);
                printf("输入不合规,请重新输入");
                Choose_Payway(cf);
                return;
        }
    }
    Add_Form(cf,head_form);  //将该订单结点添加到订单链表中
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

Material* Consume_Inventory(Complete_Form* cf,int i)  //消耗cf指向订单中第i杯原材料库存，若库存不足，则进行提示，自动取消该次订单，返回不足的原材料对应的结构体指针;若能正常消耗，返回null
{
    int beverage_id;  //饮品id
    int material_id;  //原材料id
    Beverage* beverage_struct;  //饮品结构体
    Material* material_struct;  //原材料结构体
    beverage_id = cf->forms[i-1].beverage;  //找到这个订单的第i杯饮品对应的id
    beverage_struct = Find_Beverage(beverage_id,head_beverage);  //通过id找到对应的饮品结构体
    for(int j=0;j<beverage_struct->material_num;j++)  //遍历该饮品结构体的原材料种类，并进行“消耗”
    {
        material_id = beverage_struct->materials[j];  //找到该饮品的原材料对应的id
        material_struct = Find_Material(material_id, head_material);  //通过原材料对应的id找到对应的结构体
        if (material_struct->inventory >= material_struct->consume)  //若库存足够
        {
            material_struct->inventory -= material_struct->consume;  //该材料的库存量减去该材料的单次使用量即为“消耗”
        } else  //库存不足
        {
            return material_struct;
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
    fp = fopen("..\\beverageData2.txt","w");
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


void VIPFunction()
{
    VIP *head = VIPReadFile();
    int i = 2;
    while(i--){
        head = VIPRegister(head);
    }
    VIPInputFile(head);
    head = VIPReadFile();
}
// 0代表检测不通过
int VIPCheck_Name(char *a){
    int len = strlen(a);
    if(len>20||len<=0)
        return 0;
    for (int i = 0; i < len;i++){
        if(a[i]==' '){
            return 0;
        }
    }
    return 1;
}



// return 0代表返回，1代表没关系*返回/主界面
int Register_Name(VIP *node){
    int attation = 0;
    while(1)
    {
        char temp[100] = {0};
        char name[100] = {0};
        printf("ESC 首页\n *  返回上一级");
        toxy(28, 8);
        printf("     时尚泡吧信息管理系统");
        toxy(22,9);
        printf("-------------------------------------");
        toxy(Lstrict, Upstrict+1);
        strcpy(temp, "请输入你的姓名：");
        printf("%s", temp);
        int lenname = strlen(temp);
        if(attation)
            Warning_Type;
        toxy(Lstrict, Upstrict+2);
        printf("注意！");
        toxy(Lstrict, Upstrict+3);
        printf("不应超出20个字符且不能为空!");
        toxy(Lstrict, Upstrict+4);
        printf("不应包括空格!");
        if(attation){
            toxy(Lstrict, Upstrict+5);
            Normal_Type;
            printf("请重试！");
        }
        toxy(Lstrict + lenname + 1, Upstrict+1);
        int i = 0;
        char c = 1;
        int flag = 0;
        while(c){
            int flag = 0;
            fflush(stdin);
            c = getch();
            fflush(stdin);
            switch (c)
            {
                case '*':
                    fflush(stdin);
                    system("cls");
                    return 0;
                case '\r':
                    fflush(stdin);
                    flag = 1;
                    break;
                case 27:
                    fflush(stdin);
                    system("cls");
                    flag_come_back = 1;
                    return 0;
                case '\b':
                    if(i>0){
                        name[--i] = 0;
                        printf("\b \b");
                    }

                default:
                    if(c>31){
                        if(i<20){
                            name[i++] = c;
                            printf("%c", c);
                        }
                        fflush(stdin);
                        break;
                    }
                    else{
                        fflush(stdin);
                        break;
                    }
            }
            if(flag)
                break;
        }
        if(VIPCheck_Name(name))
        {
            strcpy(node->name, name);
            system("cls");
            return 1;
        }
        else
        {
            attation = 1;
            system("cls");
            continue;
        }
    }
}
// 0代表检测不通过 -1代表电话有重复
int VIPCheck_tel(char *tel,VIP *head){
    int len = strlen(tel);
    VIP *cur = head;
    if(len>20||len<=0)
        return 0;
    for (int i = 0; i < len;i++){
        if(tel[i]>'9'||tel[i]<'0')
            return 0;
    }
    if(cur==NULL){
        return 1;
    }
    while((cur!=NULL)&&(strcmp(cur->tel,tel)!=0)){
        cur = cur->next;
    }
    if(cur == NULL){
        return 1;
    }
    return -1;
}

// 注册电话号 0未创建，1创建
int Register_tel(VIP *node,VIP *head){
    system("cls");
    int attation = 0;
    int flag_tel_repeat = 0;
    while(1)
    {
        char temp[100] = {0};
        char tel[100] = {0};
        printf("ESC 首页\n *  返回上一级");
        toxy(28, 8);
        printf("     时尚泡吧信息管理系统");
        toxy(22,9);
        printf("-------------------------------------");
        toxy(Lstrict, Upstrict+1);
        strcpy(temp, "请输入你的电话:");
        printf("%s", temp);
        int lentel = strlen(temp);
        if(attation)
            Warning_Type;
        toxy(Lstrict, Upstrict+2);
        printf("注意！");
        toxy(Lstrict, Upstrict+3);
        printf("不应超出20位数字且不能为空");
        toxy(Lstrict, Upstrict+4);
        printf("只能输入数字");
        toxy(Lstrict, Upstrict+5);
        printf("仅支持移动手机号码");
        toxy(Lstrict, Upstrict+7);
        Normal_Type;
        if(flag_tel_repeat){
            flag_tel_repeat = 0;
            Warning_Type;
            printf("此号码已存在，请重新输入！");
            Normal_Type;
            toxy(Lstrict, Upstrict+6);
            printf("请重试！");
        }
        toxy(Lstrict, Upstrict+6);
        if(attation){
            Normal_Type;
            printf("请重试！");
            attation = 0;
        }
        toxy(Lstrict + lentel + 1, Upstrict+1);
        char c=1;
        int i = 0;
        while(c){
            int flag = 0;
            fflush(stdin);
            c = getch();
            fflush(stdin);
            switch (c)
            {
                case '*':
                    fflush(stdin);
                    system("cls");
                    return 0;
                case '\r':
                    fflush(stdin);
                    flag = 1;
                    break;
                case 27:
                    fflush(stdin);
                    system("cls");
                    flag_come_back = 1;
                    return 0;
                case '\b':
                    if(i>0){
                        tel[--i] = 0;
                        printf("\b \b");
                    }
                default:
                    if(c>31){
                        if(i<20){
                            tel[i++] = c;
                            printf("%c", c);
                        }
                        fflush(stdin);
                        break;
                    }
                    else{
                        fflush(stdin);
                        break;
                    }
            }
            if(flag)
                break;
        }
        int result = VIPCheck_tel(tel, head);
        if(result==1)
        {
            strcpy(node->tel, tel);
            system("cls");
            return 1;
        }
        else if(result==-1){
            flag_tel_repeat = 1;
            system("cls");
            continue;
        }
        {
            attation = 1;
            system("cls");
            continue;
        }
    }
}

// 检测密码合法性 0通过1不通过
int VIPCheck_password(char *password){
    int len = strlen(password);
    int flag = 0, flag_a = 1, flag_1 = 1,flag__=1;

    if(len>16||len<8)
        return 0;
    for (int i = 0; i < len;i++){
        if(password[i]==' ')
            return 0;
        if((password[i]<='9'&&password[i]>='0')&&flag_1){
            flag++;
            flag_1 = 0;
        }
        if(((password[i]<='Z'&&password[i]>='A'||password[i]<='z'&&password[i]>='a'))&&flag_a){
            flag++;
            flag_a = 0;
        }
        //别的不可显示字符已经被c>31过滤掉了，如果不是字母和数字必是字符
        if(!((password[i]<='9'&&password[i]>='0')||(password[i]<='Z'&&password[i]>='A'||password[i]<='z'&&password[i]>='a'))&&flag__){
            flag++;
            flag__ = 1;
        }
    }
    if(flag<2)
        return 0;
    return 1;
}
// 注册密码 0未创建，1创建
int Register_password(VIP *node){
    system("cls");
    int attation = 0;
    char password[100] = {0};
    char repeat[100] = {0};
    int norepeat = 0;
    int lenpassword = 0;
    while(1)
    {
        system(Normal_Color);
        if (!norepeat)
        { //确认这个conyinue不是因为输入错误造成的，而是为了输入repeat
            for (int i = 0; i < 100;i++)
                password[i] = 0;
        }
        else{
            attation = 0;//如果为了repeat不要启动错误提醒
        }
        char temp[100] = {0};
        if(norepeat){
            printf("ESC 首页\n *  返回上一级");
            toxy(28, 8);
            printf("     时尚泡吧信息管理系统");
            toxy(22,9);
            printf("-------------------------------------");
            toxy(Lstrict, Upstrict+1);
            strcpy(temp, "请输入你的密码:");
            printf("%s", temp);// 重复时password已经填好了
            for (int i = 0; i < strlen(password);i++)
                printf("*");
            printf("\n");
            lenpassword = strlen(temp);
            toxy(Lstrict, Upstrict+2);
            strcpy(temp, "请重新输入密码:");
            printf("%s", temp);
            toxy(Lstrict, Upstrict+4);
            if(norepeat>=2){
                Warning_Type;
                printf("密码不匹配!");
                Normal_Type;
            }
            toxy(Lstrict, Upstrict+3);
            if (norepeat>=2)
            {
                Normal_Type;
                printf("请重试!");
                attation = 0;
            }
            if(norepeat){
                toxy(Lstrict + lenpassword + 1, Upstrict+2);
            }
            else{
                toxy(Lstrict + lenpassword + 1, Upstrict+1);
            }
            Normal_Type;
        }
        else
        {
            printf("ESC 首页\n *  返回上一级");
            toxy(28, 8);
            printf("     时尚泡吧信息管理系统");
            toxy(22,9);
            printf("-------------------------------------");
            toxy(Lstrict, Upstrict+1);
            strcpy(temp, "请输入你的密码:");
            printf("%s", temp);
            if(norepeat!=0){// 重复时password已经填好了
                for (int i = 0; i < strlen(password)-1;i++)
                    printf("*");
            }
            int lenpassword = strlen(temp);
            if (attation)
                Warning_Type;
            toxy(Lstrict, Upstrict+2);
            printf("注意!");
            toxy(Lstrict, Upstrict+3);
            printf("不能包括空格与*号");
            toxy(Lstrict, Upstrict+4);
            printf("长度必须为8-16个字符");
            toxy(Lstrict, Upstrict+5);
            printf("必须包含数字、字母、符号中至少2种");
            Normal_Type;
            toxy(Lstrict, Upstrict+6);
            if (attation)
            {
                Normal_Type;
                printf("请重试！");
            }
            if(norepeat){
                toxy(Lstrict + lenpassword + 1, Upstrict+2);
            }
            else{
                toxy(Lstrict + lenpassword + 1, Upstrict+1);
            }
        }

        char c = 1;
        int i = 0;
        char tor[100] = {0};

        while (c)
        {
            fflush(stdin);
            c = getch();//不回显函数
            fflush(stdin);
            if(c<0){//排除上下方向键的问题
                continue;
            }

            if (c == '*')
            {
                fflush(stdin);
                system("cls");
                return 0;
            }//在getch中回车会被读成\r
            else if (c == '\r'){
                fflush(stdin);
                break;
            }
            else if (c == 27)
            {
                system("cls");
                fflush(stdin);
                flag_come_back = 1;
                return 0;
            }
            else if(c>31){
                if(i<16){
                    tor[i++] = c;
                    printf("*");
                }
            }
            else if(c=='\b')
            {
                if(i>0){
                    tor[--i] = 0;
                    printf("\b \b");
                }
            }




        }
        if(norepeat){// 分别保存
            strcpy(repeat, tor);
        }
        else{
            strcpy(password, tor);
        }
        if (VIPCheck_password(password))
        {
            if (strcmp(password, repeat) == 0)
            {
                strcpy(node->password, password);
                system("cls");
                return 1;
            }
            else
            {
                norepeat++;
                system("cls");
                continue;
            }
        }
        else
        {
            attation = 1;
            system("cls");
            continue;
        }
    }
}

/*注册会员*/
VIP* VIPRegister(VIP*VIPlisthead){
    VIP *p = (VIP *)malloc(sizeof(VIP));
    VIP__init__(p);
    VIP *cur = VIPlisthead;
    if (cur != NULL) {
        while(cur->next!=NULL)
            cur = cur->next;
    }
    p->loss = 0; //默认没有挂失
    while(1)
    {// 名字部分
        /*name space*/
        if(flag_come_back||auto_back||Register_Name(p)==0){
            if(auto_back){//autoback的时候要break执行后面的
                auto_back = 0;
                break;
            }
            else{
                return VIPlisthead;//*&ESC直接回去就好
            }
        }

        while(1)// 电话部分
        {
            if(flag_come_back||auto_back||Register_tel(p,VIPlisthead)==0)
                break;
            while(1)// 密码部分
            {
                if(flag_come_back||auto_back||Register_password(p)==0)
                    break;
                while(1){// 充值部分
                    if(flag_come_back||auto_back||Register_recharge(p)==0)
                        break;
                    auto_back = 1;
                    break;
                }
            }
        }
    }
    //id 部分
    struct tm *NowTime = GetNowTime();
    p->id.year = NowTime->tm_year+1900;
    p->id.month = NowTime->tm_mon+1;
    p->id.day = NowTime->tm_mday;
    p->id.hour = NowTime->tm_hour+8;
    p->id.min = NowTime->tm_min;
    sprintf(p->id.id, "%04d%02d%02d%02d%02d\0",// 处理Idvip结构体中id字符数组部分
            p->id.year, p->id.month, p->id.day, p->id.hour, p->id.min);
    auto_back = 0;
    if(flag_come_back){//如果连续回退就不用保存
        free(p);
        return VIPlisthead;
    }
    if(cur==NULL){
        all++;
        return p;
    }
    else{
        cur->next = p;
        all++;
        return VIPlisthead;
    }
}

/*
读取vip会员名单，返回一个没有哨兵结点的VIP型链表
格式：
_index,name,tel,restmoney,password,loss(bool),(orderlist....)
index,name,tel,restmoney,password,loss(bool),(orderlist....)
(下划线'_'代表空格)
*/
VIP* VIPReadFile(){
    //如果没有文件，创建文件
    FILE *temp = fopen("PersonVIP.txt", "a");
    fclose(temp);
    FILE *fp = fopen("PersonVIP.txt", "r");
    VIP *VIPlisthead = NULL, *VIPlistcur = NULL, *VIPlistlast = NULL;
    VIPlisthead = VIPlistcur = (VIP *)malloc(sizeof(VIP));
    VIPlisthead->next = NULL;
    char flag = fgetc(fp);

    while(flag!=EOF)
    {
        int i = 0;
        VIP *p = (VIP *)malloc(sizeof(VIP));
        VIP__init__(p);
        // fscanf(fp, "%4d%2d%2d%2d%2d,%[^,],%[^,],%d,%[^,],%d",
        //        &(p->id.year), &(p->id.month), &(p->id.day), &(p->id.hour), &(p->id.min),
        //        (p->name), (p->tel), &(p->restmoney), (p->password), &(p->loss));
        fscanf(fp,"%4d%2d%2d%2d%2d",&(p->id.year), &(p->id.month), &(p->id.day), &(p->id.hour), &(p->id.min));
        fgetc(fp);
        fscanf(fp,"%[^ ]",(p->name));
        fgetc(fp);
        fscanf(fp,"%[^ ]",p->tel);
        fgetc(fp);
        fscanf(fp,"%f",&(p->restmoney));
        fgetc(fp);
        fscanf(fp,"%[^ ]",p->password);
        fgetc(fp);
        fscanf(fp,"%d",&(p->loss));
        sprintf(p->id.id, "%04d%02d%02d%02d%02d\0",// 处理Idvip结构体中id字符数组部分
                p->id.year, p->id.month, p->id.day, p->id.hour, p->id.min);
        flag = fgetc(fp);
        VIPlistcur->next = p;
        VIPlistcur = VIPlistcur->next;
        if(flag!=' ')
            continue;
        do{
            fscanf(fp, "%d", &(p->orders[i++]));
            flag = fgetc(fp);
        } while (flag == ',');

    }
    return VIPlisthead->next;
}

/*
保存会员名单，参数为一个没有哨兵结点的VIP型链表
格式：
_index,name,tel,restmoney,password,loss(bool),(orderlist....)
index,name,tel,restmoney,password,loss(bool),(orderlist....)
(下划线'_'代表空格)
*/
void VIPInputFile(VIP *VIPlisthead){
    VIP *p = VIPlisthead;
    FILE *fp = fopen("PersonVIP.txt", "w");
    int flag = 0;
    while (p != NULL)
    {   //为了使最后一行无换行
        if(flag)
        {
            fprintf(fp, "\n%04d%02d%02d%02d%02d %s %s %.2f %s %d",
                    p->id.year, p->id.month, p->id.day, p->id.hour, p->id.min,
                    p->name, p->tel, p->restmoney, p->password, p->loss);
            for (int i = 0, kase = 0; i < 100; i++)
            {
                if(p->orders[i]){
                    if(!kase++)
                        fprintf(fp," ");
                    else{
                        fprintf(fp, ",");
                    }
                    fprintf(fp, "%d", p->orders[i]);
                }
            }
        }
        else//第一行前面放个‘ ’方便检测文件是否为空
        {
            fprintf(fp, " %04d%02d%02d%02d%02d %s %s %.2f %s %d",
                    p->id.year, p->id.month, p->id.day, p->id.hour, p->id.min,
                    p->name, p->tel, p->restmoney, p->password, p->loss);
            for (int i = 0, kase = 0; i < 100; i++)
            {
                if(p->orders[i]){
                    if(!kase++)
                        fprintf(fp," ");
                    else{
                        fprintf(fp, ",");
                    }
                    fprintf(fp, "%d", p->orders[i]);
                }
            }
            flag = 1;
        }
        p = p->next;
    }
    fclose(fp);
}
//int型数组的初始化
void ClearArr_int(int *a,int size,int value){
    int i = 0;
    for (;i<size;i++)
        a[i] = value;
}
//char型数组的初始化
void ClearArr_char(char *a,int size,char value){
    int i = 0;
    for (;i<size;i++)
        a[i] = value;
}
/*VIP结构体的初始化函数，根据指针的性质，向函数中传入指针，不用返回*/
void VIP__init__(VIP *node){
    ClearArr_char(node->tel,sizeof(node->tel),0);
    ClearArr_char(node->name,sizeof(node->name),0);
    ClearArr_char(node->password, sizeof(node->password), 0);
    ClearArr_int(node->orders, sizeof(node->orders) / 4, 0); //int类型的大小是4bytes
    node->loss = 0;
    node->restmoney = 0;
    ClearArr_char(node->id.id, sizeof(node->id.id), 0);
    node->id.day = 0;
    node->id.month = 0;
    node->id.year = 0;
    node->id.min = 0;
    node->id.hour = 0;
    node->next = NULL;
}
//获取当前时间的函数，返回的是tm类型结构体，详见：L-Click+Ctrl
struct tm* GetNowTime(){
    time_t timep;
    struct tm *NowTime;
    time (&timep);
    NowTime=gmtime(&timep);
    return NowTime;
}
// 检测输入金额的合法性
int VIPCheck_money(char *money){
    int len = strlen(money);
    int kase = 0;
    if(len>20)
        return 0;
    for(int i=0;i<len;i++){
        if(money[i]=='.')
            kase++;
    }
    if(kase>1)
        return 0;
    for (int i = 0; i < len;i++){
        if(money[i]>'9'||money[i]<'0')
            if(money[i]!='.')
                return 0;
    }
    return 1;
}
// char类型数字->int类型
int C_INTtransf(char *money){
    int temp = 0;
    int i = 0;
    for (i = 0; money[i];i++);
    int quanzhong = 1;
    for (i = i - 1,quanzhong=1; i >= 0;i--,quanzhong*=10){
        temp += quanzhong * (money[i]-'0');
    }
    return temp;
}
// 充值过程 0未成功，1成功
int Register_recharge(VIP *node){
    system("cls");
    int attation = 0;
    while(1)
    {
        char temp[100] = {0};
        char money[100] = {0};
        printf("ESC 首页\n *  返回上一级");
        toxy(28, 8);
        printf("     时尚泡吧信息管理系统");
        toxy(22,9);
        printf("-------------------------------------");
        toxy(Lstrict, Upstrict+1);
        strcpy(temp, "请输入您要充值的金额（如不想充值则充值0元）");
        printf("%s", temp);
        int lentel = strlen(temp);
        if(attation)
            Warning_Type;
        toxy(Lstrict, Upstrict+2);
        printf("注意！");
        toxy(Lstrict, Upstrict+3);
        printf("充值钱数不得多于20个字符");
        toxy(Lstrict, Upstrict+4);
        printf("仅可输入金额");
        toxy(Lstrict, Upstrict+5);
        if(attation){
            Normal_Type;
            printf("请重试！");
        }
        toxy(Lstrict + lentel + 1, Upstrict+1);
        char c=1;
        int i = 0;
        while (c)
        {
            fflush(stdin);
            c = getch();//不回显函数
            fflush(stdin);
            if(c<0){//排除上下方向键的问题
                continue;
            }

            if (c == '*')
            {
                fflush(stdin);
                system("cls");
                return 0;
            }//在getch中回车会被读成\r
            else if (c == '\r'){
                if(money[0]==0){
                    fflush(stdin);
                    continue;
                }
                fflush(stdin);
                break;
            }
            else if (c == 27)
            {
                system("cls");
                fflush(stdin);
                flag_come_back = 1;
                return 0;
            }
            else if(c>31){
                if(i<20){
                    money[i++] = c;
                    printf("%c",c);
                }
            }
            else if(c=='\b')
            {
                if(i>0){
                    money[--i] = 0;
                    printf("\b \b");
                }
            }




        }
        if(VIPCheck_money(money))
        {
            node->restmoney += (float)atof(money);
            system("cls");
            return 1;
        }
        else
        {
            attation = 1;
            system("cls");
            continue;
        }
    }
}
//以电话形式进行查找，无电话返回NULL
VIP *VIPSearchTel(char *targettel,VIP *head){
    if(head == NULL)
        return NULL;
    if(head==NULL)
        return NULL;
    VIP *cur = head;
    while(cur!=NULL&&(strcmp(cur->tel,targettel)!=0)){
        cur = cur->next;
    }
    if(cur==NULL)
        return NULL;
    else{
        return cur;
    }
}
//会员登录，成功返回结点，失败返回NULL
VIP *VIPlogin(VIP *VIPheadlist){
    system("cls");
    char c = 1;
    char telnum[20] = {0};
    char password[20] = {0};
    char temp[100] = {0};//定位标点用
    int i = 0,flag_errorcode=0,flag_notele=0;
    int len = 0;
    VIP *node = NULL;
    // if(VIPheadlist == NULL){
    //     return NULL;//没有用户(按任意键继续)
    // }
    while(1){
        printf("ESC 首页\n *  返回上一级");
        toxy(28, 8);
        printf("     时尚泡吧登陆系统");
        toxy(22,9);
        printf("-------------------------------------");
        strcpy(temp, "请输入手机号：");
        toxy(Lstrict, Upstrict+1);
        printf("请输入手机号：");
        if(flag_errorcode&&flag_come_back==0)
            printf("%s", node->tel);
        if(flag_notele){
            toxy(Lstrict, Upstrict+2);
            Warning_Type;
            printf("查无手机号");
            Normal_Type;
            flag_notele = 0;
        }
        toxy(Lstrict + strlen(temp) + 1, Upstrict+1);
        ClearArr_char(temp, 100, 0);
        ClearArr_char(telnum, 20, 0);
        i = 0;// 重新对tel数组赋值
        while(!flag_errorcode){ //输入手机号 密码输入错误说明手机号输入正确了
            fflush(stdin);
            c = getch();
            fflush(stdin);
            len = strlen(telnum);

            switch(c){
                case 27:{ //ESC返回主菜单
                    fflush(stdin);
                    return NULL;
                }
                case '\b':{// backspace
                    if(len>0){
                        telnum[--i] = 0;
                        fflush(stdin);
                        printf("\b \b");
                    }
                    continue;
                }
                case '\r':{// enter
                    fflush(stdin);
                    break;
                }
                case '*':{// 输入手机号的返回和ESC作用相同
                    fflush(stdin);
                    return NULL;
                }
                default:{
                    if(c>='0'&&c<='9'){
                        printf("%c", c);
                        telnum[i++] = c;
                        fflush(stdin);
                    }
                    continue;
                }
            }
            break;
        }
        if(!flag_errorcode){
            node = VIPSearchTel(telnum,VIPheadlist);
        }
        if(node==NULL&&!flag_errorcode){
            flag_notele = 1;
            system("cls");
            continue;//查无手机号 重新开始新的查询手机号操作
        }
        strcpy(temp, "请输入密码：");
        toxy(Lstrict, Upstrict+2);
        printf("%s", temp);
        if(flag_errorcode){
            toxy(Lstrict, Upstrict+3);
            Warning_Type;
            printf("密码不正确或请重新确认密码格式");
            Normal_Type
            flag_errorcode = 0;
        }
        toxy(Lstrict + strlen(temp) + 1, Upstrict+2);
        ClearArr_char(temp, 100, 0);
        c = 1;
        i = 0;
        ClearArr_char(password, 20, 0);
        while(1){// 输入密码
            fflush(stdin);
            c = getch();
            fflush(stdin);
            len = strlen(password);
            switch(c){
                case 27:{ //ESC返回主菜单
                    fflush(stdin);
                    flag_come_back = 1;
                    return 0;
                }
                case '\b':{// backspace
                    if(len>0){
                        password[--i] = 0;
                        fflush(stdin);
                        printf("\b \b");
                    }
                    continue;
                }
                case '\r':{// enter
                    fflush(stdin);
                    break;
                }
                case '*':{
                    fflush(stdin);
                    return 0;
                    break;
                }
                default:{
                    if(c>31){
                        printf("*");
                        password[i++] = c;
                        fflush(stdin);
                    }
                    continue;
                }
            }

            if(len<=0){
                flag_errorcode = 1;//密码输入格式不对
                system("cls");
            }
            break;
        }
        if(flag_come_back){
            system("cls");
            fflush(stdin);
            flag_errorcode = 0;
            flag_come_back = 1;
            continue;
        }
        if(strcmp(password,node->password)==0){
            return node;
        }
        else{
            flag_errorcode = 1;
            system("cls");
            continue;//密码不正确
        }
    }
}
//展示user会员界面
void VIPUSERshow(VIP *node){
    if(node==NULL)
        return;
    system("cls");
    printf("ESC 首页\n *  返回上一级");
    toxy(28, 8);
    printf("     时尚泡吧会员系统");
    toxy(22,9);
    printf("-------------------------------------");
    toxy(Lstrict, Upstrict+1);
    printf("注册日期：%04d-%02d-%02d %02d:%02d",
           node->id.year,node->id.month,node->id.day,node->id.hour,node->id.min);
    toxy(Lstrict, Upstrict+2);
    printf("姓名：%s",node->name);
    toxy(Lstrict, Upstrict+3);
    printf("电话：%s",node->tel);
    toxy(Lstrict, Upstrict+4);
    printf("卡上余额：%.2f",node->restmoney);
    toxy(Lstrict, Upstrict+5);
    printf("卡状态：");
    if (node->loss)
    {
        Warning_Type;
        printf("挂失\n");
        Normal_Type;
    }
    else{
        printf("正常\n");
    }
    if(node->loss){
        toxy(22,Upstrict+6);
        printf("-------------------------------------");
        toxy(Lstrict, Upstrict+7);
        printf("| 1.充值 |");
        toxy(Lstrict, Upstrict+9);
        printf("| 2.解挂 |");
        toxy(Lstrict, Upstrict+11);
        printf("| 3.查看消费记录 |");
        toxy(Lstrict, Upstrict+13);
        printf("| 4.修改密码 |");
        toxy(Lstrict, Upstrict+15);
        printf("| 5.修改电话 |");
        toxy(Lstrict, Upstrict+17);
    }
    else{
        toxy(22,Upstrict+6);
        printf("-------------------------------------");
        toxy(Lstrict, Upstrict+7);
        printf("| 1.充值 |");
        toxy(Lstrict, Upstrict+9);
        printf("| 2.挂失 |");
        toxy(Lstrict, Upstrict+11);
        printf("| 3.查看消费记录 |");
        toxy(Lstrict, Upstrict+13);
        printf("| 4.修改密码 |");
        toxy(Lstrict, Upstrict+15);
        printf("| 5.修改电话 |");
        toxy(Lstrict, Upstrict+17);
        printf("| 6.会员点餐 |");
        toxy(Lstrict, Upstrict+19);
    }
}
// 操作反馈成功界面
void Operationok(int state){
    int time = 3;
    while(time){
        system("cls");
        toxy(Lstrict, Upstrict+2);
        if(state){
            printf("操作成功！将于%d秒后返回",time);
        }
        else{
            printf("操作失败！将于%d秒后返回",time);
        }
        Sleep(1000);
        time--;
    }
}
// 会员卡挂失
void VIPloss(VIP *node){

    while(1){
        system("cls");
        printf("会员卡号：%s\n姓名：%s\n会员卡状态：",node->id.id,node->name);
        Warning_Type;
        if(node->loss){
            printf("挂失");
            Normal_Type;
            printf("\n是否解挂？（Y/N）");
        }
        else{
            printf("未挂失");
            Normal_Type;
            printf("\n是否挂失？（Y/N）");
        }
        char choose = 0;
        int canelchoose = 0;
        while(1){//输入选择？
            fflush(stdin);
            choose = getch();
            fflush(stdin);
            int flag = 0;
            switch(choose){
                case 'N':
                    canelchoose = 1;
                    break;
                case 'Y':
                    node->loss ^= 1;
                    Operationok(1);
                    canelchoose = 1;
                    break;
                default:
                    flag = 1;
                    break;
            }
            if(flag){
                flag = 0;
            }
            else{
                break;
            }
        }
        if(canelchoose){
            canelchoose = 0;
            break;
        }
    }
}
// 展示单一订单
void show_form(Complete_Form *node){
    printf("---%04d年%02d月%02d日 %02d:%02d---\n", node->id.year, node->id.month, node->id.day, node->id.hour, node->id.min);
    int i = 0;
    for (;i < node->number;i++){
        Beverage *detail = Find_Beverage(node->forms[i].beverage, head_beverage);
        printf("饮品名称：%s  附加小料：", detail->name);
        switch(node->forms[i].ingredients){
            case 1:
                printf("布丁  ");
                break;
            case 2:
                printf("椰果  ");
                break;
            case 3:
                printf("红豆  ");
                break;
            case 4:
                printf("珍珠  ");
                break;
            case 5 :
                printf("无  ");
                break;
        }
        printf("单价：%2.1f\n", node->forms[i].total_price);
    }
    printf("---支付方式：");
    switch(node->pay_way){
        case 1:
            printf("现金支付--");
            break;
        case 2:
            printf("在线支付--");
            break;
        case 3:
            printf("会员卡支付--");
            break;
    }
    printf("总计金额：%2.1f---", node->total_price);
}
// 展示会员消费记录，多个订单
void ConsumeRecord(VIP *node,Complete_Form *head){//head为带有哨兵结点的订单链表
    system("cls");
    int len = 0;
    Complete_Form *FormInfo=NULL;
    for (; node->orders[len];len++);
    if(len==0){
        printf("无订单信息\n----------\n");
    }
    else{
        int i = 0;
        int index = node->orders[i];
        while(index){
            FormInfo = Find_Form(index, head);
            if (FormInfo != NULL)
            {
                show_form(FormInfo);
                printf("\n\n");
            }

            index = node->orders[++i];
        }
    }
    printf("按*返回 按ESC返回主菜单");
    while(1){
        char choose = 0;
        fflush(stdin);
        choose = getch();
        fflush(stdin);
        switch(choose){
            case 27:
                flag_come_back = 1;
                return;
            case '*':
                system("cls");
                return;
            default:
                break;
        }
    }
}

//user会员操作
void VIPoperationUSER(VIP *node){
    char operation = 0;
    if(node==NULL)
        return;
    while(1){

        VIPUSERshow(node);
        fflush(stdin);
        operation = getch();
        fflush(stdin);
        switch(operation){
            case 27:
                flag_come_back = 1;
                break;
            case '1':
                if(Register_recharge(node)){
                    Operationok(1);
                }
                break;
            case '2':
                VIPloss(node);
                break;
            case '3':
                ConsumeRecord(node,head_form);
                break;
            case '4':
                Register_password(node);
                break;
            case '5':
                Register_tel(node,head_vip);
                break;
            case '*':
                return;
            default:
                if(operation=='6'&&node->loss==0){
                    node_VIP = node;
                    Order();
                }
                break;
        }
        if(flag_come_back){
            flag_come_back = 1;
            return;
        }
    }
}




int pagef(VIP *head,int begin,int end){
    VIP *start = head,*last = head;
    int a=begin;
    int b = end;
    while(a--){
        if(!start)
            break;
        start = start->next;
    }

    while(b--){
        if(!last)
            break;
        last = last->next;
    }

    for (; (start != last)&&start;start=start->next){
        printf("序号：%d  注册时间：%4d-%02d-%02d %02d:%02d  姓名：%-20s  手机号：%-20s  密码：*****  余额：%-10.2f  卡状态：",
               index_vip, start->id.year,start->id.month,start->id.day,start->id.hour,start->id.min, start->name, start->tel,start->restmoney);
        if(start->loss){
            Warning_Type;
            printf("挂失\n");
            Normal_Type;
        }
        else{
            printf("正常\n");
        }
        index_vip++;
    }
    Sleep(500);
    fflush(stdin);
    return index_vip;
}

int VIPShowList(VIP *head){
    system("cls");
    VIP *cur = head;
    if(cur == NULL){
        printf("暂无会员信息");
        return 0;
    }
    int index = 1;

    if(!all){
        for(VIP *p=head;p;p=p->next)
            all++;
    }

    rest = all % 30;
    page = all / 30;
    single = 30;
    pagef(head, now*single,now*single+30);
    return all;
}



void VIPadminoperation(){
    int time = 0, money = 0;//0代表正序 1代表倒序
    int fristwhile = 0;
    while(1){
        system("cls");//打印界面
        VIPShowList(head_vip);
        printf("\n\n\n");
        printf("1.增加会员 2.注销会员 3.修改/查看会员详细信息 4.按时间排序 5.按余额排序 +.下一页 -.上一页 ESC.返回主菜单 *返回\n");
        while(1){//输入管理
            int choose = 0;
            int temp;
            fflush(stdin);
            choose = getch();
            fflush(stdin);
            switch(choose){
                case '1':
                    system("cls");
                    VIPRegister(head_vip);
                    if(flag_come_back){
                        return;
                    }
                    index_vip =1;
                    now = 0;
                    break;
                case '2':
                    VIPdetele(head_vip);
                    index_vip = 1;
                    now = 0;
                    break;
                case '3':
                    ModandShowDetail(head_vip); //修改/查看会员详细信息
                    break;
                case '4':
                    V_order_char(time ^= 1);
                    index_vip =1;
                    now = 0;
                    break;
                case '5':
                    V_order_float(money ^= 1);
                    index_vip =1;
                    now = 0;
                    break;
                case 27:
                    flag_come_back = 1;
                    return;
                case '*':
                    //返回
                    now = 0;
                    index_vip = 1;
                    return;
                case '+':
                    fflush(stdin);
                    temp = rest == 0 ? page - 1 : page;
                    if(now<temp){
                        now++;
                        fristwhile = 1;
                    }
                    else{
                        index_vip -= rest;
                        if(rest==0)
                            index_vip -= single;
                    }
                    break;
                case '-':
                    fflush(stdin);
                    temp = rest == 0 ? page  : page-1;
                    if(now>0){
                        now--;
                        if(now==temp){
                            index_vip -= (single + rest);
                        }
                        else{
                            index_vip -= single * 2;
                        }
                        fristwhile = 1;
                    }
                    else{
                        if(page==0)
                            index_vip -= rest;
                        else{
                            index_vip -= single;
                        }
                    }
                    break;
                default:
                    continue;
            }
            break;
        }
        if(flag_come_back){
            return;
        }
        if(fristwhile){
            fristwhile=0;
            continue;
        }
    }
}

void VIPdetele(VIP *head){
    index_vip = 1;
    now = 0;
    int temp_;
    int error = 0;
    int number = 0;
    int fristwhile = 0;
    char number_char[100] = {0}, index_char[100] = {0}, temp = 0;
    int i = 0, overinput = 0;
    while(1){//输入编号
        i = 0;
        system("cls");
        number = VIPShowList(head);
        printf("\n\n");
        if(error){
            error = 0;
            Warning_Type;
            printf("\n请输入正确的编号(*返回，ESC返回主菜单)\n");
            Normal_Type;
        }
        printf("请输入要注销会员的编号(+下一页，-上一页)：（*返回 ESC返回主菜单）");
        while(1){
            fflush(stdin);
            temp = getch();
            fflush(stdin);
            switch(temp){
                case 27:
                    flag_come_back = 1;
                    return;
                case '*':
                    index_vip = 1;
                    now = 0;
                    return;
                case '\r':
                    index_vip = 1;
                    now = 0;
                    overinput = 1;
                    break;
                case '\b':
                    if(i>0){
                        printf("\b \b");
                        index_char[--i] = 0;
                    }
                    break;
                case '+':
                    fflush(stdin);
                    temp_ = rest == 0 ? page - 1 : page;
                    if(now<temp_){
                        now++;
                        fristwhile = 1;
                    }
                    else{
                        index_vip -= rest;
                        if(rest==0)
                            index_vip -= single;
                        fristwhile = 1;
                    }
                    break;
                case '-':
                    -                    fflush(stdin);
                    temp_ = rest == 0 ? page  : page-1;
                    if(now>0){
                        now--;
                        if(now==temp_){
                            index_vip -= (single + rest);
                        }
                        else{
                            index_vip -= single * 2;
                        }
                        fristwhile = 1;
                    }
                    else{
                        if(page==0)
                            index_vip -= rest;
                        else{
                            index_vip -= single;

                        }
                        fristwhile = 1;
                    }
                    break;
                default:
                    if(temp<='9'&&temp>='0'){
                        if(i<=10){
                            index_char[i++] = temp;
                            printf("%c", temp);
                            break;
                        }
                    }
            }
            if(overinput == 1){
                overinput = 0;
                break;
            }
            if(fristwhile){
                break;
            }
        }
        if(fristwhile){
            fristwhile=0;
            continue;
        }
        int target = C_INTtransf(index_char);
        ClearArr_char(index_char, 100, 0);
        if (target > number||target == 0)
        {
            index_vip = 1;
            now = 0;
            error = 1;
        }
        else{//确认？
            printf("\n确定要注销序号为%d的会员？（Y/N）", target);
            char confrim = 0;
            int flag_back = 0;
            while(1){
                fflush(stdin);
                confrim = getch();
                fflush(stdin);
                switch(confrim){
                    case 'Y'://执行删除
                        head = Exedelete(head,target);
                        all--;
                        flag_back = 1;
                        Operationok(1);
                        break;
                    case 'N':
                        flag_back = 1;
                        break;
                    default:
                        continue;
                }
                if(flag_back){
                    flag_back = 0;
                    break;
                }
            }
        }
    }
}

VIP *Exedelete(VIP *head,int index){
    VIP *p=(VIP *)malloc(sizeof(VIP));
    p->next = head;
    head = p;//哨兵结点
    VIP *cur = head;
    for (int i = 1;i<index;i++){
        cur = cur->next;
    }//如果删除第n个结点cur会在n-1，p在n+1
    VIP *del = cur->next;
    p = cur->next->next;
    cur->next = p;
    free(del);
    return head->next;//去哨兵结点
}

void ModandShowDetail(VIP *head){
    now = 0;
    index_vip = 1;
    int error = 0;
    int fristwhile=0;
    int number = 0;
    int temp_;
    char number_char[100] = {0}, index_char[100] = {0}, temp = 0;
    int i = 0, overinput = 0;
    while(1){//输入编号
        i = 0;
        system("cls");
        number = VIPShowList(head);
        printf("\n\n");
        if(error){
            error = 0;
            Warning_Type;
            printf("\n请输入正确的编号(*返回，ESC返回主菜单)\n");
            Normal_Type;
        }
        printf("请输入要修改查看会员的编号(+下一页，-上一页)：（*返回 ESC返回主菜单）");
        while(1){
            fflush(stdin);
            temp = getch();
            fflush(stdin);
            switch(temp){
                case 27:
                    index_vip = 1;
                    now = 0;
                    flag_come_back = 1;
                    return;
                case '*':
                    index_vip = 1;
                    now = 0;
                    return;
                case '\r':
                    overinput = 1;
                    break;
                case '\b':
                    if(i>0){
                        printf("\b \b");
                        index_char[--i] = 0;
                    }
                    break;
                case '+':
                    fflush(stdin);
                    fristwhile = 1;
                    temp_ = rest == 0 ? page - 1 : page;
                    if(now<temp_){
                        now++;
                    }
                    else{
                        index_vip -= rest;
                        if(rest==0)
                            index_vip -= single;
                    }
                    break;
                case '-':
                    fflush(stdin);
                    fristwhile = 1;
                    temp_ = rest == 0 ? page  : page-1;
                    if(now>0){
                        now--;
                        if(now==temp_){
                            index_vip -= (single + rest);
                        }
                        else{
                            index_vip -= single * 2;
                        }
                    }
                    else{
                        if(page==0)
                            index_vip -= rest;
                        else{
                            index_vip -= single;
                        }
                    }
                    break;
                default:
                    if(temp<='9'&&temp>='0'){
                        if(i<=10){
                            index_char[i++] = temp;
                            printf("%c", temp);
                            break;
                        }
                    }
            }
            if(overinput == 1){
                overinput = 0;
                break;
            }
            if(fristwhile){
                break;
            }
        }
        if(fristwhile){
            fristwhile=0;
            continue;
        }
        float target = atof(index_char);
        ClearArr_char(index_char, 100, 0);
        if (target > number||target == 0)
        {
            error = 1;
            continue;
        }
        while(1){
            char choose = 0;
            VIP *node = Find_Vip(head, target);
            ShowVIPadmin(node);
            printf("\n\n1.修改密码 2.修改手机号 3.解挂失 4.查看消费记录 *返回 ESC返回主菜单");
            int flag_back = 0;
            while(1){// 输入
                fflush(stdin);
                choose = getch();
                fflush(stdin);
                switch(choose){
                    case '1' :
                        system("cls");
                        if(Register_password(node))
                            Operationok(1);
                        break;
                    case '2':
                        system("cls");
                        if(Register_tel(node,head))
                            Operationok(1);
                        break;
                    case '3':
                        system("cls");
                        VIPloss(node);
                        break;
                    case '4':
                        ConsumeRecord(node, head_form);
                        break;
                    case '*':
                        index_vip = 1;
                        now = 0;
                        flag_back = 1;
                        break;
                    case 27:
                        index_vip = 1;
                        now = 0;
                        flag_come_back = 1;
                        return;
                    default:
                        continue;
                }
                break;
            }
            if(flag_back){
                flag_back = 0;
                break;
            }
            if(flag_come_back){
                return;
            }
        }
    }
}

VIP *Find_Vip(VIP *head,int index){
    VIP *cur = head;
    for (int i = 1; i < index;i++){
        if(cur==NULL)
            return NULL;
        cur = cur->next;
    }
    return cur;
}

void ShowVIPadmin(VIP *node){
    system("cls");
    printf("注册时间：%04d-%02d-%02d %02d:%02d\n姓名：%s\n电话：%s\n密码：%s\n卡上余额：%.2f\n卡状态：",
           node->id.year, node->id.month, node->id.day, node->id.hour, node->id.min,
           node->name, node->tel, node->password, node->restmoney);
    if(node->loss){
        Warning_Type;
        printf("挂失\n");
        Normal_Type;
    }
    else{
        printf("正常\n");
    }
}

STAFF * STAFFReadFile(){
    //如果没有文件，创建文件
    FILE *temp = fopen("PersonStaff.txt", "a");
    fclose(temp);
    FILE *fp = fopen("PersonStaff.txt", "r");
    STAFF *head = NULL, *cur = NULL, *last = NULL;
    head = cur = (STAFF *)malloc(sizeof(STAFF));
    head->next = NULL;
    char flag = fgetc(fp);

    while(flag!=EOF)
    {
        int i = 0;
        STAFF *p = (STAFF *)malloc(sizeof(STAFF));
        STAFF__init__(p);
        // fscanf(fp, "%4d%2d%2d%2d%2d,%[^,],%[^,],%d,%[^,],%d",
        //        &(p->id.year), &(p->id.month), &(p->id.day), &(p->id.hour), &(p->id.min),
        //        (p->name), (p->tel), &(p->restmoney), (p->password), &(p->loss));
        fscanf(fp,"%4d%2d%2d%2d%2d",&(p->id.year), &(p->id.month), &(p->id.day), &(p->id.hour), &(p->id.min));
        fgetc(fp);
        fscanf(fp,"%[^ ]",(p->name));
        fgetc(fp);
        fscanf(fp,"%[^ ]",p->tel);
        fgetc(fp);
        fscanf(fp,"%f",&(p->restmoney));
        fgetc(fp);
        fscanf(fp,"%[^ ]",p->password);
        fgetc(fp);
        fscanf(fp,"%d",&(p->loss));
        fgetc(fp);
        fscanf(fp, "%d", &(p->position));
        sprintf(p->id.id, "%04d%02d%02d%02d%02d\0",// 处理Idvip结构体中id字符数组部分
                p->id.year, p->id.month, p->id.day, p->id.hour, p->id.min);
        flag = fgetc(fp);
        cur->next = p;
        cur = cur->next;
        if(flag!=' ') //没有orders
            continue;
        do{
            fscanf(fp, "%d", &(p->orders[i++]));
            flag = fgetc(fp);
        } while (flag == ',');

    }
    return head->next;
}

/*STAFF结构体的初始化函数，根据指针的性质，向函数中传入指针，不用返回*/
void STAFF__init__(STAFF *node){
    ClearArr_char(node->tel,sizeof(node->tel),0);
    ClearArr_char(node->name,sizeof(node->name),0);
    ClearArr_char(node->password, sizeof(node->password), 0);
    ClearArr_int(node->orders, sizeof(node->orders) / 4, 0); //int类型的大小是4bytes
    node->loss = 0;
    node->restmoney = 0;
    ClearArr_char(node->id.id, sizeof(node->id.id), 0);
    node->id.day = 0;
    node->id.month = 0;
    node->id.year = 0;
    node->id.min = 0;
    node->id.hour = 0;
    node->position = 0;
    node->next = NULL;
}

// 保存会员名单，参数为一个没有哨兵结点的VIP型链表
void STAFFInputFile(STAFF *head){
    STAFF *p = head;
    FILE *fp = fopen("PersonStaff.txt", "w");
    int flag = 0;
    while (p != NULL)
    {   //为了使最后一行无换行
        if(flag)
        {
            fprintf(fp, "\n%04d%02d%02d%02d%02d %s %s %.2f %s %d %d",
                    p->id.year, p->id.month, p->id.day, p->id.hour, p->id.min,
                    p->name, p->tel, p->restmoney, p->password, p->loss,p->position);
            for (int i = 0, kase = 0; i < 100; i++)
            {
                if(p->orders[i]){
                    if(!kase++)
                        fprintf(fp," ");
                    else{
                        fprintf(fp, ",");
                    }
                    fprintf(fp, "%d", p->orders[i]);
                }
            }
        }
        else//第一行前面放个‘ ’方便检测文件是否为空
        {
            fprintf(fp, " %04d%02d%02d%02d%02d %s %s %.2f %s %d %d",
                    p->id.year, p->id.month, p->id.day, p->id.hour, p->id.min,
                    p->name, p->tel, p->restmoney, p->password, p->loss,p->position);
            for (int i = 0, kase = 0; i < 100; i++)
            {
                if(p->orders[i]){
                    if(!kase++)
                        fprintf(fp," ");
                    else{
                        fprintf(fp, ",");
                    }
                    fprintf(fp, "%d", p->orders[i]);
                }
            }
            flag = 1;
        }
        p = p->next;
    }
    fclose(fp);
}

int STAFFPosition(STAFF *node){
    system("cls");
    toxy(Lstrict,Upstrict+1);
    printf("请选择职位：");
    toxy(Lstrict,Upstrict+2);
    printf("1.服务员 2.店长 3.清洁工 4.实习生 *返回 ESC退出");
    char choose=0;
    while(1){
        fflush(stdin);
        choose = getch();
        fflush(stdin);
        switch(choose){
            case '1':
                node->position = 1;
                break;
            case '2':
                node->position = 2;
                break;
            case '3':
                node->position = 3;
                break;
            case '4':
                node->position = 4;
                break;
            case 27:
                flag_come_back = 1;
                return 0;
            case '*':
                return 0;
            default:
                continue;
        }
        ClearArr_int(node->orders, 200, 0);
        return 1;
    }
}

STAFF* STAFFadd(STAFF *head){
    STAFF *p = (STAFF *)malloc(sizeof(VIP));
    STAFF__init__(p);
    STAFF *cur = head;
    if (cur != NULL) {
        while(cur->next!=NULL)
            cur = cur->next;
    }
    p->loss = 0; //默认没有挂失
    while(1)
    {// 名字部分
        /*name space*/
        if(flag_come_back||auto_back||Register_Name(p)==0){
            if(auto_back){//autoback的时候要break执行后面的
                auto_back = 0;
                break;
            }
            else{
                return head;//*&ESC直接回去就好
            }
        }

        while(1)// 电话部分
        {
            if(flag_come_back||auto_back||Register_tel(p,head)==0)
                break;
            while(1)// 密码部分
            {
                if(flag_come_back||auto_back||Register_password(p)==0)
                    break;
                while(1){// 充值部分
                    if(flag_come_back||auto_back||STAFFmoney(p)==0)
                        break;
                    while(1){
                        if(flag_come_back||auto_back||STAFFPosition(p)==0)
                            break;
                        Operationok(1);
                        auto_back = 1;
                        break;
                    }
                }
            }
        }
    }
    //id 部分
    struct tm *NowTime = GetNowTime();
    p->id.year = NowTime->tm_year+1900;
    p->id.month = NowTime->tm_mon+1;
    p->id.day = NowTime->tm_mday;
    p->id.hour = NowTime->tm_hour+8;
    p->id.min = NowTime->tm_min;
    sprintf(p->id.id, "%04d%02d%02d%02d%02d\0",// 处理Idvip结构体中id字符数组部分
            p->id.year, p->id.month, p->id.day, p->id.hour, p->id.min);
    auto_back = 0;
    if(flag_come_back){//如果连续回退就不用保存
        free(p);
        return head;
    }
    if(cur==NULL)
        return p;
    else{
        cur->next = p;
        return head;
    }
}

int STAFFmoney(VIP *node){
    system("cls");
    int attation = 0;
    while(1)
    {
        char temp[100] = {0};
        char money[100] = {0};
        printf("ESC 首页\n *  返回上一级");
        toxy(28, 8);
        printf("     时尚泡吧信息管理系统");
        toxy(22,9);
        printf("-------------------------------------");
        toxy(Lstrict, Upstrict+1);
        strcpy(temp, "请输入员工工资：");
        printf("%s", temp);
        int lentel = strlen(temp);
        if(attation)
            Warning_Type;
        toxy(Lstrict, Upstrict+2);
        printf("注意！");
        toxy(Lstrict, Upstrict+3);
        printf("不得多于20个字符");
        toxy(Lstrict, Upstrict+4);
        printf("仅可输入金额");
        toxy(Lstrict, Upstrict+5);
        if(attation){
            Normal_Type;
            printf("请重试！");
        }
        toxy(Lstrict + lentel + 1, Upstrict+1);
        char c=1;
        int i = 0;
        while (c)
        {
            fflush(stdin);
            c = getch();//不回显函数
            fflush(stdin);
            if(c<0){//排除上下方向键的问题
                continue;
            }

            if (c == '*')
            {
                fflush(stdin);
                system("cls");
                return 0;
            }//在getch中回车会被读成\r
            else if (c == '\r'){
                if(money[0]==0){
                    fflush(stdin);
                    continue;
                }
                fflush(stdin);
                break;
            }
            else if (c == 27)
            {
                system("cls");
                fflush(stdin);
                flag_come_back = 1;
                return 0;
            }
            else if(c>31){
                if(i<20){
                    money[i++] = c;
                    printf("%c",c);
                }
            }
            else if(c=='\b')
            {
                if(i>0){
                    money[--i] = 0;
                    printf("\b \b");
                }
            }
        }
        if(VIPCheck_money(money))
        {
            node->restmoney = (float)atof(money);
            system("cls");
            return 1;
        }
        else
        {
            attation = 1;
            system("cls");
            continue;
        }
    }
}

int STAFFShowList(STAFF *head){
    system("cls");
    STAFF *cur = head;
    if(cur == NULL){
        printf("暂无员工信息");
        return 0;
    }
    int index = 1;
    while(cur!=NULL){
        printf("序号：%d  上岗时间：%04d-%02d-%02d %02d:%02d:%02d  姓名：%-20s  手机号：%-20s  密码：*****  工资：%-10.2f  职位：", index, cur->id.year,cur->id.month,cur->id.day,cur->id.hour,cur->id.day,cur->id.min, cur->name, cur->tel,cur->restmoney);
        switch(cur->position){
            case 1:
                printf("服务员\n");
                break;
            case 2:
                printf("店长\n");
                break;
            case 3:
                printf("清洁工\n");
                break;
            case 4:
                printf("实习生\n");
                break;
        }
        index++;
        cur = cur->next;
    }
    return index - 1;
}

void STAFFadminoperation(){
    int money = 0, time = 0;
    while(1){
        system("cls");//打印界面
        STAFFShowList(head_staff);
        printf("\n\n\n");
        printf("1.增加员工 2.注销员工 3.修改/查看员工详细信息 4.时间排序 5.工资排序 ESC.返回主菜单 *返回\n");
        while(1){//输入管理
            int choose = 0;
            fflush(stdin);
            choose = getch();
            fflush(stdin);
            switch(choose){
                case '1':
                    system("cls");
                    head_staff = STAFFadd(head_staff);
                    if(flag_come_back){
                        return;
                    }
                    break;
                case '2':
                    STAFFdetele(head_staff);
                    break;
                case '3':
                    ModandShowstaffDetail(head_staff,1); //修改/查看员工详细信息
                    break;
                case '4':
                    F_order_char(time^=1);
                    break;
                case '5':
                    F_order_float(money^=1);
                    break;
                case 27:
                    flag_come_back = 1;
                    return;
                case '*':
                    //返回
                    return;
                default:
                    continue;
            }
            break;
        }
        if(flag_come_back){
            return;
        }
    }
}

void STAFFdetele(STAFF *head){
    int error = 0;
    int number = 0;
    char number_char[100] = {0}, index_char[100] = {0}, temp = 0;
    int i = 0, overinput = 0;
    while(1){//输入编号
        i = 0;
        system("cls");
        number = STAFFShowList(head);
        printf("\n\n");
        if(error){
            error = 0;
            Warning_Type;
            printf("\n请输入正确的编号(*返回，ESC返回主菜单)\n");
            Normal_Type;
        }
        printf("请输入要注销员工的编号：（*返回 ESC返回主菜单）");
        while(1){
            fflush(stdin);
            temp = getch();
            fflush(stdin);
            switch(temp){
                case 27:
                    flag_come_back = 1;
                    return;
                case '*':
                    return;
                case '\r':
                    overinput = 1;
                    break;
                case '\b':
                    if(i>0){
                        printf("\b \b");
                        index_char[--i] = 0;
                    }
                    break;
                default:
                    if(temp<='9'&&temp>='0'){
                        if(i<=10){
                            index_char[i++] = temp;
                            printf("%c", temp);
                            break;
                        }
                    }
            }
            if(overinput == 1){
                overinput = 0;
                break;
            }
        }
        int index = C_INTtransf(index_char);
        ClearArr_char(index_char, 100, 0);
        if (index > number||index == 0)
        {
            error = 1;
        }
        else{//确认？
            printf("\n确定要注销序号为%d的员工？（Y/N）", index);
            char confrim = 0;
            int flag_back = 0;
            while(1){
                fflush(stdin);
                confrim = getch();
                fflush(stdin);
                switch(confrim){
                    case 'Y'://执行删除
                        head = Exedelete(head,index);
                        flag_back = 1;
                        Operationok(1);
                        break;
                    case 'N':
                        flag_back = 1;
                        break;
                    default:
                        continue;
                }
                if(flag_back){
                    flag_back = 0;
                    break;
                }
            }
        }
    }
}

void ModandShowstaffDetail(STAFF *head, int version){
    int error = 0;
    int number = 0;
    char number_char[100] = {0}, index_char[100] = {0}, temp = 0;
    int i = 0, overinput = 0;
    while(1){//输入编号
        i = 0;
        system("cls");
        number = STAFFShowList(head);
        printf("\n\n");
        if(error){
            error = 0;
            Warning_Type;
            printf("\n请输入正确的编号(*返回，ESC返回主菜单)\n");
            Normal_Type;
        }
        printf("请输入要修改查看员工的编号：（*返回 ESC返回主菜单）");
        while(1){
            fflush(stdin);
            temp = getch();
            fflush(stdin);
            switch(temp){
                case 27:
                    flag_come_back = 1;
                    return;
                case '*':
                    return;
                case '\r':
                    overinput = 1;
                    break;
                case '\b':
                    if(i>0){
                        printf("\b \b");
                        index_char[--i] = 0;
                    }
                    break;
                default:
                    if(temp<='9'&&temp>='0'){
                        if(i<=10){
                            index_char[i++] = temp;
                            printf("%c", temp);
                            break;
                        }
                    }
            }
            if(overinput == 1){
                overinput = 0;
                break;
            }

        }
        float index = atof(index_char);
        ClearArr_char(index_char, 100, 0);
        if (index > number||index == 0)
        {
            error = 1;
            continue;
        }
        while(1){
            char choose = 0;
            STAFF *node = Find_Vip(head, index);
            ShowSTAFFadmin(node);
            if(node->position==1){
                printf("\n\n1.修改密码 2.修改手机号 3.修改职位 4.查看业绩 *返回 ESC返回主菜单");
                Warning_Type;
                printf("\n注意：修改职位会删除员工业绩请谨慎选择");
                Normal_Type;
            }
            else{
                printf("\n\n1.修改密码 2.修改手机号 3.修改职位 *返回 ESC返回主菜单");
            }
            int flag_back = 0;
            while(1){// 输入
                fflush(stdin);
                choose = getch();
                fflush(stdin);
                switch(choose){
                    case '1' :
                        system("cls");
                        if(Register_password(node))
                            Operationok(1);
                        break;
                    case '2':
                        system("cls");
                        if(Register_tel(node,head))
                            Operationok(1);
                        break;
                    case '3':
                        system("cls");
                        if(version==1){
                            STAFFPosition(node);
                        }
                        else{
                            system("cls");
                            toxy(Lstrict, Upstrict);
                            printf("抱歉！你无权限");
                        }
                        break;
                    case '*':
                        flag_back = 1;
                        break;
                    case 27:
                        flag_come_back = 1;
                        return;
                    default:
                        if(choose == '4'){
                            ConsumeRecord(node, head_form);
                            break;
                        }
                        else{
                            continue;
                        }
                }
                break;
            }
            if(flag_back){
                flag_back = 0;
                break;
            }
            if(flag_come_back){
                return;
            }
        }
    }
}

void ShowSTAFFadmin(STAFF *node){
    system("cls");
    printf("注册时间：%04d-%02d-%02d %02d:%02d\n姓名：%s\n电话：%s\n密码：%s\n工资：%.2f\n职位：",
           node->id.year + 1900, node->id.month + 1, node->id.day, node->id.hour + 8, node->id.min,
           node->name, node->tel, node->password, node->restmoney);
    switch(node->position){
        case 1:
            printf("服务员\n");
            break;
        case 2:
            printf("店长\n");
            break;
        case 3:
            printf("清洁工\n");
            break;
        case 4:
            printf("实习生\n");
            break;
    }
}




void STAFFUSERshow(VIP *node){
    if(node==NULL)
        return;
    system("cls");
    printf("ESC 首页\n *  返回上一级");
    toxy(28, 8);
    printf("     时尚泡吧会员系统");
    toxy(22,9);
    printf("-------------------------------------");
    toxy(Lstrict, Upstrict+1);
    printf("工号：", node->id.id);
    toxy(Lstrict, Upstrict + 2);
    printf("姓名：%s",node->name);
    toxy(Lstrict, Upstrict+3);
    printf("电话：%s",node->tel);
    toxy(Lstrict, Upstrict+4);
    printf("工资：%.2f",node->restmoney);
    toxy(Lstrict, Upstrict+5);
    printf("职位：");
    switch (node->position)
    {
        case 1:
            printf("服务员");
            break;
        case 2:
            printf("店长");
            break;
        case 3:
            printf("清洁工");
            break;
        case 4:
            printf("实习生");
            break;
        default:
            break;
    }
    if(node->position==1){
        toxy(22,Upstrict+6);
        printf("-------------------------------------");
        toxy(Lstrict, Upstrict+7);
        printf("| 1.修改登录密码 |");
        toxy(Lstrict, Upstrict+9);
        printf("| 2.修改手机号 |");
        toxy(Lstrict, Upstrict+11);
        printf("| 3.查看业绩 |");
        toxy(Lstrict, Upstrict+13);
        printf("| 4.点餐 |");
        toxy(Lstrict, Upstrict+15);
        printf("| 5.会员注册 |");
        toxy(Lstrict, Upstrict+17);
        printf("| 6.会员登录 |");
    }
    else{
        printf("\n请输入操作：1. 修改登录密码 2.修改手机号 *返回 ESC.退出登录\n");
        toxy(22,Upstrict+6);
        printf("-------------------------------------");
        toxy(Lstrict, Upstrict+7);
        printf("| 1.修改登录密码 |");
        toxy(Lstrict, Upstrict+9);
        printf("| 2.修改手机号 |");
    }
}

//user会员操作
void STAFFoperationUSER(STAFF *node){
    char operation = 0;
    if(node==NULL)
        return;
    while(1){
        STAFFUSERshow(node);
        fflush(stdin);
        operation = getch();
        fflush(stdin);
        switch(operation){
            case 27:
                flag_come_back = 1;
                return;
            case '1':
                system("cls");
                if(Register_password(node)){
                    Operationok(1);
                }
                break;
            case '2':
                system("cls");
                if(Register_tel(node,head_staff)){
                    Operationok(1);
                }
                break;
            case '*':
                return;
            default:
                //员工是服务员，并且
                if(operation=='4'&&node->position==1){
                    //选择是否是会员
                    while(1){
                        system("cls");
                        node_STAFF = node;
                        printf("ESC 首页\n *  返回上一级");
                        toxy(Lstrict, Upstrict);
                        printf("点餐顾客是会员吗？（Y/N）");
                        VIP *result = NULL;//result 0 登录失败 node成功
                        int YN = inputYorN();
                        if(YN == -1)
                            break;
                        if(YN){//是否会员
                            result = VIPlogin(head_vip);
                            if(!result){
                                continue;
                            }
                            else{
                                node_VIP = result;
                            }
                        }
                        Order();
                        break;
                    }
                }
                if(operation=='3'&&node->position==1){
                    ConsumeRecord(node, head_form);
                }
                if(operation=='5'&&node->position==1){
                    system("cls");
                    head_vip = VIPRegister(head_vip);
                }
                if(operation=='6'&&node->position==1){
                    VIP *temp_node=VIPlogin(head_vip);
                    VIPoperationUSER(temp_node);
                }
                break;
        }
        if(flag_come_back){
            flag_come_back = 1;
            return;
        }
    }
}
//Y返回1N返回0ESC返回-1
int inputYorN(){
    char choose = 0;
    while(1){
        choose = getch();
        switch(choose){
            case 'Y':
                return 1;
            case 'N':
                return 0;
            case '*':
                return -1;
            case 27:
                flag_come_back = 1;
                return -1;
            default:
                continue;
        }
    }
}

//对VIP的ID（时间）进行排序 1正序 0倒序
void V_order_char(int version){
    VIP *p = (VIP *)malloc(sizeof(VIP));
    p->next = head_vip;
    head_vip = p;//哨兵结点
    if(head_vip==NULL)
        return;
    VIP *cur = head_vip->next, *last = head_vip->next->next,*pre = head_vip;
    if(version == 1){
        int flag = 1;
        while(flag){
            flag = 0;
            cur = head_vip->next;
            last = head_vip->next->next;
            pre = head_vip;
            while(last!=NULL){
                if(strcmp(cur->id.id,last->id.id)>0){
                    VIP *temp;
                    flag = 1;
                    cur->next = last->next;
                    last->next = cur;
                    pre->next = last;
                    temp = cur;
                    cur = last;
                    last = temp;
                }
                pre = cur;
                cur = last;
                last = last->next;
            }
        }
    }

    else{
        int flag = 1;
        while(flag){
            flag = 0;
            cur = head_vip->next;
            last = head_vip->next->next;
            pre = head_vip;
            while(last!=NULL){
                if(strcmp(cur->id.id,last->id.id)<0){
                    VIP *temp;
                    flag = 1;
                    cur->next = last->next;
                    last->next = cur;
                    pre->next = last;
                    temp = cur;
                    cur = last;
                    last = temp;
                }
                pre = cur;
                cur = last;
                last = last->next;
            }
        }
    }
    p = head_vip;
    head_vip = head_vip->next;
    free(p);
}
//对VIP的余额进行排序 1正序 0倒序
void V_order_float(int version){
    VIP *p = (VIP *)malloc(sizeof(VIP));
    p->next = head_vip;
    head_vip = p;//哨兵结点
    if(head_vip==NULL)
        return;
    VIP *cur = head_vip->next, *last = head_vip->next->next,*pre = head_vip;
    if(version == 1){
        int flag = 1;
        while(flag){
            flag = 0;
            cur = head_vip->next;
            last = head_vip->next->next;
            pre = head_vip;
            while(last!=NULL){
                if(cur->restmoney>last->restmoney){
                    VIP *temp;
                    flag = 1;
                    cur->next = last->next;
                    last->next = cur;
                    pre->next = last;
                    temp = cur;
                    cur = last;
                    last = temp;
                }
                pre = cur;
                cur = last;
                last = last->next;
            }
        }
    }

    else{
        int flag = 1;
        while(flag){
            flag = 0;
            cur = head_vip->next;
            last = head_vip->next->next;
            pre = head_vip;
            while(last!=NULL){
                if(cur->restmoney<last->restmoney){
                    VIP *temp;
                    flag = 1;
                    cur->next = last->next;
                    last->next = cur;
                    pre->next = last;
                    temp = cur;
                    cur = last;
                    last = temp;
                }
                pre = cur;
                cur = last;
                last = last->next;
            }
        }
    }
    p = head_vip;
    head_vip = head_vip->next;
    free(p);
}

//对staff的ID（时间）进行排序 1正序 0倒序
void F_order_char(int version){
    VIP *p = (VIP *)malloc(sizeof(VIP));
    p->next = head_staff;
    head_staff = p;//哨兵结点
    if(head_staff==NULL)
        return;
    VIP *cur = head_staff->next, *last = head_staff->next->next,*pre = head_staff;
    if(version == 1){
        int flag = 1;
        while(flag){
            flag = 0;
            cur = head_staff->next;
            last = head_staff->next->next;
            pre = head_staff;
            while(last!=NULL){
                if(strcmp(cur->id.id,last->id.id)>0){
                    VIP *temp;
                    flag = 1;
                    cur->next = last->next;
                    last->next = cur;
                    pre->next = last;
                    temp = cur;
                    cur = last;
                    last = temp;
                }
                pre = cur;
                cur = last;
                last = last->next;
            }
        }
    }

    else{
        int flag = 1;
        while(flag){
            flag = 0;
            cur = head_staff->next;
            last = head_staff->next->next;
            pre = head_staff;
            while(last!=NULL){
                if(strcmp(cur->id.id,last->id.id)<0){
                    VIP *temp;
                    flag = 1;
                    cur->next = last->next;
                    last->next = cur;
                    pre->next = last;
                    temp = cur;
                    cur = last;
                    last = temp;
                }
                pre = cur;
                cur = last;
                last = last->next;
            }
        }
    }
    printf("ExplicitFlag");
    p = head_staff;
    head_staff = head_staff->next;
    free(p);
}

//对staff的工资进行排序 1正序 0倒序
void F_order_float(int version){
    VIP *p = (VIP *)malloc(sizeof(VIP));
    p->next = head_staff;
    head_staff = p;//哨兵结点
    if(head_staff==NULL)
        return;
    VIP *cur = head_staff->next, *last = head_staff->next->next,*pre = head_staff;
    if(version == 1){
        int flag = 1;
        while(flag){
            flag = 0;
            cur = head_staff->next;
            last = head_staff->next->next;
            pre = head_staff;
            while(last!=NULL){
                if(cur->restmoney>last->restmoney){
                    VIP *temp;
                    flag = 1;
                    cur->next = last->next;
                    last->next = cur;
                    pre->next = last;
                    temp = cur;
                    cur = last;
                    last = temp;
                }
                pre = cur;
                cur = last;
                last = last->next;
            }
        }
    }

    else{
        int flag = 1;
        while(flag){
            flag = 0;
            cur = head_staff->next;
            last = head_staff->next->next;
            pre = head_staff;
            while(last!=NULL){
                if(cur->restmoney<last->restmoney){
                    VIP *temp;
                    flag = 1;
                    cur->next = last->next;
                    last->next = cur;
                    pre->next = last;
                    temp = cur;
                    cur = last;
                    last = temp;
                }
                pre = cur;
                cur = last;
                last = last->next;
            }
        }
    }
    p = head_staff;
    head_staff = head_staff->next;
    free(p);
}

void Add_VIPOrders(VIP *node,Complete_Form * cf){
    int len=0;
    for (; node->orders[len];len++);
    node->orders[len] = cf->id.id;
}

void Inventory_Manage()  //库存管理页面
{
    system("cls");  //清屏
    char t;  //存储用户选择
    toxy(12,6);
    printf("请选择需要进行的操作:\n");
    printf("\t\t按下esc键返回主页面\n");
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
        case '0':
            ROOT();
            break;
        case 27:
            Default();
            break;
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
    printf("\t\t按下esc键返回主页面\n");
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
        case 27:
            Default();
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
    printf("\t\t按下esc键返回主页面\n");
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
        case 27:
            Default();
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
    char b[3];
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
            ShowSale();
        }

        b=Check_Number(a,12,2);


        if(b==-1||b==-2)
        {
            printf("请输入数字0-12！\n");
            Sleep(2);
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
            ShowSale();
        }

        b=Check_Number(a,4,1);


        if(b==-1||b==-2)
        {
            printf("请输入数字0-4！\n");
            Sleep(3);
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
            ShowSale();
        }

        b=Check_Number(a,1,1);


        if(b==-1||b==-2)
        {
            printf("请输入数字0-1！\n");//发现一个bug，输入错误之后要按两下以上0才可以退出。
            Sleep(2);
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

void Default()
{
    while(1)
    {
        flag_come_back = 0;
        system(Normal_Color);;  //设置背景颜色与字体颜色
        HideCursor(0); //隐藏光标
        if(ExplicitFlag)  //打印交互界面
        {
            system("cls");
            toxy(28,8);
            printf("    欢迎光临时尚泡吧! ");
            toxy(26,9);
            printf("-----------------------------");
            toxy(26,10);
            printf("|                           |");
            toxy(26,11);
            printf("|");
            toxy(35,11);
            printf("1.非会员点餐");
            toxy(54,11);
            printf("|");
            toxy(26,12);
            printf("|                           |");
            toxy(26,13);
            printf("|");
            toxy(35,13);
            printf("2.会员登陆");
            toxy(54,13);
            printf("|");
            toxy(26,14);
            printf("|                           |");
            toxy(26,15);
            printf("|");
            toxy(35,15);
            printf("3.会员注册");
            toxy(54,15);
            printf("|");
            toxy(26,16);
            printf("|                           |");
            toxy(26,17);
            printf("|");
            toxy(35,17);
            printf("4.管理员登陆");
            toxy(54,17);
            printf("|");
            toxy(26,18);
            printf("|                           |");
            toxy(26,19);
            printf("|");
            toxy(35,19);
            printf("5.老板登陆");
            toxy(54,19);
            printf("|");
            toxy(26,20);
            printf("|                           |");
            toxy(26,21);
            printf("|");
            toxy(35,21);
            printf("ESC.退出");
            toxy(54,21);
            printf("|");
            toxy(26,22);
            printf("|                           |");
            toxy(26,23);
            printf("-----------------------------");
        }
        char choose = 0;
        VIP *node_main;
        fflush(stdin);
        choose = getch();
        fflush(stdin);
        switch(choose){
            case '1':
                system("cls");
                Order();
                break;
            case '2':
                system("cls");
                node_main = VIPlogin(head_vip);
                VIPoperationUSER(node_main);
                break;
            case '3':
                system("cls");
                head_vip = VIPRegister(head_vip);
                break;
            case '4':
                system("cls");
                node_main = VIPlogin(head_staff);
                STAFFoperationUSER(node_main);
                break;
            case '5':
                ROOT();
                break;
            case 27:
                return;
            default:
                ExplicitFlag = 0;
                Default();
                ExplicitFlag = 1;
                return;
        }
    }
}

void ROOT()
{
    if(RootLogin()==0)
    {
        return;
    }
    while(1)
    {
        system("cls");
        if(flag_come_back)
            return;
        char choose = 0;
        printf("ESC 首页\n *  返回上一级");
        toxy(28, 8);
        printf("     时尚泡吧ROOT系统");
        toxy(22,9);
        printf("-------------------------------------");
        toxy(Lstrict + w, Upstrict);
        printf("| 1.员工管理 |");
        toxy(Lstrict + w, Upstrict+2);
        printf("| 2.库存管理 |");
        toxy(Lstrict + w, Upstrict+4);
        printf("| 3.商品管理 |");
        toxy(Lstrict + w, Upstrict+6);
        printf("| 4.销量管理 |");
        toxy(Lstrict + w, Upstrict+8);
        printf("| 5.会员管理 |");
        choose = getch();
        switch (choose)
        {
            case '1':
                system("cls");
                STAFFadminoperation();
                break;
            case '2':
                system("cls");
                Inventory_Manage();
                break;
            case '3':
                system("cls");
                ManageGoods();
                break;
            case '4':
                system("cls");
                ShowSale();
                break;
            case '5':
                system("cls");
                VIPadminoperation();
                break;
            case 27:
                return;
            case '*':
                return;
        }
    }
}


void NotChangeWindowSize(){
    HWND hWnd = GetConsoleWindow(); //获得cmd窗口句柄
    RECT rc;
    GetWindowRect(hWnd, &rc); //获得cmd窗口对应矩形

    //改变cmd窗口风格
    SetWindowLongPtr(hWnd,
                     GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX & ~WS_MINIMIZEBOX);
    //因为风格涉及到边框改变，必须调用SetWindowPos，否则无效果
    SetWindowPos(hWnd,
                 NULL,
                 rc.left,
                 rc.top,
                 rc.right - rc.left, rc.bottom - rc.top,
                 NULL);
}


int RootLogin()
{
    int error = 0;
    while(1)
    {
        system("cls");
        char temp_password[21]={0};
        int count = 0;
        int over = 0;
        char word=0;
        char temp[100] = "请输入密码：";
        printf("ESC 首页\n *  返回上一级");
        toxy(28, 8);
        printf("     时尚泡吧登陆系统");
        toxy(22,9);
        printf("-------------------------------------");
        toxy(Lstrict, Upstrict);
        printf("请输入密码：");
        if(error)
        {
            Warning_Type;
            toxy(Lstrict, Upstrict+1);
            printf("密码错误");
            Normal_Type;
            error = 0;
        }
        toxy(Lstrict+strlen(temp),Upstrict );
        while(1)
        {
            word = getch();
            switch(word){
                case 27:
                    flag_come_back = 1;
                    return 0;
                case '*':
                    flag_come_back = 1;
                    return 0;
                case '\b':
                    if(count>0){
                        temp_password[--count] = 0;
                        printf("\b \b");
                    }
                    break;
                case '\r':
                    over = 1;
                    break;
                default:
                    if(word>31){
                        printf("%c", word);
                        temp_password[count++] = word;
                    }
            }
            if(over)
                break;
        }
        if(strcmp(bosspassword,temp_password)==0)
        {
            return 1;
        }
        else
        {
            error = 1;
        }
    }
}

void INIT()  //初始化，从文件中读入数据创建各种链表
{
    system("mode con cols=160 lines=50");
    NotChangeWindowSize();
    setbuf(stdout, NULL);
    head_beverage = Create_BeverageLinkList();
    head_form = Create_FormLinkList();
    head_material = Create_MaterialLinkList();
    head_staff = STAFFReadFile();
    head_vip = VIPReadFile();
}

void SAVE()  //保存程序执行过程中对数据链表的改动，并重新写入文件
{
    Rewrite_FormData();  //将更新后的订单链表数据重新写入文件
    Rewrite_BeverageData();  //将更新后的饮品链表数据重新写入文件
    Rewrite_MaterialData();  //将更新后的原材料链表数据重新写入文件
    VIPInputFile(head_vip);
    STAFFInputFile(head_staff);
}


int main()
{
    INIT();
    Default();
    SAVE();
    system("pause");
    return 0;
}

