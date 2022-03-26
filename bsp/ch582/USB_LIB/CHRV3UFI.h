/* 2014.09.09
*****************************************
**   Copyright  (C)  W.ch  1999-2019   **
**   Web:      http://wch.cn           **
*****************************************
**  USB-flash File Interface for CHRV3 **
**  KEIL423, gcc 8.20          **
*****************************************
*/
/* CHRV3 U盘主机文件系统接口, 支持: FAT12/FAT16/FAT32 */

//#include "CHRV3BAS.H"

#ifndef __CHRV3UFI_H__
#define __CHRV3UFI_H__

#define CHRV3_LIB_VER		0x10

//#define DISK_BASE_BUF_LEN		512	/* 默认的磁盘数据缓冲区大小为512字节(可以选择为2048甚至4096以支持某些大扇区的U盘),为0则禁止在本文件中定义缓冲区并由应用程序在pDISK_BASE_BUF中指定 */
/* 如果需要复用磁盘数据缓冲区以节约RAM,那么可将DISK_BASE_BUF_LEN定义为0以禁止在本文件中定义缓冲区,而由应用程序在调用CHRV3LibInit之前将与其它程序合用的缓冲区起始地址置入pDISK_BASE_BUF变量 */

//#define NO_DEFAULT_ACCESS_SECTOR	1		/* 禁止默认的磁盘扇区读写子程序,下面用自行编写的程序代替它 */
//#define NO_DEFAULT_DISK_CONNECT		1		/* 禁止默认的检查磁盘连接子程序,下面用自行编写的程序代替它 */
//#define NO_DEFAULT_FILE_ENUMER		1		/* 禁止默认的文件名枚举回调程序,下面用自行编写的程序代替它 */

#ifdef __cplusplus
extern "C" {
#endif

/* ********************************************************************************************************************* */

/* FILE: CHRV3UF.H */

/* 错误码 */
#ifndef ERR_SUCCESS
#define ERR_SUCCESS				0x00	/* 操作成功 */
#endif
#ifndef ERR_DISK_DISCON
#define ERR_CHRV3_ERROR			0x81	/* CHRV3硬件错误,可能需要复位CHRV3 */
//#define ERR_DISK_DISCON			0x82	/* 磁盘尚未连接,可能磁盘已经断开 */
#define ERR_STATUS_ERR			0x83	/* 磁盘状态错误,可能正在连接或者断开磁盘 */
#define ERR_HUB_PORT_FREE		0x84	/* USB-HUB已经连接但是HUB端口尚未连接磁盘,可能磁盘已经断开 */
#define ERR_MBR_ERROR			0x91	/* 磁盘的主引导记录无效,可能磁盘尚未分区或者尚未格式化 */
#define ERR_TYPE_ERROR			0x92	/* 磁盘分区类型不支持,只支持FAT12/FAT16/BigDOS/FAT32,需要由磁盘管理工具重新分区 */
#define ERR_BPB_ERROR			0xA1	/* 磁盘尚未格式化,或者参数错误,需要由WINDOWS采用默认参数重新格式化 */
#define ERR_TOO_LARGE			0xA2	/* 磁盘非正常格式化并且容量大于4GB,或者容量大于250GB,需要由WINDOWS采用默认参数重新格式化 */
#define ERR_FAT_ERROR			0xA3	/* 磁盘的文件系统不支持,只支持FAT12/FAT16/FAT32,需要由WINDOWS采用默认参数重新格式化 */
#define ERR_DISK_FULL			0xB1	/* 磁盘文件太满,剩余空间太少或者已经没有,需要磁盘整理 */
#define ERR_FDT_OVER			0xB2	/* 目录内文件太多,没有空闲的目录项,FAT12/FAT16根目录下的文件数应该少于500个,需要磁盘整理 */
#define ERR_MISS_DIR			0xB3	/* 指定路径的某个子目录没有找到,可能是目录名称错误 */
#define ERR_FILE_CLOSE			0xB4	/* 文件已经关闭,如果需要使用,应该重新打开文件 */
#define ERR_OPEN_DIR			0x41	/* 指定路径的目录被打开 */
#define ERR_MISS_FILE			0x42	/* 指定路径的文件没有找到,可能是文件名称错误 */
#define ERR_FOUND_NAME			0x43	/* 搜索到与通配符相匹配的文件名,文件名及其完整路径在命令缓冲区中,如果需要使用,应该打开该文件 */
#endif
/* 代码2XH-3XH用于USB主机方式的通讯失败代码,由CHRV3子程序模仿CH375的返回 */
/* 代码1XH用于USB主机方式的操作状态代码,由CHRV3子程序模仿CH375的返回 */
#ifndef ERR_USB_CONNECT
#define	ERR_USB_CONNECT_LS		0x13	/* 检测到低速USB设备连接事件 */
#define	ERR_USB_CONNECT			0x15	/* 检测到USB设备连接事件,磁盘已经连接 */
#define	ERR_USB_DISCON			0x16	/* 检测到USB设备断开事件,磁盘已经断开 */
#define	ERR_USB_BUF_OVER		0x17	/* USB传输的数据有误或者数据太多缓冲区溢出 */
#define	ERR_USB_DISK_ERR		0x1F	/* USB存储器操作失败,在初始化时可能是USB存储器不支持,在读写操作中可能是磁盘损坏或者已经断开 */
#define	ERR_USB_TRANSFER		0x20	/* NAK/STALL等更多错误码在0x20~0x2F */
#endif

/* 磁盘及文件状态 */
#define DISK_UNKNOWN			0x00	/* 尚未初始化,未知状态 */
#define DISK_DISCONNECT			0x01	/* 磁盘没有连接或者已经断开 */
#define DISK_CONNECT			0x02	/* 磁盘已经连接,但是尚未初始化或者无法识别该磁盘 */
#define DISK_USB_ADDR			0x04	/* 磁盘已经分配USB设备地址,但是尚未配置USB和初始化磁盘 */
#define DISK_MOUNTED			0x05	/* 磁盘已经初始化成功,但是尚未分析文件系统或者文件系统不支持 */
#define DISK_READY				0x10	/* 已经分析磁盘的文件系统并且能够支持 */
#define DISK_OPEN_ROOT			0x12	/* 已经打开根目录,扇区模式,只能以扇区为单位读写目录的内容,使用后必须关闭,注意FAT12/FAT16根目录是固定长度 */
#define DISK_OPEN_DIR			0x13	/* 已经打开子目录,扇区模式,只能以扇区为单位读写目录的内容 */
#define DISK_OPEN_FILE			0x14	/* 已经打开文件,扇区模式,可以以扇区为单位进行数据读写 */
#define DISK_OPEN_FILE_B		0x15	/* 已经打开文件,字节模式,可以以字节为单位进行数据读写 */

/* FAT类型标志 */
#ifndef DISK_FAT16
#define DISK_FS_UNKNOWN			0		/* 未知的文件系统 */
#define DISK_FAT12				1		/* FAT12文件系统 */
#define DISK_FAT16				2		/* FAT16文件系统 */
#define DISK_FAT32				3		/* FAT32文件系统 */
#endif

/* FAT数据区中文件目录信息 */
typedef struct _FAT_DIR_INFO {
	UINT8	DIR_Name[11];				/* 00H,文件名,共11字节,不足处填空格 */
	UINT8	DIR_Attr;					/* 0BH,文件属性,参考下面的说明 */
	UINT8	DIR_NTRes;					/* 0CH */
	UINT8	DIR_CrtTimeTenth;			/* 0DH,文件创建的时间,以0.1秒单位计数 */
	UINT16	DIR_CrtTime;				/* 0EH,文件创建的时间 */
	UINT16	DIR_CrtDate;				/* 10H,文件创建的日期 */
	UINT16	DIR_LstAccDate;				/* 12H,最近一次存取操作的日期 */
	UINT16	DIR_FstClusHI;				/* 14H */
	UINT16	DIR_WrtTime;				/* 16H,文件修改时间,参考宏MAKE_FILE_TIME */
	UINT16	DIR_WrtDate;				/* 18H,文件修改日期,参考宏MAKE_FILE_DATA */
	UINT16	DIR_FstClusLO;				/* 1AH */
	UINT32	DIR_FileSize;				/* 1CH,文件长度 */
} FAT_DIR_INFO;							/* 20H */

typedef FAT_DIR_INFO *PX_FAT_DIR_INFO;

/* 文件属性 */
#define ATTR_READ_ONLY			0x01	/* 文件为只读属性 */
#define ATTR_HIDDEN				0x02	/* 文件为隐含属性 */
#define ATTR_SYSTEM				0x04	/* 文件为系统属性 */
#define ATTR_VOLUME_ID			0x08	/* 卷标 */
#define ATTR_DIRECTORY			0x10	/* 子目录 */
#define ATTR_ARCHIVE			0x20	/* 文件为存档属性 */
#define ATTR_LONG_NAME			( ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID )
/* 文件属性 UINT8 */
/* bit0 bit1 bit2 bit3 bit4 bit5 bit6 bit7 */
/*  只   隐   系   卷   目   存   未定义   */
/*  读   藏   统   标   录   档            */
/* 文件时间 UINT16 */
/* Time = (Hour<<11) + (Minute<<5) + (Second>>1) */
#define MAKE_FILE_TIME( h, m, s )	( (h<<11) + (m<<5) + (s>>1) )	/* 生成指定时分秒的文件时间数据 */
/* 文件日期 UINT16 */
/* Date = ((Year-1980)<<9) + (Month<<5) + Day */
#define MAKE_FILE_DATE( y, m, d )	( ((y-1980)<<9) + (m<<5) + d )	/* 生成指定年月日的文件日期数据 */

/* 文件名 */
#define PATH_WILDCARD_CHAR		0x2A	/* 路径名的通配符 '*' */
#define PATH_SEPAR_CHAR1		0x5C	/* 路径名的分隔符 '\' */
#define PATH_SEPAR_CHAR2		0x2F	/* 路径名的分隔符 '/' */
#ifndef MAX_PATH_LEN
#define MAX_PATH_LEN			64		/* 最大路径长度,含所有斜杠分隔符和小数点间隔符以及路径结束符00H */
#endif

/* 外部命令参数 */
typedef union _CMD_PARAM {
	struct {
		UINT8	mBuffer[ MAX_PATH_LEN ];
	} Other;
	struct {
		UINT32	mTotalSector;			/* 返回: 当前逻辑盘的总扇区数 */
		UINT32	mFreeSector;			/* 返回: 当前逻辑盘的剩余扇区数 */
		UINT32	mSaveValue;
	} Query;							/* CMD_DiskQuery, 查询磁盘信息 */
	struct {
		UINT8	mPathName[ MAX_PATH_LEN ];	/* 输入参数: 路径: [盘符,冒号,斜杠,目录名或者文件名及扩展名...,结束符00H], 其中盘符和冒号可以省略, 例如"C:\DIR1.EXT\DIR2\FILENAME.EXT",00H */
	} Open;								/* CMD_FileOpen, 打开文件 */
//	struct {
//		UINT8	mPathName[ MAX_PATH_LEN ];	/* 输入参数: 路径: [盘符,冒号,斜杠,目录名或者文件名及扩展名(含通配符*)...,结束符00H], 其中盘符和冒号可以省略, 例如"C:\DIR1.EXT\DIR2\FILE*",00H */
//	} Open;								/* CMD_FileOpen, 枚举文件, CHRV3vFileSize最高位为1则各调用xFileNameEnumer,为0则返回指定序号的文件名 */
	struct {
		UINT8	mUpdateLen;				/* 输入参数: 是否允许更新长度: 0禁止,1允许 */
	} Close;							/* CMD_FileClose, 关闭当前文件 */
	struct {
		UINT8	mPathName[ MAX_PATH_LEN ];	/* 输入参数: 路径: [盘符,冒号,斜杠,目录名或者文件名及扩展名...,结束符00H], 其中盘符和冒号可以省略, 例如"C:\DIR1.EXT\DIR2\FILENAME.EXT",00H */
	} Create;							/* CMD_FileCreate, 新建文件并打开,如果文件已经存在则先删除后再新建 */
	struct {
		UINT8	mPathName[ MAX_PATH_LEN ];	/* 输入参数: 路径: [盘符,冒号,斜杠,目录名或者文件名及扩展名...,结束符00H], 其中盘符和冒号可以省略, 例如"C:\DIR1.EXT\DIR2\FILENAME.EXT",00H */
	} Erase;							/* CMD_FileErase, 删除文件并关闭 */
	struct {
		UINT32	mFileSize;				/* 输入参数: 新的文件长度,为0FFFFFFFFH则不修改, 返回: 原长度 */
		UINT16	mFileDate;				/* 输入参数: 新的文件日期,为0FFFFH则不修改, 返回: 原日期 */
		UINT16	mFileTime;				/* 输入参数: 新的文件时间,为0FFFFH则不修改, 返回: 原时间 */
		UINT8	mFileAttr;				/* 输入参数: 新的文件属性,为0FFH则不修改, 返回: 原属性 */
	} Modify;							/* CMD_FileQuery, 查询当前文件的信息; CMD_FileModify, 查询或者修改当前文件的信息 */
	struct {
		UINT32	mSaveCurrClus;
		UINT32	mSaveLastClus;
	} Alloc;							/* CMD_FileAlloc, 根据文件长度调整为文件分配的磁盘空间 */
	struct {
		UINT32	mSectorOffset;			/* 输入参数: 扇区偏移,0则移动到文件头,0FFFFFFFFH则移动到文件尾, 返回: 当前文件指针对应的绝对线性扇区号, 0FFFFFFFFH则已到文件尾 */
		UINT32	mLastOffset;
	} Locate;							/* CMD_FileLocate, 移动当前文件指针 */
	struct {
		UINT8	mSectorCount;			/* 输入参数: 读取扇区数, 返回: 实际读取扇区数 */
		UINT8	mActCnt;
		UINT8	mLbaCount;
		UINT8	mRemainCnt;
		PUINT8	mDataBuffer;			/* 输入参数: 缓冲区起始地址, 返回: 缓冲区当前地址 */
		UINT32	mLbaStart;
	} Read;								/* CMD_FileRead, 从当前文件读取数据 */
	struct {
		UINT8	mSectorCount;			/* 输入参数: 写入扇区数, 返回: 实际写入扇区数 */
		UINT8	mActCnt;
		UINT8	mLbaCount;
		UINT8	mAllocCnt;
		PUINT8	mDataBuffer;			/* 输入参数: 缓冲区起始地址, 返回: 缓冲区当前地址 */
		UINT32	mLbaStart;
		UINT32	mSaveValue;
	} Write;							/* CMD_FileWrite, 向当前文件写入数据 */
	struct {
		UINT32	mDiskSizeSec;			/* 返回: 整个物理磁盘的总扇区数, 仅首次调用时返回 */
	} DiskReady;						/* CMD_DiskReady, 查询磁盘就绪 */
	struct {
		UINT32	mByteOffset;			/* 输入参数: 以字节为单位的偏移量, 以字节为单位的文件指针, 返回: 当前文件指针对应的绝对线性扇区号, 0FFFFFFFFH则已到文件尾 */
		UINT32	mLastOffset;
	} ByteLocate;						/* CMD_ByteLocate, 以字节为单位移动当前文件指针 */
	struct {
		UINT16	mByteCount;				/* 输入参数: 准备读取的字节数, 返回: 实际读出的字节数 */
		PUINT8	mByteBuffer;			/* 输入参数: 指向存放读出数据块的缓冲区 */
		UINT16	mActCnt;
	} ByteRead;							/* CMD_ByteRead, 以字节为单位从当前文件读取数据块 */
	struct {
		UINT16	mByteCount;				/* 输入参数: 准备写入的字节数, 返回: 实际写入的字节数 */
		PUINT8	mByteBuffer;			/* 输入参数: 指向存放读出数据块的缓冲区 */
		UINT16	mActCnt;
	} ByteWrite;						/* CMD_ByteWrite, 以字节为单位向当前文件写入数据块 */
	struct {
		UINT8	mSaveVariable;			/* 输入参数: 为0则恢复单个U盘的变量,为0x80则恢复多个U盘的变量,其它值则备份/保存变量 */
		UINT8	mReserved[3];
		PUINT8	mBuffer;				/* 输入参数: 指向子程序库的变量的备份缓冲区,长度不小于80个字节 */
	} SaveVariable;						/* CMD_SaveVariable, 备份/保存/恢复子程序库的变量 */
} CMD_PARAM;

typedef CMD_PARAM CMD_PARAM_I;
//typedef CMD_PARAM *P_CMD_PARAM;

/* SCSI命令码 */
#ifndef SPC_CMD_INQUIRY
#define SPC_CMD_INQUIRY			0x12
#define SPC_CMD_READ_CAPACITY	0x25
#define SPC_CMD_READ10			0x28
#define SPC_CMD_WRITE10			0x2A
#define SPC_CMD_TEST_READY		0x00
#define SPC_CMD_REQUEST_SENSE	0x03
#define SPC_CMD_MODESENSE6		0x1A
#define SPC_CMD_MODESENSE10		0x5A
#define SPC_CMD_START_STOP		0x1B
#endif

/* FILE: CHRV3UFI.C */

#define EN_DISK_WRITE			1

#ifndef DISK_BASE_BUF_LEN
#define DISK_BASE_BUF_LEN		512		/* 默认的磁盘数据缓冲区大小为512字节,建议选择为2048甚至4096以支持某些大扇区的U盘,为0则禁止在.H文件中定义缓冲区并由应用程序在pDISK_BASE_BUF中指定 */
#endif

/* 子程序库中提供的变量 */
extern	UINT8V	CHRV3IntStatus;				/* CHRV3操作的中断状态 */
extern	UINT8V	CHRV3DiskStatus;			/* 磁盘及文件状态 */
extern	UINT8	CHRV3vDiskFat;				/* 逻辑盘的FAT标志:1=FAT12,2=FAT16,3=FAT32 */
extern	UINT8	CHRV3vSecPerClus;			/* 逻辑盘的每簇扇区数 */
extern	UINT8	CHRV3vSectorSizeB;			/* log2(CHRV3vSectorSize) */
extern	UINT32	CHRV3vStartLba;				/* 逻辑盘的起始绝对扇区号LBA */
extern	UINT32	CHRV3vDiskRoot;				/* 对于FAT16盘为根目录占用扇区数,对于FAT32盘为根目录起始簇号 */
extern	UINT32	CHRV3vDataStart;			/* 逻辑盘的数据区域的起始LBA */
extern	UINT32	CHRV3vStartCluster;			/* 当前文件或者目录的起始簇号 */
extern	UINT32	CHRV3vFileSize;				/* 当前文件的长度 */
extern	UINT32	CHRV3vCurrentOffset;		/* 当前文件指针,当前读写位置的字节偏移 */
extern	UINT32	CHRV3vFdtLba;				/* 当前FDT所在的LBA地址 */
extern	UINT32	CHRV3vLbaCurrent;			/* 当前读写的磁盘起始LBA地址 */
extern	UINT16	CHRV3vFdtOffset;			/* 当前FDT在扇区内的偏移地址 */
extern	UINT16	CHRV3vSectorSize;			/* 磁盘的扇区大小 */
extern	UINT8	CHRV3vCurrentLun;			/* 磁盘当前操作逻辑单元号 */
extern	BOOL	CHRV3vSubClassIs6;			/* USB存储类设备的子类为6,0则非6 */
extern	PUINT8	pDISK_BASE_BUF;		/* 指向外部RAM的磁盘数据缓冲区,缓冲区长度不小于CHRV3vSectorSize,由应用程序初始化 */
extern	PUINT8	pDISK_FAT_BUF;		/* 指向外部RAM的磁盘FAT数据缓冲区,缓冲区长度不小于CHRV3vSectorSize,由应用程序初始化 */
extern	UINT16	CHRV3vPacketSize;			/* USB存储类设备的最大包长度:64@FS,512@HS/SS,由应用程序初始化 */
extern	PUINT32	pTX_DMA_A_REG;				/* 指向发送DMA地址寄存器,由应用程序初始化 */
extern	PUINT32	pRX_DMA_A_REG;				/* 指向接收DMA地址寄存器,由应用程序初始化 */
extern	PUINT16	pTX_LEN_REG;				/* 指向发送长度寄存器,由应用程序初始化 */
extern	PUINT16	pRX_LEN_REG;				/* 指向接收长度寄存器,由应用程序初始化 */

extern	CMD_PARAM_I	mCmdParam;				/* 命令参数 */

extern	UINT8	RxBuffer[ MAX_PACKET_SIZE ];  // IN, must even address
extern	UINT8	TxBuffer[ MAX_PACKET_SIZE ];  // OUT, must even address

//#define		PXUDISK_BOC_CBW	PUDISK_BOC_CBW
//#define		PXUDISK_BOC_CSW	PUDISK_BOC_CSW

#ifndef	pSetupReq
#define	pSetupReq	((PUSB_SETUP_REQ)TxBuffer)
#endif
#ifndef	pCBW
#define	pCBW		((PXUDISK_BOC_CBW)TxBuffer)
#define	pCSW		((PXUDISK_BOC_CSW)RxBuffer)
#endif
#ifndef	pBOC_buf
#define	pBOC_buf	(TxBuffer+((USB_BO_CBW_SIZE+4)&0xFE))
#endif

#if		DISK_BASE_BUF_LEN > 0
extern	UINT8	DISK_BASE_BUF[ DISK_BASE_BUF_LEN ];	/* 外部RAM的磁盘数据缓冲区,缓冲区长度为一个扇区的长度 */
#endif
extern	UINT8	CHRV3ReadSector( UINT8 SectCount, PUINT8 DataBuf );	/* 从磁盘读取多个扇区的数据到缓冲区中 */
#ifdef	EN_DISK_WRITE
extern	UINT8	CHRV3WriteSector( UINT8 SectCount, PUINT8 DataBuf );	/* 将缓冲区中的多个扇区的数据块写入磁盘 */
#endif

extern	UINT8	CHRV3DiskConnect( void );	/* 检查磁盘是否连接并更新磁盘状态 */
extern	void	xFileNameEnumer( void );	/* 调用外部定义的子程序,文件名枚举回调子程序 */

extern	UINT8	CHRV3LibInit( void );		/* 初始化CHRV3程序库,操作成功返回0 */

/* 子程序库中提供的子程序 */
/* 下述子程序中, 文件操作子程序CHRV3File*和磁盘查询子程序CHRV3DiskQuery都可能会用到磁盘数据缓冲区pDISK_BASE_BUF,
   并且有可能在pDISK_BASE_BUF中保存了磁盘信息, 所以必须保证pDISK_BASE_BUF不被用于其它用途,
   如果RAM较少, 要将pDISK_BASE_BUF临时用于其它用途, 那么在临时用完后必须调用CHRV3DirtyBuffer清除磁盘缓冲区 */
extern	UINT8	CHRV3GetVer( void );		/* 获取当前子程序库的版本号 */
extern	void	CHRV3DirtyBuffer( void );	/* 清除磁盘缓冲区 */
extern	UINT8	CHRV3BulkOnlyCmd( PUINT8 DataBuf );	/* 执行基于BulkOnly协议的命令 */
extern	UINT8	CHRV3DiskReady( void );		/* 查询磁盘是否准备好 */
extern	UINT8	CHRV3AnalyzeError( UINT8 iMode );	/* USB操作失败分析CHRV3IntStatus返回错误状态 */
extern	UINT8	CHRV3FileOpen( void );		/* 打开文件或者枚举文件 */
extern	UINT8	CHRV3FileClose( void );		/* 关闭当前文件 */
#ifdef	EN_DISK_WRITE
extern	UINT8	CHRV3FileErase( void );		/* 删除文件并关闭 */
extern	UINT8	CHRV3FileCreate( void );	/* 新建文件并打开,如果文件已经存在则先删除后再新建 */
extern	UINT8	CHRV3FileAlloc( void );		/* 根据文件长度调整为文件分配的磁盘空间 */
#endif
extern	UINT8	CHRV3FileModify( void );	/* 查询或者修改当前文件的信息 */
extern	UINT8	CHRV3FileQuery( void );		/* 查询当前文件的信息 */
extern	UINT8	CHRV3FileLocate( void );	/* 移动当前文件指针 */
extern	UINT8	CHRV3FileRead( void );		/* 从当前文件读取数据到指定缓冲区 */
#ifdef	EN_DISK_WRITE
extern	UINT8	CHRV3FileWrite( void );		/* 向当前文件写入指定缓冲区的数据 */
#endif
extern	UINT8	CHRV3ByteLocate( void );	/* 以字节为单位移动当前文件指针 */
extern	UINT8	CHRV3ByteRead( void );		/* 以字节为单位从当前位置读取数据块 */
#ifdef	EN_DISK_WRITE
extern	UINT8	CHRV3ByteWrite( void );		/* 以字节为单位向当前位置写入数据块 */
#endif
extern	UINT8	CHRV3DiskQuery( void );		/* 查询磁盘信息 */
extern	void	CHRV3SaveVariable( void );	/* 备份/保存/恢复子程序库的变量,用于子程序库在多个芯片或者U盘之间进行切换 */

extern	void	mDelayuS( UINT16 n );		// 以uS为单位延时
extern	void	mDelaymS( UINT16 n );		// 以mS为单位延时
extern	UINT8	USBHostTransact( UINT8 endp_pid, UINT8 tog, UINT32 timeout );	// CHRV3传输事务,输入目的端点地址/PID令牌,同步标志,NAK重试时间,返回0成功,超时/出错重试
extern	UINT8	HostCtrlTransfer( PUINT8 DataBuf, PUINT8 RetLen );	// 执行控制传输,8字节请求码在pSetupReq中,DataBuf为可选的收发缓冲区,实际收发长度返回在ReqLen指向的变量中
//extern	void	CopySetupReqPkg( PCCHAR pReqPkt );  // 复制控制传输的请求包
//extern	UINT8	CtrlGetDeviceDescrTB( void );  // 获取设备描述符,返回在TxBuffer中
extern	UINT8	CtrlGetConfigDescrTB( void );  // 获取配置描述符,返回在TxBuffer中
//extern	UINT8	CtrlSetUsbAddress( UINT8 addr );  // 设置USB设备地址
extern	UINT8	CtrlSetUsbConfig( UINT8 cfg );  // 设置USB设备配置
extern	UINT8	CtrlClearEndpStall( UINT8 endp );  // 清除端点STALL
#ifndef	FOR_ROOT_UDISK_ONLY
//extern	UINT8	CtrlGetHubDescr( void );  // 获取HUB描述符,返回在TxBuffer中
extern	UINT8	HubGetPortStatus( UINT8 HubPortIndex );  // 查询HUB端口状态,返回在TxBuffer中
//extern	UINT8	HubSetPortFeature( UINT8 HubPortIndex, UINT8 FeatureSelt );  // 设置HUB端口特性
extern	UINT8	HubClearPortFeature( UINT8 HubPortIndex, UINT8 FeatureSelt );  // 清除HUB端口特性
#endif

#ifdef __cplusplus
}
#endif

#endif
