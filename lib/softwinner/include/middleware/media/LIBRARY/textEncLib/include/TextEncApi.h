#ifdef __cplusplus
extern "C" {
#endif

#ifndef __TEXT_ENC_API_H__
#define __TEXT_ENC_API_H__


#define TEXTENC_PACKET_SIZE   (0x10+sizeof(RMCINFO)) //(0xc0 + sizeof(GSENSOR_t)*60 + 2)
//#define TEXTBUF_FIFO_LEVEL    10      // fifo length for input text buffer in 10s
#define TEXTENC_PACKET_CNT    10      // enced OutFrame buf count for 10s

/* fifo length for input text data */
#define FIFO_GPS_LEVEL                10        // cache cnt in 10s
#define FIFO_GSENSOR_LEVEL            100       // bigger than gsensor_rate(video_fps=25/30/60)
#define FIFO_ADAS_LEVEL               100       // bigger than adas_rate(video_fps=25/30/60)

/* input type which enter into TextSrc_Comp and text encoder */
typedef enum __text_enc_input_type {
    TEXTENC_INPUT_TYPE_GPS          =   0,
    TEXTENC_INPUT_TYPE_GSENSOR      =   1,
    TEXTENC_INPUT_TYPE_ADAS         =   2,
    TEXTENC_INPUT_TYPE_DRIVER_ID    =   3,
} __text_enc_input_type;

/* decide which type can be packed */
#define TEXTENC_PACK_TYPE_GPS           (1<<0)
#define TEXTENC_PACK_TYPE_GSENSOR       (1<<1)
#define TEXTENC_PACK_TYPE_ADAS          (1<<2)
#define TEXTENC_PACK_TYPE_DRIVER_ID     (1<<3)


typedef struct gps_info_s
{ 
    char IQVer[16];
    char IQBuildDate[16];
    unsigned int Hour;       //gps信息里输出的小时 
    unsigned int Minute;     //gps信息里输出的分 
    unsigned int Second;     //gps信息里输出的秒 
    unsigned int Year;       //gps信息里输出的年 
    unsigned int Month;      //gps信息里输出的月 
    unsigned int Day;        //gps信息里输出的天 
    
    char Status;         //gps信息里输出的状态 
    char NSInd;          //gps信息里输出的NSInd 
    char EWInd;          //gps信息里输出的EWInd 
    char reserved;       //gps信息里输出的reserved

    double Latitude;      //gps信息里输出的纬度,注意这是double型 
    double Longitude;     //gps信息里输出的经度，注意这是double型 
    float Speed;         //gps信息里输出的速度 
    float Angle;         //gps信息里输出的方向 
    
    char ID[20];         //这个ID是固化在每个相机设备的主控里的，一旦烧录就不永远不会变，每台设备的ID是不一样的。 

    unsigned int GsensorX;   //gps信息里输出的重力加速X 
    unsigned int GsensorY;   //gps信息里输出的重力加速Y 
    unsigned int GsensorZ;   //gps信息里输出的重力加速Z 

    unsigned int MHour;     //主控的时间小时 ，这个主要是用于显示在app上的相机当前时间，与gps时间不一样 
    unsigned int MMinute;   //主控的时间分，这个主要是用于显示在app上的相机当前时间，与gps时间不一样 
    unsigned int MSecond;   //主控的时间秒，这个主要是用于显示在app上的相机当前时间，与gps时间不一样 
    unsigned int MYear;     //主控的时间年，这个主要是用于显示在app上的相机当前时间，与gps时间不一样 
    unsigned int MMonth;    //主控的时间月，这个主要是用于显示在app上的相机当前时间，与gps时间不一样 
    unsigned int MDay;      //主控的时间天，这个主要是用于显示在app上的相机当前时间，与gps时间不一样 
}RMCINFO; 

typedef struct GPS_t {
    char lat_dir;       // latitude
    char lat_val[9];
    char long_dir;      // longitude
    char long_val[10];
    char speed[5];
} GPS_t;

typedef struct GSENSOR_t {
    float xa;
    float ya;
    float za;
    float ra;   // for acc_sensor, it's redundant, buf for other_sensor, this may useful
} GSENSOR_t;

typedef struct ADAS_t {
    int x;
    int y;
} ADAS_t;

typedef struct DRIVER_ID_t {
    char driver_id[16];                 // DRIVER_ID_LEN + 2
} DRIVER_ID_t;


typedef enum TEXT_ENCODER_TYPE
{
    TEXT_ENCODER_GGAD,
} TEXT_ENCODER_TYPE;

// ref to TEXTINFO_t
typedef struct __text_enc_info_t {
    int enc_type_enable_flag;           // bit0-gps; bit1-gsensor; bit2-adas; bit3-driver_id
    int text_enc_type;                  // CODEC_ID_GGAD
    int video_frame_rate;
    int gsensor_rate;
    int adas_rate;                      // n: one adas data for n video-frames
} __text_enc_info_t;

typedef enum __text_enc_result_t {
    ERR_TEXT_ENC_UNKNOWN          = -1,
    ERR_TEXT_ENC_NONE             = 0,    // encode successed, no error
    ERR_TEXT_ENC_TIME_NOT_REACH   = 1,    // 1s has not come

// input
    ERR_TEXT_ENC_INPUT_UNDERFLOW  = 2,    // input data not prepared, shoud 0.1s, then encode by force

// output
    ERR_TEXT_ENC_OUTFRM_UNDERFLOW = 3,    // rid reach wid which means out_buf_packet_cnt == TEXTENC_PACKET_CNT
} __text_enc_result_t;


typedef struct EncPacket_tag
{
    char out_data[TEXTENC_PACKET_SIZE];
    int buf_id;
    long long pts;
} EncPacket_t;

typedef struct OutFrameManager_tag
{
    int total_packet_cnt;            // enc packet count till now, keep increase for one file.

    EncPacket_t out_pkt[TEXTENC_PACKET_CNT];
    int         write_id;
    int         read_id;
    int         buf_used;           // release cnt by ReleaseTextEncBuf, when it's full, InBuf cache text, NOT encode!
    int         prefetch_cnt;       // request cnt by RequestTextEncBuf
} OutFrameManager_t;


typedef struct TextPacket_tag       // TextSrc_Comp's output to TextEnc_Comp's input port with this data struct
{
    RMCINFO       gps_data; 
//    GPS_t       gps_data;
    GSENSOR_t   gsensor_data[FIFO_GSENSOR_LEVEL];
    ADAS_t      adas_data[FIFO_ADAS_LEVEL];
    DRIVER_ID_t driver_id_data;
    long long   pts;
} TextPacket_t;

typedef struct InFrameManager_tag
{
    TextPacket_t  in_pkt[TEXTENC_PACKET_CNT];
    volatile int  write_id;
    volatile int  read_id;
    volatile int  buf_used;
} InFrameManager_t;

typedef struct TextEncDataManager {
    __text_enc_info_t info;

// use fifo or var to manage input frame
    pthread_mutex_t in_buf_lock;
    InFrameManager_t in_buf_mgr;

    long long base_pts;
    long long cur_pts;
    long long pts;

// use fifo to manage output frame
    pthread_mutex_t out_buf_lock;
    OutFrameManager_t out_buf_mgr;

    int mForceEncodeFlag;
} TextEncDataManager;


typedef struct TEXTENCCONTENT_t {
    void *priv_data;

    int (*ClearDataMgr)(void *handle);                                  // clear data in TextEncDataManager after RecordDone -> NOT use
    int (*ShowTextEncMgrData)(void *handle);                            // for debug
    int (*ShowInFrameData)(void *handle);                               // for debug
    int (*ShowOutFrameData)(void *handle, int cur_rid, int pkt_pos);    // for debug
    int (*RequestWriteBuf)(void *handle, void *pInbuf,int size);                 // src_out_port -> enc_in_port
    int (*TextEncodePro)(void *handle);                                 // enc_in_port -> enc_out_port
    int (*GetValidEncPktCnt)(void *handle);                             // packet cnt in fifo which need be sent to RecRender_Comp
    int (*GetEmptyOutFrameCnt)(void *handle);                           // out_buf_packet_unused = 0, OUTPUTUNDERFLOW
    int (*GetValidInputPktCnt)(void *handle);                           // when EncodePro failed with INPUTUNDERFLOW, check InBuf size
    int (*RequestTextEncBuf)(void *handle, void ** pOutbuf, unsigned int * outSize, long long * timeStamp, int* pbuf_id);   // enc_out_port -> recrender_in_port
    int (*ReturnTextEncBuf)(void *handle, void* pOutbuf, unsigned int outSize, long long timeStamp, int nbuf_id);           // enc_out_port <- recrender_in_port
    int (*ReleaseTextEncBuf)(void *handle, void* pOutbuf, unsigned int outSize, long long timeStamp, int nbuf_id);          // enc_out_port <- recrender_in_port
} TEXTENCCONTENT_t;

TEXTENCCONTENT_t *TextEncInit(__text_enc_info_t *pTEncInfo);
void TextEncExit(void *handle);

#endif // TEXT_ENC_API_H

#ifdef __cplusplus
}
#endif
