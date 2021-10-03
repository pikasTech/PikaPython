#ifndef __MEMORY_H__
#define __MEMORY_H__

/* 
    本文件代码修改自作者： piaolingtear
    来源：http://blog.csdn.net/u011833609/article/details/46834203
*/

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define DMEM_BLOCK_SIZE 32                                 //内存块大小为32字节
#define DMEM_BLOCK_NUM 32 * 72                             //内存块个数为2048个
#define DMEM_TOTAL_SIZE (DMEM_BLOCK_SIZE * DMEM_BLOCK_NUM) //内存总大小

typedef enum
{
    DMEM_FREE = 0,
    DMEM_USED = 1,
} DMEM_USED_ITEM;

typedef struct
{
    DMEM_USED_ITEM used; //使用状态
    uint16_t blk_s;      //起始块序号
    uint16_t blk_num;    //块个数
} DMEM_APPLY;
//用户使用
typedef struct
{
    uint8_t *addr; //申请到的内存的起始地址
    uint32_t size; //申请到的内存的大小，按照块大小分配，大于等于申请大小
    uint16_t tb;   //申请表序号，申请内存时分配，释放内存时使用，用户不使用
} DMEM;

typedef struct
{
    DMEM_USED_ITEM tb_blk[DMEM_BLOCK_NUM];
    DMEM tb_user[DMEM_BLOCK_NUM];        //用户申请内存信息
    DMEM_APPLY tb_apply[DMEM_BLOCK_NUM]; //系统分配内存信息
    uint16_t apply_num;                  //内存申请表占用数目
    uint16_t blk_num;                    //内存块占用数目
    uint16_t reqTimes;                   //memory request times
    uint16_t maxNum;
} DMEM_STATE;

//若返回空，则申请失败
DMEM *DynMemGet(uint32_t size);
void DynMemPut(DMEM *pDmem);

#endif //__MEMORY_H__
