/* 请把本文件拖到XP虚拟机的桌面上,
   再把它拖给虚拟机桌面上的TC快捷
   方式即可打开TC，最后选菜单
   Alt+R -> Run
   即可运行程序。
 */

#include <stdio.h>
#include <dos.h>
#include <bios.h>
extern unsigned char asc[256*16];

void draw_a_char(int x, int y, unsigned char *pasc)
{
   unsigned char far *p, far *py, far *px, color;
   int r, c;
   p = (unsigned char far *)0xA0000000 + (y*320+x);
   for(r=0; r<16; r++) /* 一共画16行 */
   {
      py = p+r*320; /* py->行的起点 */
      for(c=0; c<8; c++) /* 每行画8个点 */
      {
         if(*pasc & 1<<(7-c)) /* 从最高位到最低位逐位检测第7-c位是否为1, 若为1则画红点, 若为0则画蓝点 */
            color = 0x0C; /* 前景为高亮度红色 */
         else
            color = 0x01;  /* 背景为蓝色 */
         px = py+c; /* px->行中的某个点 */
         *px = color; /* 画一个点 */
      }
      pasc++;
   }
}

main()
{
   unsigned char buf[100], *pasc;
   int i;
   int x, y;
   gets(buf);          /* 输入一行字符 */
   _AX = 0x0013;
   geninterrupt(0x10); /* 切换到320*200*256色图形模式 */
   x = 0;
   y = 0;
   i = 0;
   while(buf[i] != '\0')
   {
      pasc = &asc[buf[i] << 4]; /* pasc->8*16点阵中的首字节,1个字节代表水平方向的8个点 */
      draw_a_char(x, y, pasc);
      x += 8;
      if(x >= 320)     /* 若x坐标超过屏幕右边界... */
      {
         x = 0;
         y += 16;      /* 则y坐标换到下一行 */
      }
      i++;
   }
   bioskey(0);         /* 敲任意键, 对应的汇编代码是mov ah,0; int 16h */
   _AX = 0x0003;
   geninterrupt(0x10); /* 切换到80*25文本模式 */
}

unsigned char asc[256*16] =
{ /* 8*16点阵ASCII字库, 每个字符占用16字节 */
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x7E,0x81,0xA5,0x81,0x81,0xBD,
   0x99,0x81,0x81,0x7E,0x00,0x00,0x00,0x00,
   0x00,0x00,0x7C,0xFE,0xFE,0xD6,0xFE,0xFE,
   0xBA,0xC6,0xFE,0x7C,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x6C,0xEE,0xFE,0xFE,0xFE,
   0xFE,0x7C,0x38,0x10,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x10,0x38,0x7C,0xFE,0x7C,
   0x38,0x10,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x10,0x38,0x38,0x10,0x6C,
   0xEE,0x6C,0x10,0x38,0x00,0x00,0x00,0x00,
   0x00,0x00,0x10,0x38,0x7C,0x7C,0xFE,0xFE,
   0xFE,0x6C,0x10,0x38,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x18,0x3C,0x3C,
   0x3C,0x18,0x00,0x00,0x00,0x00,0x00,0x00,
   0xFF,0xFF,0xFF,0xFF,0xFF,0xE7,0xC3,0xC3,
   0xC3,0xE7,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
   0x00,0x00,0x00,0x00,0x18,0x3C,0x66,0x66,
   0x66,0x3C,0x18,0x00,0x00,0x00,0x00,0x00,
   0xFF,0xFF,0xFF,0xFF,0xE7,0xC3,0x99,0x99,
   0x99,0xC3,0xE7,0xFF,0xFF,0xFF,0xFF,0xFF,
   0x00,0x00,0x1E,0x0E,0x1E,0x36,0x78,0xCC,
   0xCC,0xCC,0xCC,0x78,0x00,0x00,0x00,0x00,
   0x00,0x00,0x3C,0x66,0x66,0x66,0x3C,0x18,
   0x7E,0x18,0x18,0x18,0x00,0x00,0x00,0x00,
   0x00,0x00,0x1E,0x1A,0x1E,0x18,0x18,0x18,
   0x18,0x78,0xF8,0x70,0x00,0x00,0x00,0x00,
   0x00,0x00,0x3E,0x36,0x3E,0x36,0x36,0x76,
   0xF6,0x66,0x0E,0x1E,0x0C,0x00,0x00,0x00,
   0x00,0x00,0x18,0xDB,0x7E,0x3C,0x66,0x66,
   0x3C,0x7E,0xDB,0x18,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x80,0xE0,0xF0,0xFC,0xFE,
   0xFC,0xF0,0xE0,0x80,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x02,0x0E,0x3E,0x7E,0xFE,
   0x7E,0x3E,0x0E,0x02,0x00,0x00,0x00,0x00,
   0x00,0x00,0x18,0x3C,0x7E,0x18,0x18,0x18,
   0x18,0x7E,0x3C,0x18,0x00,0x00,0x00,0x00,
   0x00,0x00,0x66,0x66,0x66,0x66,0x66,0x66,
   0x66,0x00,0x66,0x66,0x00,0x00,0x00,0x00,
   0x00,0x00,0x7F,0xDB,0xDB,0xDB,0xDB,0x7B,
   0x1B,0x1B,0x1B,0x1B,0x00,0x00,0x00,0x00,
   0x00,0x00,0x7C,0xC6,0xC6,0x60,0x7C,0xF6,
   0xDE,0x7C,0x0C,0xC6,0xC6,0x7C,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0xFE,0xFE,0xFE,0xFE,0x00,0x00,0x00,0x00,
   0x00,0x00,0x18,0x3C,0x7E,0x18,0x18,0x18,
   0x7E,0x3C,0x18,0x7E,0x00,0x00,0x00,0x00,
   0x00,0x00,0x18,0x3C,0x7E,0x18,0x18,0x18,
   0x18,0x18,0x18,0x18,0x00,0x00,0x00,0x00,
   0x00,0x00,0x18,0x18,0x18,0x18,0x18,0x18,
   0x18,0x7E,0x3C,0x18,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x0C,0x0E,0xFF,
   0x0E,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x30,0x70,0xFE,
   0x70,0x30,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0xC0,
   0xC0,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x24,0x66,0xFF,
   0x66,0x24,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x10,0x38,0x38,0x38,0x7C,
   0x7C,0xFE,0xFE,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0xFE,0xFE,0x7C,0x7C,0x7C,
   0x38,0x38,0x10,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x18,0x3C,0x3C,0x3C,0x3C,0x18,
   0x18,0x00,0x18,0x18,0x00,0x00,0x00,0x00,
   0x00,0x36,0x36,0x36,0x36,0x14,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x6C,0x6C,0x6C,0xFE,0x6C,0x6C,
   0xFE,0x6C,0x6C,0x6C,0x00,0x00,0x00,0x00,
   0x00,0x00,0x18,0x18,0x7C,0xC6,0xC0,0x78,
   0x3C,0x06,0xC6,0x7C,0x18,0x18,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x62,0x66,0x0C,
   0x18,0x30,0x66,0xC6,0x00,0x00,0x00,0x00,
   0x00,0x00,0x38,0x6C,0x38,0x30,0x76,0x7E,
   0xCC,0xCC,0xCC,0x76,0x00,0x00,0x00,0x00,
   0x00,0x0C,0x0C,0x0C,0x18,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x0C,0x18,0x30,0x30,0x30,0x30,
   0x30,0x30,0x18,0x0C,0x00,0x00,0x00,0x00,
   0x00,0x00,0x30,0x18,0x0C,0x0C,0x0C,0x0C,
   0x0C,0x0C,0x18,0x30,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x6C,0x38,0xFE,
   0x38,0x6C,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x7E,
   0x18,0x18,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x0C,0x0C,0x0C,0x18,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x02,0x06,0x0C,0x18,
   0x30,0x60,0xC0,0x80,0x00,0x00,0x00,0x00,
   0x00,0x00,0x7C,0xC6,0xC6,0xCE,0xDE,0xF6,
   0xE6,0xC6,0xC6,0x7C,0x00,0x00,0x00,0x00,
   0x00,0x00,0x18,0x78,0x18,0x18,0x18,0x18,
   0x18,0x18,0x18,0x7E,0x00,0x00,0x00,0x00,
   0x00,0x00,0x7C,0xC6,0xC6,0x06,0x0C,0x18,
   0x30,0x60,0xC6,0xFE,0x00,0x00,0x00,0x00,
   0x00,0x00,0x7C,0xC6,0x06,0x06,0x3C,0x06,
   0x06,0x06,0xC6,0x7C,0x00,0x00,0x00,0x00,
   0x00,0x00,0x0C,0x1C,0x3C,0x6C,0xCC,0xCC,
   0xFE,0x0C,0x0C,0x1E,0x00,0x00,0x00,0x00,
   0x00,0x00,0xFE,0xC0,0xC0,0xC0,0xFC,0x06,
   0x06,0x06,0xC6,0x7C,0x00,0x00,0x00,0x00,
   0x00,0x00,0x7C,0xC6,0xC0,0xC0,0xFC,0xC6,
   0xC6,0xC6,0xC6,0x7C,0x00,0x00,0x00,0x00,
   0x00,0x00,0xFE,0xC6,0x06,0x0C,0x18,0x30,
   0x30,0x30,0x30,0x30,0x00,0x00,0x00,0x00,
   0x00,0x00,0x7C,0xC6,0xC6,0xC6,0x7C,0xC6,
   0xC6,0xC6,0xC6,0x7C,0x00,0x00,0x00,0x00,
   0x00,0x00,0x7C,0xC6,0xC6,0xC6,0xC6,0x7E,
   0x06,0x06,0xC6,0x7C,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x0C,0x0C,0x00,
   0x00,0x0C,0x0C,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x0C,0x0C,0x00,
   0x00,0x0C,0x0C,0x0C,0x18,0x00,0x00,0x00,
   0x00,0x00,0x00,0x0C,0x18,0x30,0x60,0xC0,
   0x60,0x30,0x18,0x0C,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0x00,
   0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x60,0x30,0x18,0x0C,0x06,
   0x0C,0x18,0x30,0x60,0x00,0x00,0x00,0x00,
   0x00,0x00,0x7C,0xC6,0xC6,0x0C,0x18,0x18,
   0x18,0x00,0x18,0x18,0x00,0x00,0x00,0x00,
   0x00,0x00,0x7C,0xC6,0xC6,0xC6,0xDE,0xDE,
   0xDE,0xDC,0xC0,0x7E,0x00,0x00,0x00,0x00,
   0x00,0x00,0x38,0x6C,0xC6,0xC6,0xC6,0xFE,
   0xC6,0xC6,0xC6,0xC6,0x00,0x00,0x00,0x00,
   0x00,0x00,0xFC,0x66,0x66,0x66,0x7C,0x66,
   0x66,0x66,0x66,0xFC,0x00,0x00,0x00,0x00,
   0x00,0x00,0x3C,0x66,0xC2,0xC0,0xC0,0xC0,
   0xC0,0xC2,0x66,0x3C,0x00,0x00,0x00,0x00,
   0x00,0x00,0xF8,0x6C,0x66,0x66,0x66,0x66,
   0x66,0x66,0x6C,0xF8,0x00,0x00,0x00,0x00,
   0x00,0x00,0xFE,0x66,0x60,0x64,0x7C,0x64,
   0x60,0x60,0x66,0xFE,0x00,0x00,0x00,0x00,
   0x00,0x00,0xFE,0x66,0x60,0x64,0x7C,0x64,
   0x60,0x60,0x60,0xF0,0x00,0x00,0x00,0x00,
   0x00,0x00,0x7C,0xC6,0xC6,0xC0,0xC0,0xC0,
   0xCE,0xC6,0xC6,0x7C,0x00,0x00,0x00,0x00,
   0x00,0x00,0xC6,0xC6,0xC6,0xC6,0xFE,0xC6,
   0xC6,0xC6,0xC6,0xC6,0x00,0x00,0x00,0x00,
   0x00,0x00,0x3C,0x18,0x18,0x18,0x18,0x18,
   0x18,0x18,0x18,0x3C,0x00,0x00,0x00,0x00,
   0x00,0x00,0x3C,0x18,0x18,0x18,0x18,0x18,
   0x18,0xD8,0xD8,0x70,0x00,0x00,0x00,0x00,
   0x00,0x00,0xC6,0xC6,0xCC,0xD8,0xF0,0xF0,
   0xD8,0xCC,0xC6,0xC6,0x00,0x00,0x00,0x00,
   0x00,0x00,0xF0,0x60,0x60,0x60,0x60,0x60,
   0x60,0x62,0x66,0xFE,0x00,0x00,0x00,0x00,
   0x00,0x00,0xC6,0xC6,0xEE,0xEE,0xFE,0xD6,
   0xD6,0xD6,0xC6,0xC6,0x00,0x00,0x00,0x00,
   0x00,0x00,0xC6,0xC6,0xE6,0xE6,0xF6,0xDE,
   0xCE,0xCE,0xC6,0xC6,0x00,0x00,0x00,0x00,
   0x00,0x00,0x7C,0xC6,0xC6,0xC6,0xC6,0xC6,
   0xC6,0xC6,0xC6,0x7C,0x00,0x00,0x00,0x00,
   0x00,0x00,0xFC,0x66,0x66,0x66,0x66,0x7C,
   0x60,0x60,0x60,0xF0,0x00,0x00,0x00,0x00,
   0x00,0x00,0x7C,0xC6,0xC6,0xC6,0xC6,0xC6,
   0xC6,0xD6,0xD6,0x7C,0x06,0x00,0x00,0x00,
   0x00,0x00,0xFC,0x66,0x66,0x66,0x7C,0x78,
   0x6C,0x66,0x66,0xE6,0x00,0x00,0x00,0x00,
   0x00,0x00,0x7C,0xC6,0xC0,0xC0,0x70,0x1C,
   0x06,0x06,0xC6,0x7C,0x00,0x00,0x00,0x00,
   0x00,0x00,0x7E,0x5A,0x18,0x18,0x18,0x18,
   0x18,0x18,0x18,0x3C,0x00,0x00,0x00,0x00,
   0x00,0x00,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,
   0xC6,0xC6,0xC6,0x7C,0x00,0x00,0x00,0x00,
   0x00,0x00,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,
   0xC6,0x6C,0x38,0x10,0x00,0x00,0x00,0x00,
   0x00,0x00,0xC6,0xC6,0xC6,0xD6,0xD6,0xD6,
   0xFE,0xEE,0xC6,0xC6,0x00,0x00,0x00,0x00,
   0x00,0x00,0xC6,0xC6,0xC6,0x6C,0x38,0x38,
   0x6C,0xC6,0xC6,0xC6,0x00,0x00,0x00,0x00,
   0x00,0x00,0x66,0x66,0x66,0x66,0x66,0x3C,
   0x18,0x18,0x18,0x3C,0x00,0x00,0x00,0x00,
   0x00,0x00,0xFE,0xC6,0x86,0x0C,0x18,0x30,
   0x60,0xC2,0xC6,0xFE,0x00,0x00,0x00,0x00,
   0x00,0x00,0x7C,0x60,0x60,0x60,0x60,0x60,
   0x60,0x60,0x60,0x7C,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x80,0xC0,0x60,0x30,
   0x18,0x0C,0x06,0x02,0x00,0x00,0x00,0x00,
   0x00,0x00,0x7C,0x0C,0x0C,0x0C,0x0C,0x0C,
   0x0C,0x0C,0x0C,0x7C,0x00,0x00,0x00,0x00,
   0x00,0x10,0x38,0x6C,0xC6,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,
   0x00,0x18,0x18,0x18,0x0C,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x78,0x0C,0x7C,
   0xCC,0xCC,0xDC,0x76,0x00,0x00,0x00,0x00,
   0x00,0x00,0xE0,0x60,0x60,0x7C,0x66,0x66,
   0x66,0x66,0x66,0xFC,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x7C,0xC6,0xC0,
   0xC0,0xC0,0xC6,0x7C,0x00,0x00,0x00,0x00,
   0x00,0x00,0x1C,0x0C,0x0C,0x7C,0xCC,0xCC,
   0xCC,0xCC,0xCC,0x7E,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x7C,0xC6,0xC6,
   0xFE,0xC0,0xC6,0x7C,0x00,0x00,0x00,0x00,
   0x00,0x00,0x1C,0x36,0x30,0x30,0xFC,0x30,
   0x30,0x30,0x30,0x78,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x76,0xCE,0xC6,
   0xC6,0xCE,0x76,0x06,0xC6,0x7C,0x00,0x00,
   0x00,0x00,0xE0,0x60,0x60,0x7C,0x66,0x66,
   0x66,0x66,0x66,0xE6,0x00,0x00,0x00,0x00,
   0x00,0x00,0x18,0x18,0x00,0x38,0x18,0x18,
   0x18,0x18,0x18,0x3C,0x00,0x00,0x00,0x00,
   0x00,0x00,0x0C,0x0C,0x00,0x1C,0x0C,0x0C,
   0x0C,0x0C,0x0C,0xCC,0xCC,0x78,0x00,0x00,
   0x00,0x00,0xE0,0x60,0x60,0x66,0x66,0x6C,
   0x78,0x6C,0x66,0xE6,0x00,0x00,0x00,0x00,
   0x00,0x00,0x18,0x18,0x18,0x18,0x18,0x18,
   0x18,0x18,0x18,0x1C,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x6C,0xFE,0xD6,
   0xD6,0xC6,0xC6,0xC6,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0xDC,0x66,0x66,
   0x66,0x66,0x66,0x66,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x7C,0xC6,0xC6,
   0xC6,0xC6,0xC6,0x7C,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0xDC,0x66,0x66,
   0x66,0x66,0x7C,0x60,0x60,0xF0,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x76,0xCC,0xCC,
   0xCC,0xCC,0x7C,0x0C,0x0C,0x1E,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0xDC,0x66,0x60,
   0x60,0x60,0x60,0xF0,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x7C,0xC6,0xC0,
   0x7C,0x06,0xC6,0x7C,0x00,0x00,0x00,0x00,
   0x00,0x00,0x30,0x30,0x30,0xFC,0x30,0x30,
   0x30,0x30,0x36,0x1C,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0xCC,0xCC,0xCC,
   0xCC,0xCC,0xCC,0x76,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0xC6,0xC6,0xC6,
   0xC6,0x6C,0x38,0x10,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0xC6,0xC6,0xD6,
   0xD6,0xD6,0xFE,0x6C,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0xC6,0xC6,0x6C,
   0x38,0x6C,0xC6,0xC6,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0xC6,0xC6,0xC6,
   0xC6,0xCE,0x76,0x06,0xC6,0x7C,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0xFE,0x86,0x0C,
   0x18,0x30,0x62,0xFE,0x00,0x00,0x00,0x00,
   0x00,0x00,0x0E,0x18,0x18,0x18,0x70,0x18,
   0x18,0x18,0x18,0x0E,0x00,0x00,0x00,0x00,
   0x00,0x00,0x18,0x18,0x18,0x18,0x00,0x18,
   0x18,0x18,0x18,0x18,0x00,0x00,0x00,0x00,
   0x00,0x00,0x70,0x18,0x18,0x18,0x0E,0x18,
   0x18,0x18,0x18,0x70,0x00,0x00,0x00,0x00,
   0x00,0x00,0x76,0xDC,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x10,0x38,0x38,
   0x6C,0x6C,0xFE,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x3C,0x66,0xC0,0xC0,0xC0,0xC6,
   0x66,0x3C,0x18,0x0C,0xCC,0x38,0x00,0x00,
   0x00,0x00,0xC6,0x00,0x00,0xC6,0xC6,0xC6,
   0xC6,0xC6,0xCE,0x76,0x00,0x00,0x00,0x00,
   0x00,0x0C,0x18,0x30,0x00,0x7C,0xC6,0xC6,
   0xFE,0xC0,0xC6,0x7C,0x00,0x00,0x00,0x00,
   0x00,0x30,0x78,0xCC,0x00,0x78,0x0C,0x7C,
   0xCC,0xCC,0xDC,0x76,0x00,0x00,0x00,0x00,
   0x00,0x00,0xCC,0x00,0x00,0x78,0x0C,0x7C,
   0xCC,0xCC,0xDC,0x76,0x00,0x00,0x00,0x00,
   0x00,0x60,0x30,0x18,0x00,0x78,0x0C,0x7C,
   0xCC,0xCC,0xDC,0x76,0x00,0x00,0x00,0x00,
   0x00,0x38,0x6C,0x38,0x00,0x78,0x0C,0x7C,
   0xCC,0xCC,0xDC,0x76,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x7C,0xC6,0xC0,0xC0,
   0xC6,0x7C,0x18,0x0C,0x6C,0x38,0x00,0x00,
   0x00,0x30,0x78,0xCC,0x00,0x7C,0xC6,0xC6,
   0xFE,0xC0,0xC6,0x7C,0x00,0x00,0x00,0x00,
   0x00,0x00,0xCC,0x00,0x00,0x7C,0xC6,0xC6,
   0xFE,0xC0,0xC6,0x7C,0x00,0x00,0x00,0x00,
   0x00,0x30,0x18,0x0C,0x00,0x7C,0xC6,0xC6,
   0xFE,0xC0,0xC6,0x7C,0x00,0x00,0x00,0x00,
   0x00,0x00,0x66,0x00,0x00,0x38,0x18,0x18,
   0x18,0x18,0x18,0x3C,0x00,0x00,0x00,0x00,
   0x00,0x18,0x3C,0x66,0x00,0x38,0x18,0x18,
   0x18,0x18,0x18,0x3C,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x38,0x18,0x18,
   0x18,0x18,0x18,0x3C,0x00,0x00,0x00,0x00,
   0x00,0xC6,0x00,0x38,0x6C,0xC6,0xC6,0xC6,
   0xFE,0xC6,0xC6,0xC6,0x00,0x00,0x00,0x00,
   0x38,0x6C,0x38,0x00,0x38,0x6C,0xC6,0xC6,
   0xFE,0xC6,0xC6,0xC6,0x00,0x00,0x00,0x00,
   0x0C,0x18,0x30,0x00,0xFE,0x60,0x60,0x7C,
   0x60,0x60,0x60,0xFE,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x66,0xDB,0x1B,0x7F,
   0xD8,0xD8,0xDF,0x76,0x00,0x00,0x00,0x00,
   0x00,0x00,0x7E,0xD8,0xD8,0xD8,0xD8,0xFE,
   0xD8,0xD8,0xD8,0xDE,0x00,0x00,0x00,0x00,
   0x00,0x30,0x78,0xCC,0x00,0x7C,0xC6,0xC6,
   0xC6,0xC6,0xC6,0x7C,0x00,0x00,0x00,0x00,
   0x00,0x00,0xC6,0x00,0x00,0x7C,0xC6,0xC6,
   0xC6,0xC6,0xC6,0x7C,0x00,0x00,0x00,0x00,
   0x00,0x30,0x18,0x0C,0x00,0x7C,0xC6,0xC6,
   0xC6,0xC6,0xC6,0x7C,0x00,0x00,0x00,0x00,
   0x00,0x30,0x78,0xCC,0x00,0xC6,0xC6,0xC6,
   0xC6,0xC6,0xCE,0x76,0x00,0x00,0x00,0x00,
   0x00,0x60,0x30,0x18,0x00,0xC6,0xC6,0xC6,
   0xC6,0xC6,0xCE,0x76,0x00,0x00,0x00,0x00,
   0x00,0x18,0x00,0x3C,0x18,0x18,0x18,0x18,
   0x18,0x18,0x18,0x3C,0x00,0x00,0x00,0x00,
   0x00,0xC6,0x00,0x7C,0xC6,0xC6,0xC6,0xC6,
   0xC6,0xC6,0xC6,0x7C,0x00,0x00,0x00,0x00,
   0x00,0xC6,0x00,0xC6,0xC6,0xC6,0xC6,0xC6,
   0xC6,0xC6,0xC6,0x7C,0x00,0x00,0x00,0x00,
   0x00,0x00,0x18,0x18,0x7C,0xC6,0xC0,0xC0,
   0xC6,0x7C,0x18,0x18,0x00,0x00,0x00,0x00,
   0x00,0x38,0x6C,0x60,0x60,0xF0,0x60,0x60,
   0x60,0x66,0xF6,0x6C,0x00,0x00,0x00,0x00,
   0x00,0x66,0x66,0x66,0x66,0x3C,0x18,0x7E,
   0x18,0x3C,0x18,0x18,0x00,0x00,0x00,0x00,
   0x00,0x00,0x3E,0x63,0x63,0x30,0x1C,0x06,
   0x63,0x63,0x3E,0x00,0x1C,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x3E,0x63,0x38,
   0x0E,0x63,0x3E,0x00,0x1C,0x00,0x00,0x00,
   0x00,0x0C,0x18,0x30,0x00,0x78,0x0C,0x7C,
   0xCC,0xCC,0xDC,0x76,0x00,0x00,0x00,0x00,
   0x00,0x0C,0x18,0x30,0x00,0x38,0x18,0x18,
   0x18,0x18,0x18,0x3C,0x00,0x00,0x00,0x00,
   0x00,0x0C,0x18,0x30,0x00,0x7C,0xC6,0xC6,
   0xC6,0xC6,0xC6,0x7C,0x00,0x00,0x00,0x00,
   0x00,0x18,0x30,0x60,0x00,0xCC,0xCC,0xCC,
   0xCC,0xCC,0xDC,0x76,0x00,0x00,0x00,0x00,
   0x00,0x00,0x76,0xDC,0x00,0xBC,0x66,0x66,
   0x66,0x66,0x66,0xE6,0x00,0x00,0x00,0x00,
   0x00,0x76,0xDC,0x00,0xC6,0xC6,0xE6,0xF6,
   0xDE,0xCE,0xC6,0xC6,0x00,0x00,0x00,0x00,
   0x00,0x21,0x1E,0x00,0x1E,0x33,0x60,0x60,
   0x67,0x63,0x33,0x1D,0x00,0x00,0x00,0x00,
   0x00,0x42,0x3C,0x00,0x3B,0x66,0x66,0x66,
   0x3E,0x06,0x66,0x3C,0x00,0x00,0x00,0x00,
   0x00,0x00,0x30,0x30,0x00,0x30,0x30,0x30,
   0x60,0xC6,0xC6,0x7C,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7E,
   0x60,0x60,0x60,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7E,
   0x06,0x06,0x06,0x00,0x00,0x00,0x00,0x00,
   0x00,0x60,0x60,0x62,0x66,0x6C,0x18,0x30,
   0x60,0xDC,0x36,0x0C,0x18,0x3E,0x00,0x00,
   0x00,0x60,0x60,0x62,0x66,0x6C,0x18,0x36,
   0x6E,0xDE,0x36,0x7E,0x06,0x06,0x00,0x00,
   0x00,0x00,0x18,0x18,0x00,0x18,0x18,0x3C,
   0x3C,0x3C,0x3C,0x18,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x36,0x6C,0xD8,
   0x6C,0x36,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0xD8,0x6C,0x36,
   0x6C,0xD8,0x00,0x00,0x00,0x00,0x00,0x00,
   0x11,0x44,0x11,0x44,0x11,0x44,0x11,0x44,
   0x11,0x44,0x11,0x44,0x11,0x44,0x11,0x44,
   0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,
   0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,
   0xDD,0x77,0xDD,0x77,0xDD,0x77,0xDD,0x77,
   0xDD,0x77,0xDD,0x77,0xDD,0x77,0xDD,0x77,
   0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
   0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
   0x18,0x18,0x18,0x18,0x18,0x18,0x18,0xF8,
   0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
   0x18,0x18,0x18,0x18,0x18,0xF8,0x18,0xF8,
   0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
   0x36,0x36,0x36,0x36,0x36,0x36,0x36,0xF6,
   0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,
   0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,
   0x00,0x00,0x00,0x00,0x00,0xF8,0x18,0xF8,
   0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
   0x36,0x36,0x36,0x36,0x36,0xF6,0x06,0xF6,
   0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,
   0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,
   0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,
   0x00,0x00,0x00,0x00,0x00,0xFE,0x06,0xF6,
   0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,
   0x36,0x36,0x36,0x36,0x36,0xF6,0x06,0xFE,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x36,0x36,0x36,0x36,0x36,0x36,0x36,0xFE,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x18,0x18,0x18,0x18,0x18,0xF8,0x18,0xF8,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF8,
   0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
   0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x1F,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x18,0x18,0x18,0x18,0x18,0x18,0x18,0xFF,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,
   0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
   0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x1F,
   0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x18,0x18,0x18,0x18,0x18,0x18,0x18,0xFF,
   0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
   0x18,0x18,0x18,0x18,0x18,0x1F,0x18,0x1F,
   0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
   0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x37,
   0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,
   0x36,0x36,0x36,0x36,0x36,0x37,0x30,0x3F,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x3F,0x30,0x37,
   0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,
   0x36,0x36,0x36,0x36,0x36,0xF7,0x00,0xFF,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0xF7,
   0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,
   0x36,0x36,0x36,0x36,0x36,0x37,0x30,0x37,
   0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,
   0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0xFF,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x36,0x36,0x36,0x36,0x36,0xF7,0x00,0xF7,
   0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,
   0x18,0x18,0x18,0x18,0x18,0xFF,0x00,0xFF,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x36,0x36,0x36,0x36,0x36,0x36,0x36,0xFF,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0xFF,
   0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,
   0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,
   0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x3F,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x18,0x18,0x18,0x18,0x18,0x1F,0x18,0x1F,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x1F,0x18,0x1F,
   0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,
   0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,
   0x36,0x36,0x36,0x36,0x36,0x36,0x36,0xFF,
   0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,
   0x18,0x18,0x18,0x18,0x18,0xFF,0x18,0xFF,
   0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
   0x18,0x18,0x18,0x18,0x18,0x18,0x18,0xF8,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,
   0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
   0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
   0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
   0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,
   0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,
   0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,
   0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,
   0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x76,0xDC,0xD8,0xD8,
   0xD8,0xD8,0xDC,0x76,0x00,0x00,0x00,0x00,
   0x00,0x00,0x78,0xCC,0xCC,0xD8,0xFC,0xC6,
   0xC6,0xC6,0xC6,0xCC,0x00,0x00,0x00,0x00,
   0x00,0x00,0xFE,0x66,0x62,0x60,0x60,0x60,
   0x60,0x60,0x60,0x60,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0xFE,0x6C,0x6C,
   0x6C,0x6C,0x6C,0x6C,0x00,0x00,0x00,0x00,
   0x00,0x00,0xFE,0xC6,0x62,0x30,0x18,0x18,
   0x30,0x62,0xC6,0xFE,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x7E,0xD8,0xCC,
   0xCC,0xCC,0xD8,0x70,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x66,0x66,0x66,0x66,
   0x66,0x7C,0x60,0xC0,0x80,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x76,0xDC,0x18,
   0x18,0x18,0x18,0x18,0x00,0x00,0x00,0x00,
   0x00,0x00,0xFE,0x38,0x38,0x6C,0xC6,0xC6,
   0x6C,0x38,0x38,0xFE,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x38,0x6C,0xC6,0xC6,0xFE,
   0xC6,0xC6,0x6C,0x38,0x00,0x00,0x00,0x00,
   0x00,0x00,0x38,0x6C,0xC6,0xC6,0xC6,0xC6,
   0x6C,0x6C,0x6C,0xEE,0x00,0x00,0x00,0x00,
   0x00,0x00,0x3E,0x60,0x60,0x3C,0x66,0xC6,
   0xC6,0xC6,0xCC,0x78,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x7E,0xDB,0xDB,
   0xDB,0x7E,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x02,0x06,0x7C,0xCE,0xDE,0xF6,
   0xF6,0x7C,0x60,0xC0,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x1C,0x30,0x60,0x60,0x7C,
   0x60,0x60,0x30,0x1C,0x00,0x00,0x00,0x00,
   0x00,0x00,0x7C,0xC6,0xC6,0xC6,0xC6,0xC6,
   0xC6,0xC6,0xC6,0xC6,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0xFE,0x00,0x00,0xFE,
   0x00,0x00,0xFE,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x18,0x18,0x7E,0x18,
   0x18,0x00,0x00,0x7E,0x00,0x00,0x00,0x00,
   0x00,0x00,0x30,0x18,0x0C,0x06,0x0C,0x18,
   0x30,0x00,0x00,0x7E,0x00,0x00,0x00,0x00,
   0x00,0x00,0x0C,0x18,0x30,0x60,0x30,0x18,
   0x0C,0x00,0x00,0x7E,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x0C,0x1E,0x1A,0x18,
   0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
   0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
   0x18,0x18,0x58,0x78,0x30,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x7E,
   0x00,0x18,0x18,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x76,0xDC,
   0x00,0x76,0xDC,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x78,0xCC,0xCC,0x78,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,
   0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x1F,0x18,0x18,0x18,0x18,0x18,
   0xD8,0xD8,0x78,0x38,0x18,0x00,0x00,0x00,
   0x00,0x00,0xD8,0x6C,0x6C,0x6C,0x6C,0x6C,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x70,0xD8,0x18,0x30,0x60,0xF8,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x7E,0x7E,0x7E,
   0x7E,0x7E,0x7E,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};