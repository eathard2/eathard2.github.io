
#include"ziku.h"

#include "stm32f10x.h"
#include"stm32f10x_gpio.h"
#include"delay.h"

#define nCS_L {GPIOA->BRR = GPIO_Pin_0;}
#define nCS_H {GPIOA->BSRR = GPIO_Pin_0;}
#define CD_L  {GPIOA->BRR = GPIO_Pin_1;}
#define CD_H  {GPIOA->BSRR = GPIO_Pin_1;}
#define nRD_L  {GPIOA->BRR = GPIO_Pin_2;}
#define nRD_H  {GPIOA->BSRR = GPIO_Pin_2;}
#define nWR_L  {GPIOA->BRR = GPIO_Pin_3;}
#define nWR_H  {GPIOA->BSRR = GPIO_Pin_3;}
#define nRES_L {GPIOA->BRR = GPIO_Pin_4;}
#define nRES_H {GPIOA->BSRR = GPIO_Pin_4;}
#define NOP    write(0,0xc3);
typedef enum {ON=0,OFF=!ON}PartDisplay_Status;



//////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//函数声明区
//
///////////////////////////////////////////////////////////////////
void GPIO_Configure(void);
void LCD_Init(void);
void write(int flag,u8 data);
void delay_init(u8 SYSCLK);
void LCD_Clear(void);
typedef u8 uchar;
typedef u16 uint;
void ShowBMP(uchar x,uchar y,uchar width, uchar high,uchar bmp[]);
void write_bmp(int count,unsigned char a[]);	
void Set_Guangbiao_Address(u8 X,u8 Y);
void Set_Window_Display(int shang,int xia,int zuo,int you);
void SetPartDisplay(int DST,int DEN,PartDisplay_Status NewStatus);
void Draw24(unsigned char X,unsigned char Y,unsigned char *P);
void words(uchar x,uchar y,uchar type,uchar *p);	

/////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////


void GPIO_Configure()
{
			GPIO_InitTypeDef GPIO_InitStr;
	    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	    
	    GPIO_InitStr.GPIO_Pin=0xff;
	    GPIO_InitStr.GPIO_Mode=GPIO_Mode_Out_PP;
	    GPIO_InitStr.GPIO_Speed=GPIO_Speed_50MHz;
	    GPIO_Init(GPIOA,&GPIO_InitStr);
	    
	    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	    GPIO_InitStr.GPIO_Pin=0xff;
	    GPIO_InitStr.GPIO_Mode=GPIO_Mode_Out_PP;
	    GPIO_InitStr.GPIO_Speed=GPIO_Speed_50MHz;
	    GPIO_Init(GPIOC,&GPIO_InitStr);

}


void LCD_Init()
{
   
	 nCS_L;

	/*reset*/
	nRES_H;
	delay_ms(10);
	nRES_L;
	delay_ms(200);			//复位先低后高，中间时间稍微长些。
	nRES_H;
	
	delay_ms(200);
	write(0,0xe2);			//内部指令复位
	delay_ms(20);

//	/*power control*/
    write(0,0x2b);			//(6)  使用内部DC-DC
	  write(0,0x25);			//(5)  温度补偿系数，在高、低温时适当补偿
	  write(0,0xa2);      //(14) 设置行扫描速度
	  write(0,0xeb);			//(26) 1/12偏压比，硬件固化好的


write(0,0xa4);   //打开关闭所有点
write(0,0xa6);   //正显 反显

////设置要显示的总行数
//	  write(0,0xf1);      //（27）              // set com end,双字命令
//	  write(0,0x7f);                    //设置COM1-COM128可用  duty=1/128
////设置起始行和结束行		
//		write(0,0xf2);			// (28) 起始行
//	  write(0,0);				//0
//	  write(0,0xf3);			// (29) 结尾行
//	  write(0,127);			// 128
//关闭局部显示
    write(0,0x84);			//(11) 关闭局部显示
		
//设置Vlcd的值，和对比度有关		
		write(0,0x81);                                 //set vlcd value
		write(0,60);                                 //VLCD=0x49;


//设置映射关系
		write(0,0xc0|0x4);                                  //set LCD mapping control 行的传输方向设为COM160-Com1,  列传输方向设为SEG1-S240    MY,MX,lc0
//ARM地址指针控制
	  write(0,0x89);      //(12)                      //递增式的逐行扫描，
	  

                                  
//设置颜色模式
		write(0,0xd1);                                  //set color pattern
		write(0,0xd5);                                  //4K-color;
		write(0,0xad);			//(17) 打开显示，没有灰度级别，ON-OFF模式，数据显示格式

//*(19)n-line inversion*/
	  write(0,0xc8);
	  write(0,0x10);			


//	/*scroll line 与 RAM 的映射关系*/
//	  write(0,0x40);			//(8)
//	  write(0,0x50);			//     无卷动
	write(0,0xc4);			//(18) 行的传输方向设为COM160-C0,  列传输方向设为SEG1-S240
  write(0,0xa1);
//    write(0,0x90);			//(13):  无部分卷动
//	  write(0,0x00);


//xian shi shi chuang
write(0,0xf4);// qi shi lie 
write(0,0x00);//start from 0
write(0,0xf6);
write(0,0x4f);
write(0,0xf5);
write(0,0x00);
write(0,0xf7);
write(0,0x7f);

write(0,0xf8);
write(0,0x89);
write(0,0xad);


























}





void SetPartDisplay(int DST,int DEN,PartDisplay_Status NewStatus){
     if(DST<DEN){
		 	 
    //DST 设置部分显示的初始行
	  write(0,0xf2);
	  write(0,DST);
	  //DEN 设置部分显示的结束行
    write(0,0xf3);
	  write(0,DEN);
	  //使能或者失能
	  if(NewStatus)write(0,0x85);
	  if(!NewStatus)write(0,0x84);
		 }
}















void write(int flag,u8 data){
   
	 nWR_H;
	 nCS_L;
	 
	if(flag)CD_H;
	if(!flag)CD_L;
	
	nWR_L;
	delay_us(15);
	GPIOC->ODR=data;
	delay_us(15);
	nWR_H;
	//nWR_L;
	nCS_H;
	delay_us(15);
  //if(flag){delay_ms(2);};
}


void LCD_Clear(){
	uchar i,j;
	Set_Guangbiao_Address(0,0);
	//Set_Window_Display(0,127,0,79);

unsigned char dat1=0x00;
unsigned char dat2=0x00;	
	for(j=0;j<128;j++)
	{
		for(i=0;i<40;i++)
		{
			if(j%2==0)
			{
				write(1,dat1);
				write(1,dat1);
				write(1,dat1);   //    写3个字节，对应屏幕上面出现6个点
			}
			else
			{
				write(1,dat2);
				write(1,dat2);
				write(1,dat2);
			}
		}
	}
}




void ShowBMP(uchar x,uchar y,uchar width,uchar high,uchar bmp[]){


uint p;
uchar i,j,k,m,Ddata;
write(0,0xf4); write(0,0x25+x); // 设置操作窗口左边界
write(0,0xf5); write(0,y); // 设置操作窗口上边界
write(0,0xf6); write(0,0x25+width/3); // 设置操作窗口右边界
write(0,0xf7); write(0,high-1); // 设置操作窗口下边界
write(0,0xf8); // 设置窗口操作使能
x=x+0x25;
write(0,x&0x0f);write(0,0x10|(x>>4)); // 设置起始列地址
write(0,0x60+(y&0x0f)); write(0,0x70+(y>>4)); // 设置起始行地址
p=0; // 数组指针初始化
for(i=0;i<high;i++) // 循环行数数据量
{
for ( j=0;j<width/8;j++) // 循环字节数/行
{
k=bmp[p++]; // 取图形数据
for (m=0;m<4;m++) // 1 字节转换成 4 字节数据写入
{
switch (k&0xc0) // 根据数据位值设置显示数据
{
case 0x00 : Ddata=0x00;break;
case 0x40 : Ddata=0x0f;break;
case 0x80 : Ddata=0xf0;break;
case 0xc0 : Ddata=0xff;break;
}
write(1,Ddata); // 写入数据
k=k<<2;
}
}
switch (width%3)
{
case 0 : break;
case 1 : write(1,0x00);break; //补充余数据为 0
case 2 : write(1,0x00);write(1,0x00);break;// 补充余数据为 0
}
}

}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//功能：设置光标的行、列地址
//参数说明：X为所在的列数，对于宽度240dot的单色屏，取值在【0，79】，包含边界。
//         Y为行数，对于高度为128dot的单色屏，取值为【0，127】 ，包含边界。   
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Set_Guangbiao_Address(u8 X,u8 Y){

    write(0,0x89);
    u8 x0=0x00|(X&0x0f);
    u8 x1=0x10|((X&0x70)>>4);
    u8 y0=0x60|(Y&0x0f);
    u8 y1=0x70|((Y&0xf0)>>4);
	  write(0,x0);
    write(0,x1);
    write(0,y0);
    write(0,y1);
}





void Set_Window_Display(int shang,int xia,int zuo,int you){

//      if(shang>127)shang=127;
//	    if(xia>127)xia=127;
//	    if(zuo>79)zuo=79;
//	    if(you>79)you=79;
     write(0,0xf4); write(0,zuo); // 设置操作窗口左边界
     write(0,0xf5); write(0,shang); // 设置操作窗口上边界
     write(0,0xf6); write(0,you); // 设置操作窗口右边界
     write(0,0xf7); write(0,xia); // 设置操作窗口下边界
     write(0,0xf8);//窗口使能

}













void write_bmp(int count,unsigned char a[]){

	for(int i=0;i<count;i++){
unsigned char d3=a[i]&0xc0;
unsigned char d2=a[i]&0x30;
unsigned char d1=a[i]&0x0c;
unsigned char d0=a[i]&0xc3;
switch(d3){
			case 0xc0:
				write(1,0xff);break;
			case 0x80:
				write(1,0xf0);break;
			case 0x40:
				write(1,0x0f);break;
			case 0x00:
				write(1,0x00);break;
		}

switch(d2){
			case 0x03:
				write(1,0xff);break;
			case 0x02:
				write(1,0xf0);break;
			case 0x01:
				write(1,0x0f);break;
			case 0x00:
				write(1,0x00);break;
		}
switch(d1){
			case 0x0c:
				write(1,0xff);break;
			case 0x08:
				write(1,0xf0);break;
			case 0x04:
				write(1,0x0f);break;
			case 0x00:
				write(1,0x00);break;
		}
switch(d0){
			case 0x03:
				write(1,0xff);break;
			case 0x02:
				write(1,0xf0);break;
			case 0x01:
				write(1,0x0f);break;
			case 0x00:
				write(1,0x00);break;
		}}}


void words(u8 x,u8 y,u8 type,u8 *p)			//type=1,ascii;type=2,Chinese character
{
	u8 i,k,j,m,n,l,x0,dat0,dat1,dat2,dat3,dat4,dat5,dat6;
	x=00+x;//每个地址对应3个像素点【3个像素点对应RGB】
	x0=0x00|(x&0x0f);
	x=0x10|((x&0xf0)>>4);         //右移4位
	for(i=0;i<3;i++)//纵向 3个字节  3*8=24行
	{
		n=i*12*type;
		for(j=0;j<8;j++)//纵向 每个字节8个点
		{
			m=i*8+j;       //每8行的变换，每行3个字节
			write(0,0x89);//SET RAM ADDRESS CONTROL	 			
			write(0,x0);//Set Column Address LSB CA[3:0]
			write(0,x);//Set Column Address MSB CA[7:4]
			write(0,0x60|((y+m)&0x0f));//Set Row Address LSB RA [3:0]
			write(0,0x70|(((y+m)&0xf0)>>4));//Set Row Address MSB RA [7:4]

			for(k=0;k<2*type;k++)
//数据转换 每次转换完成6个点的转换，转换后6个点变成 2个12点，3个字节，字符是2次循环【12点】
			{
				l=k*6+n;
				dat6=0x01<<j;                    //取出对应位，j表示8位数据变量
				dat0=(*(p+l))&dat6;              //数组对应的数据
				dat0=dat0>>j;
				dat0<<=7;                        //转换后的  高四位

				dat1=(*(p+l+1))&dat6;             //数组对应的数据
				dat1=dat1>>j;
				dat1<<=3;                        //转换后的  低四位

				dat2=*(p+2+l)&dat6;              //数组对应的数据
				dat2=dat2>>j;
				dat2<<=7;                         //转换后的  高四位

				dat3=(*(p+3+l))&dat6;             //数组对应的数据
				dat3=dat3>>j;
				dat3<<=3;                          //转换后的  低四位

				dat4=(*(p+4+l))&dat6;             //数组对应的数据
				dat4=dat4>>j;
				dat4<<=7;                          //转换后的  高四位

				dat5=(*(p+5+l))&dat6;              //数组对应的数据
				dat5=dat5>>j;
				dat5<<=3;                          //转换后的  低四位

                                                                   //转换后的合成8位数
				write(1,dat0|dat1);
				write(1,dat2|dat3);
				write(1,dat4|dat5);

			}
		write(0,0x88);//SET RAM ADDRESS CONTROL，停止自加 		
		}
	}
	write(0,0x89);//SET RAM ADDRESS CONTROL
	
}







int main(){
delay_init(72);
GPIO_Configure();
nCS_H;
nWR_H;
nRD_H;
delay_ms(10);
LCD_Init();
	
unsigned char pic1[72]={
0x00,0x00,0x00,0x02,0x01,0x00,0x01,0x01,0x00,0x40,0xC1,0xFC,0x20,0x63,0xFE,0x30,
0x0C,0x00,0x18,0x70,0x00,0x03,0x80,0x00,0x0C,0x10,0x00,0x10,0x10,0x00,0x10,0x10,
0x00,0x10,0x10,0x00,0x10,0x10,0x00,0x10,0x10,0x00,0x1F,0xFF,0xFF,0x1F,0xFF,0xFE,
0x10,0x10,0x00,0x10,0x10,0x00,0x10,0x10,0x00,0x30,0x10,0x00,0x30,0x30,0x00,0x10,
0x30,0x00,0x00,0x10,0x00,0x00,0x00,0x00,

};
unsigned char pic0[72]={00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x00,0x00,0x18,0x00,0x00,
0x18,0x00,0x00,0x18,0x00,0x00,0x18,0x00,0x00,0x18,0x00,0x00,0x18,0x00,0x00,0x18,
0x00,0x01,0x18,0x00,0x01,0x18,0x80,0x01,0x18,0x80,0x01,0x99,0x80,0x00,0xFF,0x00,
0x00,0x7E,0x00,0x00,0x3C,0x00,0x00,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

unsigned char test[]={

0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
};

unsigned char yong[]={

0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x10,0x00,0x00,0x38,0x00,0x40,0x30,0x00,
0x60,0x60,0x00,0xE0,0xC0,0x00,0x31,0x80,0x00,0x16,0x00,0x00,0x18,0x00,0x22,0x60,
0x00,0x77,0xFF,0xFC,0xC7,0xFF,0xFE,0x82,0x00,0x07,0x02,0x20,0x06,0x02,0x78,0x04,
0x02,0x7E,0x04,0x02,0x23,0x80,0x00,0x20,0xE0,0x00,0x20,0x30,0x00,0x20,0x10,0x00,
0x20,0x08,0x00,0x00,0x04,0x00,0x00,0x00

};

unsigned char mu[]={

0x00,0x00,0x00,0x00,0x00,0x40,0x02,0x00,0x40,0x06,0x00,0xE0,0x0E,0x00,0xC0,0x06,
0x01,0x80,0x02,0x03,0x00,0x02,0x06,0x00,0x02,0x18,0x00,0x02,0x60,0x00,0x43,0x80,
0x00,0x7F,0xFF,0xFE,0xFF,0xFF,0xFF,0x03,0xE0,0x00,0x02,0x70,0x00,0x02,0x1C,0x00,
0x02,0x0E,0x00,0x02,0x03,0x00,0x02,0x01,0x80,0x02,0x00,0xC0,0x02,0x00,0x40,0x02,
0x00,0x20,0x00,0x00,0x10,0x00,0x00,0x00,
};

unsigned char tou[]={

0x00,0x00,0x00,0x00,0x08,0x00,0x06,0x0E,0x00,0x03,0x8C,0x00,0x01,0xCC,0x00,0x00,
0x8C,0x00,0x00,0x0C,0x00,0x0C,0x0C,0x00,0x07,0x0C,0x00,0x03,0x8C,0x00,0x01,0x0C,
0x00,0x00,0x0C,0x08,0x00,0x0C,0x1C,0x7F,0xFF,0xFE,0x00,0x18,0x00,0x00,0x18,0x00,
0x00,0x37,0x00,0x00,0x61,0x80,0x00,0xC0,0xE0,0x01,0x80,0x70,0x06,0x00,0x38,0x18,
0x00,0x18,0x60,0x00,0x10,0x00,0x00,0x00

};

unsigned char test1[]={

0x80,0x04,0x01,0x40,0x06,0x00,0x20,0x07,0x00,0x10,0x07,0x80,0x08,0x07,0xC0,0x04,
0x07,0xE0,0x02,0x07,0xF0,0x01,0x07,0x38,0x00,0x87,0x7C,0x00,0x47,0xFE,0x00,0x27,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xE8,0x00,0x7F,0xE4,0x00,0x3C,0xE2,0x00,
0x1C,0xE1,0x00,0x0F,0xE0,0x80,0x07,0xE0,0x40,0x03,0xE0,0x20,0x01,0xE0,0x10,0x00,
0xE0,0x08,0x00,0x60,0x04,0x80,0x20,0x03,



};

LCD_Clear();
    
//Set_Window_Display(30,53,10,17);
//Set_Guangbiao_Address(17,30);
	while(1)
	{
	
  write(0,0x88);
  delay_ms(2); 
	write(0,0x89);	
  Set_Guangbiao_Address(22,60);
	write(1,0xff);
  write(1,0xff);
  write(1,0xff);
  write(1,0xff);
  write(1,0xff);
  write(1,0xff);
  write(1,0xff);
  write(1,0xff);
  write(1,0xff);
  write(1,0xff);
  write(1,0xff);
  write(1,0xff);
  write(0,0x88);

  write(0,0x89);	
 // Set_Guangbiao_Address(20,64);
  write(0,0x89);
	write(1,0xff);
  //write(1,0x00);
  //write(1,0xff);
  write(0,0x88);
Draw24(20,20,test1);
//Draw24(9,20,tou);



//for (int m=0;m<5;m++){
//for(int s=0;s<10;s++){
//Draw24(8*s,24*m,&ziku[72*s]);

//}}

         while(1);
}
		
	

	

};




















void Draw24(unsigned char X,unsigned char Y,unsigned char *P)
{

    for(unsigned char i=0;i<24;i++){
          Set_Guangbiao_Address(X,Y++);

          //write(0,0xff);       
 
          for (unsigned char j=0;j<3;j++){
          unsigned char t=3*i;
          unsigned char temp=*(P+t+j);
          
          unsigned char d3=temp&0xc0;
          unsigned char d2=temp&0x30;
          unsigned char d1=temp&0x0c;
          unsigned char d0=temp&0x03;

             //write(1,0x00); 
               
            switch(d3){
			          case 0xc0:
				        write(1,0xff);break;
			          case 0x80:
				        write(1,0xf0);break;
		          	case 0x40:
				        write(1,0x0f);break;
			          case 0x00:
				        write(1,0x00);break;
		                    }

            switch(d2){
			          case 0x03:
				        write(1,0xff);break;
			          case 0x02:
				        write(1,0xf0);break;
			          case 0x01:
				        write(1,0x0f);break;
			          case 0x00:
				        write(1,0x00);break;
		                  }
           switch(d1){
			          case 0x0c:
				        write(1,0xff);break;
			          case 0x08:
				        write(1,0xf0);break;
		          	case 0x04:
				        write(1,0x0f);break;
			          case 0x00:
				        write(1,0x00);break;
		                 }
          switch(d0){
			         case 0x03:
				       write(1,0xff);break;
			         case 0x02:
				       write(1,0xf0);break;
		         	 case 0x01:
				       write(1,0x0f);break;
			         case 0x00:
				       write(1,0x00);break;
		                }
            //write(1,0x00);
           // write(1,0x00);

          } write(0,0x88);
    
    }


}






































