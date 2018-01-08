
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

static const char default_rtc[] = "/dev/rtc0";
int fd = -1;


int  rtc_get(  struct rtc_time *rtc_tm , const char *rtc );
int rtc_set(  struct rtc_time *rtc_tm   );


int  rtc_get( struct rtc_time *rtc_tm ,  const char *rtc )
{
	int retval;
	char buf[30]={0};
	fd = -1;

	bzero(buf,30);
	fd = open(rtc, O_RDWR);
	if (fd ==  -1) {
		perror(rtc);
		exit(errno);
	}

	/* Read the RTC time/date */
	retval = ioctl(fd, RTC_RD_TIME, rtc_tm);
	if (retval == -1) {
		perror("RTC_RD_TIME ioctl");
		exit(errno);
	}
		rtc_tm->tm_hour += 8;
        if (rtc_tm->tm_hour >= 24) {
                rtc_tm->tm_sec %= 24;
                rtc_tm->tm_mday++;
        }
        if (rtc_tm->tm_mday >= 31) {
                rtc_tm->tm_mday %= 31;
                rtc_tm->tm_mon++;
        }
        if (rtc_tm->tm_mon >= 12) {
                rtc_tm->tm_mon %= 12;
                rtc_tm->tm_year++;
        }
		
	
//	fprintf(stderr, "\n\nCurrent RTC date/time is %d-%d-%d, %02d:%02d:%02d.\n",
	//	rtc_tm->tm_mday, rtc_tm->tm_mon + 1, rtc_tm->tm_year + 1900,
	//	rtc_tm->tm_hour, rtc_tm->tm_min, rtc_tm->tm_sec);
	sprintf(buf ,"%d-%02d-%02dT%02d:%02d:%02d",rtc_tm->tm_year + 1900,rtc_tm->tm_mon + 1,rtc_tm->tm_mday,rtc_tm->tm_hour,rtc_tm->tm_min,rtc_tm->tm_sec);
	printf("%s\n",buf);
	return  0;
}

int rtc_set( struct rtc_time *rtc_tm  )
{
	   	rtc_tm->tm_hour -= 8;
          if (rtc_tm->tm_hour >= 24) {
                  rtc_tm->tm_sec %= 24;
                  rtc_tm->tm_mday++;
          }
          if (rtc_tm->tm_mday >= 31) {
                  rtc_tm->tm_mday %= 31;
                  rtc_tm->tm_mon++;
          }
          if (rtc_tm->tm_mon >= 12) {
                  rtc_tm->tm_mon %= 12;
                  rtc_tm->tm_year++;
          }

	return ioctl(fd,RTC_SET_TIME, rtc_tm);
}

int main(int argc, char **argv)
{
//	int i, fd, retval, irqcount = 0;
//	unsigned long tmp, data;
	struct rtc_time rtc_tm;
	const char *rtc = default_rtc;
	memset(&rtc_tm,0,sizeof(rtc_tm));
	
	switch (argc) 
	{
	case 8:
		rtc = argv[1];
		rtc_get(&rtc_tm,rtc);
		rtc_tm.tm_year=atoi(argv[2])-1900;
		rtc_tm.tm_mon=atoi(argv[3])-1;
		rtc_tm.tm_mday=atoi(argv[4]);
		rtc_tm.tm_hour=atoi(argv[5]);
		rtc_tm.tm_min=atoi(argv[6]);
		rtc_tm.tm_sec=atoi(argv[7]);
	//	rtc_tm.inttm_isdst=-1;
	if(	rtc_set(&rtc_tm)<0)
		{
			printf("error\n");
			return -1;
		}
		printf("ok\n");
		return 0;
		/* FALLTHROUGH */
	case 2:
		return	rtc_get(&rtc_tm,rtc);
		break;
	default:
		//fprintf(stderr, "usage:  rtctest [rtcdev]\n");
		return -1;
	}
}


