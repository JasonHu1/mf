#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <getopt.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <limits.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include <pthread.h>
#include <syslog.h>
#include <libubox/uloop.h>
#include <libubox/ustream.h>
#include <libubox/utils.h>
#include <libubus.h>
#include <libubox/blobmsg_json.h>
#include "ubus_common.h"

struct blob_buf b;
struct ubus_context *ubus_ctx;
struct ubus_event_handler listener;

static void scanreq_prog_cb(struct ubus_request *req, int type, struct blob_attr *msg)
{
	char *str;
	if (!msg)
		return;
 
	/* 
	在这里处理返回的消息。
	本例子只是将返回的消息打印出来。
	*/
	str = blobmsg_format_json_indent(msg, true, 0);
	printf("%s\n", str);
	free(str);
}

int main(void)
{
	int ret;
    unsigned int id;
    int timeout=1;
 
    ubus_ctx=ubus_connect(NULL);  
	/*
    向ubusd查询是否存在"objectname"这个对象，
    如果存在，返回其id
    */
    ret = ubus_lookup_id(ubus_ctx, "objectname", &id);
    if (ret != UBUS_STATUS_OK) {
        printf("lookup scan_prog failed\n");
        return ret;
    }
    else {
        printf("lookup scan_prog successs\n");
    }
    
    /* 调用"objectname"对象的"scan"方法 
    * ubus call objectname methodname '{"mac":"00158D00029F703F"}'
    */
    memset(&b,0,sizeof(b));
	blob_buf_init(&b, 0);
	blobmsg_add_string(&b, "mac", "00158D00029F703F");
    ubus_invoke(ubus_ctx, id,"methodname", b.head, scanreq_prog_cb, NULL, timeout * 1000);
    return 0;
}