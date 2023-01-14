#ifndef _ISE_LIB_STI_H_
#define _ISE_LIB_STI_H_

#include "ISE_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ISE_DEBUG_INFO_PRINT

#define MAX_SUB_IMAGE       2       			 // 子孔径图像数量最大值

// 单一配置参数
typedef enum _ISE_KEY_SINGLE_PARAM_
{
	ISE_SINGLE_KEY_W_FOV_REV    =    0x0001     // w_fov修正值，单位为度
} ISE_SINGLE_KEY;

// 配置参数类型
typedef enum _ISE_SET_CFG_TYPE_
{
    ISE_SETCFG_BGFG_INTVL_PARAM       =    0x0001,        // 每隔多少个SLEEP_MS进行一次model
    ISE_SETCFG_GETBGD_INTVL_PARAM     =    0x0002,        // 每隔多少帧获取背景图片
    ISE_SETCFG_BGFG_SLEEP_MS_PARAM    =    0x0003         // SC16队列非空时，sleep多少MS
} ISE_SET_CFG_TYPE;

/*****************************************************************************
* 内存结构体
* size                 大小
* alignment            对齐方式
* space                位置
* attrs                属性
* base                 起始地址
******************************************************************************/
typedef struct _ISE_MEM_TAB_
{
	unsigned int             size; 
	ISE_MEM_ALIGNMENT        alignment;
	ISE_MEM_SPACE            space;
	ISE_MEM_ATTRS            attrs;
	void*                    base;
} ISE_MEM_TAB;

/*****************************************************************************
* 基本参数配置参数
* ncam                       相机数量
* in_h                       原图分辨率h，2的倍数
* in_w                       原图分辨率w，2的倍数
* pano_h                     全景图分辨率h，4的倍数（最小64，最大2048）
* pano_w                     全景图分辨率w，36的倍数（最小320，最大4096）
* ov                         重叠区总宽度，16的倍数，且小于1/10*pano_h
* yuv_type                   数据格式，只支持420/422
* p0                         cam0 P值，0-180
* p1                         cam1 P值，0-180
* stre_en                    视场拉伸开关
* stre_coeff                 视场拉伸系数，>1.0
* offset_r2l                 右图相对左图高度方向偏移像素数
*                                ！需要根据pano图分辨率不同设置为不同值
* pano_fov                   输出图像视场角，为大约值
* t_angle                    俯仰角，T值，单位为度
* hfov                       当前分辨率下水平视场角，单位为度
* wfov                       当前分辨率下垂直视场角，单位为度（只配置wfov即可）
* wfov_rev                   水平视场角修正值，单位为度
* calib_matr                 未校正相机矩阵
* calib_matr_cv              畸变校正后相机矩阵
* distort                    畸变系数
* in_luma_pitch              原图亮度pitch，32的倍数，最大4096
* in_chroma_pitch            原图色度pitch，32的倍数，最大4096
* pano_luma_pitch            全景图亮度pitch，16/32的倍数，最大4096
* pano_chroma_pitch          全景图色度pitch，16/32的倍数，最大4096
* reserved                   保留字段，字节对齐
*******************************************************************************/
typedef struct _ISE_CFG_PARA_STI_{
	int                      ncam;
    int                      in_h;
    int                      in_w;
    int                      pano_h;
    int                      pano_w;
    int                      ov;
    int                      yuv_type;
    float                    p0;
    float                    p1;
    int                      offset_r2l;
    float                    pano_fov;
    float                    t_angle;
    int                      stre_en;
    float                    stre_coeff;
    float                    hfov;
    float                    wfov;
    float                    wfov_rev;
    double                   calib_matr[3][3];
    double                   calib_matr_cv[3][3];
    double                   distort[8];
    int                      in_luma_pitch;
    int                      in_chroma_pitch;
    int                      pano_luma_pitch;
    int                      pano_chroma_pitch;
    char                     reserved[32];
}ISE_CFG_PARA_STI;

/******************************************************************************
* 主处理输入参数
* in_luma             原始图像指针数组，亮度分量，nv12或者nv16格式
* in_chroma           原始图像指针数组，色度分量，nv12或者nv16格式
*******************************************************************************/
typedef struct _ISE_PROCIN_PARA_STI_{
	unsigned int        	in_luma_phy_Addr[2];
	void					*in_luma_mmu_Addr[2];
	unsigned int        	in_chroma_phy_Addr[2];
	void					*in_chroma_mmu_Addr[2];
	char			 	  	reserved[32];
}ISE_PROCIN_PARA_STI;

/******************************************************************************
* 主处理配置参数
* bgfgmode_en                由用户决定开启/关闭背景建模功能
* main_flip_mirr             全景图像是否翻转/镜像，0:non,1:flip,2:mirror
* scalar_h                   scalar图像高度，4的倍数，最小40
* scalar_w                   scalar图像宽度，8的倍数，最小40
* scalar_luma_pitch          scalar图像亮度分量跨度,16/32的倍数，最大4096
* scalar_flip_mirr           scalar图像是否翻转/镜像，0:non,1:flip,2:mirror
* scalar_en                  scalar图像使能
*******************************************************************************/
typedef struct _ISE_PROCCFG_PARA_STI_{
	int                      bgfgmode_en;
	int						 pano_flip;
	int						 pano_mirr;
    int                      scalar_h[MAX_SCALAR_CHNL];
    int                      scalar_w[MAX_SCALAR_CHNL];
    int                      scalar_luma_pitch[MAX_SCALAR_CHNL];
	int                      scalar_chroma_pitch[MAX_SCALAR_CHNL];
	int						 scalar_flip[MAX_SCALAR_CHNL];
	int						 scalar_mirr[MAX_SCALAR_CHNL];
    int                      scalar_en[MAX_SCALAR_CHNL];
    char                     reserved[32];
}ISE_PROCCFG_PARA_STI;

/******************************************************************************
* 主处理输出参数
* pano_luma            全景图像，亮度分量，nv12或者nv16格式
* pano_chroma          全景图像，色度分量，nv12或者nv16格式
*******************************************************************************/
typedef struct _ISE_PROCOUT_PARA_STI_{
	unsigned int			pano_luma_phy_Addr;
	void					*pano_luma_mmu_Addr;
	unsigned int			pano_chroma_phy_Addr;
	void					*pano_chroma_mmu_Addr;
	unsigned int			scalar_luma_phy_Addr[MAX_SCALAR_CHNL];
	void					*scalar_luma_mmu_Addr[MAX_SCALAR_CHNL];
	unsigned int			scalar_chroma_phy_Addr[MAX_SCALAR_CHNL];
	void					*scalar_chroma_mmu_Addr[MAX_SCALAR_CHNL];
	char					reserved[32];
}ISE_PROCOUT_PARA_STI;

/*********************************************************************************
* 功  能： 创建句柄
* 参  数：
*          ise_cfg           [I] 基础配置参数
*          handle            [O] 返回模块句柄
* 返回值： 状态码
**********************************************************************************/
int ISE_Create_Sti(
	ISE_CFG_PARA_STI		*ise_cfg,
	ISE_HANDLE_STI			*handle);

/**********************************************************************************
* 功  能： 设置配置信息
* 参  数：
*          config_type       [I] 配置信息类型，用于区分配置信息
*          in_buf            [I] 指向外部输入配置信息的指针
*          buf_size          [I] 配置信息占用内存大小
*          handle            [I] 句柄
* 返回值： 状态码
* 注  释： 预留高级参数配置接口
***********************************************************************************/
int ISE_SetConfig_Sti(
	int 					config_type,
	void					*in_buf,
	int 					buf_size,
	ISE_HANDLE_STI			*handle);

/**********************************************************************************
* 功	能：设置ISE模块频率
* 参	数：
*		freMHz			[I]配置频率
*		handle			[I]句柄
* 返回值：无
***********************************************************************************/
void ISE_SetFrq_Sti(
	int						 freMHz, 
	ISE_HANDLE_STI 			*handle);

/************************************************************************************
* 功  能：主处理函数
* 参  数：
*         ise_procin         [I] 输入参数
*         ise_procout        [O] 输出参数
*         ise_proccfg        [I] 主处理配置参数
*         handle             [I] 句柄
* 返回值：状态码
*************************************************************************************/
int ISE_Proc_Sti(
	ISE_PROCIN_PARA_STI		*ise_procin,
	ISE_PROCOUT_PARA_STI	*ise_procout,
	ISE_PROCCFG_PARA_STI	*ise_proccfg,
	ISE_HANDLE_STI			*handle);

/************************************************************************************
* 功  能：销毁ISE
* 参  数：
*         handle             [I] 句柄
* 返回值：状态码
*************************************************************************************/
int ISE_Destroy_Sti(ISE_HANDLE_STI		 *handle);

#ifdef __cplusplus
}
#endif

#endif
