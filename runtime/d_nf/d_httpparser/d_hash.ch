#ifndef HASHAA_H
#define HASHAA_H

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#define D_NULLKEY 0 // 0为无记录标志
#define N 10  // 数据元素个数



typedef char* d_KeyType;// 设关键字域为char*
typedef struct{
	char key[20];
	char value[100];
}d_ElemType; // 数据元素类型



// 开放定址哈希表的存储结构

typedef struct{
	d_ElemType *elem; // 数据元素存储基址，动态分配数组
	int count; // 当前数据元素个数
	int sizeindex; // hashsize[sizeindex]为当前容量
}d_HashTable;


#define SUCCESS 1
#define UNSUCCESS 0
#define DUPLICATE -1


// 构造一个空的哈希表
int d_InitHashTable(d_HashTable *H);

//  销毁哈希表H
void d_DestroyHashTable(d_HashTable *H);
// 一个简单的哈希函数(m为表长，全局变量)

unsigned int d_Hash(char *str);

// 开放定址法处理冲突
void d_collision(int *p,int d);
// 算法9.17
// 在开放定址哈希表H中查找关键码为K的元素,若查找成功,以p指示待查数据
// 元素在表中位置,并返回SUCCESS;否则,以p指示插入位置,并返回UNSUCCESS
// c用以计冲突次数，其初值置零，供建表插入时参考。
int d_SearchHash(d_HashTable H,d_KeyType K,int *p,int *c);
int d_InsertHash(d_HashTable *,d_ElemType); // 对函数的声明
// 重建哈希表
void d_RecreateHashTable(d_HashTable *H);
// 算法9.18
// 查找不成功时插入数据元素e到开放定址哈希表H中，并返回1；
// 若冲突次数过大，则重建哈希表。
int d_InsertHash(d_HashTable *H,d_ElemType e);
// 按哈希地址的顺序遍历哈希表
void TraverseHash(d_HashTable H,void(*Vi)(int,d_ElemType));
// 在开放定址哈希表H中查找关键码为K的元素,若查找成功,以p指示待查数据
// 元素在表中位置,并返回SUCCESS;否则,返回UNSUCCESS
int d_Find(d_HashTable H,d_KeyType K,int *p);
void d_print(int p,d_ElemType r);



#endif
