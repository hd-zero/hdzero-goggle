/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : audioProcesser.c
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/05/19
  Last Modified :
  Description   : mpi functions implement
  Function List :
  History       :
******************************************************************************/


#include <errno.h>


struct SpeexResamplerState_;
typedef struct SpeexResamplerState_ SpeexResamplerState;

/** Create a new resampler with integer input and output rates.
 * @param nb_channels Number of channels to be processed
 * @param in_rate Input sampling rate (integer number of Hz).
 * @param out_rate Output sampling rate (integer number of Hz).
 * @param quality Resampling quality between 0 and 10, where 0 has poor quality
 * and 10 has very high quality.
 * @return Newly created resampler state
 * @retval NULL Error: not enough memory
 */
SpeexResamplerState *speex_resampler_init(spx_uint32_t nb_channels, 
                                          spx_uint32_t in_rate, 
                                          spx_uint32_t out_rate, 
                                          int quality,
                                          int *err);

/** Destroy a resampler state.
 * @param st Resampler state
 */
void speex_resampler_destroy(SpeexResamplerState *st);

/** Resample an int array. The input and output buffers must *not* overlap.
 * @param st Resampler state
 * @param channel_index Index of the channel to process for the multi-channel 
 * base (0 otherwise)
 * @param in Input buffer
 * @param in_len Number of input samples in the input buffer. Returns the number
 * of samples processed
 * @param out Output buffer
 * @param out_len Size of the output buffer. Returns the number of samples written
 */
int speex_resampler_process_int(SpeexResamplerState *st, 
                                 spx_uint32_t channel_index, 
                                 const spx_int16_t *in, 
                                 spx_uint32_t *in_len, 
                                 spx_int16_t *out, 
                                 spx_uint32_t *out_len);

/** Resample an interleaved int array. The input and output buffers must *not* overlap.
 * @param st Resampler state
 * @param in Input buffer
 * @param in_len Number of input samples in the input buffer. Returns the number
 * of samples processed. This is all per-channel.
 * @param out Output buffer
 * @param out_len Size of the output buffer. Returns the number of samples written.
 * This is all per-channel.
 */
int speex_resampler_process_interleaved_int(SpeexResamplerState *st, 
                                             const spx_int16_t *in, 
                                             spx_uint32_t *in_len, 
                                             spx_int16_t *out, 
                                             spx_uint32_t *out_len);

