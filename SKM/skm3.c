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
#include <linux/kthread.h>
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
static struct task_struct *mythread = NULL; //内核线程对象

void sendnlmsg(char* message){
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
		printk(KERN_ERR "skm3 : alloc_skb Error.\n");
		return ;
	}
	//设置消息头部
	nlh = nlmsg_put(skb,0,0,0,MAX_MSGSIZE,0);
	//设置发送者的端口id(内核置为0)
	NETLINK_CB(skb).portid = 0;
	////多播组号为5，但置成0好像也可以
	NETLINK_CB(skb).dst_group = 1;
	//复制信息
	memcpy(NLMSG_DATA(nlh),message,mlen+1);
	//发送多播消息到多播组5
	netlink_broadcast(nl_sk,skb,0,1,GFP_KERNEL);
	printk("send OK!\n");
	return ;
}

static int sending_thread(void *data){
	int i;
	struct completion cmpl;
	DEBUG(0);
	for(i=0;i<10;i++){
		DEBUG(1+ i*100);
		init_completion(&cmpl);
		DEBUG(2+ i*100);
		wait_for_completion_timeout(&cmpl,1 * HZ);
		DEBUG(3+ i*100);
		sendnlmsg("I am from kernel!");
		DEBUG(4+ i*100);
	}
	DEBUG(5);
	printk("sending thread exited!\n");
	return 0;
}

static int  __init  m_init(void){
	printk("<module init>\n");
	struct netlink_kernel_cfg cfg;
	cfg.groups = 5;
	cfg.flags = 0;
	cfg.input = NULL;
	cfg.cb_mutex = NULL;
	nl_sk = netlink_kernel_create(&init_net,NETLINK_TEST,&cfg);
	printk("start test!\n");
	mythread = kthread_run(sending_thread,NULL,"my thread");
	return 0;
}

static void __exit m_finl(void){
	sock_release(nl_sk->sk_socket);
	printk("<module finished>\n");
}


module_init(m_init);
module_exit(m_finl);
