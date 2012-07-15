/*
 *------------------------------------------------------------------------------
 *
 *------------------------------------------------------------------------------
 */

#ifndef __RTC_H__
#define __RTC_H__

void Init_RTC(void);

U8 RTC_GetSec(void);
U8 RTC_GetMinute(void);
U8 RTC_GetHour(void);
U8 RTC_GetDate(void);
U8 RTC_GetMonth(void);
U8 RTC_GetYear(void);

#endif /* __RTC_H__ */
