#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/ioctl.h>
#include<linux/i2c.h>
#include<linux/i2c-dev.h>
#include<stdlib.h>
//#define EEPROM_ADDR 0x50 //at24c02的地址
#define OPT_W 1
#define OPT_R 0

int main(int argc,char **argv)
{
	unsigned int slave_addr;
	unsigned char reg_addr;
	int fd;
	char data_read;
	char data_write[2];
	char iic_con_path[20]={0};
	
	sprintf(iic_con_path,"/dev/%s",argv[1]);
	int opt =atoi(argv[2]);
	slave_addr=atoi(argv[3]);
	reg_addr=(unsigned char)atoi(argv[4]);
	
	
	
	if(argc ==5)
	{
		opt = OPT_R;
	}else if (argc==6)
	{
		data_write[1] = argv[5][0];
		data_write[0]=reg_addr;
		opt = OPT_W;
	}else
	{
		printf("Error in parameter number!!!");
	}
	
	fd = open(iic_con_path,O_RDWR);//打开iic控制器
	if(fd<0)
	{
		perror("fail to open /dev/iic-1");
		return -1;
	}

	ioctl(fd,I2C_TIMEOUT,2);
	ioctl(fd,I2C_RETRIES,1);


	
	struct i2c_msg msgs_write[2]={//设置写的信息结构体
		{slave_addr,0,2,data_write},
	};

	struct i2c_msg msgs_read[2] = {//设置读的信息结构体
	{slave_addr,0,1,&reg_addr},
	{slave_addr,I2C_M_RD,1,&data_read}
	};

	
	//填充ioctl函数所需求的信息结构体
	 struct i2c_rdwr_ioctl_data   r_data = {
        .msgs = msgs_read,
        .nmsgs = 2
    };          

	struct i2c_rdwr_ioctl_data w_data={
		.msgs = msgs_write,
		.nmsgs=1
	};

	switch (opt)
	{
		case OPT_W:
			if(ioctl(fd,I2C_RDWR,&w_data)<0)
			{
				perror("fail to ioctl_write");
				return  -1;
			}
			usleep(1000);
			printf("ok\n");
			break;
		case OPT_R:
			if (ioctl(fd, I2C_RDWR, &r_data) < 0)
			{
				perror("fail to ioctl_read\n");
				return -1;
			}
			usleep(1000); //发出停止信号后， 在再发出开始信号前需要延时一会
			printf("%c\n", data_read);
		
	}
	
/*
	for(i =0;i<256;i++)
	{
		if(data_write[1]=='z')
		{
			printf("-->write-->%d\n",i);
			data_write[1] = 'A';
			count=0;
		}
		data_write[0]=i;
		data_write[1]='A'+ count++;
		
		if(ioctl(fd,I2C_RDWR,&w_data)<0)
		{
			perror("fail to ioctl_write");
			break;
		}
		usleep(10000);//at24c02芯片的电器特性需要，该芯片在写的时候需要一定的延时，否则会报错。
	}
	printf("data_write_i = %d\n", i-1);
	

	 for (i = 0; i <256; i++)
    {
       	reg_addr = i;
        if (ioctl(fd, I2C_RDWR, &r_data) < 0)
        {
            perror("fail to ioctl_read\n");
            break;
        }
        usleep(1000); //发出停止信号后， 在再发出开始信号前需要延时一会
        printf("%c ", data_read);
        if (0 == i%26)
            printf("\n");

    }
*/

	 close(fd);
    return 0;

}
