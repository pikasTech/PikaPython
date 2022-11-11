/*
 * @Author       : jiejie
 * @GitHub       : https://github.com/jiejieTop
 * @Date         : 2021-02-26 12:00:24
 * @LastEditors  : jiejie
 * @LastEditTime : 2022-06-15 19:40:10
 * @FilePath     : /mqttclient/common/random.h
 * Copyright (c) 2022 jiejie, All Rights Reserved. Please keep the author information and source code according to the license.
 */


#ifndef _RANDOM_H_
#define _RANDOM_H_

#ifdef __cplusplus
extern "C" {
#endif

#define RANDOM_MAX 0x7FFFFFFF

int random_number(void);
int random_number_range(unsigned int min, unsigned int max);
int random_string(char *buffer, int len);

#ifdef __cplusplus
}
#endif

#endif /* _RANDOM_H_ */
