#ifdef __cplusplus
extern "C"{
#endif//__cplusplus
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dbus/dbus.h>
#include <cassert>
#ifdef __cplusplus
}
#endif//__cplusplus
#include <string>
#include <memory>
#include <iostream>
#include "otbr_object.h"

using namespace tuya::otbr;

DbusInitiator::DbusInitiator(){

}
DbusInitiator::~DbusInitiator(){
   dbus_connection_flush(mConnection);
}

int DbusInitiator::init()
{
    DBusError   dbusError;
    int ret;
    dbus_error_init(&dbusError);
    mConnection = dbus_bus_get(DBUS_BUS_SYSTEM, &dbusError);
    if (dbus_error_is_set(&dbusError)) {
      fprintf(stderr, "Connection Error (%s)\n", dbusError.message);
      std::cout<<"dbusError.message"<<dbusError.message<<std::endl;
      dbus_error_free(&dbusError);
    }
    if (NULL == mConnection) {
      exit(1);
    }

    dbus_bus_register(mConnection, &dbusError);
    #if 1//可以不给连接取well-know的名字
    ret = dbus_bus_request_name(mConnection,"tuya.dbus.service", DBUS_NAME_FLAG_REPLACE_EXISTING, &dbusError);
    if (dbus_error_is_set(&dbusError)) {
        fprintf(stderr, "Name Error (%s)\n", dbusError.message);
        dbus_error_free(&dbusError);
    }
    if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) {
        exit(1);
    }
    #endif

    return 0;
}
/*
调用dbus method并阻塞到数据回来
*/
DBusMessage *DbusInitiator::method_call(std::string Properity){

    char input[1024] = {0};
    DBusMessage *request, *reply;
    char *ptr=input;

    assert((request = dbus_message_new_method_call(mServiceName.c_str(), mObjectPath.c_str(), mDbusStandardInterfaceProperties.c_str(), "Get")));
    
    /*创建迭代器，添加method的参数到迭代器*/
    DBusMessageIter iter;
    dbus_message_iter_init_append(request, &iter);

    snprintf(input, mInterfaceName.length()+1, mInterfaceName.c_str());
    assert(dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING,&ptr));

    memset(input,0,sizeof(input));
    snprintf(input, Properity.length()+1, Properity.c_str());
    assert(dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &ptr));

    DBusPendingCall *pending_return;
    assert(dbus_connection_send_with_reply(mConnection, request, &pending_return, -1));

    assert(pending_return);
    dbus_connection_flush(mConnection);      //在数据拷贝前阻塞此连接
    dbus_message_unref(request);             //释放发送数据内存块
    dbus_pending_call_block(pending_return); //调用数据返回前阻塞调用

    //拿到对方返回的数据
    assert((reply = dbus_pending_call_steal_reply(pending_return)));
    dbus_pending_call_unref(pending_return);
    return reply;
}
DBusMessageIter* DbusInitiator::dbus_iter_enter_recurse(DBusMessageIter *iter,int *iterType){
    int bType;
    DBusMessageIter *retIter,*subArrayIter=(DBusMessageIter*)malloc(sizeof(DBusMessageIter));
    dbus_message_iter_recurse(iter, subArrayIter); //解开下层迭代
    bType = dbus_message_iter_get_arg_type(subArrayIter);
    printf("bType=%c\r\n", bType);
    if(bType == 'v'){//DBUS_TYPE_VARIANT
        retIter =dbus_iter_enter_recurse(subArrayIter,iterType);
        free(subArrayIter);
        return retIter;
    }else{
        *iterType = bType;
        return subArrayIter;
    }
}
/*
sudo dbus-send --system --print-reply --type=method_call 
--dest=io.openthread.BorderRouter.wpan0 /io/openthread/BorderRouter/wpan0 org.freedesktop.DBus.Properties.Get string:'io.openthread.BorderRouter' string:'Channel'
*/
int DbusInitiator::query(std::string Properity,uint16_t*aValue){
//数据发送与解析分开
    DBusMessage *reply = method_call(Properity);
    char *s;
    int n_elements = 1;
    unsigned int channel;
    va_list reply_valist;
    DBusMessageIter *pIter=nullptr;
    DBusMessageIter dataRecievedIter; // 接收数据迭代器
    dbus_bool_t singleData;           // 保存传送过来的bool变量
    int bType;
    dbus_message_iter_init(reply, &dataRecievedIter); // 生成msg消息的数据迭代器
    int aType = dbus_message_iter_get_arg_type(&dataRecievedIter);
    printf("1aType=%c\r\n", aType);
    if(aType = 'v'){
        pIter = dbus_iter_enter_recurse(&dataRecievedIter,&bType);
        aType = bType;
    }else{
        pIter = &dataRecievedIter;
    }
    printf("jason %d,aType=%c\r\n",__LINE__,aType);
    switch (aType)
    {
    case 'q':
        dbus_message_iter_get_basic(pIter, aValue);
        break;
    
    default:
        break;
    }
    dbus_message_unref(reply);

    return 0;
}

int main(void){
    DbusInitiator initiator;
    uint16_t channel;
    initiator.init();
    initiator.query(OTBR_DBUS_PROPERTY_CHANNEL,&channel);
    printf("channel=%d\r\n",channel);
    initiator.query(OTBR_DBUS_PROPERTY_PANID,&channel);
    printf("PANID=%d\r\n",channel);
    return 0;
}