/** @brief   Source file of OPS agent
 *  @file    ops_agent.c
 *  @author  harryzwh @ USYD
 *  @version 1.0.0
 *  @date    2018/10/1
 *  @note    None
 */

#include "ops_agent.h"
#include "stdio.h"
#include "string.h"
#include "ops_log.h"

void OPS_LOG(int Log_Level, char* Message)
{
    if (Log_Level>=LOG_LEVEL)
    {
    	switch (Log_Level)
    	{
    		case LOG_LEVEL_DEBUG:
    			printf(WHITE "[OPS] [D] ");
    			break;
    		case LOG_LEVEL_INFO:
    			printf(GREEN "[OPS] [I] ");
    			break;
    		case LOG_LEVEL_WARNING:
    			printf(YELLOW "[OPS] [W] ");
    			break;
    		case LOG_LEVEL_ERROR:
    			printf(RED "[OPS] [E] ");
    			break;
    		default:
    			printf(GREEN "[OPS] [I] ");
    			break;
    	}
    	printf("%s" NONE,Message);
    }
}

void OPS_Format_Matrix(void* Matrix, int Type, int Num_Row, int Num_Col)
{
	int Idx=0;
	int Length=0;
	OPS_Debug_Buffer[Idx]='\n';
	Idx++;
	for (int i=0;i<Num_Row;i++)
	{
		for (int j=0;j<Num_Col;j++)
		{
			switch (Type)
			{
				case POINTER_CHAR:
				case POINTER_UCHAR:
					Length = sprintf(&(OPS_Debug_Buffer[Idx]), "%d ", *((char *) Matrix + i * Num_Col + j));
					break;
				case POINTER_INT:
				case POINTER_UINT:
					Length = sprintf(&(OPS_Debug_Buffer[Idx]), "%d ", *((int *) Matrix + i * Num_Col + j));
					break;
                case POINTER_SHORT:
                case POINTER_USHORT:
                    Length = sprintf(&(OPS_Debug_Buffer[Idx]), "%d ", *((short *) Matrix + i * Num_Col + j));
                    break;
                default:
                    OPS_LOG(LOG_LEVEL_ERROR,"OPS_Format_Matrix: Unknow data type!\n");
                    return;
			}
			Idx+=Length;
		}
		OPS_Debug_Buffer[Idx]='\n';
		Idx++;
	}
	OPS_Debug_Buffer[Idx]='\0';
}

char* OPS_Get_Bearer_Info(int UE_id)
{
	static char Buffer[1024];
	int Buf_Idx=0;
	memset(Buffer,0, sizeof(Buffer));
	for (int Bearer_Idx=0;Bearer_Idx<OPS_MAX_NUM_RAB;Bearer_Idx++)
	{
		if (OPS_Agent.UE_Info[UE_id].Bearer_Info[Bearer_Idx].Active)
        {
            Buf_Idx += sprintf(Buffer + Buf_Idx, "(%d %4dk %4d),", OPS_Agent.UE_Info[UE_id].Bearer_Info[Bearer_Idx].QCI,
                    OPS_Agent.UE_Info[UE_id].Bearer_Info[Bearer_Idx].Num_Byte_In_Buffer>>10,
            OPS_Agent.UE_Info[UE_id].Bearer_Info[Bearer_Idx].Num_Byte_Tx);
        }
	}
    Buffer[Buf_Idx-1]='\0';
	return Buffer;
}

void OPS_Format_Binary(int Binary , int Len)
{
	int Length;
	int Idx=0;
	for (int i=0;i<Len;i++)
	{
		Length=sprintf(&(OPS_Debug_Buffer[Idx]), "%1d ",(Binary>>i)&0x00000001);
		Idx+=Length;
	}
	OPS_Debug_Buffer[Idx]='\n';
	Idx++;
	OPS_Debug_Buffer[Idx]='\0';
}

void OPS_Set_System_Info(int Frame_Idx, int SubFrame_Idx, int Bandwidth, unsigned char* RBG_Blocking_Bit_Mask)
{
	static unsigned int OAI_RBG_Bit_Mast_0[]={0x00000000,0x000000C3,0x00001F0F,0x0001FC7F,0x0007F8FF,0x01FFC7FF};
	OPS_Agent.System_Info.Frame_Idx=Frame_Idx;
	if (SubFrame_Idx==9)
    {
        OPS_Agent.System_Info.Total_Frame += 1;
    }
    OPS_Agent.System_Info.SubFrame_Idx=SubFrame_Idx;
    OPS_Agent.System_Info.Bandwidth=Bandwidth;
    for (int i=0;i<OPS_MAX_NUM_RBG;i++)
	{
		if (RBG_Blocking_Bit_Mask[i])
		{
            OPS_Agent.System_Info.RBG_Available_Bit_Mask&=~(1<<i);
		}
		else
		{
            OPS_Agent.System_Info.RBG_Available_Bit_Mask|=1<<i;
		}
	}
	if (SubFrame_Idx==0||SubFrame_Idx==5)
	{
		OPS_Agent.System_Info.RBG_Available_Bit_Mask&=OAI_RBG_Bit_Mast_0[Bandwidth];
	}
}

void OPS_Agent_Init(void)
{
	OPS_Agent.Reset=OPS_Agent_Reset;
	OPS_Agent.Set_System_Info=OPS_Set_System_Info;
	OPS_Agent.Get_Resource_Allocation=OPS_OAI_Get_Resource_Allocation;
	OPS_Agent.Get_Bearer_Info=OPS_Get_Bearer_Info;
	OPS_Agent.Update_Num_Tx_Byte=OPS_Update_Num_Tx_Byte;
#ifdef ENABLE_EXT_LOG
    OPS_Ext_Log_Init();
#endif
}

void OPS_Agent_Reset(void)
{
    unsigned long Avg_Tx[OPS_MAX_NUM_UE];
    for (int UE_id=0;UE_id<OPS_MAX_NUM_UE;UE_id++)
    {
        Avg_Tx[UE_id]=OPS_Agent.UE_Info[UE_id].Avg_Num_Tx_Byte;
    }
    memset(OPS_Agent.UE_Info,0, sizeof(OPS_Agent.UE_Info));
    for (int UE_id=0;UE_id<OPS_MAX_NUM_UE;UE_id++)
    {
        OPS_Agent.UE_Info[UE_id].Avg_Num_Tx_Byte=Avg_Tx[UE_id];
    }

    unsigned long Total_Frame_Temp=OPS_Agent.System_Info.Total_Frame;
    memset(&OPS_Agent.System_Info,0, sizeof(OPS_Agent.System_Info));
    OPS_Agent.System_Info.Total_Frame=Total_Frame_Temp;
}

void OPS_OAI_Get_Resource_Allocation(int UE_id, unsigned short* Num_RB, unsigned char* RBG_Allocation_Mat)
{
	static int RBG_Size_Table[] = {1, 2, 2, 3, 4, 4};
	static int Last_RBG_Size_Table[] = {1, 1, 1, 2, 3, 4};
	static int Num_RBG_Table[] = {6, 8, 13, 17, 19, 25};
	int RBG_Size = RBG_Size_Table[OPS_Agent.System_Info.Bandwidth];
	int Last_RBG_Size = Last_RBG_Size_Table[OPS_Agent.System_Info.Bandwidth];
	int Num_Total_RBG=Num_RBG_Table[OPS_Agent.System_Info.Bandwidth];
    *Num_RB=0;
	for (int i=0;i<OPS_MAX_NUM_RBG;i++)
	{
		if ((OPS_Agent.UE_Info[UE_id].RBG_Allocation_Bit_Mask>>i)&0x00000001)
		{
			RBG_Allocation_Mat[i]=1;
			if (i==Num_Total_RBG-1)
			{
				//Last RBG
				*Num_RB+=Last_RBG_Size;
			}
			else
			{
				*Num_RB+=RBG_Size;
			}
		}
		else
		{
			RBG_Allocation_Mat[i]=0;
		}
	}
}

void OPS_Update_Num_Tx_Byte(int UE_id, unsigned long Total_Num_Tx_Byte)
{
    static unsigned long Num_Tx_Byte_TTI_Temp[OPS_MAX_NUM_UE]={0};
    static int RNTI_Temp[OPS_MAX_NUM_UE]={0};
    if (OPS_Agent.UE_Info[UE_id].RNTI!=RNTI_Temp[UE_id])
	{
        Num_Tx_Byte_TTI_Temp[UE_id]=0;
        RNTI_Temp[UE_id]=OPS_Agent.UE_Info[UE_id].RNTI;
	}
    OPS_Agent.UE_Info[UE_id].Num_Tx_Byte_TTI=Total_Num_Tx_Byte-Num_Tx_Byte_TTI_Temp[UE_id];
    Num_Tx_Byte_TTI_Temp[UE_id]=Total_Num_Tx_Byte;
}
