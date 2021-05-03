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

/* �궨�� */
#define Max_Form_Num 20  //ÿһ�����������е������
#define Max_User_Enter 1000  //�û����������ַ�������
#define Max_Beverage_SourceNum 10 //ÿ����Ʒ���ʹ�õ�ԭ���ϵ�������Ŀ
#define Total_Material_Num  18  //ԭ���ϵ�������
#define Normal_Type setColor(4, 15);//�׵׺���
#define Warning_Type setColor(2, 15);//�׵�����
#define Normal_Color "color f4"
#define Lstrict 30 //���־�������
#define Upstrict 10 //���ֵĸ߶�����
#define w 3 //����ˮƽƫ��ֵ
/* �ṹ�� */
//��Ʒ(beverage)
typedef struct Beverage
{
    int id;  //��Ʒ��Ӧ��id(��1��ʼ)
    char name[20];  //��Ʒ��
    int material_num;  //ԭ���ϵ���������
    float price;  //�۸�
    char materials[Max_Beverage_SourceNum];  //ԭ���� (materials)
    struct Beverage* next;
}Beverage;

//ԭ����
typedef struct Material
{
    int id;  //ԭ���϶�Ӧ��id(��1��ʼ)
    char name[20];  //ԭ������
    float consume;  //ÿ��ʹ�����ĵ�ԭ������(��λΪkg)
    float inventory;  //�ò��ϵĿ����
    struct Material* next;
}Material;

//ID
typedef struct Form_ID  //ÿ������������ID��Ϊһ���ṹ�壬�������ʱ����Ϣ������������������
{
    int id;   //��ˮ��
    int year;  //����Ϊ��������ʱ��ʱ��
    int month;
    int day;
    int hour;
    int min;
}Form_ID;

//��������
typedef struct Single_Form
{
    int beverage;  //��Ʒ����(Ϊ��Ʒ�����id)
    int ingredients;  //С��(ingredient)(12345�ֱ�Ϊ����,Ҭ��,�춹,����,����)
    int temperature;  //�¶� (1--�ӱ���2--���£�3--΢�ȣ�4--��)
    int drink_place;  //���õص�
    float total_price;  //�����ܼ�
}Single_Form;

//��������
typedef struct Complete_Form
{
    Single_Form forms[Max_Form_Num];
    int number;  //��һ���еķ���
    Form_ID id;  //��ˮ��(��ʱ�涨�����еĵ�����������ˮ�����)
    float total_price;  //�����ܼ�
    int is_member;  //�ö������������Ƿ��ǻ�Ա(0Ϊ�ǻ�Ա��1Ϊ��Ա)
    int pay_way;  //���ʽ(1.�ֽ�  2.����֧��  3.��Ա��֧��)
    struct Complete_Form *next;
}Complete_Form;

//ÿ����Ա��ID��Ϊһ���ṹ�壬�������ʱ����Ϣ������������������
typedef struct ID_VIP
{
    char id[20];//200105301526
    int year;
    int month;
    int day;
    int hour;
    int min;
}IDVIP;

//vip�Ľṹ��
typedef struct vip {
    IDVIP id;
    char name[25];
    char tel[25];
    int position;//1 ����Ա 2 �곤 3 ��๤ 4 ʵϰ��
    float restmoney;
    char password[25];
    int loss;           // 0 for loss 1 for existing
    int orders[200];  //�������飬���ö���id
    struct vip *next;
}VIP;
//����Ա�������ݽṹ����Ա�����ƣ���������̳��˻�Ա�����ݽṹ
typedef struct vip STAFF;

/* ȫ�ֱ��� */
int Total_Form_Num = 0;   //���붩���������ݵ�ʱ�����и��¸�ֵ
int Total_Beverage_Num = 0;  //��Ʒ��������Ŀ(������Ʒ�������ݵ�ʱ�����и��¸�ֵ)
int flag_come_back;  //ȫ�ֱ��� �˳� �����ɽ��
int auto_back;  //ȫ�ֱ��� ��Ϊһϵ�ж������,�������˵ı�־
int month[13][30];  //����ֵ���ʼ��Ϊ0�𣿻ᡣӦ�ö���13����Ϊ��0�в������κ�������
int season[5][30];  //Ӧ�ö���5����Ϊ��0�в������κμ��ڡ�
int year[2][30];
int judgeFormOk;
int ExplicitFlag = 1;  //����������뵼�µ���˸����
char bosspassword[21] = "11";
// ʵ�ַ�ҳ���ܵ�ȫ�ֱ���
int now = 0;
int all=0;
int page = 0;
int index_vip=1;
int single;
int rest;

//ԭ���ϵ�������
typedef struct Consumption
{
    int id;  //��Ӧԭ���ϵ�id
    float consumption;  //��������
    char name[20];  //ԭ������
    struct Consumption* pre;  //ǰ��
    struct Consumption* next;  //���
}Consumption;

/* ȫ��ָ����� */
Beverage* head_beverage;  //��Ʒ������Ϣ����
Complete_Form* head_form;  //������Ϣ����
Material* head_material;  //ԭ������Ϣ����
Consumption* head_consumption;  //ԭ����������Ϣ����
STAFF *head_staff;  //Ա����Ϣ����
VIP *head_vip;  //vip��Ϣ����
VIP *node_VIP;  //��ǰ�㵥��VIP��Ӧ�Ľ��(�����ǰ�㵥�ķ�vip,����ֵΪNULL)
STAFF *node_STAFF;  //��ǰ���в����ĵ�Ա��Ӧ�Ľ��(�����㵥ģʽʱ���޵�ԱʱֵΪNULL)

//
void Default();  //��ӭ����
void INIT();  //��ʼ�������ļ��ж������ݴ�����������
void SAVE();  //�������ִ�й����ж���������ĸĶ���������д���ļ�
Complete_Form * Create_FormLinkList();  //����������Ϣ����(���ļ��е������ж�����Ϣ),����ָ���������λ����ָ��
Complete_Form* Find_Form(int x,Complete_Form* h);  //�ҵ�hΪ��λ���Ķ���������id.idΪx�Ľ�㣬����ָ��ý���ָ��
Beverage* Create_BeverageLinkList();  //������Ʒ���������(���ļ��е�����Ʒ������Ϣ),����ָ���������λ����ָ��
Beverage* Find_Beverage(int x,Beverage *h);  //�ҵ�idΪx����Ʒ���࣬����ָ�����ָ��
Material* Create_MaterialLinkList();  //����ԭ�������������(���ļ��е�����Ʒ������Ϣ),����ָ���������λ����ָ��
Material* Find_Material(int x,Material *h);  //�ҵ�idΪx��ԭ��������,����ָ�����ָ��
int Show_Beverages(Beverage *h);  //չʾ��Ʒ�����б�
void Choose_FormNum(Complete_Form* cf);  //ѡ��cfָ�򶩵��ı���
void Choose_Beverage(Complete_Form* cf,int i);  //��cfָ�򶩵��ĵ�i�ֽ���ѡ����Ʒ����
void Choose_Ingredients(Complete_Form* cf,int i);  //��cfָ��Ķ����ĵ�i�ݽ���ѡ����
void Choose_Temperature(Complete_Form* cf,int i);  //��cfָ��Ķ����ĵ�i�ݽ���ѡ���¶�
void Choose_Drinkplace(Complete_Form* cf,int i);  //��cfָ��Ķ����ĵ�i�ݽ���ѡ����Ʒ�ص�
void Choose_Payway(Complete_Form* cf);  //��cfָ��Ķ���ѡ��֧����ʽ(����ǻ�Ա������������ѡ���Ա��֧��)
void Get_CpTotalprice(Complete_Form* cf);  //�������������ļ۸�Ϊ��total_price���Ը�ֵ,�����������������ܼ�
void Output_Form(Complete_Form* cf);  //��cfָ��Ķ�����Ϣ������ļ�
void Create_ID(Complete_Form* cf);  //��cfָ��Ķ�����id���ݵ�ǰ��ʱ�������и�ֵ
Material* Consume_Inventory(Complete_Form* cf,int i);  //����cfָ�򶩵��е�i����Ʒ��Ҫ��ԭ���Ͽ��
void Add_Form(Complete_Form*cf,Complete_Form* h);  //��cfָ��Ķ�����ӵ�����������
void Inventory_Manage();  //������ҳ��
void Add_Inventory();  //��ӿ��
int Show_Inventory();  //�鿴���п���ʣ����,���ؿ��������Ŀ
void Show_InventoryConsumption();  //�鿴�������
void Inventory_GetAdded(Material *m);  //�����û���ѡ������ӿ��
void Show_Inventory_ByConsumption();  //����ԭ������������ԭ���ϵ����Ľ�������չʾ
void ChangeShowWay(Consumption *head,Consumption *tail,int status);  //����status��ֵ��ѡ���ǰ���������С���󣬻��ǰ��������Ӵ�С����ʾԭ���ϵ���������(status=1ʱ���С����,0ʱ�Ӵ�С)
void Show_Inventory_ById();  //����ԭ���ϵ�idչʾ�����ϵ�������
Consumption* Create_ConsumptionLinkList();  //����ԭ�������ĵ�˫����������ʱ�����в�������(�������Ӵ�С)
Consumption* Find_Consumption(int x,Consumption* h);  //��ԭ�������ĵ�˫���������ҵ�idΪx�Ľ�㲢����ָ�����ָ��
void Rewrite_FormData();  //�����º�Ķ���������������д���ļ���
void Rewrite_BeverageData();  //�����º����Ʒ������������д���ļ���
void Rewrite_MaterialData();  //�����º��ԭ����������������д���ļ���
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
int pagef(VIP *head,int begin,int end);//ʵ�ַ�ҳ����
int VIPShowList(VIP *head);  //��ʾ���л�Ա��Ϣ�����ػ�Ա����
void VIPdetele(VIP *head);  //ɾ����Ա
VIP *Exedelete(VIP *head, int index);  //��Աɾ������--ִ�л�Աɾ��
void ModandShowDetail(VIP *head);  //VIPadminoperation������--�޸Ļ�Ա��Ϣ
void ShowVIPadmin(VIP *node);  //��ʾ��һ��Ա��ϸ��Ϣ
VIP *Find_Vip(VIP *head, int index);  //Ѱ��vip
/*
Ա���ļ��Ķ�ȡ��ʽ��
_index,name,tel,money,password,loss(bool),position,(if(position)=1:orderlist....)
index,name,tel,money,password,loss(bool),position,(if(position)=1:orderlist....)
(�»���'_'����ո�)
*/
STAFF *STAFFReadFile();
void STAFF__init__(STAFF *node);  //Ա������ʼ�����������ڱ����
/*
Ա���ļ���д���ʽ��
_index,name,tel,money,password,loss(bool),position,(if(position)=1:orderlist....)
index,name,tel,money,password,loss(bool),position,(if(position)=1:orderlist....)
(�»���'_'����ո�)
*/
void STAFFInputFile(STAFF *head);
int STAFFPosition(STAFF *node);  //ѡ��Ա����ְλ
int STAFFmoney(VIP *node);  //ѡ��Ա���Ĺ���
STAFF *STAFFadd(STAFF *head);  //���Ա��
int STAFFShowList(STAFF *head);  //��ʾԱ���б�
void ShowSTAFFadmin(STAFF *node);  //��ʾ����Ա����
void ModandShowstaffDetail(STAFF *head,int version); //��Ա���Ĳ������޸�
void STAFFdetele(STAFF *head);  //Ա��ɾ��
void VIPadminoperation();  //admin�Ի�Ա�Ĳ�����API��
void STAFFadminoperation();  //root��Ա���Ĳ������� ��API��
VIP *VIPRegister(VIP *VIPlisthead);
VIP *VIPlogin(VIP *VIPheadlist);
void STAFFoperationUSER(STAFF *node);
void VIPoperationUSER(VIP *node);
void Default();
void ROOT();
int RootLogin();
//��������
void toxy(int x,int y);//������ƶ���x��y���괦
void HideCursor(int x);//���ع��
char *HidePassword();//��������
int Check_Number(char ch[],int maxnum,int maxlen);  //��������Ƿ�Ϲ�(�Ϲ��Ҳ�����maxnu���ض�Ӧ������ֵ���Ϲ浫����max����-1�����Ϲ淵��-2)
void color(int x);  //����������ɫ

//����ʵ����
void toxy(int x,int y)//������ƶ���x��y���괦
{
    COORD pos={x,y};
    HANDLE Out = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(Out,pos);
}

void HideCursor(int x)//���ع�� ,��xΪ0ʱ�����أ�Ϊ1ʱ����ʾ
{
    CONSOLE_CURSOR_INFO cursor_info ={1,x};
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE),&cursor_info);
}

int Check_Number(char ch[],int maxnum,int maxlen)  //�������(������)�Ƿ�Ϲ�(�Ϲ��Ҳ�����max���ض�Ӧ������ֵ���Ϲ浫����max����-1�����Ϲ淵��-2)
{  //0001  1s23  01s  s  ��Ϊ���淶����  0Ϊ�Ϲ�����
    int i;  //ѭ������
    int len = strlen(ch);  //���鳤��
    int sum=0;  //��¼�ַ��������Ӧ��ֵ
    for(i=0;ch[i]!='\0';i++)  //����Ƿ�������
    {
        int num;
        if(ch[i]<'0'||ch[i]>'9')
        {
            return -2;  //�з��������룬���Ϲ�
        }
        num = ch[i]-'0';
        sum+=num*pow(10,len-1-i);
    }
    if(strlen(ch)>maxlen)
    {
        return -1;
    }
    if(ch[0]=='0'&&len>1)  //����0001���಻�Ϲ�����
    {
        return -2;
    }
    if(sum<=maxnum)
    {
        return sum;
    }
    else
    {
        return -1;  //������ֳ������ֵ
    }
}

Complete_Form* Create_FormLinkList()  //ͨ�������ļ����붩�����ݣ���������������ָ��������λ����ָ��
{
    Complete_Form *H,*p;  //ͷָ��
    H = (Complete_Form*)malloc(sizeof(Complete_Form));  //Ϊ��λ��������ڴ�
    H->next = NULL;
    p = H;
    FILE* fp;
    fp = fopen("..\\Form_Data.txt", "r");  //�򿪴����Ʒ���ݵ��ļ�
    if(fp==NULL)  //������ʾ
    {
        printf("Can't open the file:%s","Form_data.txt");
    }
    else
    {
        while(!feof(fp))
        {
            int x;  //�洢�����ַ�
            Complete_Form *r;
            r = (Complete_Form*)malloc(sizeof(Complete_Form));  //Ϊ�½�������ڴ�
            fscanf(fp,"%d,%d-%d-%d-%d:%d,%d",&r->id.id,&r->id.year,&r->id.month,&r->id.day,&r->id.hour,&r->id.min,&r->number);  //�������ݲ��Ҵ�ŵ��½����
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

Complete_Form* Find_Form(int x,Complete_Form* h)  //�ҵ�hΪ��λ���Ķ���������id.idΪx�Ľ�㣬����ָ��ý���ָ��
{
    Complete_Form *p=h;
    while(p->next!=NULL&&p->next->id.id!=x)
    {
        p=p->next;
    }
    return p->next;
}

Beverage* Create_BeverageLinkList()  //����beverageData.txt�е����ݴ������ڴ洢��Ʒ������������ظ������ͷָ�루ͷָ����������
{
    Beverage *H,*p;  //ͷָ��
    H = (Beverage*)malloc(sizeof(Beverage));
    H->next = NULL;
    p = H;  //����p��������������H��������(p��ʾ�����ĩβ���)
    FILE* fp;
    fp = fopen("..\\beverageData2.txt", "r");  //�򿪴����Ʒ���ݵ��ļ�
    if(fp==NULL)  //������ʾ
    {
        printf("Can't open the file:beverageData2.txt");
    }
    else
    {
        while(!feof(fp))
        {
            Beverage *r;
            r = (Beverage*)malloc(sizeof(Beverage));  //Ϊ�½�������ڴ�
            fscanf(fp,"%d,%d,%[^,],%f,",&r->id,&r->material_num,r->name,&r->price);  //�������ݲ��Ҵ�ŵ��½����
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

Beverage* Find_Beverage(int x,Beverage *h)  //�ҵ�hΪ��λ����������idΪx�Ľ�㣬����ָ��ý���ָ��
{
    Beverage *p=h;
    while(p->next!=NULL&&p->next->id!=x)
    {
        p=p->next;
    }
    return p->next;
}

Material* Create_MaterialLinkList()  //����ԭ�������������(���ļ��е�����Ʒ������Ϣ),����ָ���������λ����ָ��
{
    Material *H,*p;  //ͷָ��
    H = (Material*)malloc(sizeof(Material));
    H->next = NULL;
    p = H;  //����p��������������H��������(p��ʾ�����ĩβ���)
    FILE* fp;
    fp = fopen("..\\materialData.txt", "r");  //�򿪴����Ʒ���ݵ��ļ�
    if(fp==NULL)  //������ʾ
    {
        printf("Can't open the file!");
    }
    else
    {
        while(!feof(fp))
        {
            Material *r;
            r = (Material*)malloc(sizeof(Material));  //Ϊ�½�������ڴ�
            fscanf(fp,"%d,%[^,],%f,%f",&r->id,r->name,&r->consume,&r->inventory);  //�������ݲ��Ҵ�ŵ��½����
            r->next = NULL;
            p->next = r;
            p = r;
        }
        fclose(fp);
    }
    return H;
}

Material* Find_Material(int x,Material* h)  //�ҵ�hΪ��λ����������idΪx�Ľ�㣬����ָ��ý���ָ��
{
    Material *p=h;
    while(p->next!=NULL&&p->next->id!=x)
    {
        p=p->next;
    }
    return p->next;
}


int Show_Beverages(Beverage *h)  //չʾ��Ʒ����(��hָ��������д�ŵ���Ʒ��Ϣ�������Ļ),��������ĳ��ȣ�����Ʒ��������
{
    int num=0;
    int x1=22,x2=34,x3=Lstrict,y=13;  //������ڵ�λ������
    if(h->next==NULL)
    {
        toxy(x1,y);
        printf("������Ʒ...����ϵ����Ա�������");
    }
    else
    {
        toxy(x1,y);
        printf("��Ʒid");
        toxy(x2,y);
        printf("��Ʒ����");
        toxy(x3,y);
        printf("��Ʒ�۸�(?)");
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
        printf("����0����ѡ����");
        y+=1;
        toxy(x1,y);
        printf("������Ʒ��id����ѡ��(���س�ȷ��)��");
    }
    return num;
}

void Order()
{
    Complete_Form* CF;
    CF = (Complete_Form*)malloc(sizeof(Complete_Form));  //��������CF
    if(node_VIP!=NULL)  //��ǰ�û�Ϊ��Ա
    {
        CF->is_member = 1;
    }
    CF->next = NULL;  //��ʼ��ָ����Ϊ��
    Create_ID(CF);
    Choose_FormNum(CF);
    int i = 1;  //����������ʾ��ǰΪ��i��
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

void Choose_FormNum(Complete_Form* cf)  //ѡ��cf��Ӧ�����ı���
{
    system(Normal_Color);;  //���ñ�����ɫ��������ɫ
    system("cls");  //����
    HideCursor(1);  //��ʾ���
    toxy(28,8);
    printf("     ʱ���ݰɵ㵥ϵͳ");
    toxy(22,9);
    printf("-------------------------------------");
    if(node_VIP!=NULL)  //����㵥��Ϊ��Ա�������ʺ�
    {
        toxy(28,10);
        if(cf->id.hour<=12)
        {
            printf("�����,%s",node_VIP->name);
        }
        else if(cf->id.hour>12&&cf->id.hour<=17)
        {
            printf("�����,%s",node_VIP->name);
        }
        else
        {
            printf("���Ϻ�,%s",node_VIP->name);
        }
    }
    toxy(28,11);
    printf("������˵��ı���:");
    toxy(25,13);
    printf("��ʾ��");
    toxy(25,14);
    printf("һ������%d��(���س�ȷ��)",Max_Form_Num);
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
        printf("�������ֳ��������,��1s����������");
        Sleep(1000);
        Choose_FormNum(cf);  //�ݹ�ֱ������Ϲ�
    }
    else if(x==-2||x==0)
    {
        HideCursor(0);
        toxy(25,15);
        printf("�������ݲ��Ϲ棬��1s��������������");
        Sleep(1000);
        Choose_FormNum(cf);  //�ݹ�ֱ������Ϲ�
    }
    return;
}

void Choose_Beverage(Complete_Form* cf,int i)
{
    system(Normal_Color);;  //���ñ�����ɫ��������ɫ
    system("cls");  //����
    HideCursor(1);  //��ʾ���
    int c;  //�洢���ֺϹ���ķ���ֵ
    int Beverage_Num;  //����Ŀǰ��Ʒ������
    char ch[Max_User_Enter];  //�����û�����
    toxy(28,8);
    printf("     ʱ���ݰɵ㵥ϵͳ");
    toxy(22,9);
    printf("-------------------------------------");
    toxy(27,11);
    printf("��ѡ���%d����Ʒ:\n",i);
    Beverage_Num = Show_Beverages(head_beverage);
    fflush(stdin);
    scanf("%s",&ch);
    fflush(stdin);
    c = Check_Number(ch,Beverage_Num,3);
    if(c>0)
    {
        Beverage *q;  //�ҵ���Ʒ��idΪc����Ʒ���࣬��qָ��
        q = Find_Beverage(c,head_beverage);
        cf->forms[i-1].beverage = q->id;  //Ϊ��������Ʒ���ำֵ
        cf->forms[i-1].total_price = q->price;  //Ϊ�����ļ۸񸳳�ֵ��������Ҫ����С�ϵļ۸�
        Choose_Ingredients(cf,i);  //ѡ��С��
        return;
    }
    else if(c==0)
    {
        Order();
        return;
    }
    else if(c==-1)
    {
        printf("\t\t�������ֳ�����Ʒ������,��1s������ѡ��");
        Sleep(1000);  //��ͣ1����ʾ��ʾ��Ϣ
        Choose_Beverage(cf,i);  //��������
    }
    else
    {
        printf("\t\t�������ݲ��Ϲ棬��1s������������Ʒ��Ӧ��id");
        Sleep(1000);  //��ͣ1����ʾ��ʾ��Ϣ
        Choose_Beverage(cf,i);  //��������
    }
    return;
}
/*

��Ӧ����ɫ���

1. 0 = ��ɫ       8 = ��ɫ

1 = ��ɫ       9 = ����ɫ

2 = ��ɫ       10 = ����ɫ

3 = ǳ��ɫ     11 = ��ǳ��ɫ

4 = ��ɫ       12 = ����ɫ

5 = ��ɫ       13 = ����ɫ

6 = ��ɫ       14 = ����ɫ

7 = ��ɫ       15 = ����ɫ

*/
void setColor(unsigned short ForeColor,unsigned short BackGroundColor)

{

    HANDLE handle=GetStdHandle(STD_OUTPUT_HANDLE);//��ȡ��ǰ���ھ��

    SetConsoleTextAttribute(handle,ForeColor+BackGroundColor*0x10);//������ɫ

}

void Choose_Ingredients(Complete_Form* cf,int i)
{
    system(Normal_Color);;  //���ñ�����ɫ��������ɫ
    system("cls");  //����
    HideCursor(0);  //���ع��
    char t;  //����ѡ���Ӧ���ַ�
    toxy(28,8);
    printf("     ʱ���ݰɵ㵥ϵͳ");
    toxy(22,9);
    printf("-------------------------------------");
    toxy(26,11);
    printf("��ѡ���%d����Ҫ��ӵ�С��",i);
    toxy(27,13);
    printf("1.����--1Ԫ");
    toxy(27,14);
    printf("2.Ҭ��--1Ԫ");
    toxy(27,15);
    printf("3.�춹--2Ԫ");
    toxy(27,16);
    printf("4.����--2Ԫ");
    toxy(27,17);
    printf("5.�����С��");
    toxy(27,18);
    printf("0.����ѡ����");
    toxy(27,19);
    printf("Esc.����������");
    fflush(stdin);
    t = getch();
    fflush(stdin);
    switch (t)
    {
        case '1':
            cf->forms[i-1].ingredients = 1;  //��¼�ö�����ӵ�С��
            cf->forms[i-1].total_price += 1.0;  //�����ܼۼ��ϸ�С�ϵļ۸�
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
            printf("����ݶ�Ӧ��Ž���ѡ��");
            Choose_Ingredients(cf,i);  //�ݹ�ֱ������Ϲ�
            return;
    }
    Material* ci_return = Consume_Inventory(cf,i);  //���ݸö�����Ϣ�Կ����С����ġ�
    if(ci_return==NULL)  //������㹻
    {
        Choose_Temperature(cf,i);  //ѡ����Ʒ�¶�
        return;
    }
    else  //ԭ���Ͽ�治�㣬������ʾ�����µ㵥
    {
        toxy(23,21);
        printf("%s��治�㣬����ϵ����Ա�������",ci_return->name);
        toxy(23,22);
        printf("������Զ����ص㵥ҳ��");
        Sleep(2000);
        Order(); //���ص㵥ҳ��
        return;
    }
}

void Choose_Temperature(Complete_Form* cf,int i)
{
    system(Normal_Color);;  //���ñ�����ɫ��������ɫ
    system("cls");  //����
    HideCursor(0);  //���ع��
    char t;
    toxy(28,8);
    printf("     ʱ���ݰɵ㵥ϵͳ");
    toxy(22,9);
    printf("-------------------------------------");
    toxy(26,11);
    printf("��ѡ���%d���������¶�",i);
    toxy(27,13);
    printf("1.�ӱ�");
    toxy(27,14);
    printf("2.����");
    toxy(27,15);
    printf("3.΢��");
    toxy(27,16);
    printf("4.��");
    toxy(27,17);
    printf("0.����ѡ����");
    toxy(27,18);
    printf("Esc.����������");
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
            printf("���벻�Ϲ�,����������");
            Choose_Temperature(cf,i);  //�ݹ�ֱ������Ϲ�
            return;
    }
    Choose_Drinkplace(cf,i);
    return;
}

void Choose_Drinkplace(Complete_Form* cf,int i)
{
    system(Normal_Color);;  //���ñ�����ɫ��������ɫ
    system("cls");  //����
    HideCursor(0);  //���ع��
    char t;
    toxy(28,8);
    printf("     ʱ���ݰɵ㵥ϵͳ");
    toxy(22,9);
    printf("-------------------------------------");
    toxy(26,11);
    printf("��ѡ���%d�������õص�",i);
    toxy(27,13);
    printf("1.��ʳ");
    toxy(27,14);
    printf("2.���(���)");
    toxy(27,15);
    printf("0.����ѡ����");
    toxy(27,16);
    printf("Esc.����������");
//    printf("��ѡ���%d�������õص�\n",i);
//    printf("\t\t����esc��������ҳ��\n");
//    printf("\t\t1.��ʳ\n");
//    printf("\t\t2.���(���)\n");
//    printf("\t\t0.����ѡ����\n");
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
            Order();  //����ѡ����
            return;
        case 27:
            Default();
            return;
        default:
            toxy(25,18);
            printf("���벻�Ϲ�,����������");
            Choose_Drinkplace(cf,i);  //�ݹ�ֱ������Ϲ�
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

void Get_CpTotalprice(Complete_Form* cf)  //�������������ļ۸�Ϊ��total_price���Ը�ֵ
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
    system(Normal_Color);;  //���ñ�����ɫ��������ɫ
    system("cls");  //����
    HideCursor(0);  //���ع��
    char t;
    Get_CpTotalprice(cf);  //���㵱ǰ�����۸�
    toxy(28,8);
    printf("     ʱ���ݰɵ㵥ϵͳ");
    toxy(22,9);
    printf("-------------------------------------");
    toxy(26,11);
    printf("�����ܼ�Ϊ:%.1fԪ,��ѡ��֧����ʽ",cf->total_price);
    toxy(27,13);
    printf("1.�ֽ�֧��");
    toxy(27,14);
    printf("2.����֧��");
//    printf("�����ܼ�Ϊ:%.1fԪ,��ѡ��֧����ʽ\n",cf->total_price);
//    printf("\t\t����esc��������ҳ��\n");
//    printf("\t\t1.�ֽ�֧��\n");
//    printf("\t\t2.����֧��\n");
    if(cf->is_member>0)
    {
        toxy(27,15);
        printf("3.��Ա��֧��(85��)");
        toxy(27,16);
        printf("Esc.����������");
    }
    else
    {
        toxy(27,15);
        printf("Esc.����������");
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
                printf("лл�ݹˣ�����");
                judgeFormOk = 1;
                Sleep(1000);  //��ͣ1����ʾ��ʾ��Ϣ
                break;
            case '2':
                cf->pay_way = 2;
                toxy(25,18);
                printf("лл�ݹˣ�����");
                judgeFormOk = 1;
                Sleep(1000);  //��ͣ1����ʾ��ʾ��Ϣ
                break;
            case 27:
                Default();
                return;
            default:
                toxy(25,18);
                printf("���벻�Ϲ�,����������");
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
                printf("лл�ݹˣ�����");
                judgeFormOk = 1;
                Sleep(1000);  //��ͣ1����ʾ��ʾ��Ϣ
                break;
            case '2':
                cf->pay_way = 2;
                toxy(25,18);
                printf("лл�ݹˣ�����");
                judgeFormOk = 1;
                Sleep(1000);  //��ͣ1����ʾ��ʾ��Ϣ
                break;
            case '3':
                cf->pay_way = 3;
                if(node_VIP->restmoney>cf->total_price)
                {
                    node_VIP->restmoney -= cf->total_price*0.85;
                    toxy(24,18);
                    printf("��Ա��֧�����,����ʣ��%.2fԪ",node_VIP->restmoney);
                    toxy(25,18);
                    printf("лл�ݹ�!!!");
                    judgeFormOk = 1;
                    Sleep(1000);
                }
                else
                {
                    toxy(24,18);
                    printf("��Ա������,֧��ʧ��");
                    Sleep(1000);
                    return;  //֧��ʧ�ܲ�����ӵ������б�
                }
                break;
            case 27:
                Default();
                return;
            default:
                toxy(25,18);
                printf("���벻�Ϲ�,����������");
                Choose_Payway(cf);
                return;
        }
    }
    Add_Form(cf,head_form);  //���ö��������ӵ�����������
    return;
}

void Create_ID(Complete_Form* cf)  //��cfָ��Ķ�����id���ݵ�ǰ��ʱ�������и�ֵ
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

void Add_Form(Complete_Form*cf,Complete_Form* h)  //��cfָ��Ķ�����ӵ���������h��ĩβ
{
    Complete_Form * p;
    p = h;  //��λ���ָ��
    while(p->next!=NULL)  //�ҵ�β���
    {
        p = p->next;
    }
    p->next = cf;
    return;
}

Material* Consume_Inventory(Complete_Form* cf,int i)  //����cfָ�򶩵��е�i��ԭ���Ͽ�棬����治�㣬�������ʾ���Զ�ȡ���ôζ��������ز����ԭ���϶�Ӧ�Ľṹ��ָ��;�����������ģ�����null
{
    int beverage_id;  //��Ʒid
    int material_id;  //ԭ����id
    Beverage* beverage_struct;  //��Ʒ�ṹ��
    Material* material_struct;  //ԭ���Ͻṹ��
    beverage_id = cf->forms[i-1].beverage;  //�ҵ���������ĵ�i����Ʒ��Ӧ��id
    beverage_struct = Find_Beverage(beverage_id,head_beverage);  //ͨ��id�ҵ���Ӧ����Ʒ�ṹ��
    for(int j=0;j<beverage_struct->material_num;j++)  //��������Ʒ�ṹ���ԭ�������࣬�����С����ġ�
    {
        material_id = beverage_struct->materials[j];  //�ҵ�����Ʒ��ԭ���϶�Ӧ��id
        material_struct = Find_Material(material_id, head_material);  //ͨ��ԭ���϶�Ӧ��id�ҵ���Ӧ�Ľṹ��
        if (material_struct->inventory >= material_struct->consume)  //������㹻
        {
            material_struct->inventory -= material_struct->consume;  //�ò��ϵĿ������ȥ�ò��ϵĵ���ʹ������Ϊ�����ġ�
        } else  //��治��
        {
            return material_struct;
        }
    }
    return NULL;
}

void Rewrite_FormData()  //�����º�Ķ���������������д���ļ���
{
    if(head_form->next==NULL)
    {
        return;
    }
    Complete_Form* cf = head_form->next;
    FILE * fp;
    int i;  //ѭ������
    fp = fopen("..\\Form_Data.txt","w");  //�Ը��ӷ�ʽ���ļ�
    while(cf!=NULL)  //���ȫ��
    {
        fprintf(fp,"%d,%d-%d-%d-%d:%d,%d\n",cf->id.id,cf->id.year,cf->id.month,cf->id.day,cf->id.hour,cf->id.min,cf->number);
        for(i=0;i<cf->number;i++)
        {
            fprintf(fp,"%d,%d,%d,%d,%d,%.1f\n",i+1,cf->forms[i].beverage,cf->forms[i].ingredients,cf->forms[i].temperature,cf->forms[i].drink_place,cf->forms[i].total_price);
        }
        fprintf(fp,"%.1f,%d",cf->total_price,cf->pay_way);
        if(cf->next!=NULL)  //���һ�в������з�
        {
            fprintf(fp,"\n");
        }
        //�ļ�д���ʽΪ       ������,��-��-��-ʱ:��,�ö����ķ���
        //                    ��1��,��Ʒ����,С��,�����¶�,���õص�,��һ�ݵ��ܼ�
        //                    ��2��,��Ʒ����,С��,�����¶�,���õص�,��һ�ݵ��ܼ�
        //                    �����ܼ�,���ʽ
        cf=cf->next;
    }
    fclose(fp);
    return;
}

void Rewrite_BeverageData()  //�����º����Ʒ������������д���ļ���
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

void Rewrite_MaterialData()  //�����º��ԭ����������������д���ļ���
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
// 0�����ⲻͨ��
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



// return 0�����أ�1����û��ϵ*����/������
int Register_Name(VIP *node){
    int attation = 0;
    while(1)
    {
        char temp[100] = {0};
        char name[100] = {0};
        printf("ESC ��ҳ\n *  ������һ��");
        toxy(28, 8);
        printf("     ʱ���ݰ���Ϣ����ϵͳ");
        toxy(22,9);
        printf("-------------------------------------");
        toxy(Lstrict, Upstrict+1);
        strcpy(temp, "���������������");
        printf("%s", temp);
        int lenname = strlen(temp);
        if(attation)
            Warning_Type;
        toxy(Lstrict, Upstrict+2);
        printf("ע�⣡");
        toxy(Lstrict, Upstrict+3);
        printf("��Ӧ����20���ַ��Ҳ���Ϊ��!");
        toxy(Lstrict, Upstrict+4);
        printf("��Ӧ�����ո�!");
        if(attation){
            toxy(Lstrict, Upstrict+5);
            Normal_Type;
            printf("�����ԣ�");
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
// 0�����ⲻͨ�� -1����绰���ظ�
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

// ע��绰�� 0δ������1����
int Register_tel(VIP *node,VIP *head){
    system("cls");
    int attation = 0;
    int flag_tel_repeat = 0;
    while(1)
    {
        char temp[100] = {0};
        char tel[100] = {0};
        printf("ESC ��ҳ\n *  ������һ��");
        toxy(28, 8);
        printf("     ʱ���ݰ���Ϣ����ϵͳ");
        toxy(22,9);
        printf("-------------------------------------");
        toxy(Lstrict, Upstrict+1);
        strcpy(temp, "��������ĵ绰:");
        printf("%s", temp);
        int lentel = strlen(temp);
        if(attation)
            Warning_Type;
        toxy(Lstrict, Upstrict+2);
        printf("ע�⣡");
        toxy(Lstrict, Upstrict+3);
        printf("��Ӧ����20λ�����Ҳ���Ϊ��");
        toxy(Lstrict, Upstrict+4);
        printf("ֻ����������");
        toxy(Lstrict, Upstrict+5);
        printf("��֧���ƶ��ֻ�����");
        toxy(Lstrict, Upstrict+7);
        Normal_Type;
        if(flag_tel_repeat){
            flag_tel_repeat = 0;
            Warning_Type;
            printf("�˺����Ѵ��ڣ����������룡");
            Normal_Type;
            toxy(Lstrict, Upstrict+6);
            printf("�����ԣ�");
        }
        toxy(Lstrict, Upstrict+6);
        if(attation){
            Normal_Type;
            printf("�����ԣ�");
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

// �������Ϸ��� 0ͨ��1��ͨ��
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
        //��Ĳ�����ʾ�ַ��Ѿ���c>31���˵��ˣ����������ĸ�����ֱ����ַ�
        if(!((password[i]<='9'&&password[i]>='0')||(password[i]<='Z'&&password[i]>='A'||password[i]<='z'&&password[i]>='a'))&&flag__){
            flag++;
            flag__ = 1;
        }
    }
    if(flag<2)
        return 0;
    return 1;
}
// ע������ 0δ������1����
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
        { //ȷ�����conyinue������Ϊ���������ɵģ�����Ϊ������repeat
            for (int i = 0; i < 100;i++)
                password[i] = 0;
        }
        else{
            attation = 0;//���Ϊ��repeat��Ҫ������������
        }
        char temp[100] = {0};
        if(norepeat){
            printf("ESC ��ҳ\n *  ������һ��");
            toxy(28, 8);
            printf("     ʱ���ݰ���Ϣ����ϵͳ");
            toxy(22,9);
            printf("-------------------------------------");
            toxy(Lstrict, Upstrict+1);
            strcpy(temp, "�������������:");
            printf("%s", temp);// �ظ�ʱpassword�Ѿ������
            for (int i = 0; i < strlen(password);i++)
                printf("*");
            printf("\n");
            lenpassword = strlen(temp);
            toxy(Lstrict, Upstrict+2);
            strcpy(temp, "��������������:");
            printf("%s", temp);
            toxy(Lstrict, Upstrict+4);
            if(norepeat>=2){
                Warning_Type;
                printf("���벻ƥ��!");
                Normal_Type;
            }
            toxy(Lstrict, Upstrict+3);
            if (norepeat>=2)
            {
                Normal_Type;
                printf("������!");
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
            printf("ESC ��ҳ\n *  ������һ��");
            toxy(28, 8);
            printf("     ʱ���ݰ���Ϣ����ϵͳ");
            toxy(22,9);
            printf("-------------------------------------");
            toxy(Lstrict, Upstrict+1);
            strcpy(temp, "�������������:");
            printf("%s", temp);
            if(norepeat!=0){// �ظ�ʱpassword�Ѿ������
                for (int i = 0; i < strlen(password)-1;i++)
                    printf("*");
            }
            int lenpassword = strlen(temp);
            if (attation)
                Warning_Type;
            toxy(Lstrict, Upstrict+2);
            printf("ע��!");
            toxy(Lstrict, Upstrict+3);
            printf("���ܰ����ո���*��");
            toxy(Lstrict, Upstrict+4);
            printf("���ȱ���Ϊ8-16���ַ�");
            toxy(Lstrict, Upstrict+5);
            printf("����������֡���ĸ������������2��");
            Normal_Type;
            toxy(Lstrict, Upstrict+6);
            if (attation)
            {
                Normal_Type;
                printf("�����ԣ�");
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
            c = getch();//�����Ժ���
            fflush(stdin);
            if(c<0){//�ų����·����������
                continue;
            }

            if (c == '*')
            {
                fflush(stdin);
                system("cls");
                return 0;
            }//��getch�лس��ᱻ����\r
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
        if(norepeat){// �ֱ𱣴�
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

/*ע���Ա*/
VIP* VIPRegister(VIP*VIPlisthead){
    VIP *p = (VIP *)malloc(sizeof(VIP));
    VIP__init__(p);
    VIP *cur = VIPlisthead;
    if (cur != NULL) {
        while(cur->next!=NULL)
            cur = cur->next;
    }
    p->loss = 0; //Ĭ��û�й�ʧ
    while(1)
    {// ���ֲ���
        /*name space*/
        if(flag_come_back||auto_back||Register_Name(p)==0){
            if(auto_back){//autoback��ʱ��Ҫbreakִ�к����
                auto_back = 0;
                break;
            }
            else{
                return VIPlisthead;//*&ESCֱ�ӻ�ȥ�ͺ�
            }
        }

        while(1)// �绰����
        {
            if(flag_come_back||auto_back||Register_tel(p,VIPlisthead)==0)
                break;
            while(1)// ���벿��
            {
                if(flag_come_back||auto_back||Register_password(p)==0)
                    break;
                while(1){// ��ֵ����
                    if(flag_come_back||auto_back||Register_recharge(p)==0)
                        break;
                    auto_back = 1;
                    break;
                }
            }
        }
    }
    //id ����
    struct tm *NowTime = GetNowTime();
    p->id.year = NowTime->tm_year+1900;
    p->id.month = NowTime->tm_mon+1;
    p->id.day = NowTime->tm_mday;
    p->id.hour = NowTime->tm_hour+8;
    p->id.min = NowTime->tm_min;
    sprintf(p->id.id, "%04d%02d%02d%02d%02d\0",// ����Idvip�ṹ����id�ַ����鲿��
            p->id.year, p->id.month, p->id.day, p->id.hour, p->id.min);
    auto_back = 0;
    if(flag_come_back){//����������˾Ͳ��ñ���
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
��ȡvip��Ա����������һ��û���ڱ�����VIP������
��ʽ��
_index,name,tel,restmoney,password,loss(bool),(orderlist....)
index,name,tel,restmoney,password,loss(bool),(orderlist....)
(�»���'_'����ո�)
*/
VIP* VIPReadFile(){
    //���û���ļ��������ļ�
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
        sprintf(p->id.id, "%04d%02d%02d%02d%02d\0",// ����Idvip�ṹ����id�ַ����鲿��
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
�����Ա����������Ϊһ��û���ڱ�����VIP������
��ʽ��
_index,name,tel,restmoney,password,loss(bool),(orderlist....)
index,name,tel,restmoney,password,loss(bool),(orderlist....)
(�»���'_'����ո�)
*/
void VIPInputFile(VIP *VIPlisthead){
    VIP *p = VIPlisthead;
    FILE *fp = fopen("PersonVIP.txt", "w");
    int flag = 0;
    while (p != NULL)
    {   //Ϊ��ʹ���һ���޻���
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
        else//��һ��ǰ��Ÿ��� ���������ļ��Ƿ�Ϊ��
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
//int������ĳ�ʼ��
void ClearArr_int(int *a,int size,int value){
    int i = 0;
    for (;i<size;i++)
        a[i] = value;
}
//char������ĳ�ʼ��
void ClearArr_char(char *a,int size,char value){
    int i = 0;
    for (;i<size;i++)
        a[i] = value;
}
/*VIP�ṹ��ĳ�ʼ������������ָ������ʣ������д���ָ�룬���÷���*/
void VIP__init__(VIP *node){
    ClearArr_char(node->tel,sizeof(node->tel),0);
    ClearArr_char(node->name,sizeof(node->name),0);
    ClearArr_char(node->password, sizeof(node->password), 0);
    ClearArr_int(node->orders, sizeof(node->orders) / 4, 0); //int���͵Ĵ�С��4bytes
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
//��ȡ��ǰʱ��ĺ��������ص���tm���ͽṹ�壬�����L-Click+Ctrl
struct tm* GetNowTime(){
    time_t timep;
    struct tm *NowTime;
    time (&timep);
    NowTime=gmtime(&timep);
    return NowTime;
}
// ���������ĺϷ���
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
// char��������->int����
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
// ��ֵ���� 0δ�ɹ���1�ɹ�
int Register_recharge(VIP *node){
    system("cls");
    int attation = 0;
    while(1)
    {
        char temp[100] = {0};
        char money[100] = {0};
        printf("ESC ��ҳ\n *  ������һ��");
        toxy(28, 8);
        printf("     ʱ���ݰ���Ϣ����ϵͳ");
        toxy(22,9);
        printf("-------------------------------------");
        toxy(Lstrict, Upstrict+1);
        strcpy(temp, "��������Ҫ��ֵ�Ľ��粻���ֵ���ֵ0Ԫ��");
        printf("%s", temp);
        int lentel = strlen(temp);
        if(attation)
            Warning_Type;
        toxy(Lstrict, Upstrict+2);
        printf("ע�⣡");
        toxy(Lstrict, Upstrict+3);
        printf("��ֵǮ�����ö���20���ַ�");
        toxy(Lstrict, Upstrict+4);
        printf("����������");
        toxy(Lstrict, Upstrict+5);
        if(attation){
            Normal_Type;
            printf("�����ԣ�");
        }
        toxy(Lstrict + lentel + 1, Upstrict+1);
        char c=1;
        int i = 0;
        while (c)
        {
            fflush(stdin);
            c = getch();//�����Ժ���
            fflush(stdin);
            if(c<0){//�ų����·����������
                continue;
            }

            if (c == '*')
            {
                fflush(stdin);
                system("cls");
                return 0;
            }//��getch�лس��ᱻ����\r
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
//�Ե绰��ʽ���в��ң��޵绰����NULL
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
//��Ա��¼���ɹ����ؽ�㣬ʧ�ܷ���NULL
VIP *VIPlogin(VIP *VIPheadlist){
    system("cls");
    char c = 1;
    char telnum[20] = {0};
    char password[20] = {0};
    char temp[100] = {0};//��λ�����
    int i = 0,flag_errorcode=0,flag_notele=0;
    int len = 0;
    VIP *node = NULL;
    // if(VIPheadlist == NULL){
    //     return NULL;//û���û�(�����������)
    // }
    while(1){
        printf("ESC ��ҳ\n *  ������һ��");
        toxy(28, 8);
        printf("     ʱ���ݰɵ�½ϵͳ");
        toxy(22,9);
        printf("-------------------------------------");
        strcpy(temp, "�������ֻ��ţ�");
        toxy(Lstrict, Upstrict+1);
        printf("�������ֻ��ţ�");
        if(flag_errorcode&&flag_come_back==0)
            printf("%s", node->tel);
        if(flag_notele){
            toxy(Lstrict, Upstrict+2);
            Warning_Type;
            printf("�����ֻ���");
            Normal_Type;
            flag_notele = 0;
        }
        toxy(Lstrict + strlen(temp) + 1, Upstrict+1);
        ClearArr_char(temp, 100, 0);
        ClearArr_char(telnum, 20, 0);
        i = 0;// ���¶�tel���鸳ֵ
        while(!flag_errorcode){ //�����ֻ��� �����������˵���ֻ���������ȷ��
            fflush(stdin);
            c = getch();
            fflush(stdin);
            len = strlen(telnum);

            switch(c){
                case 27:{ //ESC�������˵�
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
                case '*':{// �����ֻ��ŵķ��غ�ESC������ͬ
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
            continue;//�����ֻ��� ���¿�ʼ�µĲ�ѯ�ֻ��Ų���
        }
        strcpy(temp, "���������룺");
        toxy(Lstrict, Upstrict+2);
        printf("%s", temp);
        if(flag_errorcode){
            toxy(Lstrict, Upstrict+3);
            Warning_Type;
            printf("���벻��ȷ��������ȷ�������ʽ");
            Normal_Type
            flag_errorcode = 0;
        }
        toxy(Lstrict + strlen(temp) + 1, Upstrict+2);
        ClearArr_char(temp, 100, 0);
        c = 1;
        i = 0;
        ClearArr_char(password, 20, 0);
        while(1){// ��������
            fflush(stdin);
            c = getch();
            fflush(stdin);
            len = strlen(password);
            switch(c){
                case 27:{ //ESC�������˵�
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
                flag_errorcode = 1;//���������ʽ����
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
            continue;//���벻��ȷ
        }
    }
}
//չʾuser��Ա����
void VIPUSERshow(VIP *node){
    if(node==NULL)
        return;
    system("cls");
    printf("ESC ��ҳ\n *  ������һ��");
    toxy(28, 8);
    printf("     ʱ���ݰɻ�Աϵͳ");
    toxy(22,9);
    printf("-------------------------------------");
    toxy(Lstrict, Upstrict+1);
    printf("ע�����ڣ�%04d-%02d-%02d %02d:%02d",
           node->id.year,node->id.month,node->id.day,node->id.hour,node->id.min);
    toxy(Lstrict, Upstrict+2);
    printf("������%s",node->name);
    toxy(Lstrict, Upstrict+3);
    printf("�绰��%s",node->tel);
    toxy(Lstrict, Upstrict+4);
    printf("������%.2f",node->restmoney);
    toxy(Lstrict, Upstrict+5);
    printf("��״̬��");
    if (node->loss)
    {
        Warning_Type;
        printf("��ʧ\n");
        Normal_Type;
    }
    else{
        printf("����\n");
    }
    if(node->loss){
        toxy(22,Upstrict+6);
        printf("-------------------------------------");
        toxy(Lstrict, Upstrict+7);
        printf("| 1.��ֵ |");
        toxy(Lstrict, Upstrict+9);
        printf("| 2.��� |");
        toxy(Lstrict, Upstrict+11);
        printf("| 3.�鿴���Ѽ�¼ |");
        toxy(Lstrict, Upstrict+13);
        printf("| 4.�޸����� |");
        toxy(Lstrict, Upstrict+15);
        printf("| 5.�޸ĵ绰 |");
        toxy(Lstrict, Upstrict+17);
    }
    else{
        toxy(22,Upstrict+6);
        printf("-------------------------------------");
        toxy(Lstrict, Upstrict+7);
        printf("| 1.��ֵ |");
        toxy(Lstrict, Upstrict+9);
        printf("| 2.��ʧ |");
        toxy(Lstrict, Upstrict+11);
        printf("| 3.�鿴���Ѽ�¼ |");
        toxy(Lstrict, Upstrict+13);
        printf("| 4.�޸����� |");
        toxy(Lstrict, Upstrict+15);
        printf("| 5.�޸ĵ绰 |");
        toxy(Lstrict, Upstrict+17);
        printf("| 6.��Ա��� |");
        toxy(Lstrict, Upstrict+19);
    }
}
// ���������ɹ�����
void Operationok(int state){
    int time = 3;
    while(time){
        system("cls");
        toxy(Lstrict, Upstrict+2);
        if(state){
            printf("�����ɹ�������%d��󷵻�",time);
        }
        else{
            printf("����ʧ�ܣ�����%d��󷵻�",time);
        }
        Sleep(1000);
        time--;
    }
}
// ��Ա����ʧ
void VIPloss(VIP *node){

    while(1){
        system("cls");
        printf("��Ա���ţ�%s\n������%s\n��Ա��״̬��",node->id.id,node->name);
        Warning_Type;
        if(node->loss){
            printf("��ʧ");
            Normal_Type;
            printf("\n�Ƿ��ң���Y/N��");
        }
        else{
            printf("δ��ʧ");
            Normal_Type;
            printf("\n�Ƿ��ʧ����Y/N��");
        }
        char choose = 0;
        int canelchoose = 0;
        while(1){//����ѡ��
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
// չʾ��һ����
void show_form(Complete_Form *node){
    printf("---%04d��%02d��%02d�� %02d:%02d---\n", node->id.year, node->id.month, node->id.day, node->id.hour, node->id.min);
    int i = 0;
    for (;i < node->number;i++){
        Beverage *detail = Find_Beverage(node->forms[i].beverage, head_beverage);
        printf("��Ʒ���ƣ�%s  ����С�ϣ�", detail->name);
        switch(node->forms[i].ingredients){
            case 1:
                printf("����  ");
                break;
            case 2:
                printf("Ҭ��  ");
                break;
            case 3:
                printf("�춹  ");
                break;
            case 4:
                printf("����  ");
                break;
            case 5 :
                printf("��  ");
                break;
        }
        printf("���ۣ�%2.1f\n", node->forms[i].total_price);
    }
    printf("---֧����ʽ��");
    switch(node->pay_way){
        case 1:
            printf("�ֽ�֧��--");
            break;
        case 2:
            printf("����֧��--");
            break;
        case 3:
            printf("��Ա��֧��--");
            break;
    }
    printf("�ܼƽ�%2.1f---", node->total_price);
}
// չʾ��Ա���Ѽ�¼���������
void ConsumeRecord(VIP *node,Complete_Form *head){//headΪ�����ڱ����Ķ�������
    system("cls");
    int len = 0;
    Complete_Form *FormInfo=NULL;
    for (; node->orders[len];len++);
    if(len==0){
        printf("�޶�����Ϣ\n----------\n");
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
    printf("��*���� ��ESC�������˵�");
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

//user��Ա����
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
        printf("��ţ�%d  ע��ʱ�䣺%4d-%02d-%02d %02d:%02d  ������%-20s  �ֻ��ţ�%-20s  ���룺*****  ��%-10.2f  ��״̬��",
               index_vip, start->id.year,start->id.month,start->id.day,start->id.hour,start->id.min, start->name, start->tel,start->restmoney);
        if(start->loss){
            Warning_Type;
            printf("��ʧ\n");
            Normal_Type;
        }
        else{
            printf("����\n");
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
        printf("���޻�Ա��Ϣ");
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
    int time = 0, money = 0;//0�������� 1������
    int fristwhile = 0;
    while(1){
        system("cls");//��ӡ����
        VIPShowList(head_vip);
        printf("\n\n\n");
        printf("1.���ӻ�Ա 2.ע����Ա 3.�޸�/�鿴��Ա��ϸ��Ϣ 4.��ʱ������ 5.��������� +.��һҳ -.��һҳ ESC.�������˵� *����\n");
        while(1){//�������
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
                    ModandShowDetail(head_vip); //�޸�/�鿴��Ա��ϸ��Ϣ
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
                    //����
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
    while(1){//������
        i = 0;
        system("cls");
        number = VIPShowList(head);
        printf("\n\n");
        if(error){
            error = 0;
            Warning_Type;
            printf("\n��������ȷ�ı��(*���أ�ESC�������˵�)\n");
            Normal_Type;
        }
        printf("������Ҫע����Ա�ı��(+��һҳ��-��һҳ)����*���� ESC�������˵���");
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
        else{//ȷ�ϣ�
            printf("\nȷ��Ҫע�����Ϊ%d�Ļ�Ա����Y/N��", target);
            char confrim = 0;
            int flag_back = 0;
            while(1){
                fflush(stdin);
                confrim = getch();
                fflush(stdin);
                switch(confrim){
                    case 'Y'://ִ��ɾ��
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
    head = p;//�ڱ����
    VIP *cur = head;
    for (int i = 1;i<index;i++){
        cur = cur->next;
    }//���ɾ����n�����cur����n-1��p��n+1
    VIP *del = cur->next;
    p = cur->next->next;
    cur->next = p;
    free(del);
    return head->next;//ȥ�ڱ����
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
    while(1){//������
        i = 0;
        system("cls");
        number = VIPShowList(head);
        printf("\n\n");
        if(error){
            error = 0;
            Warning_Type;
            printf("\n��������ȷ�ı��(*���أ�ESC�������˵�)\n");
            Normal_Type;
        }
        printf("������Ҫ�޸Ĳ鿴��Ա�ı��(+��һҳ��-��һҳ)����*���� ESC�������˵���");
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
            printf("\n\n1.�޸����� 2.�޸��ֻ��� 3.���ʧ 4.�鿴���Ѽ�¼ *���� ESC�������˵�");
            int flag_back = 0;
            while(1){// ����
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
    printf("ע��ʱ�䣺%04d-%02d-%02d %02d:%02d\n������%s\n�绰��%s\n���룺%s\n������%.2f\n��״̬��",
           node->id.year, node->id.month, node->id.day, node->id.hour, node->id.min,
           node->name, node->tel, node->password, node->restmoney);
    if(node->loss){
        Warning_Type;
        printf("��ʧ\n");
        Normal_Type;
    }
    else{
        printf("����\n");
    }
}

STAFF * STAFFReadFile(){
    //���û���ļ��������ļ�
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
        sprintf(p->id.id, "%04d%02d%02d%02d%02d\0",// ����Idvip�ṹ����id�ַ����鲿��
                p->id.year, p->id.month, p->id.day, p->id.hour, p->id.min);
        flag = fgetc(fp);
        cur->next = p;
        cur = cur->next;
        if(flag!=' ') //û��orders
            continue;
        do{
            fscanf(fp, "%d", &(p->orders[i++]));
            flag = fgetc(fp);
        } while (flag == ',');

    }
    return head->next;
}

/*STAFF�ṹ��ĳ�ʼ������������ָ������ʣ������д���ָ�룬���÷���*/
void STAFF__init__(STAFF *node){
    ClearArr_char(node->tel,sizeof(node->tel),0);
    ClearArr_char(node->name,sizeof(node->name),0);
    ClearArr_char(node->password, sizeof(node->password), 0);
    ClearArr_int(node->orders, sizeof(node->orders) / 4, 0); //int���͵Ĵ�С��4bytes
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

// �����Ա����������Ϊһ��û���ڱ�����VIP������
void STAFFInputFile(STAFF *head){
    STAFF *p = head;
    FILE *fp = fopen("PersonStaff.txt", "w");
    int flag = 0;
    while (p != NULL)
    {   //Ϊ��ʹ���һ���޻���
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
        else//��һ��ǰ��Ÿ��� ���������ļ��Ƿ�Ϊ��
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
    printf("��ѡ��ְλ��");
    toxy(Lstrict,Upstrict+2);
    printf("1.����Ա 2.�곤 3.��๤ 4.ʵϰ�� *���� ESC�˳�");
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
    p->loss = 0; //Ĭ��û�й�ʧ
    while(1)
    {// ���ֲ���
        /*name space*/
        if(flag_come_back||auto_back||Register_Name(p)==0){
            if(auto_back){//autoback��ʱ��Ҫbreakִ�к����
                auto_back = 0;
                break;
            }
            else{
                return head;//*&ESCֱ�ӻ�ȥ�ͺ�
            }
        }

        while(1)// �绰����
        {
            if(flag_come_back||auto_back||Register_tel(p,head)==0)
                break;
            while(1)// ���벿��
            {
                if(flag_come_back||auto_back||Register_password(p)==0)
                    break;
                while(1){// ��ֵ����
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
    //id ����
    struct tm *NowTime = GetNowTime();
    p->id.year = NowTime->tm_year+1900;
    p->id.month = NowTime->tm_mon+1;
    p->id.day = NowTime->tm_mday;
    p->id.hour = NowTime->tm_hour+8;
    p->id.min = NowTime->tm_min;
    sprintf(p->id.id, "%04d%02d%02d%02d%02d\0",// ����Idvip�ṹ����id�ַ����鲿��
            p->id.year, p->id.month, p->id.day, p->id.hour, p->id.min);
    auto_back = 0;
    if(flag_come_back){//����������˾Ͳ��ñ���
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
        printf("ESC ��ҳ\n *  ������һ��");
        toxy(28, 8);
        printf("     ʱ���ݰ���Ϣ����ϵͳ");
        toxy(22,9);
        printf("-------------------------------------");
        toxy(Lstrict, Upstrict+1);
        strcpy(temp, "������Ա�����ʣ�");
        printf("%s", temp);
        int lentel = strlen(temp);
        if(attation)
            Warning_Type;
        toxy(Lstrict, Upstrict+2);
        printf("ע�⣡");
        toxy(Lstrict, Upstrict+3);
        printf("���ö���20���ַ�");
        toxy(Lstrict, Upstrict+4);
        printf("����������");
        toxy(Lstrict, Upstrict+5);
        if(attation){
            Normal_Type;
            printf("�����ԣ�");
        }
        toxy(Lstrict + lentel + 1, Upstrict+1);
        char c=1;
        int i = 0;
        while (c)
        {
            fflush(stdin);
            c = getch();//�����Ժ���
            fflush(stdin);
            if(c<0){//�ų����·����������
                continue;
            }

            if (c == '*')
            {
                fflush(stdin);
                system("cls");
                return 0;
            }//��getch�лس��ᱻ����\r
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
        printf("����Ա����Ϣ");
        return 0;
    }
    int index = 1;
    while(cur!=NULL){
        printf("��ţ�%d  �ϸ�ʱ�䣺%04d-%02d-%02d %02d:%02d:%02d  ������%-20s  �ֻ��ţ�%-20s  ���룺*****  ���ʣ�%-10.2f  ְλ��", index, cur->id.year,cur->id.month,cur->id.day,cur->id.hour,cur->id.day,cur->id.min, cur->name, cur->tel,cur->restmoney);
        switch(cur->position){
            case 1:
                printf("����Ա\n");
                break;
            case 2:
                printf("�곤\n");
                break;
            case 3:
                printf("��๤\n");
                break;
            case 4:
                printf("ʵϰ��\n");
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
        system("cls");//��ӡ����
        STAFFShowList(head_staff);
        printf("\n\n\n");
        printf("1.����Ա�� 2.ע��Ա�� 3.�޸�/�鿴Ա����ϸ��Ϣ 4.ʱ������ 5.�������� ESC.�������˵� *����\n");
        while(1){//�������
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
                    ModandShowstaffDetail(head_staff,1); //�޸�/�鿴Ա����ϸ��Ϣ
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
                    //����
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
    while(1){//������
        i = 0;
        system("cls");
        number = STAFFShowList(head);
        printf("\n\n");
        if(error){
            error = 0;
            Warning_Type;
            printf("\n��������ȷ�ı��(*���أ�ESC�������˵�)\n");
            Normal_Type;
        }
        printf("������Ҫע��Ա���ı�ţ���*���� ESC�������˵���");
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
        else{//ȷ�ϣ�
            printf("\nȷ��Ҫע�����Ϊ%d��Ա������Y/N��", index);
            char confrim = 0;
            int flag_back = 0;
            while(1){
                fflush(stdin);
                confrim = getch();
                fflush(stdin);
                switch(confrim){
                    case 'Y'://ִ��ɾ��
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
    while(1){//������
        i = 0;
        system("cls");
        number = STAFFShowList(head);
        printf("\n\n");
        if(error){
            error = 0;
            Warning_Type;
            printf("\n��������ȷ�ı��(*���أ�ESC�������˵�)\n");
            Normal_Type;
        }
        printf("������Ҫ�޸Ĳ鿴Ա���ı�ţ���*���� ESC�������˵���");
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
                printf("\n\n1.�޸����� 2.�޸��ֻ��� 3.�޸�ְλ 4.�鿴ҵ�� *���� ESC�������˵�");
                Warning_Type;
                printf("\nע�⣺�޸�ְλ��ɾ��Ա��ҵ�������ѡ��");
                Normal_Type;
            }
            else{
                printf("\n\n1.�޸����� 2.�޸��ֻ��� 3.�޸�ְλ *���� ESC�������˵�");
            }
            int flag_back = 0;
            while(1){// ����
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
                            printf("��Ǹ������Ȩ��");
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
    printf("ע��ʱ�䣺%04d-%02d-%02d %02d:%02d\n������%s\n�绰��%s\n���룺%s\n���ʣ�%.2f\nְλ��",
           node->id.year + 1900, node->id.month + 1, node->id.day, node->id.hour + 8, node->id.min,
           node->name, node->tel, node->password, node->restmoney);
    switch(node->position){
        case 1:
            printf("����Ա\n");
            break;
        case 2:
            printf("�곤\n");
            break;
        case 3:
            printf("��๤\n");
            break;
        case 4:
            printf("ʵϰ��\n");
            break;
    }
}




void STAFFUSERshow(VIP *node){
    if(node==NULL)
        return;
    system("cls");
    printf("ESC ��ҳ\n *  ������һ��");
    toxy(28, 8);
    printf("     ʱ���ݰɻ�Աϵͳ");
    toxy(22,9);
    printf("-------------------------------------");
    toxy(Lstrict, Upstrict+1);
    printf("���ţ�", node->id.id);
    toxy(Lstrict, Upstrict + 2);
    printf("������%s",node->name);
    toxy(Lstrict, Upstrict+3);
    printf("�绰��%s",node->tel);
    toxy(Lstrict, Upstrict+4);
    printf("���ʣ�%.2f",node->restmoney);
    toxy(Lstrict, Upstrict+5);
    printf("ְλ��");
    switch (node->position)
    {
        case 1:
            printf("����Ա");
            break;
        case 2:
            printf("�곤");
            break;
        case 3:
            printf("��๤");
            break;
        case 4:
            printf("ʵϰ��");
            break;
        default:
            break;
    }
    if(node->position==1){
        toxy(22,Upstrict+6);
        printf("-------------------------------------");
        toxy(Lstrict, Upstrict+7);
        printf("| 1.�޸ĵ�¼���� |");
        toxy(Lstrict, Upstrict+9);
        printf("| 2.�޸��ֻ��� |");
        toxy(Lstrict, Upstrict+11);
        printf("| 3.�鿴ҵ�� |");
        toxy(Lstrict, Upstrict+13);
        printf("| 4.��� |");
        toxy(Lstrict, Upstrict+15);
        printf("| 5.��Աע�� |");
        toxy(Lstrict, Upstrict+17);
        printf("| 6.��Ա��¼ |");
    }
    else{
        printf("\n�����������1. �޸ĵ�¼���� 2.�޸��ֻ��� *���� ESC.�˳���¼\n");
        toxy(22,Upstrict+6);
        printf("-------------------------------------");
        toxy(Lstrict, Upstrict+7);
        printf("| 1.�޸ĵ�¼���� |");
        toxy(Lstrict, Upstrict+9);
        printf("| 2.�޸��ֻ��� |");
    }
}

//user��Ա����
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
                //Ա���Ƿ���Ա������
                if(operation=='4'&&node->position==1){
                    //ѡ���Ƿ��ǻ�Ա
                    while(1){
                        system("cls");
                        node_STAFF = node;
                        printf("ESC ��ҳ\n *  ������һ��");
                        toxy(Lstrict, Upstrict);
                        printf("��͹˿��ǻ�Ա�𣿣�Y/N��");
                        VIP *result = NULL;//result 0 ��¼ʧ�� node�ɹ�
                        int YN = inputYorN();
                        if(YN == -1)
                            break;
                        if(YN){//�Ƿ��Ա
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
//Y����1N����0ESC����-1
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

//��VIP��ID��ʱ�䣩�������� 1���� 0����
void V_order_char(int version){
    VIP *p = (VIP *)malloc(sizeof(VIP));
    p->next = head_vip;
    head_vip = p;//�ڱ����
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
//��VIP������������ 1���� 0����
void V_order_float(int version){
    VIP *p = (VIP *)malloc(sizeof(VIP));
    p->next = head_vip;
    head_vip = p;//�ڱ����
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

//��staff��ID��ʱ�䣩�������� 1���� 0����
void F_order_char(int version){
    VIP *p = (VIP *)malloc(sizeof(VIP));
    p->next = head_staff;
    head_staff = p;//�ڱ����
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

//��staff�Ĺ��ʽ������� 1���� 0����
void F_order_float(int version){
    VIP *p = (VIP *)malloc(sizeof(VIP));
    p->next = head_staff;
    head_staff = p;//�ڱ����
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

void Inventory_Manage()  //������ҳ��
{
    system("cls");  //����
    char t;  //�洢�û�ѡ��
    toxy(12,6);
    printf("��ѡ����Ҫ���еĲ���:\n");
    printf("\t\t����esc��������ҳ��\n");
    printf("\t\t1.���ӿ��\n");
    printf("\t\t2.�鿴�������\n");
    printf("\t\t0.������һ��");
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
            printf("\t\t���벻�Ϲ棬����������");  //�ݹ�ֱ������Ϲ�
            Inventory_Manage();
            break;
    }
    return;
}

void Add_Inventory()  //��ӿ��
{
    int inventory_num;  //�����������
    int enter_id;  //�û������id
    char enter_choose;  //�û�ѡ����ӵ���
    Material* operate_material;  //������Ӳ�����ԭ���Ͻṹ���Ӧ��ָ��(id��Ӧ��ԭ���Ͻṹ��)
    system("cls");  //����
    toxy(12,6);
    printf("��ѡ��Ҫ��ӵ�ԭ��������:\n");
    inventory_num = Show_Inventory();
    char ch[Max_User_Enter];
    printf("\t\t��������Ʒ��Ӧ��id����ѡ��:");
    fflush(stdin);
    scanf("%s",ch);
    fflush(stdin);
    int x = Check_Number(ch,inventory_num,3);  //�û������ж�
    if(x>0)
    {
        enter_id = x;
        HideCursor(0);
    }
    else if(x==-1)
    {
        printf("\t\t�������ֳ��������");
        Add_Inventory();  //�ݹ�ֱ������Ϲ�
        return;
    }
    else if(x==-2)
    {
        printf("\t\t�������ݲ��Ϲ棬����������");
        Add_Inventory();  //�ݹ�ֱ������Ϲ�
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
    printf("\t\tID  \t ԭ������\tʣ������\n");
    while(m!=NULL)
    {
        printf("\t\t%d\t  %s  \t%.2f\n",m->id,m->name,m->inventory);
        max_id = m->id;
        m = m->next;
    }
    return max_id;
}

void Show_InventoryConsumption()  //�鿴�������
{
    char t;
    system("cls");  //����
    toxy(12,6);
    HideCursor(0);
    printf("��ѡ��鿴������ĵĹؼ���:\n");
    printf("\t\t����esc��������ҳ��\n");
    printf("\t\t1.����������������\n");
    printf("\t\t2.��ԭ����id��������\n");
    printf("\t\t0.������һ��\n");
    fflush(stdin);
    t = getch();  //�����û�ѡ��
    fflush(stdin);
    head_consumption = Create_ConsumptionLinkList();  //���û�ѡ��Ҫ�鿴�������֮�󴴽�һ�����������������˫������
    switch(t)  //�����û�ѡ�����ִ�ж�Ӧ����
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
            printf("\t\t���벻�Ϲ棬����������");
            Show_InventoryConsumption(); //�ݹ�ֱ������Ϲ�
            return;
    }
    return;
}

void Inventory_GetAdded(Material *m)  //�û�ѡ����ӵ����������û���ѡ��(1---1kg��2---����������3---����)
{
    char t;
    system("cls");
    toxy(12,6);
    printf("��ѡ����ӵ���:\n");
    printf("\t\t����esc��������ҳ��\n");
    printf("\t\t1.�����һ��(5kg)\n");
    printf("\t\t2.�������(10kg)\n");
    printf("\t\t0.������һ��\n");
    fflush(stdin);
    t = getch();
    fflush(stdin);
    switch(t)
    {
        case '1':
            m->inventory = 5.0;
            printf("\t\t%s����������һ��!!!",m->name);
            Sleep(1000);
            break;
        case '2':
            m->inventory = 10.0;
            printf("\t\t%s������������!!!",m->name);
            Sleep(1000);
            break;
        case '0':
            Add_Inventory();
            return;
        case 27:
            Default();
            return;
        default:
            printf("\t\t���벻�Ϲ棬����������");
            Inventory_GetAdded(m);  //�ݹ�ֱ��������ȷ
            return;
    }
    return;
}

void ChangeShowWay(Consumption *head,Consumption *tail,int status)  //����status��ֵ��ѡ���ǰ���������С���󣬻��ǰ��������Ӵ�С����ʾԭ���ϵ���������(status=1ʱ���С����,0ʱ�Ӵ�С)
{
    system("cls");
    toxy(12,6);
    Consumption *con;
    int i = 1; //������
    printf("ԭ���ϵ�������Ϊ:\n");
    if(status == 1)
    {
        con = head;
        while(con!=NULL)
        {
            printf("\t\t%d.%s\t%.2f\n",i,con->name,con->consumption);
            con = con->next;
            i++;
        }
        printf("\t\t��0������һ��\n");
        printf("\t\t��1�л�����ʽ\n");
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
        printf("\t\t��0������һ��\n");
        printf("\t\t��1�л�����ʽ\n");
    }
    return;
}
Consumption* SortLinkList(Consumption *h)  //����һ����hΪͷ���(����λ���)��˫������İ������������ĸ���(��С����),���ظø�����ͷ���
{
    if(h == NULL)
    {
        return NULL;
    }
    Consumption *soldier;  //������λ���
    soldier = (Consumption *)malloc(sizeof(Consumption));
    soldier->pre = NULL;
    soldier->next = NULL;
    Consumption *m;  //���������½��
    m = (Consumption *)malloc(sizeof(Consumption));
    m->consumption = h->consumption;  //�½�㸳ֵ
    m->id = h->id;
    strcpy(m->name,h->name);
    m->pre = soldier;  //��h���ĸ���m�������������λ������
    m->next = NULL;
    soldier->next = m;
    while(h->next!=NULL)  //��������
    {
        Consumption *q;  //��һ�����ĸ���
        Consumption *n;  //��m���˫ָ�������в�������
        m = soldier;
        n = soldier->next;
        q = (Consumption *)malloc(sizeof(Consumption));
        q->id = h->next->id;
        q->consumption = h->next->consumption;
        strcpy(q->name,h->next->name);
        q->next = NULL;
        q->pre = NULL;
        while(q->consumption > n->consumption&&n!=NULL)  //ѭ��Ϊ�½��q�ҵ�����λ��
        {
            m = m->next;
            n = n->next;
        }
        m->next = q;  //��q����m��n֮��
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

void Show_Inventory_ByConsumption()  //����ԭ������������ԭ���ϵ����Ľ�������չʾ
{
    char t;
    int i;  //ѭ������
    int status;  //���򷽷���־
    Consumption *head,*tail;  //ͷ��㣬β���
    head = SortLinkList(head_consumption->next);
    tail = head;
    while(tail->next!=NULL)  //��β��㸳ֵ
    {
        tail = tail->next;
    }
    status = 1;
    while(1)
    {
        ChangeShowWay(head,tail,status);  //Ĭ�ϰ���������С��������ʾ
        fflush(stdin);  //�û�����
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
                printf("\n\t\t���벻�Ϲ�");
                break;
        }
    }
}

void Show_Inventory_ById()  //����ԭ���ϵ�idչʾ�����ϵ�������
{
    char t;
    Consumption* con;
    system("cls");  //����
    toxy(12,6);
    HideCursor(0);
    printf("ԭ������\t������(kg)\n");
    con = head_consumption->next;
    while(con!=NULL)
    {
        printf("\t\t%d.%s\t%.2f\n",con->id,con->name,con->consumption);
        con = con->next;
    }
    printf("\t\t��0������һ��\n");
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

Consumption* Find_Consumption(int x,Consumption* h)  //��ԭ�������ĵ�˫���������ҵ�idΪx�Ľ�㲢����ָ�����ָ��
{
    Consumption *p=h;
    while(p->next!=NULL&&p->next->id!=x)
    {
        p=p->next;
    }
    return p->next;
}

Consumption* Create_ConsumptionLinkList()  //����ԭ�������ĵ�˫������,��ԭ����id��������,����˫���������λ���ָ��
{
    Consumption* h;  //��������
    Consumption* p,*q;
    Complete_Form* cf;
    Beverage* bever;
    Material* mater;
    int i,j,k,m;  //ѭ�����Ʊ���
    h = (Consumption*)malloc(sizeof(Consumption));  //hΪ��λ��㣬��������
    h->id = 0;
    h->next = NULL;
    h->pre = NULL;
    q = h;  //��q���д���˫������h���ڷ���
    for(i=0;i<Total_Material_Num;i++)
    {
        p = (Consumption*)malloc(sizeof(Consumption));  //����ջ���ռ�
        p->id = i+1;  //Ϊ�½��������ֵ
        strcpy(p->name,Find_Material(i+1,head_material)->name);
        p->consumption = 0.0;
        p->next = NULL;
        p->pre = q;
        q->next = p;  //���½������������
        q = p;
    }
    for(j=0;j<Total_Form_Num;j++)  //�������������ԭ��������������и�ֵ
    {
        cf = head_form->next;
        for(k=0;k<cf->number;k++)  //��������������ÿ��������
        {
            bever = Find_Beverage(cf->forms[k].beverage,head_beverage);  //�ҵ�ÿ����������Ʒ����
            for(m=0;m<bever->material_num;m++)  // 1.��Ʒԭ���ϵ�������
            {
                mater = Find_Material(bever->materials[m],head_material);  //�ҵ�����Ʒ�������ĵ�ԭ���ϣ���materָ��
                p = Find_Consumption(mater->id,h);  //�ҵ���ԭ���϶�Ӧ�����������
                p->consumption += mater->consume;  //������ԭ����ÿ�ε�������������consumption�����ϣ���ɸ�ֵ
            }
            switch(cf->forms[k].ingredients)  // 2.��ӵ�С�ϵ�ԭ����������
            {
                case 1:  //������������
                    mater = Find_Material(16,head_material);
                    p = Find_Consumption(16,h);
                    p->consumption += mater->consume;
                    break;
                case 2:  //Ҭ����������
                    mater = Find_Material(18,head_material);
                    p = Find_Consumption(18,h);
                    p->consumption += mater->consume;
                    break;
                case 3:  //�춹��������
                    mater = Find_Material(15,head_material);
                    p = Find_Consumption(15,h);
                    p->consumption += mater->consume;
                    break;
                case 4:  //�����������
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
//��˸��
void ShowMonthSales(int a);
void ShowSeasonSales(int a);
void ShowYearSales(int a);
void Check_Month_Sales();//��ȡhead_form��������Ľ��������ݡ����鱣����ӦID������������a[1]�����һ�����ϵ�����������������һ��bug���Ƕ������������ĵ�10��Ԫ��Ϊ2�������Ѿ�ɾ������� ����ô�Ͳ�չʾ��
void Check_Season_Sales();
void Check_Year_Sales();
void ShowSale();
void AddGoods();
void DeleteGoods();
void ManageGoods();
void ChangeGoods();//��ɾ��Ʒ���ࡣ

void ShowMonthSales(int a)
{
    printf("%d�·ݸ�����Ʒ���������£�\n",a);
    printf("---------------------------\n");

    for(int j=0;j<Total_Beverage_Num;j++)//�ȱ������飬�������Ϊ0����������������Ӧ�±����ֵ=0�����Ϊ0����Ļ���ô����Ŀռ��ȫ����ˣ���30���ռ��أ�������������id����û��������֣�Ҳ�������
    {
        //printf("idΪ%d��4������Ϊ��%d\n",j+1,month[4][j+1]);//��������id��Ӧ�����֡�
        Beverage*d=Find_Beverage(j+1,head_beverage);//���ص���ָ������ṹ���ָ��
        printf("%s��%d������Ϊ��%d\n",d->name,a,month[a][j+1]);
    }
}

void ShowSeasonSales(int a)
{
    char b[3];
    switch(a)
    {
        case 1:strcat(b,"����");break;
        case 2:strcat(b,"�ļ�");break;
        case 3:strcat(b,"�＾");break;
        case 4:strcat(b,"����");break;
    }

    printf("%s������Ʒ���������£�\n",b);
    printf("---------------------------\n");

    for(int j=0;j<Total_Beverage_Num;j++)//�ȱ������飬�������Ϊ0����������������Ӧ�±����ֵ=0�����Ϊ0����Ļ���ô����Ŀռ��ȫ����ˣ���30���ռ��أ�������������id����û��������֣�Ҳ�������
    {
        //printf("idΪ%d��4������Ϊ��%d\n",j+1,month[4][j+1]);//��������id��Ӧ�����֡�
        Beverage*d=Find_Beverage(j+1,head_beverage);//���ص���ָ������ṹ���ָ��
        printf("%s��%s����Ϊ��%d\n",d->name,b,season[a][j+1]);
    }
}

void ShowYearSales(int a)
{

    printf("���������Ʒ���������£�\n");
    printf("---------------------------\n");

    for(int j=0;j<Total_Beverage_Num;j++)//�ȱ������飬�������Ϊ0����������������Ӧ�±����ֵ=0�����Ϊ0����Ļ���ô����Ŀռ��ȫ����ˣ���30���ռ��أ�������������id����û��������֣�Ҳ�������
    {
        //printf("idΪ%d��4������Ϊ��%d\n",j+1,month[4][j+1]);//��������id��Ӧ�����֡�
        Beverage*d=Find_Beverage(j+1,head_beverage);//���ص���ָ������ṹ���ָ��
        printf("%s�ı�������Ϊ��%d\n",d->name,year[a][j+1]);
    }
}

void Check_Month_Sales()//��ȡhead_form��������Ľ��������ݡ����鱣����ӦID������������a[1]�����һ�����ϵ�����������������һ��bug���Ƕ������������ĵ�10��Ԫ��Ϊ2�������Ѿ�ɾ������� ����ô�Ͳ�չʾ��
{
    char a[3];
    int b;
    int line,row;//lineΪ�У�rowΪ��

    Complete_Form *c=head_form;

    while(c->next!=NULL)//���˼·������form��������н�㣬���µĶ�ά�����У��ѽ����������ڵ��·���Ϊ�У�����form�����ĵ��������ṹ�����飬������������id���ڶ�ά��������Ķ�Ӧid�ط�����++������
    {
        c=c->next;

        for(int i=0;i<c->number;i++)
        {
            row=c->forms[i].beverage;
            line=c->id.month;
            month[line][row]++;//line��ʾ�·ݣ�row��ʾ��Ʒid��������Ʒid�Ǵ�1��ʼ�ģ�����month[line][0]��ʲô��û��
        }
    }//���д���ά����Ӧ����check��������д����������������д�ȽϺá�

//    for(int k=1;k<13;k++)//˫��forѭ��������ά���飨1-12�£�
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
        printf("��������Ҫ�鿴���·ݣ�(0�˳�)\n");
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
            printf("����������0-12��\n");
            Sleep(2);
            system("CLS");
            Check_Month_Sales();
        }
        else
        {
            ShowMonthSales(b);
            printf("--------------------------\n");
            printf("\n�밴�����������\n");
            getch();
        }
    }

}

void Check_Season_Sales()
{
    char a[2];
    int b;
    int line,row;//lineΪ�У�rowΪ��

    Complete_Form *c=head_form;

    while(c->next!=NULL)//���˼·������form��������н�㣬���µĶ�ά�����У��ѽ����������ڵ��·���Ϊ�У�����form�����ĵ��������ṹ�����飬������������id���ڶ�ά��������Ķ�Ӧid�ط�����++������
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
    }//���д���ά����Ӧ����check��������д����������������д�ȽϺ�?


    while(1)
    {
        system("CLS");
        printf("��������Ҫ�鿴�ļ��ȣ�0���أ���\n1.����\t2.�ļ�\t3.�＾\t4.����\n");

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
            printf("����������0-4��\n");
            Sleep(3);
            system("CLS");

            Check_Season_Sales();
        }
        else
        {
            ShowSeasonSales(b);
            printf("--------------------------\n");
            printf("\n�밴�����������\n");
            getch();
        }
    }
}

void Check_Year_Sales()
{
    char a[2];
    int b;
    int line,row;//lineΪ�У�rowΪ��

    Complete_Form *c=head_form;

    while(c->next!=NULL)//���˼·������form��������н�㣬���µĶ�ά�����У��ѽ����������ڵ��·���Ϊ�У�����form�����ĵ��������ṹ�����飬������������id���ڶ�ά��������Ķ�Ӧid�ط�����++������
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
    }//���д���ά����Ӧ����check��������д����������������д�ȽϺ�?


    while(1)
    {
        system("CLS");
        printf("������1�鿴����ȵ���Ʒ��0���أ���\n");

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
            printf("����������0-1��\n");//����һ��bug���������֮��Ҫ����������0�ſ����˳���
            Sleep(2);
            system("CLS");
            Check_Year_Sales();//���ǵ�����֮��return����һ��Check_Year_Sales()
        }
        else
        {
            ShowYearSales(b);
            printf("--------------------------\n");
            printf("\n�밴�����������\n");
            getch();
        }
    }
}

void ModifyPrice()//������Ʒ�۸������˼·��1������Ʒ����2����2.1�ҵ�����ʾ�۸�����ʾ��Ϊ����Ǯ2.2û�ҵ�����ʾû�ҵ��Ƿ������3���������ñ�����3.1��������������Ʒ����˵�
{
    char a[21];
    Beverage *b;
    b=head_beverage;
    char c,d;//�����ж��Ƿ��

    system("CLS");
    printf("��ӭ�����޸ļ۸���棡(0����)\n");
    printf("---------------------------\n");
    printf("�������޸ļ۸����Ʒ���֣�\n");//���ԣ�台式奶茶

    fflush(stdin);
    scanf("%20s",a);
    fflush(stdin);

    if(strcmp(a,"0")==0)
    {
        ManageGoods();
    }

    //֮��ʼ����
    while(b->next!=NULL&&strcmp(b->name,a)!=0)
    {
        b=b->next;
    }
    if(strcmp(b->name,a)==0)
    {
        printf("�����%s�ļ۸��%f��Ϊ��\n",b->name,b->price);

        fflush(stdin);
        scanf("%f",&b->price);
        fflush(stdin);

        printf("�޸ĳɹ���\n�Ƿ������\n1.��  2.��\n");

        while(1)
        {
            fflush(stdin);
            scanf("%c",&c);
            fflush(stdin);

            switch(c)
            {
                case '1':ModifyPrice();
                case '2':printf("�����˳�����Ʒ����˵���\n");Sleep(2);ManageGoods();
                default :printf("������1��2��\n");break;
            }
        }
    }
    else
    {
        system("CLS");
        printf("�Ҳ�����Ʒ%s���Ƿ����\n1.��  2.��\n",a);


        while(1)
        {
            fflush(stdin);
            scanf("%c",&d);
            fflush(stdin);

            switch(d)
            {
                case '1':ModifyPrice();
                case '2':printf("�����˳�����Ʒ����˵���\n");Sleep(2);ManageGoods();
                default :printf("������1��2��\n");break;
            }
        }
    }

}

void AddGoods()
{
    char a[21];//�����������������
    char c;//c����ѡ���
    Beverage*b,*d,*e;//b�������أ�d�����洢��Ϣ,e���������������洢��Ϣ��ָ����Ҫ���ٿռ䣬�����غ���������Ĳ���Ҫ��
    e=b=Create_BeverageLinkList();

    printf("��ӭ���������Ʒ���棡(0����)\n");
    printf("-----------------------------\n");
    printf("��������Ʒ���ƣ�\n");//֮��Ҫ������һ���Ƿ���һ�������Ƶ���Ʒ���о���ʾ���ˣ�û�о���ʾ����Ʒ��

    fflush(stdin);
    scanf("%20s",a);
    fflush(stdin);

    if(strcmp(a,"0")==0)
    {
        ChangeGoods();
    }

    while(b->next!=NULL&&strcmp(a,b->name)!=0)//�á�台式奶茶�����ԣ���Ϊ������
    {
        b=b->next;
    }
    if(strcmp(a,b->name)==0)
    {
        printf("�Ѿ��д���Ʒ�ˣ�����Ҫ�ظ���ӣ�");
        Sleep(2);
    }
    else//û�д���Ʒ
    {
        while(1)
        {
            printf("����Ʒ��δ��ӣ�\n���������\n1.��\t2.��\n");

            fflush(stdin);
            scanf("%c",&c);//printf("���ԣ�\n");
            fflush(stdin);

            if(c=='1')
            {
                system("CLS");
                d=(Beverage*)malloc(sizeof(Beverage));

                strcpy(d->name,a);//����������ִ洢��a�����У�Ȼ��ֵ��d�����name
                printf("��������Ʒ%s�Ķ��ۣ�");

                fflush(stdin);
                scanf("%f",&d->price);//֮���d�󶨵��������棨id�Զ����ɣ�
                fflush(stdin);

                d->id=++Total_Beverage_Num;
                d->next=NULL;
                e->next=d;
                e=d;//��������Ժ������main�����е�����д�����ˡ�

                printf("��ӳɹ���\n");//֮��Ҫ����һ��id����Ҫд�뵽�ļ����棺��ү��˵���Ȱ������Ϣд�����ȫ���������棬Ȼ�������������д�뵽�ļ����档
                //���ID����ô�Զ����ɣ�->�����µĽ����ӵ�ȫ�����������֮ǰ��Ҫ��ָ���Ƶ�����ĩβ����ָ�����һλ����++�����Ͱ��Ǹ�++�������id�š�
                Sleep(2);
                break;
            }
            else if(c=='2')//δ�꣺����֮�⻹Ҫ��������Ƿ��ǳ���1��2֮�������
            {
                system("CLS");
                printf("�����˳��������棡\n");
                Sleep(2);
                ManageGoods();
            }
            else
            {
                printf("������1����2!\n");
                Sleep(2);
                system("CLS");
            }
        }
    }
}


void DeleteGoods()//������ƣ�1����ɾ����Ʒ������2����Ƿ��д���Ʒ3.1û������ʾû�д���Ʒ3.2������ʾ�ɴ���Ʒ���Ƿ�ɾ��4.1�ǵĻ���ʾɾ���ɹ�4.2��Ļ���ʾ���������ϼ�
{
    char a[20];//�洢��Ʒ���ơ�
    Beverage*b,*c;//b���������ҵ�Ŀ�꣬c����ָ��ǰһ����
    int temp=1;//������¼�ǵڼ�������ҵ�����Ʒ�ģ�temp-1Ҳ�����ƶ�����ɾ������ǰһ����
    char d,e;//����ѡ��
    c=head_beverage;
    b=c->next;
    printf("��ӭ����ɾ�����棡(0����)\n");
    printf("------------------------\n");
    printf("������ɾ����Ʒ�����ƣ�");//���԰�����台式奶茶
    fflush(stdin);
    scanf("%s",a);
    fflush(stdin);
    if(strcmp(a,"0")==0)//����һ��0���أ����ǻ���bug
    {
        ChangeGoods();
    }
    while(b!=NULL&&strcmp(b->name,a)!=0)//������Ǳ���ȫ�ֱ�������Ǹ������ˡ�
    {
        b=b->next;//printf("temp=%d\n",temp);
        temp++;
    }
    if(b!=NULL)//������ҵ��˵��������֪Ϊ�λ���if(strcmp(b->name,a)==0)�жϾ��޷�ִ��else?���ǻ���������ִ���ҵ���Ʒ
    {
        while(1)
        {
            system("CLS");
            printf("�ҵ��˴���Ʒ���Ƿ�ɾ����\n1.��\t2.��\n");//������ʵ���԰ѡ��ˡ�����ɽ�����֡�
            fflush(stdin);
            scanf("%c",&d);
            fflush(stdin);
            if(d=='1')
            {
                system("CLS");
                //ɾ���������
                for(int i=1;i<temp;i++)
                {
                    c=c->next;
                }
                c->next=b->next;
                free(b);

                printf("ɾ���ɹ�����������ɾ�����档\n");
                Sleep(2);

                break;
            }
            else if(d=='2')//δ�꣺����֮�⻹Ҫ��������Ƿ��ǳ���1��2֮�������
            {
                system("CLS");
                printf("�����˳����ϼ����棡\n");
                Sleep(2);
                DeleteGoods();
            }
            else
            {
                printf("������1����2!\n");
                Sleep(2);
                system("CLS");
            }
        }
    }
    else
    {
        system("CLS");
        printf("δ�ҵ�����Ʒ���Ƿ���Ҫ���ң�\n1.�� 2.��\n");
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
                printf("�����˳�����Ʒ����˵����棡\n");
                Sleep(2);
                ManageGoods();
            }
            else
            {
                printf("������1����2!\n");
                Sleep(2);
                system("CLS");
            }
        }
    }
}

void ChangeGoods()//��ɾ��Ʒ���ࡣ
{
    char a;//�����ж�
    while(1)
    {
        system("cls");
        printf("��ɾ��Ʒ������棺\n");
        printf("--------------------\n");
        printf("1.������Ʒ��\n");
        printf("2.ɾ����Ʒ��\n");
        printf("0.�˳����ϼ���\n");
        fflush(stdin);
        scanf("%c",&a);
        fflush(stdin);
        if(a=='0')
            ManageGoods();
        system("cls");
        switch(a)
        {
            case '1':AddGoods();break;//switchҲ��ѭ�����ģ�breakֻ������һ��ѭ��
            case '2':DeleteGoods();break;
            default: printf("������0��1��2��\n");Sleep(2);break;
        }

    }
}

void ShowSale()
{
    int c;
    while(1)
    {

        system("CLS");
        printf("��ӭ�����鿴�������棡\n");
        printf("----------------------\n");
        printf("1.�鿴��������\n");
        printf("2.�鿴����������\n");
        printf("3.�鿴��������\n");
        printf("������������1-3��\n");
        fflush(stdin);
        c = getch();
        fflush(stdin);
        switch(c){
            case 27: return;//27��ʾesc����
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
        printf("��Ʒ����˵�\n");
        printf("--------------\n");
        printf("1.������Ʒ�۸�\n");
        printf("2.��ɾ��Ʒ����\n");
        printf("--------------\n");
        printf("��������1 or 2\n");
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
        system(Normal_Color);;  //���ñ�����ɫ��������ɫ
        HideCursor(0); //���ع��
        if(ExplicitFlag)  //��ӡ��������
        {
            system("cls");
            toxy(28,8);
            printf("    ��ӭ����ʱ���ݰ�! ");
            toxy(26,9);
            printf("-----------------------------");
            toxy(26,10);
            printf("|                           |");
            toxy(26,11);
            printf("|");
            toxy(35,11);
            printf("1.�ǻ�Ա���");
            toxy(54,11);
            printf("|");
            toxy(26,12);
            printf("|                           |");
            toxy(26,13);
            printf("|");
            toxy(35,13);
            printf("2.��Ա��½");
            toxy(54,13);
            printf("|");
            toxy(26,14);
            printf("|                           |");
            toxy(26,15);
            printf("|");
            toxy(35,15);
            printf("3.��Աע��");
            toxy(54,15);
            printf("|");
            toxy(26,16);
            printf("|                           |");
            toxy(26,17);
            printf("|");
            toxy(35,17);
            printf("4.����Ա��½");
            toxy(54,17);
            printf("|");
            toxy(26,18);
            printf("|                           |");
            toxy(26,19);
            printf("|");
            toxy(35,19);
            printf("5.�ϰ��½");
            toxy(54,19);
            printf("|");
            toxy(26,20);
            printf("|                           |");
            toxy(26,21);
            printf("|");
            toxy(35,21);
            printf("ESC.�˳�");
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
        printf("ESC ��ҳ\n *  ������һ��");
        toxy(28, 8);
        printf("     ʱ���ݰ�ROOTϵͳ");
        toxy(22,9);
        printf("-------------------------------------");
        toxy(Lstrict + w, Upstrict);
        printf("| 1.Ա������ |");
        toxy(Lstrict + w, Upstrict+2);
        printf("| 2.������ |");
        toxy(Lstrict + w, Upstrict+4);
        printf("| 3.��Ʒ���� |");
        toxy(Lstrict + w, Upstrict+6);
        printf("| 4.�������� |");
        toxy(Lstrict + w, Upstrict+8);
        printf("| 5.��Ա���� |");
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
    HWND hWnd = GetConsoleWindow(); //���cmd���ھ��
    RECT rc;
    GetWindowRect(hWnd, &rc); //���cmd���ڶ�Ӧ����

    //�ı�cmd���ڷ��
    SetWindowLongPtr(hWnd,
                     GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX & ~WS_MINIMIZEBOX);
    //��Ϊ����漰���߿�ı䣬�������SetWindowPos��������Ч��
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
        char temp[100] = "���������룺";
        printf("ESC ��ҳ\n *  ������һ��");
        toxy(28, 8);
        printf("     ʱ���ݰɵ�½ϵͳ");
        toxy(22,9);
        printf("-------------------------------------");
        toxy(Lstrict, Upstrict);
        printf("���������룺");
        if(error)
        {
            Warning_Type;
            toxy(Lstrict, Upstrict+1);
            printf("�������");
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

void INIT()  //��ʼ�������ļ��ж������ݴ�����������
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

void SAVE()  //�������ִ�й����ж���������ĸĶ���������д���ļ�
{
    Rewrite_FormData();  //�����º�Ķ���������������д���ļ�
    Rewrite_BeverageData();  //�����º����Ʒ������������д���ļ�
    Rewrite_MaterialData();  //�����º��ԭ����������������д���ļ�
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

