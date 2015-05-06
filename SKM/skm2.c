#ifndef __KERNEL__
#define __KERNEL__
#endif

#ifndef MODULE
#define MODULE
#endif

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/skbuff.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <net/sock.h>
#include <net/netlink.h>		//该文头文件里包含了linux/netlink.h，因为我们要用到net/netlink.h中的某些API函数，nlmsg_pug()

#define NETLINK_TEST 31
#define MAX_MSGSIZE 1024

#define DBG
#ifdef DBG
#define DEBUG(x) printk("***************** %d *******************\n",x)
#endif

MODULE_LICENSE("GPL");
MODULE_AUTHOR("LiangZhuoZhi");

struct sock *nl_sk=NULL;

void sendnlmsg(char* message,int dstPID){
	struct sk_buff *skb;
	struct nlmsghdr *nlh;
	int len = NLMSG_SPACE(MAX_MSGSIZE);
	int mlen = strlen(message);
	if(!message || !nl_sk){
		return ;
	}
	//申请sk_buff的空间
	skb = alloc_skb(len,GFP_KERNEL);
	if(!skb){
		printk(KERN_ERR "skm2 : alloc_skb Error.\n");
		return ;
	}
	//设置消息头部
	nlh = nlmsg_put(skb,0,0,0,MAX_MSGSIZE,0);
	//设置发送者的端口id(内核置为0)
	NETLINK_CB(skb).portid = 0;
	//如果目的组为内核或某一进程，该字段也置0
	NETLINK_CB(skb).dst_group = 0;
	//复制信息
	memcpy(NLMSG_DATA(nlh),message,mlen+1);
	//通过netlink_unicast()将消息发送用户空间由dstPID所指定了进程号的进程
	netlink_unicast(nl_sk,skb,dstPID,0);
	printk("send OK!\n");
	return ;
}

static void nl_data_ready(struct sk_buff * skb){
	struct nlmsghdr *nlh=NULL;
	void* data = NULL;
	printk("skb: %u\n",skb->len);
	if(skb->len >= nlmsg_total_size(0)){
		nlh = nlmsg_hdr(skb);
		data = NLMSG_DATA(nlh);
		if(data!=NULL){
			printk("%s:received netlink message payload: %s \n",__FUNCTION__,(char*)data);
		}
		//自己编写的发送接口
		sendnlmsg("I see you",nlh->nlmsg_pid);
	}
}

static int  __init  m_init(void){
	printk("<module init>\n");
	struct netlink_kernel_cfg cfg;
	cfg.groups = 0;
	cfg.flags = 0;
	cfg.input = nl_data_ready;
	cfg.cb_mutex = NULL;
	nl_sk = netlink_kernel_create(&init_net,NETLINK_TEST,&cfg);
	return 0;
}

static void __exit m_finl(void){
	printk("<module finished>\n");
	sock_release(nl_sk->sk_socket);
}


module_init(m_init);
module_exit(m_finl);
