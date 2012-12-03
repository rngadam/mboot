/* ----------------------------------------------------------------------------
 *		 
 *        mboot for Lophilo embedded system
 *		
 * ----------------------------------------------------------------------------
 */

#include "types.h"
#include "trace.h"
#include "option.h"

#include "sfr.h"
#include "system.h"
#include "rtc.h"

U8 t_hour, t_min, t_sec;
U8 d_mon, d_date, d_day;
U32 d_year;

S8* month[] = {0, "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

//__DATE__, __TIME__

U8 BCD2INT(U8 bcd)
{
	return ((bcd)&0xF) + 10 * ((bcd>>4)&0xF);
}

void Reset_RTC(void)
{
	U32 i;
// 	U32 timeout;
	
// 	timeout = 0;
// 	while(((rRTC_SR & (1 << 2)) != (1 << 2)) && (++timeout < 10000));
	while((rRTC_SR & (1 << 2)) != (1 << 2));
	rRTC_CR |= (1 << 0);
// 	timeout = 0;
// 	while(((rRTC_SR & (1 << 0)) != (1 << 0)) && (++timeout < 10000));
	while((rRTC_SR & (1 << 0)) != (1 << 0));
	rRTC_SCCR = (1 << 0);
	for(i=0; i<10000; i++) rRTC_TIMR = (0x10<<16) + (0x30<<8) + (0x0<<0);
	rRTC_CR &= ~(1 << 0);
	rRTC_SCCR |= (1 << 2);

// 	timeout = 0;
// 	while(((rRTC_SR & (1 << 2)) != (1 << 2)) && (++timeout < 10000));
	while((rRTC_SR & (1 << 2)) != (1 << 2));
	rRTC_CR |= (1 << 1);
// 	timeout = 0;
// 	while(((rRTC_SR & (1 << 0)) != (1 << 0)) && (++timeout < 10000));
	while((rRTC_SR & (1 << 0)) != (1 << 0));
	rRTC_SCCR = (1 << 0);
	rRTC_CALR = (0x6<<24) + (0x1<<21) + (0x8<<16) + (0x12<<8) + (0x20<<0);
	rRTC_CR &= ~(1 << 1);
	rRTC_SCCR |= (1 << 2);
	
	TRACE_WRN("Invalid time reset to: Aug-6-2012, 10:30:00");
}

void Init_RTC(void)
{
	rRTC_IDR = 0x1F; // Disable all RTC interrupts.
	
	if((rRTC_MR)&&((rRTC_TIMR>>22)&(0x1))) t_hour = BCD2INT((U8)((rRTC_TIMR>>16)&(0x3F))) + 12;
	else t_hour = BCD2INT((U8)((rRTC_TIMR>>16)&(0x3F)));
	t_min = BCD2INT((U8)((rRTC_TIMR>>8)&(0x7F)));
	t_sec = BCD2INT((U8)((rRTC_TIMR)&(0x7F)));
	
	d_date = BCD2INT((U8)((rRTC_CALR>>24)&(0x3F)));
	d_day = BCD2INT((U8)((rRTC_CALR>>21)&(0x7)));
	d_mon = BCD2INT((U8)((rRTC_CALR>>16)&(0x1F)));
	d_year = BCD2INT((U8)((rRTC_CALR>>8)&(0xFF))) + 100 * BCD2INT((U8)((rRTC_CALR)&(0x7F)));

	if(d_year<2012){
		Reset_RTC();
	}
	else if((d_year==2012)&&(d_mon<8)){
		Reset_RTC();
	}
	else if((d_year==2012)&&(d_mon==8)&&(d_date<6)){
		Reset_RTC();
	}
	else{
		TRACE_MSG("System Time: %s-%d-%d, %02d:%02d:%02d", month[d_mon], d_date, d_year, t_hour, t_min, t_sec);
	}
}

U8 RTC_GetSec(void)
{
	return BCD2INT((U8)((rRTC_TIMR)&(0x7F)));
}

U8 RTC_GetMinute(void)
{
	return BCD2INT((U8)((rRTC_TIMR>>8)&(0x7F)));
}

U8 RTC_GetHour(void)
{
	if((rRTC_MR)&&((rRTC_TIMR>>22)&(0x1))) return BCD2INT((U8)((rRTC_TIMR>>16)&(0x3F))) + 12;
	return t_hour = BCD2INT((U8)((rRTC_TIMR>>16)&(0x3F)));
}

U8 RTC_GetDate(void)
{
	return BCD2INT((U8)((rRTC_CALR>>24)&(0x3F)));
}

U8 RTC_GetMonth(void)
{
	return BCD2INT((U8)((rRTC_CALR>>16)&(0x1F)));
}

U8 RTC_GetYear(void)
{
	return BCD2INT((U8)((rRTC_CALR>>8)&(0xFF)));
}
