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

/* ȫ�ֱ��� */
int Total_Form_Num = 0;   //���붩���������ݵ�ʱ�����и��¸�ֵ
int Total_Beverage_Num = 0;  //��Ʒ��������Ŀ(������Ʒ�������ݵ�ʱ�����и��¸�ֵ)
int month[13][30];//����ֵ���ʼ��Ϊ0�𣿻ᡣӦ�ö���13����Ϊ��0�в������κ�������
int season[5][30];//Ӧ�ö���5����Ϊ��0�в������κμ��ڡ�
int year[2][30];

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
Beverage* head_beverage;
Complete_Form* head_form;
Material* head_material;
Consumption* head_consumption;

//
void Default();  //��ӭ����
void Order();  //��� --�ݶ�order�޷��أ����Ը�Ϊ����һ��ָ��ô�����������ָ��
void Guest();  //��ͨ�û�����
void Admin();  //����Ա����
void Root();  //boss����
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
Material* Consume_Inventory(Complete_Form* cf);  //����cfָ�򶩵�����Ҫ��ԭ���Ͽ��
void Add_Form(Complete_Form*cf,Complete_Form* h);  //��cfָ��Ķ�����ӵ�����������
void Rewrite_FormData();  //�����º�Ķ���������������д���ļ���
void Rewrite_BeverageData();  //�����º����Ʒ������������д���ļ���
void Rewrite_MaterialData();  //�����º��ԭ����������������д���ļ���

// void Clear_Price(Complete_Form* cf);  //��cfָ��Ķ��������е��������ļ۸�����

//��������
void toxy(int x,int y);//������ƶ���x��y���괦
void HideCursor(int x);//���ع��
char *HidePassword();//��������
int Check_Number(char ch[],int maxnum,int maxlen);  //��������Ƿ�Ϲ�(�Ϲ��Ҳ�����maxnu���ض�Ӧ������ֵ���Ϲ浫����max����-1�����Ϲ淵��-2)
// int Number_Check(char ch[]);

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
        return -1;
    }
}

/* void Default()  //��ӭ����
{
    char t;  //����ѡ��
    do
    {
        HideCursor(0); //���ع��
        system("color 72");
        toxy(28,8);
        printf(" ��ӭ����ʱ���ݰɣ���ѡ��ʹ�������");
        toxy(26,9);
        printf("-----------------------------");
        toxy(27,11);
        printf("1.����Ա 2.�û� 3.Boss");
        while(1)  //��ѭ��Ϊ��ֹ������������
        {
            t=getch();  //�����Ժ���
            if(t=='1')  //�����1��������¼����
            Admin();
            else if(t=='2')  //�����2�������ע�����
            Guest();
            else if(t=='3')
            Root();
        }  //����Ȳ���1Ҳ����2��3����ѭ������
    }while(1);  //��ԶΪ��
} */

/* �Ѳ��� */
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
        printf("Can't open the file!");
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
    if(h->next==NULL)
    {
        printf("������Ʒ...����ϵ����Ա�������");
    }
    else
    {
        printf("\t\t��Ʒid\t��Ʒ��\t��Ʒ�۸�(?)\n");
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
    Complete_Form* CF;  //����һ��ָ�����������ṹ��ָ��,�����װ���������������޸�
    CF = (Complete_Form*)malloc(sizeof(Complete_Form));
    CF->next = NULL;  //��ʼ��ָ����Ϊ��
    Create_ID(CF);
    Choose_FormNum(CF);
    int i = 1;  //����������ʾ��ǰΪ��i��
    // while(i<=CF->number)
    // {
    Choose_Beverage(CF,i);
    //     i++;
    // }
    // Choose_Payway(CF);
    return;
}

void Choose_FormNum(Complete_Form* cf)  //ѡ��cfָ�򶩵��ı���,���ر���
{
    system("cls");  //����
    toxy(12,6);
    HideCursor(1);
    printf("���뱭��(һ�����ɵ�%d��):",Max_Form_Num);
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
        printf("\t\t�������ֳ��������");
        Choose_FormNum(cf);  //�ݹ�ֱ������Ϲ�
    }
    else if(x==-2)
    {
        printf("\t\t�������ݲ��Ϲ棬����������");
        Choose_FormNum(cf);  //�ݹ�ֱ������Ϲ�
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
//         printf("���벻�Ϲ棬��1s����������");
//         //Sleep(1000);  //��ͣ1����ʾ��ʾ��Ϣfgfgfgfgcvcv
//         Choose_FormNum(cf);  //�ݹ�ֱ������Ϲ�
//     }
//     return;
// }
}

void Choose_Beverage(Complete_Form* cf,int i)
{
    system("cls");
    int c;  //�洢���ֺϹ���ķ���ֵ
    int Beverage_Num;  //����Ŀǰ��Ʒ������
    char ch[Max_User_Enter];  //�����û�����
    toxy(12,6);
    printf("��ѡ���%d����Ʒ:\n",i);
    Beverage_Num = Show_Beverages(head_beverage);
    printf("\t\t����0����ѡ����\n");
    printf("\t\t������Ʒ��id����ѡ��(���س�ȷ��)��");
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
        //Sleep(1000);  //��ͣ1����ʾ��ʾ��Ϣ
        Choose_Beverage(cf,i);  //��������
    }
    else
    {
        printf("\t\t�������ݲ��Ϲ棬��1s��������Ʒ��Ӧ��id");
        //Sleep(1000);  //��ͣ1����ʾ��ʾ��Ϣ
        Choose_Beverage(cf,i);  //��������
    }
    return;
}

void Choose_Ingredients(Complete_Form* cf,int i)
{
    system("cls");
    char t;
    toxy(12,6);
    HideCursor(0);
    printf("��ѡ���%d����Ҫ��ӵ�С��\n",i);
    printf("\t\t1.����--1Ԫ\n");
    printf("\t\t2.Ҭ��--1Ԫ\n");
    printf("\t\t3.�춹--2Ԫ\n");
    printf("\t\t4.����--2Ԫ\n");
    printf("\t\t5.�����С��\n");
    printf("\t\t0.����ѡ����\n");
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
        default:
            printf("\t\t���벻�Ϲ�,����������");
            //Sleep(1000);  //��ͣ1����ʾ��ʾ��Ϣ
            Choose_Ingredients(cf,i);  //�ݹ�ֱ������Ϲ�
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
    printf("��ѡ���%d���������¶�\n",i);
    printf("\t\t1.�ӱ�\n");
    printf("\t\t2.����\n");
    printf("\t\t3.΢��\n");
    printf("\t\t4.��\n");
    printf("\t\t0.����ѡ����");
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
            printf("\t\t���벻�Ϲ�,����������");
            //Sleep(1000);  //��ͣ1����ʾ��ʾ��Ϣ
            Choose_Temperature(cf,i);  //�ݹ�ֱ������Ϲ�
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
    printf("��ѡ���%d�������õص�\n",i);
    printf("\t\t1.��ʳ\n");
    printf("\t\t2.���(���)\n");
    printf("\t\t0.����ѡ����\n");
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
        default:
            printf("\t\t���벻�Ϲ�,����������");
            //Sleep(1000);  //��ͣ1����ʾ��ʾ��Ϣ
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
    system("cls");
    char t;
    toxy(12,6);
    HideCursor(0);
    Get_CpTotalprice(cf);
    printf("�����ܼ�Ϊ:%.1fԪ,��ѡ��֧����ʽ\n",cf->total_price);
    printf("\t\t1.�ֽ�֧��\n");
    printf("\t\t2.����֧��\n");
    if(cf->is_member>0)
    {
        printf("\t\t3.��Ա��֧��(85��)\n");
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
                printf("\t\tлл�ݹˣ�����");
                //Sleep(1000);  //��ͣ1����ʾ��ʾ��Ϣ
                break;
            case '2':
                cf->pay_way = 2;
                printf("\t\tлл�ݹˣ�����");
                //Sleep(1000);  //��ͣ1����ʾ��ʾ��Ϣ
                break;
            default:
                printf("\t\t���벻�Ϲ�,����������");
                //Sleep(1000);  //��ͣ1����ʾ��ʾ��Ϣ
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
                printf("\t\tлл�ݹˣ�����");
                Sleep(1000);  //��ͣ1����ʾ��ʾ��Ϣ
                break;
            case '2':
                cf->pay_way = 2;
                printf("\t\tлл�ݹˣ�����");
                Sleep(1000);  //��ͣ1����ʾ��ʾ��Ϣ
                break;
            case '3':
                cf->pay_way = 3;
                printf("\t\t��Ա��֧�����");
                break;
            default:
                printf("\t\t���벻�Ϲ�,����������");
                //Sleep(1000);  //��ͣ1����ʾ��ʾ��Ϣ
                Choose_Payway(cf);
                return;
        }
    }
    Material* ci_return = Consume_Inventory(cf);
    if(ci_return==NULL)  //���ݸö�����Ϣ�Կ����С����ġ�
    {                       //������㹻
        Add_Form(cf,head_form);  //���ö��������ӵ�����������
    }
    else  //ԭ���Ͽ�治�㣬������ʾ�����µ㵥
    {
        printf("%s��治�㣬����ϵ����Ա�������,������Զ����ص㵥ҳ��",ci_return->name);
        Sleep(2000);
        Order(); //���ص㵥ҳ��
        return;
    }
    // Out_put(cf);  //����������Ϣ��ֵ���,������ļ�(�������洢)
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

Material* Consume_Inventory(Complete_Form* cf)  //����cfָ�򶩵�����Ҫ��ԭ���Ͽ�棬����治�㣬�������ʾ���Զ�ȡ���ôζ��������ز����ԭ���϶�Ӧ�Ľṹ��ָ��;�����������ģ�����null
{
    int beverage_id;  //��Ʒid
    int material_id;  //ԭ����id
    Beverage* beverage_struct;  //��Ʒ�ṹ��
    Material* material_struct;  //ԭ���Ͻṹ��
    for(int i=0;i<cf->number;i++)  //�������е�������
    {
        beverage_id = cf->forms[i].beverage;  //�ҵ������������Ʒ��Ӧ��id
        beverage_struct = Find_Beverage(beverage_id,head_beverage);  //ͨ��id�ҵ���Ӧ����Ʒ�ṹ��
        for(int j=0;j<beverage_struct->material_num;j++)  //��������Ʒ�ṹ���ԭ�������࣬�����С����ġ�
        {
            material_id = beverage_struct->materials[j];  //�ҵ�����Ʒ��ԭ���϶�Ӧ��id
            material_struct = Find_Material(material_id,head_material);  //ͨ��ԭ���϶�Ӧ��id�ҵ���Ӧ�Ľṹ��
            if(material_struct->inventory >= material_struct->consume)  //������㹻
            {
                material_struct->inventory -= material_struct->consume;  //�ò��ϵĿ������ȥ�ò��ϵĵ���ʹ������Ϊ�����ġ�
            }
            else  //��治��
            {
                return material_struct;
            }
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

// void Clear_Price(Complete_Form* cf)  //��cfָ��Ķ��������е��������ļ۸�����
// {
//     int i;
//     for(i=0;i<Max_Form_Num;i++)
//     {
//         cf->forms[i].total_price = 0.0;
//     }
//     return;
// }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//��������
void Inventory_Manage();  //������ҳ��
void Add_Inventory();  //��ӿ��
int Show_Inventory();  //�鿴���п���ʣ����,���ؿ��������Ŀ
void Show_InventoryConsumption();  //�鿴�������
void Inventory_GetAdded(Material *m);  //�����û���ѡ������ӿ��
void Show_Inventory_ByConsumption();  //����ԭ������������ԭ���ϵ����Ľ�������չʾ
void ChangeShowWay(Consumption *head,Consumption *tail,int status);  //����status��ֵ��ѡ���ǰ���������С���󣬻��ǰ��������Ӵ�С����ʾԭ���ϵ���������(status=1ʱ���С����,0ʱ�Ӵ�С)
void Show_Inventory_ById();  //����ԭ���ϵ�idչʾ�����ϵ�������
float Get_TotalConsumption(int x);  //�������ж�����Ϣ,����ԭ����idΪx��ԭ���ϵ���������
Consumption* Create_ConsumptionLinkList();  //����ԭ�������ĵ�˫����������ʱ�����в�������(�������Ӵ�С)
Consumption* Find_Consumption(int x,Consumption* h);  //��ԭ�������ĵ�˫���������ҵ�idΪx�Ľ�㲢����ָ�����ָ��

void Inventory_Manage()  //������ҳ��
{
    system("cls");  //����
    char t;  //�洢�û�ѡ��
    toxy(12,6);
    printf("��ѡ����Ҫ���еĲ���:\n");
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
            // case '0':

            //     break;
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
    int b[3];
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
            main();
        }

        b=Check_Number(a,12,2);


        if(b==-1||b==-2)
        {
            printf("����������0-12��\n");
            sleep(2);
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
            main();
        }

        b=Check_Number(a,4,1);


        if(b==-1||b==-2)
        {
            printf("����������0-4��\n");
            sleep(3);
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
            main();
        }

        b=Check_Number(a,1,1);


        if(b==-1||b==-2)
        {
            printf("����������0-1��\n");//����һ��bug���������֮��Ҫ����������0�ſ����˳���
            sleep(2);
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
