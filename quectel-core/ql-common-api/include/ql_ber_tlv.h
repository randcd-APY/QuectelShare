/*-----------------------------------------------------------------------------------------------*/
/**
  @file ql_ber_tlv.h
  @brief internal API
*/
/*-----------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------
  Copyright (c) 2018 Quectel Wireless Solution, Co., Ltd. All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
-------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------
  EDIT HISTORY
  This section contains comments describing changes made to the file.
  Notice that changes are listed in reverse chronological order.
  $Header: $
  when       who          what, where, why
  --------   ---          ----------------------------------------------------------
  20190321   baron.qian  Created, move from modem source
-------------------------------------------------------------------------------------------------*/

#ifndef BER_TLV_H_
#define BER_TLV_H_
//#ifndef FEATURE_QUECTEL_LOCATOR_ENHANCE
#define FEATURE_QUECTEL_LOCATOR_ENHANCE

#define TLV_TRUE                (1)
#define TLV_FALSE               (0)


#define Q_ENCRIPT_XOR 			1
#define MAX_RAW_KEY_NUM         (8)
#define XOR_KEY_LEN             (8)

#define BER_CONSTRUCTOR         (0x20)
#define BER_LONG_LEN            (0x80)
#define BER_MORE_TAG            (0x80)
#define BER_EXTENSION_ID        (0x1F)

/*单个TLV长度最大为2046字节*/
#define MAX_SIGLE_TLV_LEN       (2046)

#define PARASE_RECURSION_TREE_ERROR (65535)

/*auth info length define*/
#define AUTH_NAME_LEN 			16
#define AUTH_PWD_LEN			16
#define AUTH_IMEI_LEN			15
#define AUTH_MD5_LEN			8
#ifdef FEATURE_QUECTEL_LOCATOR_ENHANCE
#define AUTH_TOTKEN_LEN 16
#endif
#define AUTH_FIXINFO_LEN        17	/* 固定长度：IMEI(15)+RAND(2)=17*/
#define MAX_AUTH_INFO_SIZE  (AUTH_NAME_LEN+AUTH_PWD_LEN+AUTH_FIXINFO_LEN)
#define MAX_AUTH_REQ_SIZE    (AUTH_NAME_LEN+AUTH_FIXINFO_LEN)
#define MAX_SCELL_INFO_SIZE 19/*17 为三角定位增加bcch*/ /*单个cell占的字节数*/

/* 服务类型*/
#define Q_SERV_TYPE_LOC		  0x01
#define Q_SERV_TYPE_LPOS      0x02
#define Q_SERV_TYPE_APS	      0x03
#define Q_SERV_TYPE_USER	  0x04

/* 定位方法: 发给服务器的locMethod*/
typedef enum QLocMethod_Tag
{
    Q_LOC_METHOD_BEGIN             = 0,
    Q_LOC_METHOD_NORMAL            = 1,
    Q_LOC_METHOD_MUTI              = 2,
    Q_LOC_METHOD_SERVER_TRIANGLE   = 3,
    Q_LOC_METHOD_CLIENT_TRIANGLE   = 4,
    Q_LOC_METHOD_NUM
}QLocMethod_e;

/*基站定位请求TAG宏定义*/
#define BER_LOC_TAG	              0x01	/* 基站定位请求，嵌套类型*/
#define BER_LOC_BASIC_TAG		  0x01  /* 基本信息*/
#define BER_LOC_AUTH_TAG		  0x02  /* 鉴权信息*/
#define BER_LOC_CELL1_TAG		  0x03  /* cell info1*/
#define BER_LOC_CELL2_TAG		  0x04  /* cell info2*/
#define BER_LOC_CELL3_TAG		  0x05  /* cell info3*/
#define BER_LOC_CELL4_TAG		  0x06  /* cell info4*/
#define BER_LOC_CELL5_TAG		  0x07  /* cell info5*/
#define BER_LOC_CELL6_TAG		  0x08  /* cell info6*/
#define BER_LOC_MD5_TAG		      0x09  /* MD5*/
/*Begin - 2013.6.13 Kim 增加分步式服务器重定向功能*/
#define BER_LOC_SRV_LIST_VER_TAG  0x0A /*固化服务器表version*/
#define BER_LOC_ERROR_REPORT1_TAG 0x0B /*前一次访问出错上报*/
#define BER_LOC_ERROR_REPORT2_TAG 0x0C /*前一次访问出错上报*/
#define BER_LOC_ERROR_REPORT3_TAG 0x0D /*前一次访问出错上报*/
/*End - 2013.6.13 Kim 增加分步式服务器重定向功能*/

/* 短整型大小端互换*/
#define BigLittleSwap16(A)  ((((unsigned short)(A) & 0xff00) >> 8) | (((unsigned short)(A) & 0x00ff) << 8))

/* 长整型大小端互换*/
#define BigLittleSwap32(A)  ((((unsigned int)(A) & 0xff000000) >> 24) | \
													(((unsigned int)(A) & 0x00ff0000) >> 8) | \
													(((unsigned int)(A) & 0x0000ff00) << 8) | \
													(((unsigned int)(A) & 0x000000ff) << 24))

#define ber_malloc              malloc
//#define ber_printf              printf
#define ber_printf(format, ...) \
				printf("[DEBUG][%s:%d] "format,  __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define ber_free				free
#define SAFE_FREE(p)			{if(p){ber_free(p); (p)=NULL;}}  
///////////////////////////////////////////////////////////////
typedef enum QuectelBerClass_Tag{
	BER_TAG_CLASS_UNIVERSAL        		= 0,         	/* 0b00 */
	BER_TAG_CLASS_APPLICATION    		= 1,           	/* 0b01 */
	BER_TAG_CLASS_CONTEXT            	= 2,       		/* 0b10 */
	BER_TAG_CLASS_PRIVATE             	= 3	    		/* 0b11 */
}ber_class_tag;

typedef enum QuectelBerConstruct_Tag{
	BER_TAG_PRIMITIVE,
	BER_TAG_CONSTRUCTED
}ber_construct_tag;

typedef struct QuectelBerTlv_Tag
{
	unsigned int tag_value;
	ber_construct_tag construct_tag;
	ber_class_tag class_tag;
}ber_tlv_tag;

/*返回位置信息*/
#define BER_POS_TAG                       0x02	/* 复合结构*/
#define BER_POS_STAT_TAG                  0x01	
#define BER_POS_BASIC_TAG                 0x02
#define BER_POS_RAND_TAG                  0x03
#define BER_POS_POS1_TAG                  0x04 
#define BER_POS_POS2_TAG                  0x05
#define BER_POS_POS3_TAG                  0x06
#define BER_POS_POS4_TAG                  0x07 
#define BER_POS_POS5_TAG                  0x08 
#define BER_POS_POS6_TAG                  0x09 
#define BER_POS_MD5_TAG                   0x0A  /* MD5*/
#define BER_POS_REDIRECT1_TAG         	  0x0B /*重定向表数据1*/
#define BER_POS_REDIRECT2_TAG             0x0C /*重定向表数据2*/
#define BER_POS_REDIRECT3_TAG         	  0x0D /*重定向表数据3*/
#define BER_POS_SRVLIST_VER_TAG     	  0x0E /*固化服务器表版本信息*/
#define BER_POS_SRVLIST1_TAG           	  0x0F /*固化服务器表数据1*/
#define BER_POS_SRVLIST2_TAG              0x10
#define BER_POS_SRVLIST3_TAG              0x11
#define BER_POS_SRVLIST4_TAG              0x12
#define BER_POS_SRVLIST5_TAG              0x13
#define BER_POS_SRVLIST6_TAG              0x14
#define BER_POS_SRVLIST7_TAG              0x15
#define BER_POS_SRVLIST8_TAG              0x16
#define BER_POS_SRVLIST9_TAG              0x17
#define BER_POS_SRVLIST10_TAG         	  0x18 /*固化服务器表数据10*/
#define BER_POS_ADDRESS_TAG               0x19 /*街道信息*/
#define BER_POS_TRIANGLE_FRE_TAG		  0x1A

/*basic info*/
typedef struct
{
    unsigned char server_type;       /*服务类型*/
    unsigned char encrypt_type;     /*加密类型*/
    unsigned char key_index;         /*密钥索引*/
    unsigned char pos_data_type;  /*数据包类型*/
    unsigned char loc_method;        /*定位类型*/
}QTlvBasicInfo;

/*auth info*/
typedef struct
{
	unsigned char name[AUTH_NAME_LEN+1];                 /*用户名*/
	unsigned char pwd[AUTH_PWD_LEN+1];                   /*密码*/
	unsigned char imei[AUTH_IMEI_LEN+1];                 /* IMEI*/
#ifdef FEATURE_QUECTEL_LOCATOR_ENHANCE	
	unsigned char token[AUTH_TOTKEN_LEN*2+1];
#endif
	unsigned short rand;                                 /*  随机数*/
}QTlvAuthInfo;

/*Cell Info*/
typedef struct
{
    unsigned char  radio_type;//0:GSM,1:WCDMA,2:LTE
    unsigned short mcc;
    unsigned short mnc;
    unsigned int lac;
    unsigned int cellId;
    short signal;
    unsigned short timead;
    unsigned short bcch; /*2013.9.23 add for 三角定位*/
}QTlvCellInfo;




#define MAX_BASIC_INFO_SIZE 5

/*返回的位置信息结构体*/
typedef struct
{
    int isValid;         			/*是否是有效的位置信息*/
    float longitude;            	/*经度*/
    float latitude;               	/*纬度*/
    unsigned short accuracy; 		/*精度*/
    unsigned char  uCellFlag;    	/*0-正常基站，1 - 表示此基站为无效基站*/
}QCellLocPositonInfo;

/*TLV结构体*/
typedef struct QTLVNode_Tag {
    //q_link_type  link_ptr;
	ber_tlv_tag tag;                     	/*标记值*/
	int length;                   			/*数据长度*/
	unsigned char* value;                  	/*数据*/
	struct QTLVNode_Tag* next;              /*兄弟*/
	struct QTLVNode_Tag* child;             /*孩子*/

	struct QTLVNode_Tag*(*AddBrother)(struct QTLVNode_Tag* curNode, struct QTLVNode_Tag* newNode);
	struct QTLVNode_Tag*(*AddChild)(struct QTLVNode_Tag* curNode, struct QTLVNode_Tag* newNode);
}QTLVNode;

/*Tlv node operate*/
QTLVNode* QTLVNode_Create(void);
void tlv_safe_delete_node(QTLVNode *node);
unsigned char* tlv_realloc_memory(unsigned char* buf1,unsigned int buf1Len, unsigned char* buf2, unsigned int buf2Len);
void tlv_destroy_tree(QTLVNode *root);

/*encryption operate*/
void data_encryption_xor(unsigned char keyIndex, const unsigned char *src, unsigned char *dest, unsigned int len);


/*helper functions*/
void HexToStr(unsigned char *pbDest, unsigned char *pbSrc, int nLen);
//void trace_dump_hex(char *prompt, unsigned char *buf, int len);
int QTlv_make_md5_value(QTlvBasicInfo *baseinfo, QTlvAuthInfo *authinfo, QTlvCellInfo *cellinfo, unsigned char *retMD5Value);
int QTlv_check_md5_value(unsigned char stat, QTlvBasicInfo *basicinfo, QTlvAuthInfo *authinfo, QCellLocPositonInfo *posinfo, unsigned char *retMD5Value);
int tlv_parse_to_tree(const unsigned char* buffer, int bufSize, QTLVNode** root);
QTLVNode *tlv_find_by_level_traverse(QTLVNode *startNode, unsigned int tag);
void tlv_set_token(unsigned char *token, int token_len, unsigned char *dest, int dest_len);
int tlv_encode_info(QTlvBasicInfo *basic_info, QTlvAuthInfo *auth_info, 
					QTlvCellInfo *cell_info, int cell_num, unsigned char *md5value, 
					int md5len, unsigned char **req_info);
/*serialize functions*/
unsigned int QTlv_Serialize_BasicInfo_Buf(QTlvBasicInfo* pBaseInfo, unsigned char *retBuffer);
unsigned int QTlv_Serialize_AuthInfo_Buf(QTlvAuthInfo *pAuthInfo, int bWithPwd, unsigned char *retBuffer);
unsigned int QTlv_Serialize_CellInfo_Buf(QTlvCellInfo *cellInfo, unsigned char *retBuffer);
unsigned int QTlv_Serialize_RetPosInfo_Buf(QCellLocPositonInfo *pRetPosInfo, unsigned char *retBuffer);
int tlv_serialize_tlv_buffer(const ber_tlv_tag *tlvtag, int tlvlen, const unsigned char *tlvvalue, int valuesize, unsigned char **retbuf);


float NtoH_float(float n);
float HtoN_float(float h);
unsigned int NtoH_uint32(unsigned int n);
unsigned int HtoN_uint32(unsigned int h);
unsigned short NtoH_uint16(unsigned short n);
unsigned short HtoN_uint16(unsigned short h);



#endif /* !BER_TLV_H_ */
