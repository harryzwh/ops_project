/** @brief   Header file of OPS scheduler
 *  @file    ops_scheduler.h
 *  @author  harryzwh @ USYD
 *  @version 1.0.0
 *  @date    2018/10/1
 *  @note    None
 */

#ifndef OPENAIRINTERFACE_OPS_SCHEDULER_H
#define OPENAIRINTERFACE_OPS_SCHEDULER_H

#include "ops_agent.h"

/**
 * @brief Entry point of PDSCH scheduler when compiled as static library
 * @note This is used for debug only
 */
void OPS_PDSCH_Scheduler(void);

/**
 * @brief Entry point of PDSCH scheduler when compiled as shared library
 * @param[in,out] OPS_Agent Pointer of OPS agent
 * @note This is used when enabling real-time scheduling
 */
void OPS_PDSCH_Scheduler_dll(struct sOPS_Agent* OPS_Agent);

/**
 * @brief Metric calculation based on RR
 * @param[in] OPS_Agent Pointer of OPA agent
 * @param[out] Metric of each UE
 * @note This is to calculate a metric for each UE based on RR. One RBG will be allocated to selected UE which has maximum metric
 */
void OPS_UE_Selection_Metric_RR(struct sOPS_Agent* OPS_Agent, double* Metric);

/**
 * @brief Metric calculation based on PF
 * @param[in] OPS_Agent Pointer of OPA agent
 * @param[out] Metric of each UE
 * @note This is to calculate a metric for each UE based on PF. One RBG will be allocated to selected UE which has maximum metric
 */
void OPS_UE_Selection_Metric_PF(struct sOPS_Agent* OPS_Agent, double* Metric);

/**
 * @brief Metric calculation to invoke CQI
 * @param[in] OPS_Agent Pointer of OPA agent
 * @param[out] Metric of each UE
 * @note This is an example to invoke CQI. More resource will be allocated to UE has higher priority (lower CQI). UEs with the same
 * CQI will be scheduled based on RR. One RBG will be allocated to selected UE which has maximum metric
 */
void OPS_UE_Selection_Metric_TP(struct sOPS_Agent* OPS_Agent, double* Metric);

/**
 * @brief Get RBG configuration based on bandwidth
 * @param[in] Bandwidth system bandwidth
 * - OPS_BW_1
 * - OPS_BW_3
 * - OPS_BW_5
 * - OPS_BW_10
 * - OPS_BW_15
 * - OPS_BW_20
 * @param[out] RBG_Size Number of RBs per RBG
 * @param[out] Last_RBG_Size Number of RBs in the last RBG
 * @param[out] Num_Rest_RBG Number of available RBGs in current scheduling process
 * @param[out] Num_Total_RBG Total number of RBGs (including those not available)
 * @note This is helper function used by scheduler only
 */
void OPS_BW2RB_RBG(int Bandwidth, int* RBG_Size, int* Last_RBG_Size, int* Num_Rest_RBG, int* Num_Total_RBG);

/**
 * @brief Get the number of byte a RB can carry based on CQI
 * @param[in] CQI CQI of a UE
 * @return number of byte
 * @note This is helper function used by scheduler only
 */
int OPS_CQI2Byte(int CQI);

/**
 * @brief Calculate the hamming weight for a binary number
 * @param[in] n 32 bits binary number
 * @return number of '1's in n when represented in binary
 * @note This can be used to calculate how many RBGs is allocated for a UE with RBG bit-mask as input.
 * This is helper function used by scheduler only
 */
int OPS_Hamming_Weight(int n);

/**
 * @brief Get the number of allocate RBs
 * @param[in] OPS_Agent Pointer of OPS agent
 * @param[in] UE_id UE index in OPS agent
 * @return number of allocated RBs for specified UE
 * @note This is helper function used by scheduler only
 */
int OPS_Num_Allocated_RB(struct sOPS_Agent* OPS_Agent, int UE_id);

/**
 * @brief Function to update average throughput of all UEs
 * @param[in,out] OPS_Agent
 * @note The average throughput is set back to OPS_Agent. This is helper function used by scheduler only
 */
void OPS_Update_Average_Throughput(struct sOPS_Agent* OPS_Agent);

/**
 * @brief Get MCS based on CQI
 * @param[in] CQI CQI of a UE
 * @return MCS before rate match
 * @note This is helper function used by scheduler only
 */
int OPS_CQI2MCS(int CQI);

/**
 * @brief Get TBS based on MCS and number of RBs
 * @param MCS MCS of a UE
 * @param N_RB Number of PRB
 * @return TBS in byte
 * @note This is helper function used by scheduler only
 */
int OPS_MCS2TBS(int MCS, int N_RB);

/**
 * @brief Function to set MCS of all UEs
 * @param[in,out] OPS_Agent
 * @note This is helper function used by scheduler only
 */
void OPS_Set_MCS(struct sOPS_Agent* OPS_Agent);

/**
 * @brief Calculate number of requred RBs for each UE
 * @param[in] OPS_Agent
 * @param[out] Num_Required_RB required number of RBs, limited by maximun number of RB in Table 7.1.7.2.1-1 in 3GPP TS 36.213 V8.6.0
 * @note This is helper function used by scheduler only
 */
void OPS_Find_Num_Required_RB(struct sOPS_Agent *OPS_Agent, int *Num_Required_RB);

/**
 * @brief Find the UE with maximun metric for RBG allocation
 * @param[in] OPS_Agent
 * @param[in] Metric Metric table for selection
 * @param[out] Num_Required_RB table of number of required RBs. UEs with enough RBs will not be selected
 * @return Index of selected UE
 * @note This is helper function used by scheduler only
 */
int OPS_Find_Selected_UE(struct sOPS_Agent* OPS_Agent, double* Metric, int* Num_Required_RB);

/**
 * @brief Adjust MCS to match TBS to payload size
 * @param[in,out] OPS_Agent
 * @param[in] UE_id Index of UE with new allocated RBG
 * @note This is to make sure MCS will not be to high if TBS is much larger than payload size. This is helper function used by scheduler only
 */
void OPS_Adjust_MCS(struct sOPS_Agent* OPS_Agent, int UE_id);

/**
 * @brief Metric calculation for bearer based on QCI prioritization
 * @param[in] OPS_Agent
 * @param[out] Metric Metric for bearer selection
 * @param[in] UE_id UE to be scheduled
 * @note By default 3/4 of resource is allocated to QCI 8 the rest is allocated to QCI 9. This is helper function used by scheduler only
 */
void OPS_Bearer_Selection_Metric_TP(struct sOPS_Agent* OPS_Agent, double* Metric, int UE_id);

/**
 * @brief Select a bearer to allocate buffer space
 * @param[in] OPS_Agent
 * @param[in] Metric metric for selection
 * @param[in] UE_id UE to be scheduled
 * @return Selected bearer index
 * @note This is helper function used by scheduler only
 */
int OPS_Find_Selected_Bearer(struct sOPS_Agent* OPS_Agent, double* Metric, int UE_id);

#endif //OPENAIRINTERFACE_OPS_SCHEDULER_H
