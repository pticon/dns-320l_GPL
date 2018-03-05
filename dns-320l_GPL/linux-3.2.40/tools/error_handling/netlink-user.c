#include <sys/socket.h>
#include <linux/netlink.h>

#define NETLINK_USER 31
#define MAX_PAYLOAD 1024  /* maximum payload size*/


#define MV_DRAM_UNIT_CODE 		(1<<9)
#define MV_MBUS_ERRORS_SOURCE 		0x20A20
#define MV_1BIT_ERROR_MASK  		0x1
#define MV_2BIT_ERROR_MASK      	0x2
#define MV_DATA_PATH_ERROR_MASK      	0x3

struct axp_error_nl_message{
        int unit_value;
        int cause_value;
        int unit_address;
        int more_data;
};

struct sockaddr_nl src_addr, dest_addr;
struct nlmsghdr *nlh = NULL;
struct iovec iov;
int sock_fd;
struct msghdr msg;
struct axp_error_nl_message* data;

void main() {
	pid_t childID;
printf("Started\n");
    sock_fd=socket(PF_NETLINK, SOCK_RAW, NETLINK_USER);
    if(sock_fd<0)
	return -1;
    printf("Sock opened\n");

    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid();  /* self pid */
    /* interested in group 1<<0 */
    bind(sock_fd, (struct sockaddr*)&src_addr,
        sizeof(src_addr));

    memset(&dest_addr, 0, sizeof(dest_addr));
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0;   /* For Linux Kernel */
    dest_addr.nl_groups = 0; /* unicast */

    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
    memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
    nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_flags = 0;
    strcpy(NLMSG_DATA(nlh), "Hello");
    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;
    msg.msg_name = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    printf("Sending message to kernel\n");
    sendmsg(sock_fd,&msg,0);

   printf("Waiting for message from kernel\n");
while(1==1)
{

    /* Read message from kernel */
    recvmsg(sock_fd, &msg, 0);
    data = NLMSG_DATA(nlh);

    /*Amalyze message async*/
    childID = fork();
    if( childID >= 0)
    {
	if( childID == 0)
	{
            printf(" Received message Unit value 0x%X, Cause value 0x%X, addr 0x%X\n",
                data->unit_value,
                data->cause_value,
                data->unit_address);
	    if( data->unit_address == MV_MBUS_ERRORS_SOURCE )
		printf("The Error from Units\n");
	    else
                printf("The Error from Fabric\n");

            if( data->cause_value ==  MV_DRAM_UNIT_CODE)
                printf("The Error from DDR Unit\n");
            else
                printf("The Error from Other Unit\n");

            if( (data->unit_value&MV_1BIT_ERROR_MASK) != 0)
                printf("The Error is 1Bit ECC\n");
            if( (data->unit_value&MV_2BIT_ERROR_MASK) != 0)
                printf("The Error is 2Bit ECC\n");
            if( (data->unit_value&MV_DATA_PATH_ERROR_MASK) != 0)
                printf("The Error is Data Path Parity\n");
            printf("Address register is 0x%X\n", data->more_data);
	}
    }

}
    close(sock_fd);
}
