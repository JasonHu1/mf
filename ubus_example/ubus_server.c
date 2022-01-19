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
static const char * cli_path;
/*
   ubus call objectname methodname '{"mac":"00158D00029F703F"}'
*/
static int uproto_get_devinfo(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{		
	char *str = NULL;
	char macArray[8] = {0};//{0x00,0x15,0x8D,0x00,0x02,0x9F,0x70,0x3F};
	char bufstr[256] = {0};

	str = blobmsg_format_json(msg, true);
	if (str != NULL)
	{
        //解析取参数
		free(str);
	}
	blob_buf_init(&b, 0);
	
	blobmsg_add_string(&b, "mac", "bufstr");
	blobmsg_add_u32(&b, "rx_rssi", 66);
	ubus_send_reply(ctx, req, b.head);
	return 0;	
}
/*
ubus send eventname '{"k":"v"}'
*/
static void ubus_receive_event(struct ubus_context *ctx,struct ubus_event_handler *ev, 
		const char *type,struct blob_attr *msg)
{
	char *str;
	printf("-----------------[ubus msg]: handler ....-----------------\n");
	str = blobmsg_format_json(msg, true);
	if (str != NULL)
	{
		printf("[ubus msg]: [%s]\n", str);
 
		free(str);
	}
	else{
        printf("error\r\n");
    }
		
}

static const struct blobmsg_policy devinfo_policy =
{ .name = "devmac", .type = BLOBMSG_TYPE_STRING };


static const struct ubus_method uproto_methods[] = {
    { .name = "methodname", .handler = uproto_get_devinfo, .policy = &devinfo_policy, .n_policy = 1 },
};

static struct ubus_object_type uproto_object_type =
UBUS_OBJECT_TYPE("objectname", uproto_methods);

static struct ubus_object uproto_object = {
	.name = "objectname",
	.type = &uproto_object_type,
	.methods = uproto_methods,
	.n_methods = ARRAY_SIZE(uproto_methods),
};



static void ubus_reconn_timer(struct uloop_timeout *timeout)
{
	static struct uloop_timeout retry =
	{
		.cb = ubus_reconn_timer,
	};
	int t = 2;
 
	if (ubus_reconnect(ubus_ctx, cli_path) != 0) {
		uloop_timeout_set(&retry, t * 1000);
		return;
	}
	ubus_add_uloop(ubus_ctx);
	#ifdef FD_CLOEXEC
		fcntl(ubus_ctx->sock.fd, F_SETFD,
			fcntl(ubus_ctx->sock.fd, F_GETFD) | FD_CLOEXEC);
	#endif
}

static void ubus_connection_lost(struct ubus_context *ctx)
{
	ubus_reconn_timer(NULL);
}

int main(void)
{
	int ret;
    memset(&b,0,sizeof(b));
    blob_buf_init(&b,0);
    ubus_ctx=ubus_connect(NULL);
    
	/*注册ubus event*/
    memset(&listener, 0, sizeof(listener));
	listener.cb = ubus_receive_event;
    ubus_register_event_handler(ubus_ctx, &listener, "eventname");
	/*添加ubus object*/
	ret = ubus_add_object(ubus_ctx, &uproto_object);
	if (ret)
		printf("Failed to add uproto object: %s\n", ubus_strerror(ret));
	
	printf("connected as %08x\n", ubus_ctx->local_id);
	ubus_ctx->connection_lost = ubus_connection_lost;

	/*将ubus fd放到uloop中监听*/
	uloop_init();
	ubus_add_uloop(ubus_ctx);
	#ifdef FD_CLOEXEC
		fcntl(ubus_ctx->sock.fd, F_SETFD,
			fcntl(ubus_ctx->sock.fd, F_GETFD) | FD_CLOEXEC);
	#endif
    uloop_run();
    if(ubus_ctx){
        ubus_free(ubus_ctx);
        ubus_ctx = NULL;
    }
    uloop_done();
    return 0;
}