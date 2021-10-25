/*
  ******************************************************************************
  *
  * COPYRIGHT(c) 2020, China Mobile IOT
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of China Mobile IOT nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */ 
/*****************************************************************************
* File Name: cm32m101a_rng.h
* Function: Declaring RNG algorithm library API
* version: V1.1
* date: 2020-4-8
* ****************************************************************************/ 

#ifndef  _CM32M101A_RNG_H_
#define  _CM32M101A_RNG_H_

#include <stdint.h>

enum{
	RNG_OK = 0x5a5a5a5a,
	LENError = 0x311ECF50,   //RNG generation of key length error 
    ADDRNULL = 0x7A9DB86C,   // This address is empty
};


//u32 RNG_init(void);
/**
 * @brief Get pseudo random number
 * @param[out] rand pointer to random number
 * @param[in] the wordlen of random number
 * @param[in] the seed, can be NULL
 * @return RNG_OK:get random number success; othets: get random number fail 
* @note attention: you can this function over,the RCC->CFG3 18bit  TRNG1MEN has not closed
 */
uint32_t GetPseudoRand_U32(uint32_t *rand, uint32_t wordLen,uint32_t seed[2]);


/**
 * @brief Get true random number
 * @param[out] rand pointer to random number
 * @param[in] the wordlen of random number
 * @return RNG_OK:get random number success; othets: get random number fail 
 * @note  attention: you can this function over,the RCC->CFG3 18bit  TRNG1MEN has not closed
 */
uint32_t GetTrueRand_U32(uint32_t *rand, uint32_t wordLen);

/**
 * @brief Get RNG lib version
 * @param[out] type pointer one byte type information represents the type of the lib, like Commercial version.\
 * @Bits 0~4 stands for Commercial (C), Security (S), Normal (N), Evaluation (E), Test (T), Bits 5~7 are reserved. e.g. 0x09 stands for CE version.
 * @param[out] customer pointer one byte customer information represents customer ID. for example, 0x00 stands for standard version, 0x01 is for Tianyu customized version...
 * @param[out] date pointer array which include three bytes date information. If the returned bytes are 18,9,13,this denotes September 13,2018 
 * @param[out] version pointer one byte version information represents develop version of the lib. e.g. 0x12 denotes version 1.2.
 * @return none
 * @1.You can recall this function to get RSA lib information
 */
void RNG_Version(uint8_t *type, uint8_t *customer, uint8_t date[3], uint8_t *version);

#endif


