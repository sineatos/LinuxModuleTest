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
#include <linux/netlink.h>

#define NETLINK_TEST 31

MODULE_LICENSE("GPL");
MODULE_AUTHOR("LiangZhuoZhi");

struct sock *nl_sk=NULL;

static void nl_data_ready(struct sk_buff * skb){
	struct nlmsghdr *nhl=NULL;
	void* data = NULL;
	printk("skb: %u\n",skb->len);
	if(skb->len >= nlmsg_total_size(0)){
		nhl = nlmsg_hdr(skb);
		data = NLMSG_DATA(nhl);
		if(data!=NULL){
			printk("%s:received netlink message payload: %s \n",__FUNCTION__,(char*)data);
		}
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
