//
// Created by enb on 18/09/18.
//

#include "ops_log.h"
#include "ops_agent.h"
#include "time.h"
#include "string.h"

void OPS_Ext_Log_Init(void)
{
    char Filename[256] = {0};
    time_t timer = time(NULL);
    strftime(Filename, sizeof(Filename), "OPS_%Y%m%d_%H%M%S.csv", localtime(&timer));
    OPS_Agent.external_log=fopen(Filename,"w");
    fputs("TTI,UE_ID,RNTI,Speed\n",OPS_Agent.external_log);
}

void OPS_Ext_Log_Add_Record(void)
{
    for (int UE_id=0; UE_id<OPS_MAX_NUM_UE;UE_id++)
    {
        if (OPS_Agent.UE_Info[UE_id].Active)
        {
            sprintf(OPS_Debug_Buffer,"%ld,%d,%d,%.4f\n",
                    OPS_Agent.System_Info.Total_Frame*10+OPS_Agent.System_Info.SubFrame_Idx,
                    UE_id,
                    OPS_Agent.UE_Info[UE_id].RNTI,
                    OPS_Agent.UE_Info[UE_id].Avg_Num_Tx_Byte/125.0);

            fputs(OPS_Debug_Buffer,OPS_Agent.external_log);
        }
    }
}
