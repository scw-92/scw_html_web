#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <libwebsockets.h>
//#include <jsoncpp/json/json.h>
#include <json/json.h>
#include<fcntl.h>
#include<assert.h>
#include<termios.h>
#include<sys/types.h>
#include<errno.h>

//#include <json/json.h>
#define UART_ON 1
#define UART_OFF 2
#define UART_SEND 3
#define UART_READ 4


int uart_send_en = 0;//the flag of uart_con,1=on ,0 = off
int web_recv_flags=-1;//ws recv con_bit;

static int ret;
int uart_fd= -1;
char uart_buf_read[1024];
char  uart_buf_write[1024];
int uart_read_en = 0;
int uart_on =0;
int uart_send_auto = 0;
int uart_send_time = 0;

struct lws *uart_wsi = NULL;

static int websocket_write_back(struct lws *wsi_in, const char *str, int str_size_in); 

void* pthread_read(void * arg) ;
// writer
void * pthread_write(void * arg) ;
ssize_t safe_write(int uart_fd, const void *vptr, size_t n) 
{
    size_t  nleft;
    ssize_t nwritten;
    const char *ptr;

    ptr = (char *)vptr;
    nleft = n;
	//printf("safe_write+40\n");

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
		//printf("-->%ld\n",nwritten);
    }
	printf("safe_write%s-->%d\n",vptr,__LINE__);
	if(uart_send_auto==0)
	{
		bzero((void *)vptr,1024);
	}
    return(n);
}



ssize_t safe_read(int uart_fd,char *vptr,size_t n)
{
    size_t nleft;
    ssize_t nread;
    char *ptr;

    ptr=vptr;
    nleft=n;
	bzero((void *)vptr,1024);
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
    //assert(pathname);

	char pathbuf[20] = {0};
	sprintf(pathbuf,"/dev/%s",pathname);
//	std::string path = pathname;
    /*打开串口*/
   //uart_fd = open(pathname,O_RDWR|O_NOCTTY|O_NDELAY);
   printf("-->%s---->%d\n",pathbuf,__LINE__);
    uart_fd = open(pathbuf,O_RDWR|O_NOCTTY|O_NDELAY);
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

	pthread_t read_id;
	pthread_t write_id;

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
			printf("115200+%d\n",__LINE__);
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
			printf("c_flow+%d\n",__LINE__);
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
    switch(bits)
    {
        case 5:

            options.c_cflag |= CS5;
            break;
        case 6:
            options.c_cflag &= ~CSIZE;//屏蔽其它标志位
            options.c_cflag |= CS6;
            break;
        case 7:
            options.c_cflag &= ~CSIZE;//屏蔽其它标志位
            options.c_cflag |= CS7;
            break;
        case 8:
            options.c_cflag &= ~CSIZE;//屏蔽其它标志位
            options.c_cflag |= CS8;
			printf("bits+%d\n",__LINE__);
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

			printf("parity+%d\n",__LINE__);
            options.c_cflag &= ~PARENB;//PARENB：产生奇偶位，执行奇偶校验(本句取反，取消奇偶检验)
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
			printf("stop+%d\n",__LINE__);
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
    options.c_cc[VMIN] = 0;//最少读取一个字符

    /*如果发生数据溢出，只接受数据，但是不进行读操作*/
    tcflush(uart_fd,TCIFLUSH);

    /*激活配置*/
    if(tcsetattr(uart_fd,TCSANOW,&options) < 0)
    {
        perror("tcsetattr failed");
        return -1;
    }
	
	if(  pthread_create(&read_id, NULL, pthread_read, NULL ) != 0)
	{
		perror("fail to pthread_create");
		exit(1);
	}

	if(  pthread_create(&write_id, NULL, pthread_write, NULL ) != 0)
	{
		perror("fail to pthread_create");
		exit(1);
	}

	uart_on = 1;
    return 0;
}

int uart_read(int fd,char *r_buf,size_t len,struct lws *wsi)
{
    ssize_t cnt = 0;
   //printf("uart_read--->%d\n",__LINE__);
				cnt = safe_read(fd,r_buf,len);
				printf("uart_read-->%s\n",r_buf);
				websocket_write_back(wsi ,r_buf,strlen(r_buf));
				if(cnt == -1)
				{
					fprintf(stderr,"read error!\n");
					return -1;
				}
				return cnt;
}


int uart_write(int fd,const char *w_buf,size_t len)
{
    ssize_t cnt = 0;
	if(uart_send_en = 1)
	{
  	  cnt = safe_write(fd,w_buf,len);
 	   if(cnt == -1)
  	  {
      	  fprintf(stderr,"write error!\n");
     	   return -1;
  	  }

  	  return cnt;
	}
}

int uart_close(int uart_fd)
{

    close(uart_fd);
	uart_read_en=0;
	uart_send_en =0;
	uart_on = 0;
	uart_fd = -1;
    /*可以在这里做些清理工作*/

    return 0;
}








int uart_main(char * file_uart,int baude,int c_flow,int bits,int  parity,int stop,int uart_send_en, struct lws *wsi)
{
	printf("uart_file:%s\nuart_baudraet:%d\nuart_flow:%d\nuart_date:%d\nuart_check:%d\n",file_uart,baude,c_flow,bits,parity);

	uart_fd = uart_open(uart_fd,file_uart);/*串口号/dev/ttySn,USB口号/dev/ttyUSBn*/
    if(uart_fd == -1)
    {
        fprintf(stderr,"uart_open error\n");
	  return -1;
    }

	if(uart_set(uart_fd,baude,c_flow,bits,parity,stop) == -1)
    {
        fprintf(stderr,"uart set failed!\n");
	   return -1;
    }

}

static int json_date( char *src,struct lws *wsi)
{
	//char *uart_write_buf = NULL;
	Json::Reader reader;
	Json::Value root;
	Json::FastWriter writer;
	if(!reader.parse(src,root))
	{
		return -1;
	}

	if(strcmp((char *)root["type"].asString().data(),"uart_on") == 0)
	{
		web_recv_flags = UART_ON;
		uart_wsi = wsi;
		printf("web_rece%d\n",__LINE__);
	}else if(strcmp((char *)root["type"].asString().data(),"uart_off") == 0)
	{
		web_recv_flags = UART_OFF;
		printf("%d_web_rece%d\n",__LINE__,web_recv_flags);
	}else if(strcmp((char *)root["type"].asString().data(),"uart_send") == 0)
	{
		web_recv_flags = UART_SEND;
		printf("%d_web_rece%d\n",__LINE__,web_recv_flags);
	}else if(strcmp((char *)root["type"].asString().data(),"uart_read") == 0)
	{
		web_recv_flags = UART_READ;
		printf("%d_web_rece%d\n",__LINE__,web_recv_flags);
	}


	printf("---->%d\n",root["uart_set_time"].asInt());


	switch (web_recv_flags)
	{

			case UART_ON:
					if(!uart_on)
					{

							printf("web_rece%d\n",__LINE__);
							uart_main((char *)root["uart_number"].asString().data(),
											root["uart_baudrate"].asInt(),
											root["uart_flow_con"].asInt(),
											root["uart_data_bit"].asInt(),
											root["uart_check_bit"].asInt(),
											root["uart_stop_bit"].asInt(),
											uart_send_en,
											wsi);
					}
			
			break;
		case UART_OFF:
			printf("-->%d\n",__LINE__);
			uart_send_en = 0;
			uart_read_en=0;
			if(uart_on)
			{
				uart_close(uart_fd);
			}
			break;
		case UART_SEND:		
			printf("-->%d\n",__LINE__);
			if(uart_on == 1)
			{
				bzero((void*)uart_buf_write,1024);
				uart_send_en = 1;
					
				 if(strcmp((char *)root["uart_how_to_send"].asString().data(),"uart_send_auto") == 0)
				{
			
					uart_send_time = root["uart_set_time"].asInt()*1000;
					printf("-->how to send--> auto-->%d---->send_time-->%d\n",__LINE__,uart_send_time);
					uart_send_auto = 1;
				}else
				{
					uart_send_auto = 0;
					uart_send_time = 0;
				}
				printf("-->%d\n",__LINE__);
				strcpy(uart_buf_write , (char *)root["uart_data_send"].asString().data());
				printf("strcpy-->%s-->%d\n",uart_buf_write,__LINE__);
				
  			  }
			  break;
		 case UART_READ:
		 	if(uart_on == 1)
			{
		 		uart_read_en = 1;
				printf("-->%d\n",__LINE__);
			}
			break;
	}
}

//协议回调函数之 callback_http
static int callback_http(struct lws *wsi,
    enum lws_callback_reasons reason, void *user,
    void *in, size_t len)
{
	return 0;
}

static int websocket_write_back(struct lws *wsi_in, const char *str, int str_size_in)   
{  
    if (str == NULL || wsi_in == NULL)  
        return -1;  
  
    int n;  
    int len;  
    unsigned char *out = NULL;  
  
    if (str_size_in < 1)   
        len = strlen(str);  
    else  
        len = str_size_in;  
  
    out = (unsigned char *)malloc(sizeof(unsigned char)*(LWS_SEND_BUFFER_PRE_PADDING + len + LWS_SEND_BUFFER_POST_PADDING));  
    //* setup the buffer*/  
    memcpy (out + LWS_SEND_BUFFER_PRE_PADDING, str, len );  
    //* write out*/  
    n = lws_write(wsi_in, out + LWS_SEND_BUFFER_PRE_PADDING, len, LWS_WRITE_TEXT);  
  
    printf("[websocket_write_back] %s\n", str);  
    //* free the buffer*/  
    free(out);  
  
    return n;  
}  

//callback_dumb_increment
static int callback_dumb_increment(struct lws *wsi,
    enum lws_callback_reasons reason,
    void *user, void *in, size_t len)
{
	int temp = 0;


    switch (reason) {
        case LWS_CALLBACK_ESTABLISHED:
            printf("connection is ok!!\n");
            break;
        case LWS_CALLBACK_RECEIVE: 
            printf("received data: %s--->%d\n", (char *) in,__LINE__);
			json_date((char *)in,wsi);	
			break;
		case LWS_CALLBACK_SERVER_WRITEABLE:
			break;
        default:
            break;
    }

    return 0;
}


//注册协议
static struct lws_protocols protocols[] = {
	/* first protocol must always be HTTP handler */

	{
		"http-only",		/* name */
		callback_http,		/* callback */
	//	sizeof (struct per_session_data__http),	/* per_session_data_size */
		0,			/* max frame size / rx buffer */
	},
	{
		"dumb-increment-protocol",
		callback_dumb_increment,
	//	sizeof(struct per_session_data__dumb_increment),
		10, 
		/* rx buf size must be >= permessage-deflate rx size
		     * dumb-increment only sends very small packets, so we set
		     * this accordingly.  If your protocol will send bigger
		     * things, adjust this to match */
	},

	{ NULL, NULL, 0, 0 } /* terminator */
};



void* pthread_read(void * arg)
{
	while(1)
	{
		if(uart_on)
		{
			if(uart_read_en)
			{
				
				if(uart_on && uart_read_en)
				{
					//printf("---->%d\n",__LINE__);
					ret = uart_read(uart_fd,uart_buf_read,1024,uart_wsi);
					if(ret == -1)
   					 {
      					  fprintf(stderr,"uart read failed!\n");
						
					}
		
				}
			}
		}
		sleep(1);
	}
}

void * pthread_write(void *arg)
{
	while(1)
	{
		if(uart_on)
		{
			if(uart_send_en)
			{
				if(strlen(uart_buf_write))
				{
					if(uart_send_auto)
					{
						ret = uart_write(uart_fd,uart_buf_write,strlen(uart_buf_write));
						if(ret == -1)
  						  {
      					  fprintf(stderr,"uart write failed!\n");
						}
						usleep(uart_send_time);
						printf("send_time-->%d",uart_send_time);
					}else
					{
						ret = uart_write(uart_fd,uart_buf_write,strlen(uart_buf_write));
						if(ret == -1)
  						  {
      					  fprintf(stderr,"uart write failed!\n");
						}
						sleep(1);
					}
				}
	    	}
    	}
   }
}


int main(void) 
{
	int port = 9988;//端口号 
	struct lws_context_creation_info info;//上下文对象的信息
	struct lws_context *context;//上下文对象
	int opts = 0;//本软件的功能选项

	//设置info，填充info信息体
	memset(&info,0,sizeof(info));
	info.port = port;
	info.iface=NULL;
	info.protocols = protocols;
	info.extensions = NULL;
	info.ssl_cert_filepath = NULL;  
    info.ssl_private_key_filepath = NULL; 
	info.ssl_ca_filepath = NULL;
    info.gid = -1;  
    info.uid = -1;  
    info.options = opts;  
	info.ka_time = 0;
	info.ka_probes = 0;
	info.ka_interval = 0;
	
	context = lws_create_context(&info);//创建上下文对面，管理ws
	if (context == NULL) 
	{  
        printf(" Websocket context create error.\n");  
        return -1;  
    }
	
	printf("starting server with thread: %d...\n", lws_get_count_threads(context));

	while(1)
	{

			
		lws_callback_on_writable_all_protocol(context,&protocols[1]);
		lws_service(context, 50);//启动ws服务
		
	}
	usleep(10);
	lws_context_destroy(context);
	
	return 0;
}
