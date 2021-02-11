/*
 * Macro Definitions
 */
#define TOOL_ERR_ENCODE_ERROR_CODE(base,no)        ( -1 * (((base) << 16) + no) )
#define TOOL_ERR_GET_BASE_FROM_ERROR_CODE(code)    ( (((code) * -1) >> 16) & 0x0000FFFF )
#define TOOL_ERR_GET_NO_FROM_ERROR_CODE(code)      ( ((code) * -1) & 0x0000FFFF )


/*
 * Error Base Definitions
 */
#define UNIX_ERR_BASE                  0
#define OOSQL_EXPORT_ERR_BASE          1

#define TOOL_NUM_OF_ERROR_BASES        2


/*
 * Error Definitions for OOSQL_EXPORT_ERR_BASE
 */
#define eBADPARAMETER_IMPORT           TOOL_ERR_ENCODE_ERROR_CODE(OOSQL_EXPORT_ERR_BASE,0)
#define eCANNOT_OPEN_FILE_IMPORT       TOOL_ERR_ENCODE_ERROR_CODE(OOSQL_EXPORT_ERR_BASE,1)
#define eCANNOT_SEEK_FILE_IMPORT       TOOL_ERR_ENCODE_ERROR_CODE(OOSQL_EXPORT_ERR_BASE,2)
#define eCANNOT_CLOSE_FILE_IMPORT      TOOL_ERR_ENCODE_ERROR_CODE(OOSQL_EXPORT_ERR_BASE,3)
#define eLOGVOLUME_NOT_DEFINED_IMPORT  TOOL_ERR_ENCODE_ERROR_CODE(OOSQL_EXPORT_ERR_BASE,4)
#define eFILE_READ_FAIL_IMPORT         TOOL_ERR_ENCODE_ERROR_CODE(OOSQL_EXPORT_ERR_BASE,5)
#define eFILE_WRITE_FAIL_IMPORT        TOOL_ERR_ENCODE_ERROR_CODE(OOSQL_EXPORT_ERR_BASE,6)
#define eFILE_EXEC_FAIL_IMPORT         TOOL_ERR_ENCODE_ERROR_CODE(OOSQL_EXPORT_ERR_BASE,7)
#define eUNDEFINED_DATABASE_NAME_IMPORT TOOL_ERR_ENCODE_ERROR_CODE(OOSQL_EXPORT_ERR_BASE,8)
#define eUNDEFINED_TABLES_IMPORT       TOOL_ERR_ENCODE_ERROR_CODE(OOSQL_EXPORT_ERR_BASE,9)
#define eALREADY_EXIST_TABLE_IMPORT    TOOL_ERR_ENCODE_ERROR_CODE(OOSQL_EXPORT_ERR_BASE,10)
#define eUNHANDLED_CASE_IMPORT         TOOL_ERR_ENCODE_ERROR_CODE(OOSQL_EXPORT_ERR_BASE,11)
#define eUNHANDLED_TYPE_IMPORT         TOOL_ERR_ENCODE_ERROR_CODE(OOSQL_EXPORT_ERR_BASE,12)
#define eNO_EXIST_DATA_IMPORT          TOOL_ERR_ENCODE_ERROR_CODE(OOSQL_EXPORT_ERR_BASE,13)
#define eCORRUPT_DATA_FILE_IMPORT      TOOL_ERR_ENCODE_ERROR_CODE(OOSQL_EXPORT_ERR_BASE,14)
#define eLRDS_ERROR_IMPORT             TOOL_ERR_ENCODE_ERROR_CODE(OOSQL_EXPORT_ERR_BASE,15)
#define eLOM_ERROR_IMPORT              TOOL_ERR_ENCODE_ERROR_CODE(OOSQL_EXPORT_ERR_BASE,16)
#define eCATALOG_ERROR_IMPORT          TOOL_ERR_ENCODE_ERROR_CODE(OOSQL_EXPORT_ERR_BASE,17)
#define eOOSQL_ERROR_IMPORT            TOOL_ERR_ENCODE_ERROR_CODE(OOSQL_EXPORT_ERR_BASE,18)
#define NUM_ERRORS_OOSQL_EXPORT_ERR_BASE 19