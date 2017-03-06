#ifndef D_NF_ITEM_H
#define D_NF_ITEM_H

#include "d_network_function_base.cuh"

typedef unsigned short int uint16;

typedef unsigned long int uint32;



// 短整型大小端互换

#define BigLittleSwap16(A)  ((((uint16)(A) & 0xff00) >> 8) | (((uint16)(A) & 0x00ff) << 8))



// 长整型大小端互换

#define BigLittleSwap32(A)  ((((uint32)(A) & 0xff000000) >> 24) | (((uint32)(A) & 0x00ff0000) >> 8) | (((uint32)(A) & 0x0000ff00) << 8) | (((uint32)(A) & 0x000000ff) << 24))



static constexpr int max_chain_length = 8;;



// 本机大端返回1，小端返回0

__device__ int checkCPUendian()

{

       union{

              unsigned long int i;

              unsigned char s[4];

       }c;



       c.i = 0x12345678;

       return (0x12 == c.s[0]);

}



// 模拟htonl函数，本机字节序转网络字节序

__device__ unsigned long int Htonl(unsigned long int h)

{

       // 若本机为大端，与网络字节序同，直接返回

       // 若本机为小端，转换成大端再返回

       return checkCPUendian() ? h : BigLittleSwap32(h);

}



// 模拟ntohl函数，网络字节序转本机字节序

__device__ unsigned long int Ntohl(unsigned long int n)

{

       // 若本机为大端，与网络字节序同，直接返回

       // 若本机为小端，网络数据转换成小端再返回

       return checkCPUendian() ? n : BigLittleSwap32(n);

}



// 模拟htons函数，本机字节序转网络字节序

__device__ unsigned short int Htons(unsigned short int h)

{

       // 若本机为大端，与网络字节序同，直接返回

       // 若本机为小端，转换成大端再返回

       return checkCPUendian() ? h : BigLittleSwap16(h);

}



// 模拟ntohs函数，网络字节序转本机字节序

__device__ unsigned short int Ntohs(unsigned short int n)

{

       // 若本机为大端，与网络字节序同，直接返回

       // 若本机为小端，网络数据转换成小端再返回

       return checkCPUendian()? n:BigLittleSwap16(n);

}

//自定义实现计算字符串的长度

__device__ unsigned long myStrlen(char string[])
{
    unsigned long length = 0;

    while (string[length] != '\0') {
        length++;
    }

    return length;
}
//自定义实现字符串的拷贝
__device__ void myStrcpy(char* string1, const char* string2)
{
    int i = 0;
    while (string2[i] != '\0') {
        string1[i] = string2[i];
        i++;
    }
    string1[i] = '\0';
}
//自定义函数实现字符串的凭拼接
__device__ void myStrcat(char string1[], char string2[])
{
    //找string1的'\0'位置
    int i = 0;
    while (string1[i] != '\0') {
        i++;
    }

    //把string2加到string1后面
    int j = 0;
    while (string2[j] != '\0') {
        string1[i++] = string2[j++];
    }

//不要忘记在最后添加\0
    string1[i] = '\0';
}
//自定义函数实现字符串的比较
__device__ int myStrcmp(char* string1, char* string2)
{
    int i = 0;
    while (string1[i] == string2[i] && string1[i] != '\0') {
        i++;
    }

    return string1[i] - string2[i];
}

struct d_flow_actor_nfs{
  d_network_function_base* nf[max_chain_length];
};

struct d_flow_actor_fs{
  char* nf_flow_state_ptr[max_chain_length];
};

struct d_flow_actor_fs_size{
  size_t nf_flow_state_size[max_chain_length];
};

static_assert(sizeof(d_flow_actor_nfs) == 64, "flow_actor_nfs can't fit into a cache line");

static_assert(sizeof(d_flow_actor_fs) == 64, "flow_actor_fs can't fit into a cache line");

static_assert(sizeof(d_flow_actor_fs_size) == 64, "flow_actor_fs_size can't fit into a cache line");

static_assert(std::is_pod<d_flow_actor_nfs>::value, "flow_actor_nfs is not pod");

static_assert(std::is_pod<d_flow_actor_fs>::value, "flow_actor_fs is not pod");

static_assert(std::is_pod<d_flow_actor_fs_size>::value, "flow_actor_fs_size is not pod");


#endif
