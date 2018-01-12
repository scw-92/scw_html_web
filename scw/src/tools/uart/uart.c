#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<assert.h>
#include<termios.h>
#include<string.h>
#include<sys/time.h>
#include<sys/types.h>
#include<errno.h>

static int ret;
int uart_fd= -1;

ssize_t safe_write(int uart_fd, const void *vptr, size_t n)
{
    size_t  nleft;
    ssize_t nwritten;
    const char *ptr;

    ptr = vptr;
    nleft = n;

    while(nleft > 0)
    {
    if((nwritten = write(uart_fd, ptr, nleft)) <= 0)
        {
            if(nwritten < 0&&errno == EINTR)//被中断打断
                nwritten = 0;
            else
                return -1;
        }
        nleft -= nwritten;
        ptr   += nwritten;
    }
    return(n);
}

ssize_t safe_read(int uart_fd,void *vptr,size_t n)
{
    size_t nleft;
    ssize_t nread;
    char *ptr;

    ptr=vptr;
    nleft=n;

    while(nleft > 0)
    {
        if((nread = read(uart_fd,ptr,nleft)) < 0)
        {
            if(errno == EINTR)//被信号中断
                nread = 0;
            else
                return -1;
        }
        else
        if(nread == 0)
            break;
        nleft -= nread;
        ptr += nread;
    }
    return (n-nleft);
}

int uart_open(int uart_fd,const char *pathname)
{
    assert(pathname);

    /*打开串口*/
    uart_fd = open(pathname,O_RDWR|O_NOCTTY|O_NDELAY);
    if(uart_fd == -1)
    {
        perror("Open UART failed!");
        return -1;
    }

    /*清除串口非阻塞标志*/
    if(fcntl(uart_fd,F_SETFL,0) < 0)
    {
        fprintf(stderr,"fcntl failed!\n");
        return -1;
    }

    return uart_fd;
}

int uart_set(int uart_fd,int baude,int c_flow,int bits,int parity,int stop)
{
    struct termios options;

	memset(&options,0,sizeof(options));
    /*获取终端属性*/
    if(tcgetattr(uart_fd,&options) < 0)
    {
        perror("tcgetattr error");
        return -1;
    }


    /*设置输入输出波特率，两者保持一致*/
    switch(baude)
    {
        case 4800:
            cfsetispeed(&options,B4800);
            cfsetospeed(&options,B4800);
            break;
        case 9600:
            cfsetispeed(&options,B9600);
            cfsetospeed(&options,B9600);
            break;
        case 19200:
            cfsetispeed(&options,B19200);
            cfsetospeed(&options,B19200);
            break;
        case 38400:
            cfsetispeed(&options,B38400);
            cfsetospeed(&options,B38400);
            break;
		case 115200:
            cfsetispeed(&options,B115200);
            cfsetospeed(&options,B115200);
            break;
        default:
            fprintf(stderr,"Unkown baude!\n");
            return -1;
    }

    /*设置控制模式*/
    options.c_cflag |= CLOCAL;//保证程序不占用串口
    options.c_cflag |= CREAD;//保证程序可以从串口中读取数据

    /*设置数据流控制*/
    switch(c_flow)
    {
        case 0://不进行流控制
            options.c_cflag &= ~CRTSCTS;
            break;
        case 1://进行硬件流控制
            options.c_cflag |= CRTSCTS;
            break;
        case 2://进行软件流控制
            options.c_cflag |= IXON|IXOFF|IXANY;
            break;
        default:
            fprintf(stderr,"Unkown c_flow!\n");
            return -1;
    }

    /*设置数据位*/
	options.c_cflag &= ~CSIZE;//屏蔽其它标志位
    switch(bits)
    {
        case 5:
            
            options.c_cflag |= CS5;
            break;
        case 6:
          //  options.c_cflag &= ~CSIZE;//屏蔽其它标志位
            options.c_cflag |= CS6;
            break;
        case 7:
           // options.c_cflag &= ~CSIZE;//屏蔽其它标志位
            options.c_cflag |= CS7;
            break;
        case 8:
          //  options.c_cflag &= ~CSIZE;//屏蔽其它标志位
            options.c_cflag |= CS8;
            break;
        default:
            fprintf(stderr,"Unkown bits!\n");
            return -1;
    }

    /*设置校验位*/
    switch(parity)
    {
        /*无奇偶校验位*/
        case 0:
            options.c_cflag &= ~PARENB;//PARENB：产生奇偶位，执行奇偶校验(本句取反，取消奇偶检验)
            options.c_cflag &= ~INPCK;//INPCK：使奇偶校验起作用
            break;
        /*设为空格,即停止位为2位*/
        case 3:
            options.c_cflag &= ~PARENB;//PARENB：产生奇偶位，执行奇偶校验
            options.c_cflag &= ~CSTOPB;//CSTOPB：使用两位停止位
            break;
        /*设置奇校验*/
        case 1:
            options.c_cflag |= PARENB;//PARENB：产生奇偶位，执行奇偶校验
            options.c_cflag |= PARODD;//PARODD：若设置则为奇校验,否则为偶校验
            options.c_cflag |= INPCK;//INPCK：使奇偶校验起作用
         //   options.c_cflag |= ISTRIP;//ISTRIP：若设置则有效输入数字被剥离7个字节，否则保留全部8位
            break;
        /*设置偶校验*/
        case 2:
            options.c_cflag |= PARENB;//PARENB：产生奇偶位，执行奇偶校验
            options.c_cflag &= ~PARODD;//PARODD：若设置则为奇校验,否则为偶校验
            options.c_cflag |= INPCK;//INPCK：使奇偶校验起作用
          //  options.c_cflag |= ISTRIP;//ISTRIP：若设置则有效输入数字被剥离7个字节，否则保留全部8位
            break;
        default:
            fprintf(stderr,"Unkown parity!\n");
            return -1;
    }

    /*设置停止位*/
    switch(stop)
    {
        case 1:
            options.c_cflag &= ~CSTOPB;//CSTOPB：使用两位停止位
            break;
        case 2:
            options.c_cflag |= CSTOPB;//CSTOPB：使用两位停止位
            break;
        default:
            fprintf(stderr,"Unkown stop!\n");
            return -1;
    }

    /*设置输出模式为原始输出*/
    options.c_oflag &= ~OPOST;//OPOST：若设置则按定义的输出处理，否则所有c_oflag失效

    /*设置本地模式为原始模式*/
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    /*
     *ICANON：允许规范模式进行输入处理
     *ECHO：允许输入字符的本地回显
     *ECHOE：在接收EPASE时执行Backspace,Space,Backspace组合
     *ISIG：允许信号
     */

    /*设置等待时间和最小接受字符*/
    options.c_cc[VTIME] = 0;//可以在select中设置
    options.c_cc[VMIN] = 1;//最少读取一个字符

    /*如果发生数据溢出，只接受数据，但是不进行读操作*/
    tcflush(uart_fd,TCIFLUSH);

    /*激活配置*/
    if(tcsetattr(uart_fd,TCSANOW,&options) < 0)
    {
        perror("tcsetattr failed");
        return -1;
    }

    return 0;
}

int uart_read(int fd,char *r_buf,size_t len)
{
    ssize_t cnt = 0;
    fd_set rfds;
    struct timeval time;

    /*将文件描述符加入读描述符集合*/
    FD_ZERO(&rfds);
    FD_SET(fd,&rfds);

    /*设置超时为15s*/
    time.tv_sec = 15;
    time.tv_usec = 0;

    /*实现串口的多路I/O*/
	while(1)
	{
		FD_ZERO(&rfds);
		FD_SET(fd,&rfds);
		ret = select(fd+1,&rfds,NULL,NULL,&time);
		switch(ret)
		{
			case -1:
				fprintf(stderr,"select error!\n");
				return -1;
			case 0:
				fprintf(stderr,"time over!\n");
				return -1;
			default:
				cnt = safe_read(fd,r_buf,len);
				if(cnt == -1)
				{
					fprintf(stderr,"read error!\n");
					return -1;
				}
				return cnt;
		}
	}
}

int uart_write(int fd,const char *w_buf,size_t len)
{
    ssize_t cnt = 0;

    cnt = safe_write(fd,w_buf,len);
    if(cnt == -1)
    {
        fprintf(stderr,"write error!\n");
        return -1;
    }

    return cnt;
}

int uart_close(int uart_fd)
{
    
    close(uart_fd);

    /*可以在这里做些清理工作*/

    return 0;
}
int main(int argc ,char * argv[])
{
	const char *w_buf = "something to write";
	size_t w_len = sizeof(w_buf);
	char r_buf[1024];
	bzero(r_buf,1024);
	
	//uart_fd = uart_open(uart_fd,argv[1]);/*串口号/dev/ttySn,USB口号/dev/ttyUSBn*/
	uart_fd = uart_open(uart_fd,"/dev/ttyO1");/*串口号/dev/ttySn,USB口号/dev/ttyUSBn*/
    if(uart_fd == -1)
    {
        fprintf(stderr,"uart_open error\n");
      //  exit(EXIT_FAILURE);
	  return -1;
    }
//	if(uart_set(uart_fd,atoi(argv[2]),atoi(argv[3]),atoi[4],atoi[5],atoi[6]) == -1)
//   {
//      fprintf(stderr,"uart set failed!\n");
//    exit(EXIT_FAILURE);
//}
	if(uart_set(uart_fd,115200,0,8,'N',1) == -1)
    {
        fprintf(stderr,"uart set failed!\n");
       // exit(EXIT_FAILURE);
	   return -1;
    }
	
	ret = uart_write(uart_fd,w_buf,w_len);
	if(ret == -1)
    {
        fprintf(stderr,"uart write failed!\n");
       // exit(EXIT_FAILURE);
	   return -1;
    }
	
	ret = uart_read(uart_fd,r_buf,1024);
    if(ret == -1)
    {
        fprintf(stderr,"uart read failed!\n");
        //exit(EXIT_FAILURE);
		return -1;
    }
	ret = uart_close(uart_fd);
    if(ret == -1)
    {
        fprintf(stderr,"uart_close error\n");
       // exit(EXIT_FAILURE);
	   return -1;
    }
	return 0;
	
	
	
	
}
