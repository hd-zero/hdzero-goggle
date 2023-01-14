#ifndef AWMD_Interface_h
#define AWMD_Interface_h

#ifdef __cplusplus
extern "C"
{
#endif
	typedef struct AWMDVar
	{
		int state;
		void* bk;
		void* scene;
		void* analyze;
		void* para;
	}AWMDVar;

	/*------------------------------*/
	//vpRun return value
	//1:motion
	//2:roi shelter
	//4:camera shelter
	//3:motion + (roi shelter),other and so on.
	/*------------------------------*/
	typedef struct AWMD
	{
		//var
		AWMDVar variable;
		//interface function
		int (*vpInit)(AWMDVar* p_awmd_var,int sensitivity_level);
		int (*vpSetROIMask)(AWMDVar* p_awmd_var,unsigned char* p_mask,int width,int height);
		const unsigned char* (*vpGetMask)(AWMDVar* p_awmd_var);
		int (*vpSetSensitivityLevel)(AWMDVar* p_awmd_var,int  sensitivity_level);
		int (*vpSetShelterPara)(AWMDVar* p_awmd_var,int b_shelter_detect,int b_roi_shelter_detect);
		int (*vpRun)(unsigned char* frame_buf,AWMDVar* p_awmd_var);
		int (*vpReset)(AWMDVar* p_awmd_var);
	}AWMD;

	AWMD* allocAWMD(int width,int height,int sample_num,int time_sample_num, int sub_sampe_factor);
	int freeAWMD(AWMD *ptr);
#ifdef __cplusplus
};
#endif



#endif

