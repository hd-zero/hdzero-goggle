#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "spspps_patch.h"

// sps bs struct
typedef struct
{
    uint8_t* dat;  //stream data
    int len;       //stream data bits num
    int idx;       //current index
    int sps_start; //sps data first bit pos
    int sps_end;   //sps data last bit pos+1
} sps_bit_stream;

int bs_fine_sps_pos(sps_bit_stream* bs, uint8_t nal_type_sps)
{
    int i, ret = -1;
    for (i = 3; i < (bs->len>>3) ; i++) {
        if (bs->dat[i - 3] == 0x00 && bs->dat[i - 2] == 0x00 && bs->dat[i - 1] == 0x01 && bs->dat[i] == nal_type_sps) {
            bs->sps_start = i << 3;
            ret = 0;
            break;
        }
    }

    if (ret == -1)
        return -1;

    bs->sps_end = bs->len;
    for (i = (bs->sps_start >> 3); i < (bs->len >> 3) - 2; i++) {
        if (bs->dat[i] == 0x00 && bs->dat[i + 1] == 0x00 && bs->dat[i + 2] <= 0x01) {
            bs->sps_end = i << 3;
            break;
        }
    }

    return ret;
}

// is_end of bs
int bs_eof(sps_bit_stream* bs)
{
    return (bs->idx >= bs->len);
}

// pop data from bs
uint32_t bs_pop_u(sps_bit_stream* bs, int pop_cnt)
{
    int i;
    uint32_t val = 0;
    for (i = 0; i < pop_cnt; i++) {
        val <<= 1;
        if (bs_eof(bs)) {
            val = 0;
            break;
        }
        else if (bs->dat[(bs->idx) >> 3] & (0x80 >> (bs->idx % 8))) {
            val |= 1;
        }

        bs->idx++;
    }

    return val;
}

// pop unsigned Exponential-Golomb coding data from bs
uint32_t bs_pop_ue(sps_bit_stream* bs)
{
    int zeroNum = 0;
    while (bs_pop_u(bs, 1) == 0 && !bs_eof(bs) && zeroNum < 32) {
        zeroNum++;
    }

    return (uint32_t)((1 << zeroNum) - 1 + bs_pop_u(bs, zeroNum));
}

// pop signed Exponential-Golomb coding data from bs
int32_t bs_pop_se(sps_bit_stream* bs)
{
    uint32_t ueVal = (int32_t)bs_pop_ue(bs);

    int32_t seVal = (ueVal+1) >> 1;
    if (ueVal % 2 == 0) {
        seVal = -seVal;
    }

    return seVal;
}

void bs_push_u(uint32_t push_dat, int push_cnt, sps_bit_stream* bs)
{
    int i = 0;
    uint8_t val = 0;

    for (i = 0; i < push_cnt; i++) {
        val = (push_dat >> (push_cnt - i - 1)) & 0x01;
        if (val) {
            val = (0x80 >> (bs->idx % 8));
            bs->dat[(bs->idx) >> 3] |= val;
        }
        else {
            val = (0x80 >> (bs->idx % 8));
            val = ~val;
            bs->dat[(bs->idx) >> 3] &= val;
        }

        bs->idx++;
    }
}

void bs_insert_u(uint32_t insert_dat, int insert_cnt, sps_bit_stream* bs)
{
    int i, m;
    uint8_t val;

    for (i = bs->len-1; i >= bs->idx; i--) {
        m = i + insert_cnt;
        val = (0x80 >> (m % 8));

        if (bs->dat[i >> 3] & (0x80 >> (i % 8))) {
            bs->dat[m >> 3] |= val;
        }
        else {
            val = ~val;
            bs->dat[m >> 3] &= val;
        }
    }
    bs->len += insert_cnt;
    bs->sps_end += insert_cnt;
    bs_push_u(insert_dat, insert_cnt, bs);
}

void vui_para_parse(sps_bit_stream* bs, int fps)
{
    uint32_t fps_cal = 0;

    uint32_t aspect_ratio_info_present_flag = bs_pop_u(bs, 1);
    if (aspect_ratio_info_present_flag) {
        uint32_t aspect_ratio_idc = bs_pop_u(bs, 8);
        if (aspect_ratio_idc == 255) {      //Extended_SAR
            bs_pop_u(bs, 16);      //sar_width
            bs_pop_u(bs, 16);      //sar_height
        }
    }

    uint32_t overscan_info_present_flag = bs_pop_u(bs, 1);
    if (overscan_info_present_flag) {
        bs_pop_u(bs, 1);       //overscan_appropriate_flag
    }

    uint32_t video_signal_type_present_flag = bs_pop_u(bs, 1);
    if (video_signal_type_present_flag) {
        bs_pop_u(bs, 3);       //video_format
        bs_pop_u(bs, 1);       //video_full_range_flag
        uint32_t colour_description_present_flag = bs_pop_u(bs, 1);
        if (colour_description_present_flag) {
            bs_pop_u(bs, 8);       //colour_primaries
            bs_pop_u(bs, 8);       //transfer_characteristics
            bs_pop_u(bs, 8);       //matrix_coefficients
        }
    }

    uint32_t chroma_loc_info_present_flag = bs_pop_u(bs, 1);
    if (chroma_loc_info_present_flag) {
        bs_pop_ue(bs);     //chroma_sample_loc_type_top_field
        bs_pop_ue(bs);     //chroma_sample_loc_type_bottom_field
    }

    uint32_t timing_info_present_flag = bs_pop_u(bs, 1);
    if (timing_info_present_flag) {
        bs_push_u(1, 32, bs);
        bs->idx -= 32;
        uint32_t num_units_in_tick = bs_pop_u(bs, 32);

        bs_push_u(fps, 32, bs);
        bs->idx -= 32;
        uint32_t time_scale = bs_pop_u(bs, 32);

        bs_push_u(1, 1, bs);
        bs->idx -= 1;
        uint32_t fixed_frame_rate_flag = bs_pop_u(bs, 1);

        fps_cal = (uint32_t)((float)time_scale / (float)num_units_in_tick);
        if (1/*fixed_frame_rate_flag*/) {
            fps_cal = fps_cal / 2;
        }
    }
    else {
        bs->idx -= 1;
        bs_push_u(1, 1, bs);

        bs->idx -= 1;
        timing_info_present_flag = bs_pop_u(bs, 1);
        if (timing_info_present_flag) {
            bs_insert_u(1, 32, bs);
            bs->idx -= 32;
            uint32_t num_units_in_tick = bs_pop_u(bs, 32);

            bs_insert_u(fps, 32, bs);
            bs->idx -= 32;
            uint32_t time_scale = bs_pop_u(bs, 32);

            bs_insert_u(1, 1, bs);
            bs->idx -= 1;
            uint32_t fixed_frame_rate_flag = bs_pop_u(bs, 1);

            fps_cal = (uint32_t)((float)time_scale / (float)num_units_in_tick);
            if (1/*fixed_frame_rate_flag*/) {
                fps_cal = fps_cal / 2;
            }
        }
    }

    uint32_t nal_hrd_parameters_present_flag = bs_pop_u(bs, 1);
    if (nal_hrd_parameters_present_flag) {
        //hrd_parameters()  //see E.1.2 HRD parameters syntax
    }

    //后面代码需要hrd_parameters()函数接口实现才有用
    uint32_t vcl_hrd_parameters_present_flag = bs_pop_u(bs, 1);
    if (vcl_hrd_parameters_present_flag) {
        //hrd_parameters()
    }
    if (nal_hrd_parameters_present_flag || vcl_hrd_parameters_present_flag) {
        bs_pop_u(bs, 1);   //low_delay_hrd_flag
    }

    bs_pop_u(bs, 1);       //pic_struct_present_flag
    uint32_t bitstream_restriction_flag = bs_pop_u(bs, 1);
    if (bitstream_restriction_flag) {
        bs_pop_u(bs, 1);   //motion_vectors_over_pic_boundaries_flag
        bs_pop_ue(bs);     //max_bytes_per_pic_denom
        bs_pop_ue(bs);     //max_bits_per_mb_denom
        bs_pop_ue(bs);     //log2_max_mv_length_horizontal
        bs_pop_ue(bs);     //log2_max_mv_length_vertical
        bs_pop_ue(bs);     //max_num_reorder_frames
        bs_pop_ue(bs);     //max_dec_frame_buffering
    }
}

int bs_insert_vui(sps_bit_stream* bs, int fps)
{
    int org_idx;

    bs->idx--;
    org_idx = bs->idx;
    bs_push_u(1, 1, bs); // vui_parameters_present_flag

    bs_insert_u(0, 1, bs); // aspect_ratio_info_present_flag
    bs_insert_u(0, 1, bs); // overscan_info_present_flag
    bs_insert_u(0, 1, bs); // video_signal_type_present_flag
    bs_insert_u(0, 1, bs); // chroma_loc_info_present_flag

    bs_insert_u(1, 1, bs); // timing_info_present_flag
    bs_insert_u(1, 32, bs); // num_units_in_tick
    bs_insert_u(fps, 32, bs); // time_scale
    bs_insert_u(1, 1, bs); // fixed_frame_rate_flag

    bs_insert_u(0, 1, bs); // nal_hrd_parameters_present_flag
    bs_insert_u(0, 1, bs); // vcl_hrd_parameters_present_flag
    bs_insert_u(0, 1, bs); // pic_struct_present_flag
    bs_insert_u(0, 1, bs); // bitstream_restriction_flag

    return (bs->idx - org_idx);
}

void bs_remove_emulation(sps_bit_stream* bs)
{
    int i = 0, j = 0;

    for (i = (bs->sps_start >> 3); i < ((bs->sps_end >> 3) - 2); i++) {
        if((bs->dat[i] == 0x00) && (bs->dat[i + 1] == 0x00) && (bs->dat[i + 2] == 0x03)) {
            for (j = i + 2; j < (bs->len >> 3) - 1; j++) {
                bs->dat[j] = bs->dat[j + 1];
            }

            bs->len -= 8;  // Byte_num - 1
            bs->sps_end -= 8;
        }
    }
}

void bs_add_emulation(sps_bit_stream* bs)
{
    int i;

    for (i = (bs->sps_start >> 3) + 2; i < (bs->sps_end >> 3); i++) {
        if (bs->dat[i - 2] == 0x00 && bs->dat[i - 1] == 0x00 && bs->dat[i] <= 0x03) {
            bs->idx = (i << 3);
            bs_insert_u(0x03, 8, bs);
            i++;
        }
    }
}

void profile_tier_level(sps_bit_stream* bs, int maxNumSubLayersMinus1)
{
    int i, j;
    uint8_t general_profile_compatibility_flag[32] = { 0 };
    uint8_t sub_layer_profile_present_flag[32] = { 0 };
    uint8_t sub_layer_level_present_flag[32] = { 0 };
    uint8_t sub_layer_profile_idc[8] = { 0 };
    uint8_t sub_layer_profile_compatibility_flag[8][32] = { 0 };

    bs_pop_u(bs, 2); // general_profile_space
    bs_pop_u(bs, 1); // general_tier_flag
    uint32_t general_profile_idc = bs_pop_u(bs, 5); // general_profile_idc
    for (j = 0; j < 32; j++) {
        general_profile_compatibility_flag[j] = (uint8_t)bs_pop_u(bs, 1); // general_profile_compatibility_flag
    }
    bs_pop_u(bs, 1); // general_progressive_source_flag
    bs_pop_u(bs, 1); // general_interlaced_source_flag
    bs_pop_u(bs, 1); // general_non_packed_constraint_flag
    bs_pop_u(bs ,1); // general_frame_only_constraint_flag
    //profile_tier_level_data->general_reserved_zero_44bits = (uint64)read_bits64(pnal_buffer, 44);
    if (general_profile_idc == 4 || general_profile_compatibility_flag[4] ||
        general_profile_idc == 5 || general_profile_compatibility_flag[5] ||
        general_profile_idc == 6 || general_profile_compatibility_flag[6] ||
        general_profile_idc == 7 || general_profile_compatibility_flag[7]) {
        /* The number of bits in this syntax structure is not affected by this condition */
        bs_pop_u(bs, 1); // general_max_12bit_constraint_flag
        bs_pop_u(bs, 1); // general_max_10bit_constraint_flag
        bs_pop_u(bs, 1); //	general_max_8bit_constraint_flag
        bs_pop_u(bs, 1); // general_max_422chroma_constraint_flag
        bs_pop_u(bs, 1); // general_max_420chroma_constraint_flag
        bs_pop_u(bs, 1); // general_max_monochrome_constraint_flag
        bs_pop_u(bs, 1); // general_intra_constraint_flag
        bs_pop_u(bs, 1); // general_one_picture_only_constraint_flag
        bs_pop_u(bs, 1); // general_lower_bit_rate_constraint_flag
        bs_pop_u(bs, 34); // general_reserved_zero_34bits
    }
    else {
        bs_pop_u(bs, 43);// general_reserved_zero_34bits
        if ((general_profile_idc >= 1 && general_profile_idc <= 5) ||
            general_profile_compatibility_flag[1] || general_profile_compatibility_flag[2] ||
            general_profile_compatibility_flag[3] || general_profile_compatibility_flag[4] ||
            general_profile_compatibility_flag[5]) {
            /* The number of bits in this syntax structure is not affected by this condition */
            bs_pop_u(bs, 1); // general_inbld_flag
        }
        else {
            bs_pop_u(bs, 1); // general_reserved_zero_bit
        }
    }
    bs_pop_u(bs, 8); // general_level_idc
    for (i = 0; i < maxNumSubLayersMinus1; i++) {
        sub_layer_profile_present_flag[i] = (uint8_t)bs_pop_u(bs, 1);
        sub_layer_level_present_flag[i] = (uint8_t)bs_pop_u(bs, 1);
    }

    if (maxNumSubLayersMinus1 > 0) {
        for (i = maxNumSubLayersMinus1; i < 8; i++) {
            bs_pop_u(bs, 2); // reserved_zero_2bits[i]
        }
    }

    for (i = 0; i < maxNumSubLayersMinus1; i++) {
        if (sub_layer_profile_present_flag[i]) {
            bs_pop_u(bs, 2); // sub_layer_profile_spa
            bs_pop_u(bs, 1); // sub_layer_tier_flag
            sub_layer_profile_idc[i] = (uint8_t)bs_pop_u(bs, 5); // sub_layer_profile_idc
            for (j = 0; j < 32; j++) {
                sub_layer_profile_compatibility_flag[i][j] = bs_pop_u(bs, 1);
            }
            bs_pop_u(bs, 1); // sub_layer_progressive_source_flag
            bs_pop_u(bs, 1); // sub_layer_interlaced_source_flag
            bs_pop_u(bs, 1); // sub_layer_non_packed_constraint_flag
            bs_pop_u(bs, 1); // sub_layer_frame_only_constraint_flag
            //profile_tier_level_data->sub_layer_reserved_zero_44bits[i] = (uint64)read_bits64(pnal_buffer, 44);

            if (sub_layer_profile_idc[i] == 4 ||
                sub_layer_profile_compatibility_flag[i][4] ||
                sub_layer_profile_idc[i] == 5 ||
                sub_layer_profile_compatibility_flag[i][5] ||
                sub_layer_profile_idc[i] == 6 ||
                sub_layer_profile_compatibility_flag[i][6] ||
                sub_layer_profile_idc[i] == 7 ||
                sub_layer_profile_compatibility_flag[i][7]) {

                /* The number of bits in this syntax structure is not affected by this condition */
                bs_pop_u(bs, 1);// sub_layer_max_12bit_constraint_flag
                bs_pop_u(bs, 1);// sub_layer_max_10bit_constraint_flag
                bs_pop_u(bs, 1);// sub_layer_max_8bit_constraint_flag
                bs_pop_u(bs, 1);// sub_layer_max_422chroma_constraint_flag
                bs_pop_u(bs, 1);// sub_layer_max_420chroma_constraint_flag
                bs_pop_u(bs, 1);// sub_layer_max_monochrome_constraint_flag
                bs_pop_u(bs, 1);// sub_layer_intra_constraint_flag
                bs_pop_u(bs, 1);// sub_layer_one_picture_only_constraint_flag
                bs_pop_u(bs, 1);// sub_layer_lower_bit_rate_constraint_flag
                bs_pop_u(bs, 34);// sub_layer_reserved_zero_34bits
            }
            else {
                bs_pop_u(bs, 43);// sub_layer_reserved_zero_43bits
            }
            if ((sub_layer_profile_idc[i] >= 1 && sub_layer_profile_idc[i] <= 5) ||
                sub_layer_profile_compatibility_flag[1] ||
                sub_layer_profile_compatibility_flag[2] ||
                sub_layer_profile_compatibility_flag[3] ||
                sub_layer_profile_compatibility_flag[4] ||
                sub_layer_profile_compatibility_flag[5]) {
                /* The number of bits in this syntax structure is not affected by this condition */
                bs_pop_u(bs, 1);// sub_layer_inbld_flag
            }
            else {
                bs_pop_u(bs, 1); // sub_layer_reserved_zero_bit
            }
        }
        if (sub_layer_level_present_flag[i]) {
            bs_pop_u(bs, 8);
        }
    }
}

void scaling_list_data(sps_bit_stream* bs)
{
    uint8_t scaling_list_pred_mode_flag[4][6] = { 0 };
    for (int size_id = 0; size_id < 4; size_id++) {
        for (int matrix_id = 0; matrix_id < (size_id == 3) ? 2 : 6; matrix_id++) {
            scaling_list_pred_mode_flag[size_id][matrix_id] = bs_pop_u(bs, 1); // [sizeId][matrixId]
            if (!scaling_list_pred_mode_flag[size_id][matrix_id]) { // [sizeId][matrixId]
                bs_pop_ue(bs); // scaling_list_pred_matrix_id_delta
            }
            else {
                uint8_t next_coef = 8;
                uint32_t coef_num = (1 << (4 + (size_id << 1)));
                if (coef_num > 64) coef_num = 64;

                if (size_id > 1) {
                    next_coef = bs_pop_ue(bs); // scaling_list_dc_coef_minus8[size_id − 2][matrix_id]
                }
                for (int i = 0; i < coef_num; i++) {
                    next_coef = (next_coef + bs_pop_se(bs) + 256) % 256;
                    //					scaling_list_data->scaling_list[size_id][matrix_id][i] = next_coef;
                }
            }
        }
    }
}

void short_term_ref_pic_set(sps_bit_stream* bs, int stRpsIdx, int num_short_term_ref_pic_sets)
{
#define MAX_pic_sets    64
#define MAX_k           16
    static int num_negative_pics[MAX_pic_sets] = { 0 };
    static int num_positive_pics[MAX_pic_sets] = { 0 };
    static int NumDeltaPocs[MAX_pic_sets] = { 0 };
    static int m_deltaPOC[MAX_pic_sets][MAX_k] = { 0 };
    static uint8_t m_used[MAX_pic_sets][MAX_k] = { 0 };

    uint8_t inter_ref_pic_set_prediction_flag = 0;
    uint32_t delta_idx_minus1 = 0;

    if (stRpsIdx != 0) {
        inter_ref_pic_set_prediction_flag = bs_pop_u(bs, 1);
    }

    if (inter_ref_pic_set_prediction_flag) {
        if (stRpsIdx == num_short_term_ref_pic_sets) {
            delta_idx_minus1 = bs_pop_ue(bs);
        }

        int RefRpsIdx = stRpsIdx - (delta_idx_minus1 + 1);

        int k = 0, k0 = 0, k1 = 0;
        int m = 0;

        uint8_t rbit = bs_pop_u(bs, 1); // delta_rps_sign
        uint32_t rcode = bs_pop_ue(bs); // abs_delta_rps_minus1
        int deltaRPS = (1 - 2 * rbit) * (rcode + 1); // delta_RPS

        for (int j = 0; j <= NumDeltaPocs[RefRpsIdx]; j++) {
            uint8_t used_by_curr_pic_flag = bs_pop_u(bs, 1);
            int refIdc = used_by_curr_pic_flag;

            if (refIdc == 0) { // (!used_by_curr_pic_flag[j])
                uint8_t use_delta_flag = bs_pop_u(bs, 1); // use_delta_flag[j]
                refIdc = use_delta_flag << 1;
            }

            if (refIdc == 1 || refIdc == 2) {
                int deltaPOC = deltaRPS + ((j < NumDeltaPocs[RefRpsIdx]) ? m_deltaPOC[RefRpsIdx][j] : 0);
                m_deltaPOC[stRpsIdx][k] = deltaPOC; // rps->setDeltaPOC(k, deltaPOC);
                m_used[stRpsIdx][k] = (refIdc == 1) ? 1 : 0; // rps->setUsed(k, (refIdc == 1));

                if (deltaPOC < 0) {
                    k0++;
                }
                else {
                    k1++;
                }
                k++;
            }
            //rps->setRefIdc(j, refIdc);
        }
        //rps->setNumRefIdc(NumDeltaPocs[RefRpsIdx] + 1);
        NumDeltaPocs[stRpsIdx] = k;       // rps->setNumberOfPictures(k);
        num_negative_pics[stRpsIdx] = k0; // rps->setNumberOfNegativePictures(k0);
        num_positive_pics[stRpsIdx] = k1; // rps->setNumberOfPositivePictures(k1);
//printf("sssssssssssssssss-k=%d, k0=%d, k1=%d\n", k, k0, k1);

        //rps->sortDeltaPOC();-----------------------------------------------------
         // sort in increasing order (smallest first)
        for (m = 1; m < NumDeltaPocs[stRpsIdx]; m++)
        {
            int deltaPOC_t = m_deltaPOC[stRpsIdx][m]; // getDeltaPOC(m);
            uint8_t used = m_used[stRpsIdx][m]; // getUsed(m);
            for (int k = m - 1; k >= 0; k--)
            {
                int temp = m_deltaPOC[stRpsIdx][k]; // getDeltaPOC(k);
                if (deltaPOC_t < temp)
                {
                    m_deltaPOC[stRpsIdx][k + 1] = temp; // setDeltaPOC(k + 1, temp);
                    m_used[stRpsIdx][k + 1] = m_used[stRpsIdx][k]; // setUsed(k + 1, getUsed(k));
                    m_deltaPOC[stRpsIdx][k] = deltaPOC_t; // setDeltaPOC(k, deltaPOC_t);
                    m_used[stRpsIdx][k] = used; // setUsed(k, used);
                }
            }
        }
//printf("sssssssssssssssss0-m=%d\n", m);
        // flip the negative values to largest first
        int numNegPics = num_negative_pics[stRpsIdx];
        for (m = 0, k = numNegPics - 1; m < numNegPics >> 1; m++, k--)
        {
            int deltaPOC_t = m_deltaPOC[stRpsIdx][m]; // getDeltaPOC(m);
            uint8_t used = m_used[stRpsIdx][m]; // getUsed(m);
            m_deltaPOC[stRpsIdx][m] = m_deltaPOC[stRpsIdx][k]; // setDeltaPOC(m, getDeltaPOC(k));
            m_used[stRpsIdx][m] = m_used[stRpsIdx][k]; // setUsed(m, getUsed(k));
            m_deltaPOC[stRpsIdx][k] = deltaPOC_t; // setDeltaPOC(k, deltaPOC_t);
            m_used[stRpsIdx][k] = used; // setUsed(k, used);
        }
//printf("sssssssssssssssss1-m=%d, k=%d, k=%d\n", m, numNegPics - 1, k);
        //-------------------------------------------------------------------------
    }
    else {
        num_negative_pics[stRpsIdx] = bs_pop_ue(bs);
        num_positive_pics[stRpsIdx] = bs_pop_ue(bs);

        int prev = 0;
        int poc;
        uint32_t rcode;
        int i;

        for (i = 0; i < num_negative_pics[stRpsIdx]; i++) {
            rcode = bs_pop_ue(bs); // delta_poc_s0_minus1
            poc = prev - rcode - 1;
            prev = poc;
            m_deltaPOC[stRpsIdx][i] = poc; // rps->setDeltaPOC(i, poc);
            m_used[stRpsIdx][i] = bs_pop_u(bs, 1); // used_by_curr_pic_s0_flag
        }
//printf("sssssssssssssssss0-i=%d\n", i);

        prev = 0;
        for (i = num_negative_pics[stRpsIdx]; i < (num_negative_pics[stRpsIdx] + num_positive_pics[stRpsIdx]); i++) {
            rcode = bs_pop_ue(bs); // delta_poc_s1_minus1
            poc = prev + rcode + 1;
            prev = poc;
            m_deltaPOC[stRpsIdx][i] = poc; // rps->setDeltaPOC(i, poc);
            m_used[stRpsIdx][i] = bs_pop_u(bs, 1); // used_by_curr_pic_s1_flag
        }
//printf("sssssssssssssssss1-i=%d\n", i);

        NumDeltaPocs[stRpsIdx] = num_negative_pics[stRpsIdx] + num_positive_pics[stRpsIdx];
    }
}

void sub_layer_hrd_parameters(sps_bit_stream* bs, uint32_t CpbCnt, uint8_t sub_pic_hrd_params_present_flag)
{
    for (int i = 0; i <= CpbCnt; i++) {
        bs_pop_ue(bs); // cpb_size_value_minus1
        bs_pop_ue(bs); // bit_rate_value_minus1
        if (sub_pic_hrd_params_present_flag) {
            bs_pop_ue(bs); // cpb_size_du_value_minus1
            bs_pop_ue(bs); // bit_rate_du_value_minus1
        }
        bs_pop_u(bs, 1); // cbr_flag
    }
}

void hrd_parameters(sps_bit_stream* bs, uint8_t commonInfPresentFlag, uint32_t maxNumSubLayersMinus1)
{
    uint8_t nal_hrd_parameters_present_flag, vcl_hrd_parameters_present_flag;
    uint8_t sub_pic_cpb_params_present_flag;
    if (commonInfPresentFlag) {
        nal_hrd_parameters_present_flag = bs_pop_u(bs, 1);
        vcl_hrd_parameters_present_flag = bs_pop_u(bs, 1);
        if (nal_hrd_parameters_present_flag || vcl_hrd_parameters_present_flag) {
            sub_pic_cpb_params_present_flag = bs_pop_u(bs, 1);
            if (sub_pic_cpb_params_present_flag) {
                bs_pop_u(bs, 8); // tick_divisor_minus2
                bs_pop_u(bs, 5); // du_cpb_removal_delay_increment_length_minus1
                bs_pop_u(bs, 1); // sub_pic_cpb_params_in_pic_timing_sei_flag
                bs_pop_u(bs, 5); // dpb_output_delay_du_length_minus1
            }

            bs_pop_u(bs, 4); // bit_rate_scale
            bs_pop_u(bs, 4); // cpb_size_scale
            if (sub_pic_cpb_params_present_flag) {
                bs_pop_u(bs, 4); // cpb_size_du_scale
            }
            bs_pop_u(bs, 5); // initial_cpb_removal_delay_length_minus1
            bs_pop_u(bs, 5); // au_cpb_removal_delay_length_minus
            bs_pop_u(bs, 5); // dpb_output_delay_length_minus1
        }
    }

    uint8_t fixed_pic_rate_general_flag[32] = { 0 };
    uint8_t fixed_pic_rate_within_cvs_flag[32] = { 0 };
    uint8_t low_delay_hrd_flag[32] = { 0 };
    uint32_t cpb_cnt_minus1[32] = { 0 };
    for (int i = 0; i <= maxNumSubLayersMinus1; i++) {
        fixed_pic_rate_general_flag[i] = bs_pop_u(bs, 1);
        if (!fixed_pic_rate_general_flag[i]) {
            fixed_pic_rate_within_cvs_flag[i] = bs_pop_u(bs, 1);
        }
        fixed_pic_rate_within_cvs_flag[i] = bs_pop_u(bs, 1);
        if (fixed_pic_rate_within_cvs_flag[i]) {
            bs_pop_ue(bs); // elemental_duration_in_tc_minus1
        }
        else {
            low_delay_hrd_flag[i] = bs_pop_u(bs, 1);
        }
        if (!low_delay_hrd_flag[i]) {
            cpb_cnt_minus1[i] = bs_pop_ue(bs);
        }
        if (nal_hrd_parameters_present_flag) {
            sub_layer_hrd_parameters(bs, cpb_cnt_minus1[i], sub_pic_cpb_params_present_flag);
        }
        if (vcl_hrd_parameters_present_flag) {
            sub_layer_hrd_parameters(bs, cpb_cnt_minus1[i], sub_pic_cpb_params_present_flag);
        }
    }
}

int h265_insert_vui(sps_bit_stream* bs, int fps)
{
    int org_idx;

    bs->idx--;
    org_idx = bs->idx;
    bs_push_u(1, 1, bs); // vui_parameters_present_flag

    bs_insert_u(0, 1, bs); // aspect_ratio_info_present_flag
    bs_insert_u(0, 1, bs); // overscan_info_present_flag
    bs_insert_u(0, 1, bs); // video_signal_type_present_flag
    bs_insert_u(0, 1, bs); // chroma_loc_info_present_flag

    bs_insert_u(0, 1, bs);  // neutral_chroma_indication_flag
    bs_insert_u(0, 1, bs);  // field_seq_flag
    bs_insert_u(0, 1, bs);  // frame_field_info_present_flag
    bs_insert_u(0, 1, bs);  // default_display_window_flag

    bs_insert_u(1, 1, bs); // vui_timing_info_present_flag
    bs_insert_u(1, 32, bs); // num_units_in_tick
    bs_insert_u(fps, 32, bs); // time_scale
    bs_insert_u(0, 1, bs); // vui_poc_proportional_to_timing_flag
    bs_insert_u(0, 1, bs); // vui_hrd_parameters_present_flag

    bs_insert_u(0, 1, bs); // bitstream_restriction_flag

    return (bs->idx - org_idx);
}


void h265_vui_parameters(sps_bit_stream* bs, uint8_t sps_max_sub_layers_minus1, int fps)
{
    uint8_t aspect_ratio_info_present_flag = bs_pop_u(bs, 1);
    if (aspect_ratio_info_present_flag) {
        uint8_t aspect_ratio_idc = bs_pop_u(bs, 8);
        if (aspect_ratio_idc == 255) {
            bs_pop_u(bs, 16); // sar_width
            bs_pop_u(bs, 16); // sar_height
        }
    }

    uint8_t overscan_info_present_flag = bs_pop_u(bs, 1);
    if (overscan_info_present_flag) {
        bs_pop_u(bs, 1); // overscan_appropriate_flag
    }

    uint8_t video_signal_type_present_flag = bs_pop_u(bs, 1);
    if (video_signal_type_present_flag) {
        bs_pop_u(bs, 3);       //video_format
        bs_pop_u(bs, 1);       //video_full_range_flag
        uint32_t colour_description_present_flag = bs_pop_u(bs, 1);
        if (colour_description_present_flag) {
            bs_pop_u(bs, 8);       //colour_primaries
            bs_pop_u(bs, 8);       //transfer_characteristics
            bs_pop_u(bs, 8);       //matrix_coefficients
        }
    }

    uint8_t chroma_loc_info_present_flag = bs_pop_u(bs, 1);
    if (chroma_loc_info_present_flag) {
        bs_pop_ue(bs);     //chroma_sample_loc_type_top_field
        bs_pop_ue(bs);     //chroma_sample_loc_type_bottom_field
    }

    bs_pop_u(bs, 1); // neutral_chroma_indication_flag
    bs_pop_u(bs, 1); // field_seq_flag
    bs_pop_u(bs, 1); // frame_field_info_present_flag
    uint8_t default_display_window_flag = bs_pop_u(bs, 1);
    if (default_display_window_flag) {
        bs_pop_ue(bs); // def_disp_win_left_offset
        bs_pop_ue(bs); // def_disp_win_right_offset
        bs_pop_ue(bs); // def_disp_win_top_offset
        bs_pop_ue(bs); // def_disp_win_bottom_offset
    }

    uint8_t vui_timing_info_present_flag = bs_pop_u(bs, 1);
    if (vui_timing_info_present_flag) {
        bs_push_u(1, 32, bs);
        bs->idx -= 32;
        uint32_t vui_num_units_in_tick = bs_pop_u(bs, 32);

        bs_push_u(fps, 32, bs);
        bs->idx -= 32;
        uint32_t vui_time_scale = bs_pop_u(bs, 32);

        uint8_t vui_poc_proportional_to_timing_flag = bs_pop_u(bs, 1);
        if (vui_poc_proportional_to_timing_flag) {
            bs_pop_ue(bs); // vui_num_ticks_poc_diff_one_minus1
        }

        uint8_t vui_hrd_parameters_present_flag = bs_pop_u(bs, 1);
        if (vui_hrd_parameters_present_flag) {
            hrd_parameters(bs, 1, sps_max_sub_layers_minus1);
        }
    }
    else {
        bs->idx -= 1;
        bs_push_u(1, 1, bs); // enable timing_info_present_flag

        bs_insert_u(1, 32, bs);
        bs->idx -= 32;
        uint32_t vui_num_units_in_tick = bs_pop_u(bs, 32);

        bs_insert_u(fps, 32, bs);
        bs->idx -= 32;
        uint32_t vui_time_scale = bs_pop_u(bs, 32);

        bs_insert_u(0, 1, bs); // vui_poc_proportional_to_timing_flag
        bs_insert_u(0, 1, bs); // vui_hrd_parameters_present_flag
    }

    uint8_t bitstream_restriction_flag = bs_pop_u(bs, 1);
    if (bitstream_restriction_flag)
    {
        bs_pop_u(bs, 1); // tiles_fixed_structure_flag
        bs_pop_u(bs, 1); // motion_vectors_over_pic_boundaries_flag
        bs_pop_u(bs, 1); // restricted_ref_pic_lists_flag
        bs_pop_ue(bs); // min_spatial_segmentation_idc
        bs_pop_ue(bs); // max_bytes_per_pic_denom
        bs_pop_ue(bs); // max_bits_per_mincu_denom
        bs_pop_ue(bs); // log2_max_mv_length_horizontal
        bs_pop_ue(bs); // log2_max_mv_length_vertical
    }
}

// src: sps/pps original data
// srcSize: original data length (Bytes num)
// fps: framerate to be set
// return value: modified data length (Bytes num)
int H264_Modify_SPS(uint8_t* src, int srcSize, int fps)
{
    int ret = 0;
    int i = 0;
    sps_bit_stream bs;

    uint32_t width = 0, height = 0;
    uint32_t nal_unit_type = 0, profile_idc = 0, level_idc = 0;

    uint32_t chroma_format_idc = 1;
    uint32_t seq_scaling_matrix_present_flag;
    uint32_t pic_order_cnt_type;
    uint32_t num_ref_frames_in_pic_order_cnt_cycle;
    uint32_t pic_width_in_mbs_minus1;
    uint32_t pic_height_in_map_units_minus1;
    uint32_t frame_mbs_only_flag;
    uint32_t frame_cropping_flag;
    uint32_t frame_crop_left_offset;
    uint32_t frame_crop_right_offset;
    uint32_t frame_crop_top_offset;
    uint32_t frame_crop_bottom_offset;
    int32_t crop_unit_x;
    int32_t crop_unit_y;
    uint32_t vui_parameters_present_flag;

    if (!src || srcSize <= 0) {
        printf("H264_Modify_SPS Error: input data invld.\n");
        return -1;
    }

    fps <<= 1;

    // init bs
    bs.dat = src;
    bs.len = srcSize << 3;
    bs.idx = 0;
    if (bs_fine_sps_pos(&bs, 0x67) == -1) {
        printf("H264_Modify_SPS Error: Missing SPS data.\n");
        return -1;
    }

    // remove emulation byte(s)
    bs_remove_emulation(&bs);

    bs.idx = bs.sps_start;

    // nal info
    bs_pop_u(&bs, 1); //forbidden_zero_bit
    bs_pop_u(&bs, 2); //nal_ref_idc
    nal_unit_type = bs_pop_u(&bs, 5);

    if (nal_unit_type == 0x7) { //Nal SPS Flag

        profile_idc = bs_pop_u(&bs, 8); //profile_idc

        bs_pop_u(&bs, 1); //constraint_set0_flag
        bs_pop_u(&bs, 1); //constraint_set1_flag
        bs_pop_u(&bs, 1); //constraint_set2_flag
        bs_pop_u(&bs, 1); //constraint_set3_flag
        bs_pop_u(&bs, 1); //constraint_set4_flag
        bs_pop_u(&bs, 1); //constraint_set4_flag
        bs_pop_u(&bs, 2); //reserved_zero_2bits

        level_idc = bs_pop_u(&bs, 8); //level_idc

        bs_pop_ue(&bs); //seq_parameter_set_id

        if (profile_idc == 100 || profile_idc == 110 || profile_idc == 122 ||
            profile_idc == 244 || profile_idc == 44  || profile_idc == 83  ||
            profile_idc == 86  || profile_idc == 118 || profile_idc == 128 ||
            profile_idc == 138 || profile_idc == 139 || profile_idc == 134 ||
            profile_idc == 135) {
            chroma_format_idc = bs_pop_ue(&bs);
            if (chroma_format_idc == 3) {
                bs_pop_u(&bs, 1);      //separate_colour_plane_flag
            }

            bs_pop_ue(&bs);        //bit_depth_luma_minus8
            bs_pop_ue(&bs);        //bit_depth_chroma_minus8
            bs_pop_u(&bs, 1);      //qpprime_y_zero_transform_bypass_flag
            seq_scaling_matrix_present_flag = bs_pop_u(&bs, 1);
            if (seq_scaling_matrix_present_flag) {
                //uint32_t seq_scaling_list_present_flag[8] = { 0 };
                for (i = 0; i < ((chroma_format_idc != 3) ? 8 : 12); i++) {
                    /*seq_scaling_list_present_flag[i] = */bs_pop_u(&bs, 1);
                    /*if (seq_scaling_list_present_flag[i]) {
                        if (i < 6) {    //scaling_list(ScalingList4x4[i], 16, UseDefaultScalingMatrix4x4Flag[i])
                        }
                        else {    //scaling_list(ScalingList8x8[i ? 6], 64, UseDefaultScalingMatrix8x8Flag[i ? 6] )
                        }
                    }*/
                }
            }
        }

        bs_pop_ue(&bs);        //log2_max_frame_num_minus4
        pic_order_cnt_type = bs_pop_ue(&bs);
        if (pic_order_cnt_type == 0) {
            bs_pop_ue(&bs);        //log2_max_pic_order_cnt_lsb_minus4
        }
        else if (pic_order_cnt_type == 1) {
            bs_pop_u(&bs, 1);      //delta_pic_order_always_zero_flag
            bs_pop_se(&bs);        //offset_for_non_ref_pic
            bs_pop_se(&bs);        //offset_for_top_to_bottom_field

            num_ref_frames_in_pic_order_cnt_cycle = bs_pop_ue(&bs);
            //int32_t* offset_for_ref_frame = (int32_t*)malloc((uint32_t)num_ref_frames_in_pic_order_cnt_cycle * sizeof(int32_t));
            for (i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; i++) {
                /*offset_for_ref_frame[i] = */bs_pop_se(&bs);
            }
            //free(offset_for_ref_frame);
        }

        bs_pop_ue(&bs);      //max_num_ref_frames
        bs_pop_u(&bs, 1);      //gaps_in_frame_num_value_allowed_flag

        pic_width_in_mbs_minus1 = bs_pop_ue(&bs);
        pic_height_in_map_units_minus1 = bs_pop_ue(&bs);      //47
        frame_mbs_only_flag = bs_pop_u(&bs, 1);

        width = (uint32_t)(pic_width_in_mbs_minus1 + 1) * 16;
        height = (uint32_t)(2 - frame_mbs_only_flag) * (pic_height_in_map_units_minus1 + 1) * 16;

        if (!frame_mbs_only_flag) {
            bs_pop_u(&bs, 1);      //mb_adaptive_frame_field_flag
        }

        bs_pop_u(&bs, 1);     //direct_8x8_inference_flag
        frame_cropping_flag = bs_pop_u(&bs, 1);
        if (frame_cropping_flag) {
            frame_crop_left_offset   = bs_pop_ue(&bs);
            frame_crop_right_offset  = bs_pop_ue(&bs);
            frame_crop_top_offset    = bs_pop_ue(&bs);
            frame_crop_bottom_offset = bs_pop_ue(&bs);

            //See 6.2 Source, decoded, and output picture formats
            crop_unit_x = 1;
            crop_unit_y = 2 - frame_mbs_only_flag;      //monochrome or 4:4:4
            if (chroma_format_idc == 1) {   //4:2:0
                crop_unit_x = 2;
                crop_unit_y = 2 * (2 - frame_mbs_only_flag);
            }
            else if (chroma_format_idc == 2) {    //4:2:2
                crop_unit_x = 2;
                crop_unit_y = 2 - frame_mbs_only_flag;
            }

            width -= crop_unit_x * (frame_crop_left_offset + frame_crop_right_offset);
            height -= crop_unit_y * (frame_crop_top_offset + frame_crop_bottom_offset);
        }

        vui_parameters_present_flag = bs_pop_u(&bs, 1);
        if (vui_parameters_present_flag) {
            vui_para_parse(&bs, fps);
        }
        else { // insert vui
            int insert_len = bs_insert_vui(&bs, fps);
            /*bs.idx -= insert_len;
            vui_parameters_present_flag = bs_pop_u(&bs, 1);
            if (vui_parameters_present_flag) {
                vui_para_parse(&bs, fps);
            }*/
        }

        //insert end of sps
        bs.idx = bs.sps_end;
        if(bs.sps_end % 8)
            bs_insert_u(0, 8 - (bs.sps_end % 8), &bs);

        // add emulation byte(s)
        bs_add_emulation(&bs);

        ret = bs.len >> 3;
    }

    return ret;
}

int H265_Modify_SPS(uint8_t* src, int srcSize, int fps)
{
    int ret = 0;
    int i = 0;
    sps_bit_stream bs;

    uint32_t width, height;

    if (!src || srcSize <= 0) {
        printf("H265_Modify_SPS Error: input data invld.\n");
        return -1;
    }

    // init bs
    bs.dat = src;
    bs.len = srcSize << 3;
    bs.idx = 0;
    if (bs_fine_sps_pos(&bs, 0x42) == -1) {
        printf("H265_Modify_SPS Error: Missing SPS data.\n");
        return -1;
    }

    // remove emulation byte(s)
    bs_remove_emulation(&bs);

    bs.idx = bs.sps_start;

    // nal info
    bs_pop_u(&bs, 1); // forbidden_zero_bit
    uint32_t nal_unit_type = bs_pop_u(&bs, 6);
    bs_pop_u(&bs, 6); // nuh_layer_id
    bs_pop_u(&bs, 3); // nuh_temporal_id_plus1

    if (nal_unit_type == 33) { // Nal SPS Flag
        bs_pop_u(&bs, 4); // sps_video_parameter_set_id
        uint8_t sps_max_sub_layers_minus1 = bs_pop_u(&bs, 3); // sps_max_sub_layers_minus1
        bs_pop_u(&bs, 1); // sps_temporal_id_nesting_flag
        profile_tier_level(&bs, sps_max_sub_layers_minus1);

        bs_pop_ue(&bs); // sps_seq_parameter_set_id
        uint32_t chroma_format_idc = bs_pop_ue(&bs); // chroma_format_idc
        if (chroma_format_idc == 3) {
            bs_pop_u(&bs, 1);      //separate_colour_plane_flag
        }

        width = bs_pop_ue(&bs); // pic_width_in_luma_samples
        height = bs_pop_ue(&bs); // pic_height_in_luma_samples

        uint8_t conformance_window_flag = bs_pop_u(&bs, 1);
        if (conformance_window_flag) {
            bs_pop_ue(&bs); // conf_win_left_offset
            bs_pop_ue(&bs); // conf_win_right_offset
            bs_pop_ue(&bs); // conf_win_top_offset
            bs_pop_ue(&bs); // conf_win_bottom_offset
        }

        bs_pop_ue(&bs);        //bit_depth_luma_minus8
        bs_pop_ue(&bs);        //bit_depth_chroma_minus8
        uint32_t log2_max_pic_order_cnt_lsb_minus4 = bs_pop_ue(&bs); // log2_max_pic_order_cnt_lsb_minus4

        uint8_t sps_sub_layer_ordering_info_present_flag = bs_pop_u(&bs, 1); // sps_sub_layer_ordering_info_present_flag
        for (i = (sps_sub_layer_ordering_info_present_flag ? 0 : sps_max_sub_layers_minus1); i <= sps_max_sub_layers_minus1; i++) {
            bs_pop_ue(&bs); // sps_max_dec_pic_buffering_minus1
            bs_pop_ue(&bs); // sps_max_num_reorder_pics
            bs_pop_ue(&bs); // sps_max_latency_increase_plus1
        }
        bs_pop_ue(&bs); // log2_min_luma_coding_block_size_minus3
        bs_pop_ue(&bs); // log2_diff_max_min_luma_coding_block_size
        bs_pop_ue(&bs); // log2_min_transform_block_size_minus2
        bs_pop_ue(&bs); // log2_diff_max_min_transform_block_size
        bs_pop_ue(&bs); // max_transform_hierarchy_depth_inter
        bs_pop_ue(&bs); // max_transform_hierarchy_depth_intra

        uint8_t scaling_list_enabled_flag = bs_pop_u(&bs, 1);
        if (scaling_list_enabled_flag) {

            uint8_t sps_scaling_list_data_present_flag = bs_pop_u(&bs, 1);
            if (sps_scaling_list_data_present_flag) {
                scaling_list_data(&bs);
            }
        }

        bs_pop_u(&bs, 1); // amp_enabled_flag
        bs_pop_u(&bs, 1); // sample_adaptive_offset_enabled_flag

        uint8_t pcm_enabled_flag = bs_pop_u(&bs, 1);
        if (pcm_enabled_flag) {
            bs_pop_u(&bs, 4); // pcm_sample_bit_depth_luma_minus1
            bs_pop_u(&bs, 4); // pcm_sample_bit_depth_chroma_minus1
            bs_pop_ue(&bs); // log2_min_pcm_luma_coding_block_size_minus3
            bs_pop_ue(&bs); // log2_diff_max_min_pcm_luma_coding_block_size
            bs_pop_u(&bs, 1); // pcm_loop_filter_disable_flag
        }

        uint32_t num_short_term_ref_pic_sets = bs_pop_ue(&bs);
        for (i = 0; i < num_short_term_ref_pic_sets; i++) {
            short_term_ref_pic_set(&bs, i, num_short_term_ref_pic_sets);
        }

        uint8_t long_term_ref_pics_present_flag = bs_pop_u(&bs, 1);
        if (long_term_ref_pics_present_flag) {
            uint32_t num_long_term_ref_pics_sps = bs_pop_ue(&bs);
            for (i = 0; i < num_long_term_ref_pics_sps; i++) {
                bs_pop_u(&bs, log2_max_pic_order_cnt_lsb_minus4); // lt_ref_pic_poc_lsb_sps
                bs_pop_u(&bs, 1); // used_by_curr_pic_lt_sps_flag
            }
        }
        bs_pop_u(&bs, 1); // sps_temporal_mvp_enabled_flag
        bs_pop_u(&bs, 1); // strong_intra_smoothing_enabled_flag

        uint8_t vui_parameters_present_flag = bs_pop_u(&bs, 1);
        if (vui_parameters_present_flag) {
            h265_vui_parameters(&bs, sps_max_sub_layers_minus1, fps);
        }
        else { // insert vui
            int insert_len = h265_insert_vui(&bs, fps);
        }

        bs_pop_u(&bs, 1); // sps_extension_flag

        //insert end of sps
        bs.idx = bs.sps_end;
        if (bs.sps_end % 8)
            bs_insert_u(0, 8 - (bs.sps_end % 8), &bs);

        // add emulation byte(s)
        bs_add_emulation(&bs);

        ret = bs.len >> 3;
    }

    return ret;
}

#if(0)
int main_test()
{
    int len_modified;

#if(0)
    uint8_t h264_extradata720p30[39] = {
    0x00, 0x00, 0x00, 0x01, 0x67, 0x64, 0x00, 0x1f,
    0xac, 0xd9, 0x40, 0x50, 0x05, 0xba, 0x10, 0x00,
    0x00, 0x03, 0x00, 0x10, 0x00, 0x00, 0x03, 0x03,
    0xc0, 0xf1, 0x83, 0x19, 0x60, 0x00, 0x00, 0x00,
    0x01, 0x68, 0xeb, 0xe3, 0xcb, 0x22, 0xc0,
    };

    uint8_t h264_extradata720p30_aw[28] = {
    0x00, 0x00, 0x00, 0x01, 0x67, 0x4d, 0x00, 0x33,
    0x96, 0x54, 0x02, 0x80, 0x2d, 0x93, 0x70, 0x50,
    0x60, 0x50, 0x20, 0x00, 0x00, 0x00, 0x01, 0x68,
    0xee, 0x3c, 0x80, 0x00,
    };

    uint8_t h265_extradata1080p30[] = {
    0x00, 0x00, 0x00, 0x01, 0x42, 0x01, 0x01, 0x01,
    0x60, 0x00, 0x00, 0x03, 0x00, 0x90, 0x00, 0x00,
    0x03, 0x00, 0x00, 0x03, 0x00, 0x78, 0xa0, 0x03,
    0xc0, 0x80, 0x10, 0xe5, 0x96, 0x4a, 0x92, 0x49,
    0x02, 0x60, 0x10, 0x00, 0x00, 0x3e, 0x80, 0x00,
    0x07, 0x53, 0x00, 0x80
    };

    uint8_t h265_extradata720p30_aw[104] = {
    0x00, 0x00, 0x00, 0x01, 0x40, 0x01, 0x0c, 0x01,
    0xff, 0xff, 0x01, 0x60, 0x00, 0x00, 0x03, 0x00,
    0x00, 0x03, 0x00, 0x00, 0x03, 0x00, 0x00, 0x03,
    0x00, 0xba, 0xac, 0x09, 0x00, 0x00, 0x00, 0x01,
    0x42, 0x01, 0x01, 0x01, 0x60, 0x00, 0x00, 0x03,
    0x00, 0x00, 0x03, 0x00, 0x00, 0x03, 0x00, 0x00,
    0x03, 0x00, 0xba, 0xa0, 0x02, 0x80, 0x80, 0x2d,
    0x1f, 0xe5, 0xae, 0xe4, 0x48, 0x81, 0x02, 0xfc,
    0xf3, 0xcf, 0x3c, 0xf3, 0xcf, 0x3c, 0xf3, 0xcf,
    0x3c, 0xf3, 0xcf, 0x3c, 0xf3, 0xcf, 0x3c, 0xf3,
    0xcf, 0x3c, 0xf3, 0xcf, 0x3c, 0xb5, 0x37, 0x05,
    0x06, 0x05, 0x00, 0x80, 0x00, 0x00, 0x00, 0x01,
    0x44, 0x01, 0xc0, 0xf6, 0x88, 0x40, 0x33, 0x24
    };

    uint8_t h265_extradata1080p30_aw[104] = {
    0x00, 0x00, 0x00, 0x01, 0x40, 0x01, 0x0c, 0x01,
    0xff, 0xff, 0x01, 0x60, 0x00, 0x00, 0x03, 0x00,
    0x00, 0x03, 0x00, 0x00, 0x03, 0x00, 0x00, 0x03,
    0x00, 0xba, 0xac, 0x09, 0x00, 0x00, 0x00, 0x01,
    0x42, 0x01, 0x01, 0x01, 0x60, 0x00, 0x00, 0x03,
    0x00, 0x00, 0x03, 0x00, 0x00, 0x03, 0x00, 0x00,
    0x03, 0x00, 0xba, 0xa0, 0x03, 0xc0, 0x80, 0x10,
    0xe7, 0xf9, 0x6b, 0xb9, 0x12, 0x20, 0x40, 0xbf,
    0xf3, 0xcf, 0x3c, 0xf3, 0xcf, 0x3c, 0xf3, 0xcf,
    0x3c, 0x3c, 0xf3, 0xcf, 0x3c, 0xf3, 0xcf, 0x3c,
    0xf3, 0xcf, 0x3c, 0xf3, 0xcf, 0x2d, 0x4d, 0xc1,
    0x41, 0x81, 0x40, 0x20, 0x00, 0x00, 0x00, 0x01,
    0x44, 0x01, 0xc0, 0xf6, 0x88, 0x40, 0x33, 0x24,
    };

    uint8_t h265_spsdata_ts4[86] = {
        0x00, 0x00, 0x00, 0x01, 0x42, 0x01, 0x01, 0x01,
        0x60, 0x00, 0x00, 0x03, 0x00, 0x00, 0x03, 0x00,
        0x00, 0x03, 0x00, 0x00, 0x03, 0x00, 0xba, 0xa0,
        0x02, 0x80, 0x80, 0x2d, 0x1f, 0xe5, 0xae, 0xe4,
        0x48, 0x81, 0xf2, 0xfc, 0xf3, 0xcf, 0x3c, 0xf3,
        0xcf, 0x3c, 0xf3, 0xcf, 0x3c, 0xf3, 0xcf, 0x3c,
        0xf3, 0xcf, 0x3c, 0xf3, 0xcf, 0x3c, 0xf3, 0xcf,
        0x3c, 0xf3, 0xcf, 0x3c, 0xf3, 0xcf, 0x3c, 0xf3,
        0xcf, 0x3c, 0xf3, 0xcf, 0x3c, 0xf3, 0xcf, 0x3c,
        0xf3, 0xcf, 0x3c, 0xf3, 0xcf, 0x3c, 0xf3, 0xcb,
        0x53, 0x70, 0x50, 0x60, 0x50, 0x08
    };

    uint8_t bs[256] = { 0 };
    memcpy(bs, h265_extradata1080p30, 44);

    //len_modified = H264_Modify_SPS(bs, 28, 90);
    len_modified = H265_Modify_SPS(bs, 44, 90);
#else
    uint8_t bs[126*2] = {
        0x00, 0x00, 0x00, 0x01, 0x40, 0x01, 0x0c, 0x01, 0xff, 0xff, 0x01, 0x60, 0x00, 0x00, 0x03, 0x00,
        0x00, 0x03, 0x00, 0x00, 0x03, 0x00, 0x00, 0x03, 0x00, 0xba, 0xac, 0x09, 0x00, 0x00, 0x00, 0x01,
        0x42, 0x01, 0x01, 0x01, 0x60, 0x00, 0x00, 0x03, 0x00, 0x00, 0x03, 0x00, 0x00, 0x03, 0x00, 0x00,
        0x03, 0x00, 0xba, 0xa0, 0x02, 0x80, 0x80, 0x2d, 0x1f, 0xe5, 0xae, 0xe4, 0x48, 0x81, 0xf2, 0xfc,
        0xf3, 0xcf, 0x3c, 0xf3, 0xcf, 0x3c, 0xf3, 0xcf, 0x3c, 0xf3, 0xcf, 0x3c, 0xf3, 0xcf, 0x3c, 0xf3,
        0xcf, 0x3c, 0xf3, 0xcf, 0x3c, 0xf3, 0xcf, 0x3c, 0xf3, 0xcf, 0x3c, 0xf3, 0xcf, 0x3c, 0xf3, 0xcf,
        0x3c, 0xf3, 0xcf, 0x3c, 0xf3, 0xcf, 0x3c, 0xf3, 0xcf, 0x3c, 0xf3, 0xcb, 0x53, 0x70, 0x50, 0x60,
        0x50, 0x08, 0x00, 0x00, 0x00, 0x01, 0x44, 0x01, 0xc0, 0xf6, 0x88, 0x40, 0x33, 0x24
    };

    //len_modified = H264_Modify_SPS(bs, 28, 90);
    len_modified = H265_Modify_SPS(bs, 126, 60);
    printf("len_modified=%d\n", len_modified);
#endif
}

int main_test2(uint8_t* buff, int len)
{
    int len_modified;
    uint8_t bs[126*2] = {
        0x00, 0x00, 0x00, 0x01, 0x40, 0x01, 0x0c, 0x01, 0xff, 0xff, 0x01, 0x60, 0x00, 0x00, 0x03, 0x00,
        0x00, 0x03, 0x00, 0x00, 0x03, 0x00, 0x00, 0x03, 0x00, 0xba, 0xac, 0x09, 0x00, 0x00, 0x00, 0x01,
        0x42, 0x01, 0x01, 0x01, 0x60, 0x00, 0x00, 0x03, 0x00, 0x00, 0x03, 0x00, 0x00, 0x03, 0x00, 0x00,
        0x03, 0x00, 0xba, 0xa0, 0x02, 0x80, 0x80, 0x2d, 0x1f, 0xe5, 0xae, 0xe4, 0x48, 0x81, 0xf2, 0xfc,
        0xf3, 0xcf, 0x3c, 0xf3, 0xcf, 0x3c, 0xf3, 0xcf, 0x3c, 0xf3, 0xcf, 0x3c, 0xf3, 0xcf, 0x3c, 0xf3,
        0xcf, 0x3c, 0xf3, 0xcf, 0x3c, 0xf3, 0xcf, 0x3c, 0xf3, 0xcf, 0x3c, 0xf3, 0xcf, 0x3c, 0xf3, 0xcf,
        0x3c, 0xf3, 0xcf, 0x3c, 0xf3, 0xcf, 0x3c, 0xf3, 0xcf, 0x3c, 0xf3, 0xcb, 0x53, 0x70, 0x50, 0x60,
        0x50, 0x08, 0x00, 0x00, 0x00, 0x01, 0x44, 0x01, 0xc0, 0xf6, 0x88, 0x40, 0x33, 0x24
    };

    memcpy(buff, bs, sizeof(bs));

    //len_modified = H264_Modify_SPS(bs, 28, 90);
    len_modified = H265_Modify_SPS(buff, 126, 60);
    printf("len_modified=%d, %p\n", len_modified, bs);
}

#endif
