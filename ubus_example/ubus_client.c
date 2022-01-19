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

__attribute__((unused))void UbusInitiator::scanreq_prog_cb1(struct ubus_request *req, int type, struct blob_attr *msg)
{
	char *str;
    int errCode;
    struct json_object* ObjMember=NULL;
	if (!msg)
		return;
	/* 
	在这里处理返回的消息。
	本例子只是将返回的消息打印出来。
	*/
	str = blobmsg_format_json_indent(msg, true, 0);
	printf("%s\n", str);
    struct json_object* obj = json_tokener_parse(str);
    if(obj){
        struct json_object* ObjError = json_object_object_get(obj,"Error");
        if(ObjError){
            errCode=json_object_get_int(ObjError);
            if(errCode==0){
                if(NULL!=(ObjMember=json_object_object_get(obj,"PanId"))){
                    std::string panidStr=json_object_get_string(ObjMember);
                    std::cout<<"panidStr = "<<panidStr<<std::endl;
                    OtbrManager::GetInstance()->OtbrSetPanId(stoi(panidStr,nullptr,16));
                    std::cout<<"PanId ="<<OtbrManager::GetInstance()->OtbrGetPanId()<<std::endl;
                    OtbrManager::GetInstance()->mOtbrDelegate->OtbrNetworkNameQuery();
                }else if(NULL!=(ObjMember=json_object_object_get(obj,"NetworkName"))){
                    OtbrManager::GetInstance()->OtbrSetNetworkName(json_object_get_string(ObjMember));
                    std::cout<<"NetworkName ="<<OtbrManager::GetInstance()->OtbrGetNetworkName()<<std::endl;
                    OtbrManager::GetInstance()->mOtbrDelegate->OtbrRloc16Query();
                }else if(NULL!=(ObjMember=json_object_object_get(obj,"Channel"))){
                    OtbrManager::GetInstance()->OtbrSetChannel(json_object_get_int(ObjMember));
                    std::cout<<"Channel ="<<OtbrManager::GetInstance()->OtbrGetChannel()<<std::endl;
                    OtbrManager::GetInstance()->mOtbrDelegate->OtbrPanIdQuery();
                }else if(NULL!=(ObjMember=json_object_object_get(obj,"rloc16"))){
                    size_t idx=2;
                    std::string rloc16Str=json_object_get_string(ObjMember);
                    OtbrManager::GetInstance()->OtbrSetRloc16(stoi(rloc16Str,&idx,16));
                    std::cout<<"rloc16 ="<<OtbrManager::GetInstance()->GetInstance()->OtbrGetRloc16()<<std::endl;
                    OtbrManager::GetInstance()->mOtbrDelegate->OtbrExtendPanIdQuery();
                }else if(NULL!=(ObjMember=json_object_object_get(obj,"ExtPanId"))){
                    OtbrManager::GetInstance()->OtbrSetExtendPanId(json_object_get_string(ObjMember));
                    std::cout<<"ExtPanId ="<<OtbrManager::GetInstance()->OtbrGetExtendPanId()<<std::endl;
                    OtbrManager::GetInstance()->mOtbrDelegate->OtbrNetworkKeyQuery();
                }else if(NULL!=(ObjMember=json_object_object_get(obj,"Networkkey"))){
                    OtbrManager::GetInstance()->OtbrSetNetworkKey(json_object_get_string(ObjMember));
                    std::cout<<"Networkkey ="<<OtbrManager::GetInstance()->OtbrGetNetworkKey()<<std::endl;
                    OtbrManager::GetInstance()->mOtbrDelegate->OtbrPskcQuery();
                }else if(NULL!=(ObjMember=json_object_object_get(obj,"pskc"))){
                    OtbrManager::GetInstance()->OtbrSetPskc(json_object_get_string(ObjMember));
                    std::cout<<"pskc ="<<OtbrManager::GetInstance()->OtbrGetPskc()<<std::endl;
                }else{
                    std::cout<<": "<<__LINE__<< ",errCode = "<<errCode<<std::endl;
                }
            }else{
                std::cout<<": "<<__LINE__<< ",errCode = "<<errCode<<std::endl;
            }
        }else{
            std::cout<<":%d "<<__LINE__<<std::endl;
        }
        json_object_put(obj);
    }else{
        std::cout<<":%d "<<__LINE__<<std::endl;
    }

	free(str);
    return ;
}

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