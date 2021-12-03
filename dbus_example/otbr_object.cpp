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
    printf("dbus_iter_enter_recurse bType=%c\r\n", bType);
    if(bType == 'v'){//DBUS_TYPE_VARIANT
        retIter =dbus_iter_enter_recurse(subArrayIter,iterType);
        free(subArrayIter);
        return retIter;
    }else{
        *iterType = bType;
        return subArrayIter;
    }
}
bool DbusInitiator::dbus_dataTye_array_parse(DBusMessageIter *iter,void*pVal){
    DBusMessageIter eleIter;
    dbus_message_iter_recurse(iter,&eleIter);
    int elementType=dbus_message_iter_get_arg_type(&eleIter);
    if(elementType==DBUS_TYPE_INVALID){
        std::cout<<"elementType invalid"<<std::endl;
        return -1;
    }
                        const char * act_conn_obj;
                        int ddd;
    int idx=0;
    do{
        printf("dbus_dataTye_array_parse : elementType=%c\r\n",elementType);
        switch (elementType)
        {
            case DBUS_TYPE_UINT16:
            case DBUS_TYPE_UINT64:
            case DBUS_TYPE_STRING://const char**
                dbus_message_iter_get_basic(iter, pVal);
                break;
            case DBUS_TYPE_BYTE:
                {
                    dbus_message_iter_get_basic(&eleIter, (char*)pVal++);
                    break;
                }
            default:
                printf("ERROR,NOT Process!!!\r\n");
                dbus_message_iter_get_basic(iter, pVal);
                break;
        }
    }while(dbus_message_iter_next (&eleIter));
    printf("\r\n");
}
bool DbusInitiator::dbus_dataType_parse(DBusMessageIter *iter,int iterType,void*pVal){
    printf("dbus_dataType_parse : iterType=%c\r\n",iterType);
    switch (iterType)
    {
    case DBUS_TYPE_UINT16:
    case DBUS_TYPE_UINT64:
    case DBUS_TYPE_STRING://const char**
        dbus_message_iter_get_basic(iter, pVal);
        break;
    case DBUS_TYPE_ARRAY:
        {
            dbus_dataTye_array_parse(iter, pVal);
            break;
        }
    default:
        printf("ERROR,NOT Process!!!aType=%c,\r\n",iterType);
        dbus_message_iter_get_basic(iter, pVal);
        break;
    }
}
bool DbusInitiator::dbus_dataType_parse1(DBusMessageIter *iter,int iterType,void*pVal){
    printf("dbus_dataType_parse : iterType=%c\r\n",iterType);
    char *p=nullptr;
    switch (iterType)
    {
    case 's'://string
        dbus_message_iter_get_basic(iter, &p);
        printf("p %s\r\n",p);
        break;
    default:
        printf("ERROR,NOT Process!!!aType=%c,\r\n",iterType);
        dbus_message_iter_get_basic(iter, pVal);
        break;
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
    DBusMessageIter *dataRecievedIter=(DBusMessageIter*)malloc(sizeof(DBusMessageIter)); // 接收数据迭代器
    dbus_bool_t singleData;           // 保存传送过来的bool变量
    int bType;
    dbus_message_iter_init(reply, dataRecievedIter); // 生成msg消息的数据迭代器
    int aType = dbus_message_iter_get_arg_type(dataRecievedIter);
    if(aType==DBUS_TYPE_INVALID){
        std::cout<<"aType invalid"<<std::endl;
        return -1;
    }
    if(aType = 'v'){
        pIter = dbus_iter_enter_recurse(dataRecievedIter,&bType);
        aType = bType;
    }else{
        pIter = dataRecievedIter;
    }
    printf("jason %d,aType=%c\r\n",__LINE__,aType);
    switch (aType)
    {
    case 'q':
        dbus_message_iter_get_basic(pIter, aValue);
        break;
    
    default:
        printf("ERROR,NOT Process!!!aType=%c,\r\n",aType);
        break;
    }
    free(pIter);
    dbus_message_unref(reply);

    return 0;
}
int DbusInitiator::query(std::string Properity,uint8_t*aValue){
//数据发送与解析分开
    DBusMessage *reply = method_call(Properity);
    char *s;
    int n_elements = 1;
    unsigned int channel;
    va_list reply_valist;
    DBusMessageIter *pIter=nullptr;
    DBusMessageIter *dataRecievedIter=(DBusMessageIter*)malloc(sizeof(DBusMessageIter)); // 接收数据迭代器
    dbus_bool_t singleData;           // 保存传送过来的bool变量
    int bType;
    dbus_message_iter_init(reply, dataRecievedIter); // 生成msg消息的数据迭代器
    int aType = dbus_message_iter_get_arg_type(dataRecievedIter);
    if(aType==DBUS_TYPE_INVALID){
        std::cout<<"aType invalid"<<std::endl;
        return -1;
    }
    printf("query ,uint8_t* aType=%c\r\n", aType);
    if(aType = 'v'){
        pIter = dbus_iter_enter_recurse(dataRecievedIter,&bType);
        aType = bType;
    }else{
        pIter = dataRecievedIter;
    }
    dbus_dataType_parse(pIter,aType,aValue);
    free(pIter);
    dbus_message_unref(reply);

    return 0;
}
int DbusInitiator::query(std::string Properity,void*aValue){
//数据发送与解析分开
    DBusMessage *reply = method_call(Properity);
    if(reply==nullptr){
        printf("Get reply is nullptr\r\n");
        return -1;
    }
    char *s;
    int n_elements = 1;
    unsigned int channel;
    va_list reply_valist;
    DBusMessageIter *pIter=nullptr;
    DBusMessageIter *dataRecievedIter=(DBusMessageIter*)malloc(sizeof(DBusMessageIter)); // 接收数据迭代器
    dbus_bool_t singleData;           // 保存传送过来的bool变量
    int bType;
    dbus_message_iter_init(reply, dataRecievedIter); // 生成msg消息的数据迭代器
    int aType = dbus_message_iter_get_arg_type(dataRecievedIter);
    if(aType==DBUS_TYPE_INVALID){
        std::cout<<"aType invalid"<<std::endl;
        return -1;
    }
    printf("void* query aType=%c\r\n", aType);
    if(aType = 'v'){
        pIter = dbus_iter_enter_recurse(dataRecievedIter,&bType);
        aType = bType;
    }else{
        pIter = dataRecievedIter;
    }
    dbus_dataType_parse(pIter,aType,aValue);
    free(pIter);
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
    printf("PANID=0x%04x\r\n",channel);
    unsigned char extPanId[128]={0};
    initiator.query(OTBR_DBUS_PROPERTY_EXTPANID,(uint8_t*)&extPanId);
    printf("extPanId=0x%02x%02x%02x%02x%02x%02x%02x%02x\r\n",extPanId[7],extPanId[6],extPanId[5],extPanId[4],extPanId[3],extPanId[2],extPanId[1],extPanId[0]);
    const char*sPtr=nullptr;
    initiator.query(OTBR_DBUS_PROPERTY_NETWORK_NAME,(void*)&sPtr);
    printf("NetworkName=%s\r\n",sPtr);

    char buff[128]={0};
    /*Network Key*/
    memset(buff,sizeof(buff),0);
    initiator.query(OTBR_DBUS_PROPERTY_NETWORK_KEY,(uint8_t*)&buff);
    printf("Network Key= 0x");
    for(int i=0;i<16;i++){
        printf("%02x",buff[i]);
    }
    printf("\r\n");
    /*Mesh Local Prefix*/
    // initiator.query(OTBR_DBUS_PROPERTY_MESH_LOCAL_PREFIX,(void*)&sPtr);
    // printf("Mesh Local Prefix=%s\r\n",sPtr);
    // memset(buff,sizeof(buff),0);
    /*Mesh Local Prefix*/
    initiator.query(OTBR_DBUS_PROPERTY_ACTIVE_DATASET_TLVS,(uint8_t*)&buff);
    printf("ActiveDatasetTlvs= 0x");
    for(int i=0;i<16;i++){
        printf("%02x",buff[i]);
    }
    printf("\r\n");
    return 0;
}