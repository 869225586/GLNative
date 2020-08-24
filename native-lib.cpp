#include <jni.h>
#include <string>
#include "android/log.h"

#define LOGD(FORMAT, ...) __android_log_print(ANDROID_LOG_DEBUG,"syy",FORMAT,##__VA_ARGS__);

//-----------------------回调 方法指针---------
//回调
void add(int a, int b) {
    LOGD("我是回调方法的调用")
}

typedef void (*add_p)(int a, int b); //定义回调方法

//一个方法 需要传入一个方法指针
void test(add_p addP, int a, int b) {
     addP(a,b);
}
//---------------------字符串操作----------
//字符串的长度
int getChar(char *str){
    int len=0;
    while ((*str)!='\0'){
        len++;
        str++;//指针后移 直到找到 \0 即字符串的结尾这样 可以计算出 字符串的长度
    }
    return len;
}
//字符串类型转换
void change(){
    char * str="12";
    //ato函数  类型转换  //转换不了的情况不会报错（"xxx"）返回 int 0 float 0。0000 ，（如果有其他不是数字的就会被剔除 比如 1xxx  int 返回1 ，float 返回1.000）
    int numer=atoi(str);
    float fnumer=atof(str);
    LOGD("转换成int=%d",numer);
    LOGD("转换成float/double=%lf",fnumer);

}
//字符串的比较
void str_compare(){
    char * str="abc";
    char * str1="abc";
    int cmp_result= strcmp(str,str1);//区分大小写 0 表示相等
    LOGD("两个字符串=%d",cmp_result);
}
void str_len(){
    char str[]={'a','b','c','\0'};

    LOGD("字符串长度 %d",getChar(str));
}

//字符串的查找  截取  包含  拼接 大小写转换
void  str_find(){
    char* str1="name is tom";
    char* sub= "tom";
    char * position=strstr(str1,sub);//返回的是sub  字符串第一次出现的位置 指针 也就是t 的位置 没有找到返回NULL
    int po=position-str1;//用找到t的地址 减去 首地址  得到的就是索引 。因为数组的地址是连续的
    LOGD("找到的tom的位置 返回的是 %c",*position);
    LOGD("t 的位置 索引%d",po);
    if(position){
        //包含  说明position 不是NULL
    }else{
        // 不包含
    }

    char  dst[strlen(str1)];
    strcpy(dst, str1);//拷贝
    strcat(dst,sub);
    LOGD("拼接以后%s",dst);
}

//-----------------------指针--------
void testP(){
    int i = 100;
    int *p = &i;

    int y = 200;
    int *p1 = &y;
    LOGD(" i的地址 %p", &i);
    LOGD("p1的地址%p", &p1);

//    *p = *p1;
    int **p2 = &p;
    int **p3 = &p1;
    LOGD(" p2的值 %p", *p2);
    LOGD("p3的值%p", p3);
    int arr[] = {1, 2, 4};
    int i2 = 0;
    for (; i2 < 3; i2++) {

    }
    int * arrP=arr;
    LOGD("p的指针%p",p);
}

//-----------------------------内存开辟---------

void  mlloc(){
    //    int arr2[1024*1024*10];//静态内存开辟
    void* srr=malloc(sizeof(int) * 1024 * 1024*100); //动态内存开辟
    free(srr);
}
//---------------------------- 结构体-----------
struct Worker{
    char name[10];
    int age;
    int sallery;
}worker,worker2; //这种方式 结构体里面的变量 会有默认初始值

void testStruct(){
    //    Worker worker3= Worker();//这种方式没有 默认初始值
    //    worker.age=20;
    //    worker.sallery=30;
    //    strcpy(worker.name,"你好");
    //    strcpy(worker2.name,"哈哈");
    //
    Worker *wor_p=&worker;//声明结构体指针
    wor_p->age=20;
    LOGD("结构体%d",wor_p->age);

    Worker arr[10];//结构体数组

}
extern "C" JNIEXPORT jstring JNICALL
Java_com_kwai_video_uikit_ndk_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {

    test(add, 1, 2);//调用test放啊 需要传入一个方法指针
    change();
    str_len();
    str_compare();
    str_find();
    testStruct();
    std::string hello = "Hello from C++";

    return env->NewStringUTF(hello.c_str());
}


