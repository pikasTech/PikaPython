#ifndef __SHARE_H
#define __SHARE_H

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x)				((sizeof(x)/sizeof(x[0])))		//计算数组元素个数
#endif
void Delay_Ms(int ms);
#endif
