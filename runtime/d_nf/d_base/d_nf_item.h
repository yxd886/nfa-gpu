#ifndef D_NF_ITEM_H
#define D_NF_ITEM_H

#include "d_network_function_base.ch"

typedef unsigned short int uint16;

typedef unsigned long int uint32;



// 短整型大小端互换

#define BigLittleSwap16(A)  ((((uint16)(A) & 0xff00) >> 8) | (((uint16)(A) & 0x00ff) << 8))



// 长整型大小端互换

#define BigLittleSwap32(A)  ((((uint32)(A) & 0xff000000) >> 24) | (((uint32)(A) & 0x00ff0000) >> 8) | (((uint32)(A) & 0x0000ff00) << 8) | (((uint32)(A) & 0x000000ff) << 24))



static constexpr int max_chain_length = 8;;



// 本机大端返回1，小端返回0

int checkCPUendian()

{

       union{

              unsigned long int i;

              unsigned char s[4];

       }c;



       c.i = 0x12345678;

       return (0x12 == c.s[0]);

}



// 模拟htonl函数，本机字节序转网络字节序

unsigned long int htonl(unsigned long int h)

{

       // 若本机为大端，与网络字节序同，直接返回

       // 若本机为小端，转换成大端再返回

       return checkCPUendian() ? h : BigLittleSwap32(h);

}



// 模拟ntohl函数，网络字节序转本机字节序

unsigned long int ntohl(unsigned long int n)

{

       // 若本机为大端，与网络字节序同，直接返回

       // 若本机为小端，网络数据转换成小端再返回

       return checkCPUendian() ? n : BigLittleSwap32(n);

}



// 模拟htons函数，本机字节序转网络字节序

unsigned short int htons(unsigned short int h)

{

       // 若本机为大端，与网络字节序同，直接返回

       // 若本机为小端，转换成大端再返回

       return checkCPUendian() ? h : BigLittleSwap16(h);

}



// 模拟ntohs函数，网络字节序转本机字节序

unsigned short int ntohs(unsigned short int n)

{

       // 若本机为大端，与网络字节序同，直接返回

       // 若本机为小端，网络数据转换成小端再返回

       return checkCPUendian()? n:BigLittleSwap16(n);

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
