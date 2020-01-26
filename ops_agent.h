/** @brief   Header file of OPS agent
 *  @file    ops_agent.h
 *  @author  harryzwh @ USYD
 *  @version 1.0.0
 *  @date    2018/10/1
 *  @note    None
 */

#ifndef OPENAIRINTERFACE_OPS_H
#define OPENAIRINTERFACE_OPS_H

#include "stdio.h"


/**
* @name Log color
* @{
*/
#define NONE                 "\e[0m"
#define BLACK                "\e[0;30m"
#define L_BLACK              "\e[1;30m"
#define RED                  "\e[0;31m"
#define L_RED                "\e[1;31m"
#define GREEN                "\e[0;32m"
#define L_GREEN              "\e[1;32m"
#define BROWN                "\e[0;33m"
#define YELLOW               "\e[1;33m"
#define BLUE                 "\e[0;34m"
#define L_BLUE               "\e[1;34m"
#define PURPLE               "\e[0;35m"
#define L_PURPLE             "\e[1;35m"
#define CYAN                 "\e[0;36m"
#define L_CYAN               "\e[1;36m"
#define GRAY                 "\e[0;37m"
#define WHITE                "\e[1;37m"
/** @}*/

/**
* @name Log level
* @{
*/
#define LOG_LEVEL_ERROR			3
#define LOG_LEVEL_WARNING		2
#define LOG_LEVEL_INFO			1
#define LOG_LEVEL_DEBUG			0
#define LOG_LEVEL 				LOG_LEVEL_DEBUG
/** @}*/

/**
* @name Date type of pointer
* @{
*/
#define POINTER_CHAR			1
#define POINTER_UCHAR			2
#define POINTER_INT				3
#define POINTER_UINT			4
#define POINTER_SHORT			5
#define POINTER_USHORT			6
/** @}*/

/**
* @name eNodeB  constant
* @{
*/
#define OPS_MAX_NUM_UE          16    ///< Maximum number of UEs
#define OPS_MAX_NUM_RBG         25    ///< Maximum number of RB group (20 MHz)
#define OPS_MAX_NUM_DRB			8     ///< Maximum number of data radio bearer
#define OPS_DRB_LCID_OFFSET     3     ///< DRB start from 3 to 10
#define OPS_CCCH                0     ///< SRB0 LCID
#define OPS_DCCH1               1     ///< SRB1 LCID
#define OPS_DCCH2               2     ///< SRB2 LCID
#define OPS_MAX_NUM_RAB			(OPS_MAX_NUM_DRB+OPS_DRB_LCID_OFFSET)	  ///< Number of SRB+DRB
/** @}*/

/**
* @name System bandwidth
* @{
*/
#define OPS_BW_1                0     ///< 1.4 MHz
#define OPS_BW_3                1     ///< 3.2 MHz
#define OPS_BW_5                2     ///< 5 MHz
#define OPS_BW_10               3     ///< 10 MHz
#define OPS_BW_15               4     ///< 15 MHz
#define OPS_BW_20               5     ///< 20 MHz
/** @}*/

/**
* @name Private
* @{
*/
#define ENABLE_DLL
//#define ENABLE_EXT_LOG
#define OPS_DEBUG_SF            2
#define OPS_CC_0                0
/** @}*/


char OPS_Debug_Buffer[1024];///< Debug information buffer

/**
* @brief OPS bearer information
*
* Parameters of each data bearer of each UE
*/
struct sOPS_Bearer_Info
{
	int Active;///< - Input \n Status of this bearer
	int QCI;///< - Input \n QCI of this bearer (not used if SRB, set to 0 by default)
	int Num_Byte_In_Buffer;///< - Input \n Number of bytes waiting for transmission
	int Num_Byte_Tx;///< - Output \n Number of bytes to transmit in current TTI (not used if SRB, set to 0 by default)
};

/**
* @brief OPS UE information
*
* Parameters of each UE
*/
struct sOPS_UE_Info
{
	int RNTI;///< - Input \n RNTI of this UE
	int Required_nof_RB;///< - Output \n umber of required RBs (for debug purpose only)
	int CQI;///< - Input \n Wideband CQI [0 ... 15]
	int MCS;///< - Output \n MCS
	unsigned long Avg_Num_Tx_Byte;///< - Internal \n Average number of Tx bytes per TTI
	unsigned long Num_Tx_Byte_TTI;///< - Input \n Number of Tx bytes in previous TTI
	char Active;///< - Input \n UE status
	unsigned int RBG_Allocation_Bit_Mask;///< - Input \n RB group allocation bit mask
	struct sOPS_Bearer_Info Bearer_Info[OPS_MAX_NUM_RAB];///< - Input \n Array for bearer parameters

};

/**
* @brief OPS System information
*
* Parameters of radio access network
*/
struct sOPS_System_Info
{
    int Frame_Idx;///< - Input \n Frame index of current TTI [0 ... 1023] (reserved for future use)
    int SubFrame_Idx;///< - Input \n Subframe index of current TTI [0 ... 9] (reserved for future use)
    int Bandwidth;///< - Input \n System bandwidth [0 ... 5]
    unsigned long Total_Frame;///< - Internal \n Total number of frame after system starts (reserved for future use)
	unsigned int RBG_Available_Bit_Mask;///< - Input \n Bit mask of available RB groups 0xFFFF if all RB groups are available
};

/**
* @brief OPS Agent
*
* Top level data structure of OPS \n
* Declare one only as global variable in the system \n
* Call OPS_Agent_Init() once before use it \n
* Call Reset in each TTI before setting inputs for OPS scheduler \n
*/
struct sOPS_Agent
{
	struct sOPS_UE_Info UE_Info[OPS_MAX_NUM_UE];///< - Input \n Array for UE parameters
    struct sOPS_System_Info System_Info;///< - Input \n System parameters
	void (*Set_System_Info)(int Frame_Idx, int SubFrame_Idx, int Bandwidth, unsigned char* RBG_Blocking_Bit_Mask);///< - API set parameters of radio access network to global variable @see void OPS_Set_System_Info(int Frame_Idx, int SubFrame_Idx, int Bandwidth, unsigned char* RBG_Blocking_Bit_Mask)
    void (*Reset)(void);///< - API \n Reset UE and system parameters in global variable @see void OPS_Agent_Reset(void)
    void (*Get_Resource_Allocation)(int UE_id, unsigned short* Num_RB, unsigned char* RBG_Allocation_Mat);///< - API \n Get allocation result from global variable and set back to eNB @see void OPS_OAI_Get_Resource_Allocation(int UE_id, unsigned short* Num_RB, unsigned char* RBG_Allocation_Mat)
	char* (*Get_Bearer_Info)(int UE_id);///< - API \n Get information of radio data bearer in string (for logging only) @see char* OPS_Get_Bearer_Info(int UE_id)
	void (*Update_Num_Tx_Byte)(int UE_id, unsigned long Total_Num_Tx_Byte);///< - API \n Update historical throughput for PF scheduler @see void OPS_Update_Througthput_History(int UE_id, int RNTI, unsigned long Total_Num_Tx_Byte)

	FILE* external_log;///< Private
}OPS_Agent;///< global variable for eNB and scheduler

/**
 * @brief Logging function
 * @param[in] Log_Level
 * - LOG_LEVEL_ERROR
 * - LOG_LEVEL_WARNING
 * - LOG_LEVEL_INFO
 * - LOG_LEVEL_DEBUG
 * @param[in] Message point of a string
 * @note This is a optional function of debug and logging only
 */
void OPS_LOG(int Log_Level, char* Message);

/**
 * @brief Format matrix to string
 * @param[in] Matrix point of a 2D array
 * @param[in] Type Data type
 * - POINTER_CHAR
 * - POINTER_UCHAR
 * - POINTER_INT
 * - POINTER_UINT
 * - POINTER_SHORT
 * - POINTER_USHORT
 * @param[in] Num_Col number of rows
 * @param[in] Num_Row number of column
 * @note This is a optional function of debug and logging only
 */
void OPS_Format_Matrix(void* Matrix, int Type, int Num_Row, int Num_Col);

/**
 * @brief format binary to string
 * @param[in] Binary an integer
 * @param[in] Len number of bits need to be formated
 * @note This is a optional function of debug and logging only
 */
void OPS_Format_Binary(int Binary , int Len);

/**
 * @brief Agent init
 * @note This is a required function to initialize the point of APIs and variables in OPS_Agent
 */
void OPS_Agent_Init(void);

/**
 * @brief Agent reset
 * @note This is a required function to reset the parameters of UE and Agent in OPS_Agent, cross TTI information will be kept
 */
void OPS_Agent_Reset(void);

/**
 * @brief Agent reset
 * @param[in] Frame_Idx frame index in eNB [0 ... 1023]
 * @param[in] SubFrame_Idx subframe index in eNB [0 ... 9]
 * @param[in] Bandwidth system bandwidth
 * - OPS_BW_1
 * - OPS_BW_3
 * - OPS_BW_5
 * - OPS_BW_10
 * - OPS_BW_15
 * - OPS_BW_20
 * @param[in] RBG_Blocking_Bit_Mask bit mask for blocking RB group, set corresponding bit if RB group is not available for current scheduling
 * @note This is a required function to reset the parameters of UE and Agent in OPS_Agent. cross TTI information will be kept
 */
void OPS_Set_System_Info(int Frame_Idx, int SubFrame_Idx, int Bandwidth, unsigned char* RBG_Blocking_Bit_Mask);

/**
 * @brief Get allocation result from global variable
 * @param[in] UE_id private UE id in eNB
 * @param[out] Num_RB number of allocated RBs, will be set by this API after return
 * @param[out] RBG_Allocation_Mat array of allocation results
 * @note This is an example for OAI agent only
 */
void OPS_OAI_Get_Resource_Allocation(int UE_id, unsigned short* Num_RB, unsigned char* RBG_Allocation_Mat);

/**
 * @brief Get bearer information in string
 * @param[in] UE_id private UE id in eNB
 * @return address of string
 * @note This is a optional function of debug and logging only
 */
char* OPS_Get_Bearer_Info(int UE_id);

/**
 * @brief Get allocation result from global variable
 * @param[in] UE_id private UE id in eNB
 * @param[in] Total_Num_Tx_Byte total number of Tx bytes that the UE send
 * @note This is an example for OAI agent only
 */
void OPS_Update_Num_Tx_Byte(int UE_id, unsigned long Total_Num_Tx_Byte);

#endif //OPENAIRINTERFACE_OPS_H

