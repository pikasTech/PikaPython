#include "dataMemory.h"
/* 
    本文件代码修改自作者： piaolingtear
    来源：http://blog.csdn.net/u011833609/article/details/46834203
*/
static uint8_t DMEMORY[DMEM_TOTAL_SIZE];

DMEM_STATE DMEMS = {0};

DMEM *DynMemGet(uint32_t size)
{
    uint16_t loop = 0;
    uint16_t find = 0;
    uint16_t blk_num_want = 0;
    DMEM *user = NULL;
    DMEM_APPLY *apply = NULL;
    DMEMS.reqTimes++;

    //申请内存大小不能为0
    if (size == 0)
    {
        return NULL;
    }
    //申请内存不可超过总内存大小
    if (size > DMEM_TOTAL_SIZE)
    {
        return NULL;
    }
    //申请内存不可超过剩余内存大小
    if (size > (DMEM_BLOCK_NUM - DMEMS.blk_num) * DMEM_BLOCK_SIZE)
    {
        return NULL;
    }
    //申请表必须有空余
    if (DMEMS.apply_num >= DMEM_BLOCK_NUM)
    {
        return NULL;
    }

    //计算所需连续块的个数
    blk_num_want = (size + DMEM_BLOCK_SIZE - 1) / DMEM_BLOCK_SIZE;

    //寻找申请表
    for (loop = 0; loop < DMEM_BLOCK_NUM; loop++)
    {
        if (DMEMS.tb_apply[loop].used == DMEM_FREE)
        {
            apply = &DMEMS.tb_apply[loop];               //申请表已找到
            user = &DMEMS.tb_user[loop];                 //用户表对应找到
            user->tb = loop;                             //申请表编号记录
            user->size = blk_num_want * DMEM_BLOCK_SIZE; //分配大小计算
            break;
        }
    }

    //没有找到可用申请表，理论上是不会出现此现象的，申请表剩余已在上面校验
    if (loop == DMEM_BLOCK_NUM)
    {
        return NULL;
    }

    //寻找连续内存块
    for (loop = 0; loop < DMEM_BLOCK_NUM; loop++)
    {
        if (DMEMS.tb_blk[loop] == DMEM_FREE)
        { //找到第一个空闲内存块
            for (find = 1; (find < blk_num_want) && (loop + find < DMEM_BLOCK_NUM); find++)
            { //找到下一个空闲内存块
                if (DMEMS.tb_blk[loop + find] != DMEM_FREE)
                { //发现已使用内存块
                    break;
                }
            }
            if (find >= blk_num_want)
            {                                                          //寻找到的空闲内存块数目已经够用
                user->addr = (char *)DMEMORY + loop * DMEM_BLOCK_SIZE; //计算申请到的内存的地址
                apply->blk_s = loop;                                   //记录申请到的内存块首序号
                apply->blk_num = blk_num_want;                         //记录申请到的内存块数目
                for (find = 0; find < apply->blk_num; find++)
                {
                    DMEMS.tb_blk[loop + find] = DMEM_USED;
                }
                apply->used = DMEM_USED; //标记申请表已使用
                DMEMS.apply_num += 1;
                DMEMS.blk_num += blk_num_want;

                if (DMEMS.blk_num > DMEMS.maxNum)
                {
                    DMEMS.maxNum = DMEMS.blk_num;
                }
                return user;
            }
            else
            { //寻找到的空闲内存块不够用，从下一个开始找
                loop += find;
            }
        }
    }

    //搜索整个内存块，未找到大小适合的空间
    return NULL;
}

void DynMemPut(DMEM *user)
{
    uint16_t loop = 0;
    //若参数为空，直接返回
    if (NULL == user)
    {
        return;
    }

    //释放内存空间
    for (loop = DMEMS.tb_apply[user->tb].blk_s; loop < DMEMS.tb_apply[user->tb].blk_s + DMEMS.tb_apply[user->tb].blk_num; loop++)
    {
        DMEMS.tb_blk[loop] = DMEM_FREE;
        DMEMS.blk_num -= 1;
    }
    //释放申请表
    DMEMS.tb_apply[user->tb].used = DMEM_FREE;
    DMEMS.apply_num -= 1;
    user = NULL;
}
