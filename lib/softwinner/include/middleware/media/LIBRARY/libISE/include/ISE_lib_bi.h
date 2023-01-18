#ifndef _ISE_LIB_BI_H_
#define _ISE_LIB_BI_H_

#include "ISE_common.h"

// mem tab
#define ISE_MTAB_NUM_BI         1 

/*****************************************************************************
* 内存结构体
* size                 大小
* alignment            对齐方式
* space                位置
* attrs                属性
* base                 起始地址
******************************************************************************/
typedef struct _ISE_MEM_TAB_BI_
{
	unsigned int             size; 
	ISE_MEM_ALIGNMENT        alignment;
	ISE_MEM_SPACE            space;
	ISE_MEM_ATTRS            attrs;
	void*                    base;
} ISE_MEM_TAB_BI;


/*****************************************************************************
* 基本参数配置参数
* in_h                  原图分辨率h，4的倍数
* in_w                  原图分辨率w，4的倍数
* pano_h            	全景图分辨率h，4的倍数（最小320，最大4032）
* pano_w                全景图分辨率w，8的倍数（最小320，最大8064）
* in_luma_pitch         原图亮度pitch
* in_chroma_pitch       原图色度pitch
* pano_luma_pitch       全景图亮度pitch
* pano_chroma_pitch     全景图色度pitch
* change_focus          视差校正系数
* feather               重叠区域半径
* trans_width           过渡区大小
* findpath_width        最佳路径查找区域大小
* pyr_level             金字塔级数设定
* findpath_enable       最佳路径使能控制
* inv_scale             最佳路径查找缩放比例
* reserved          	保留字段，字节对齐
*******************************************************************************/
typedef struct _ISE_CFG_PARA_BI_{
	int                      in_h;
	int                      in_w;
	int						 in_luma_pitch;
	int						 in_chroma_pitch;
	int                      in_yuv_type;
	int						 out_en[1+MAX_SCALAR_CHNL];
	int                      out_h[1+MAX_SCALAR_CHNL];
	int                      out_w[1+MAX_SCALAR_CHNL];
	int						 out_flip[1+MAX_SCALAR_CHNL];
	int					     out_mirror[1+MAX_SCALAR_CHNL];
	int						 out_luma_pitch[1+MAX_SCALAR_CHNL];
	int						 out_chroma_pitch[1+MAX_SCALAR_CHNL];
	int                      out_yuv_type;
	float					 p0; 
	float	      		     cx0;
	float 			         cy0;
  	float                    p1;
  	float 					 cx1;
  	float 					 cy1;
	float					 calib_matr[3][3];
	double					 calib_matr_cv[3][3];
	double					 distort[8];
	float				   	 change_focus;
	int					   	 feather;
	int					   	 trans_width;
	int					   	 findpath_width;
	int					   	 pyr_level;
	int					   	 findpath_enable;
	float					 inv_scale;
	char                     reserved[32];
}ISE_CFG_PARA_BI;

/******************************************************************************
* 主处理输入参数
* in_luma                     原始图像指针数组，亮度分量，nv12或者nv16格式
* in_chroma                   原始图像指针数组，色度分量，nv12或者nv16格式
*******************************************************************************/
typedef struct _ISE_PROCIN_PARA_BI_{
	unsigned int        	in_luma_phy_Addr[2];
	void*           	  	in_luma_mmu_Addr[2];
	unsigned int        	in_chroma_phy_Addr[2];
	void*               	in_chroma_mmu_Addr[2];
	char			 	  	reserved[32];
}ISE_PROCIN_PARA_BI;

/******************************************************************************
* 主处理输出参数
* pano_luma            全景图像，亮度分量，nv12或者nv16格式
* pano_chroma          全景图像，色度分量，nv12或者nv16格式
*******************************************************************************/
typedef struct _ISE_PROCOUT_PARA_BI_{
	unsigned int        	out_luma_phy_Addr[1+MAX_SCALAR_CHNL];
	void*           	   	out_luma_mmu_Addr[1+MAX_SCALAR_CHNL];
	unsigned int        	out_chroma_u_phy_Addr[1+MAX_SCALAR_CHNL];
	void*           		out_chroma_u_mmu_Addr[1+MAX_SCALAR_CHNL];
	unsigned int        	out_chroma_v_phy_Addr[1+MAX_SCALAR_CHNL];
	void*           		out_chroma_v_mmu_Addr[1+MAX_SCALAR_CHNL];
	char			 		reserved[32];
}ISE_PROCOUT_PARA_BI;

// 硬件处理接口函数
int ISE_Create_Bi(ISE_CFG_PARA_BI *ise_cfg, ISE_HANDLE_BI *handle);

void ISE_SetFrq_Bi(int freMHz, ISE_HANDLE_BI *handle);

int ISE_SetAttr_Bi(ISE_HANDLE_BI *handle);

int ISE_Proc_Bi(
	ISE_HANDLE_BI 		*handle,
	ISE_PROCIN_PARA_BI 	*ise_procin, 
	ISE_PROCOUT_PARA_BI *ise_procout);

int ISE_Destroy_Bi(ISE_HANDLE_BI *handle);

// 软件处理接口函数
int ISE_Create_BI_Soft(
    ISE_CFG_PARA_BI     *ise_cfg,
    ISE_HANDLE_BI_SOFT  *handle);

int ISE_SetConfig_BI_Soft(ISE_HANDLE_BI_SOFT *handle);

int ISE_Proc_BI_Soft(
    ISE_HANDLE_BI_SOFT  *handle,
    ISE_PROCIN_PARA_BI  *ise_procin,
    ISE_PROCOUT_PARA_BI *ise_procout);

int ISE_Destroy_BI_Soft(ISE_HANDLE_BI_SOFT *handle);

#endif


