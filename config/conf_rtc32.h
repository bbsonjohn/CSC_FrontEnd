/**
 * \file
 *
 * \brief RTC32 configuration
 *
 * Copyright (c) 2010 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
#ifndef CONF_RTC32_H
#define CONF_RTC32_H

typedef enum RTC_PRESCALER_enum
{
	RTC_PRESCALER_OFF_gc = (0x00<<0),  /* RTC Off */
	RTC_PRESCALER_DIV1_gc = (0x01<<0),  /* RTC Clock */
	RTC_PRESCALER_DIV2_gc = (0x02<<0),  /* RTC Clock / 2 */
	RTC_PRESCALER_DIV8_gc = (0x03<<0),  /* RTC Clock / 8 */
	RTC_PRESCALER_DIV16_gc = (0x04<<0),  /* RTC Clock / 16 */
	RTC_PRESCALER_DIV64_gc = (0x05<<0),  /* RTC Clock / 64 */
	RTC_PRESCALER_DIV256_gc = (0x06<<0),  /* RTC Clock / 256 */
	RTC_PRESCALER_DIV1024_gc = (0x07<<0),  /* RTC Clock / 1024 */
} RTC_PRESCALER_t;

#define CONFIG_RTC32_COMPARE_INT_LEVEL RTC32_COMPINTLVL_LO_gc
#define CONFIG_RTC32_CLOCK_1024HZ
#define CONFIG_RTC32_PRESCALER          RTC_PRESCALER_DIV1_gc
#define CONFIG_RTC32_CLOCK_SOURCE       CLK_RTCSRC_RCOSC_gc


#endif /* CONF_RTC32_H */
