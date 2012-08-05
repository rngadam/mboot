/*
 *------------------------------------------------------------------------------
 *
 *------------------------------------------------------------------------------
 */

#ifndef __INIT_H__
#define __INIT_H__

#define SYS_RESET()			{rPIOA_CODR = (1 << 29);}
#define SYS_UNRESET()		{rPIOA_SODR = (1 << 29);}

#define BTNDIS_N()			{rPIOA_OER = (1 << 28);}
#define BTNDIS_P()			{rPIOA_ODR = (1 << 28);}
// #define BTNDIS_N()			{rPIOB_OER = (1 << 2);}
// #define BTNDIS_P()			{rPIOB_ODR = (1 << 2);}

#endif /* __INIT_H__ */
