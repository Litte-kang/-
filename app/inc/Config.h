#ifndef _CONFIG_H_
#define _CONFIG_H_

#define SYS_CONFIG_BUFF_SIZE	1024

#define ERR_LOG_PATH		 	"./log/err.log"
#define RUN_LOG_PATH			"./log/run.log"
#define SYS_CONFIG_PATH			"./config/sys.config"
#define QUEUE_DB_PATH		 	".queuedb.db"
#define AISLE_DATA_TABLE	 	"aisle_data_"

//system config order
#define SYS_CONFIG_MIDDLE_NO_INFO	0
#define SYS_CONFIG_UART_DEV_INFO	1
#define SYS_CONFIG_SERVER_INFO		2

#endif //_CONFIG_H_