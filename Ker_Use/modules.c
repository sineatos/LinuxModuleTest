#ifndef __KERNEL__
#define __KERNEL__
#endif

#ifndef MODULE
#define MODULE
#endif

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/init.h>
#include <linux/netfilter_ipv4.h>
#include <linux/uaccess.h>
#define SOCKET_OPS_BASE	128
#define SOCKET_OPS_SET		(SOCKET_OPS_BASE)
#define SOCKET_OPS_GET		(SOCKET_OPS_BASE)
#define SOCKET_OPS_MAX		(SOCKET_OPS_BASE+1)

#define KMSG	  "a message from kernel"
#define KMSG_LEN		sizeof("a message from kernel")

MODULE_LICENSE("GPL");

static int recv_msg(struct sock *sk,int cmd,void __user* user,unsigned int len){
	int ret = 0;
	printk(KERN_INFO "sockopt: recv_msg()\n");
	if(cmd == SOCKET_OPS_SET){
		char umsg[64];
		int len = sizeof(char)*64;
		memset(umsg,0,len);
		ret = copy_from_user(umsg,user,len);
		printk("recv_msg:umsg=%s. ret=%d\n",umsg,ret);
	}
	return 0;
}

static int send_msg(struct sock *sk,int cmd,void __user *user,int *len){
	int ret = 0;
	printk(KERN_INFO "sockopt:send_msg()\n");
	if(cmd == SOCKET_OPS_GET){
		ret = copy_to_user(user,KMSG,KMSG_LEN);
		printk("send_msg:umsg=%s. ret=%d.success\n",KMSG,ret);
	}
	return 0;
}

static struct nf_sockopt_ops test_sockops;

static int __init init_sockopt(void){
	test_sockops.pf = PF_INET;
	test_sockops.set_optmin = SOCKET_OPS_SET;
	test_sockops.set_optmax = SOCKET_OPS_MAX;
	test_sockops.set = recv_msg;
	test_sockops.get_optmin = SOCKET_OPS_GET;
	test_sockops.get_optmax = SOCKET_OPS_MAX;
	test_sockops.get = send_msg;
	test_sockops.owner = THIS_MODULE;

	printk(KERN_INFO "sockopt: init_sockopt()\n");
	return nf_register_sockopt(&test_sockops);
}

static void __exit fini_sockopt(void){
	printk(KERN_INFO "sockopt:fini_sockopt()\n");
	nf_unregister_sockopt(&test_sockops);
}

module_init(init_sockopt);
module_exit(fini_sockopt);
