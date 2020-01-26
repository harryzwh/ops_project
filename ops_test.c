//
// Created by enb on 10/07/18.
//

#include <dlfcn.h>
#include <zconf.h>
#include "stdio.h"
#include "ops_agent.h"
#include "ops_scheduler.h"

int main(void)
{
    unsigned char RBG_Blocking_Bit_Maks[OPS_MAX_NUM_RBG]={0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0};
    unsigned short Num_Allocated_RB;
    unsigned char RBG_Allocation_Bit_Mask[4][OPS_MAX_NUM_RBG];

    void (*my_sche_dl)(struct sOPS_Agent* OPS_Agent);
    void *lib_handle;

    int count=0;

    while(1)
    {
        //lib_handle = dlopen("./libOPS_SCHED.so", RTLD_LAZY);
        //my_sche_dl = dlsym(lib_handle, "OPS_PDSCH_Scheduler_dll");

        OPS_Agent_Init();
        OPS_Agent_Reset();

        for (int UE_id = 0; UE_id < OPS_MAX_NUM_UE; UE_id++)
        {
            if (UE_id < 1)
            {
                OPS_Agent.UE_Info[UE_id].Active = 1;
                OPS_Agent.UE_Info[UE_id].OAI_UE_Id = UE_id;
                OPS_Agent.UE_Info[UE_id].CQI = 15;
                OPS_Agent.UE_Info[UE_id].RNTI = 0x3210 + UE_id;
                OPS_Agent.UE_Info[UE_id].Required_nof_RB = UE_id * 20 + 50;
            } else
            {
                OPS_Agent.UE_Info[UE_id].Active = 0;
            }
        }
        OPS_Agent.Set_System_Info(0, 2, OPS_BW_10, RBG_Blocking_Bit_Maks);
        OPS_PDSCH_Scheduler();
        //my_sche_dl(&OPS_Agent);
        for (int UE_id = 0; UE_id < OPS_MAX_NUM_UE; UE_id++)
        {
            if (OPS_Agent.UE_Info[UE_id].Active)
            {
                OPS_Agent.OAI_Get_Resource_Allocation(UE_id, &Num_Allocated_RB, RBG_Allocation_Bit_Mask[OPS_CC_0]);
                sprintf(OPS_Debug_Buffer, "UE_id=%d; Allocated %d RBs\n",
                        OPS_Agent.UE_Info[UE_id].OAI_UE_Id,
                        Num_Allocated_RB);
                OPS_LOG(LOG_LEVEL_INFO, OPS_Debug_Buffer);
                OPS_Format_Matrix(RBG_Allocation_Bit_Mask[OPS_CC_0], POINTER_UCHAR, 1, OPS_MAX_NUM_RBG);
                OPS_LOG(LOG_LEVEL_INFO, OPS_Debug_Buffer);
            }
        }
        printf("%d\n",++count);
        //dlclose(lib_handle);
        sleep(1);
    }
    return 0;
}
