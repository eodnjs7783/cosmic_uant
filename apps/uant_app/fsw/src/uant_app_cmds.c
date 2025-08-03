/************************************************************************
 * NASA Docket No. GSC-18,719-1, and identified as “core Flight System: Bootes”
 *
 * Copyright (c) 2020 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/**
 * \file
 *   This file contains the source code for the Sample App Ground Command-handling functions
 */

/*
** Include Files:
*/
#include "uant.h"
#include "uant_app.h"
#include "uant_app_cmds.h"
#include "uant_app_msgids.h"
#include "uant_app_eventids.h"
#include "uant_app_version.h"
#include "uant_app_tbl.h"
#include "uant_app_utils.h"
#include "uant_app_msg.h"
// !!!! Most of the cmds are in the device directory 
/* The uant_lib module provides the UANT_Function() prototype */
//#include "uant_lib.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function is triggered in response to a task telemetry request */
/*         from the housekeeping task. This function will gather the Apps     */
/*         telemetry, packetize it and send it to the housekeeping task via   */
/*         the software bus                                                   */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
CFE_Status_t UANT_APP_SendHkCmd(const UANT_APP_SendHkCmd_t *Msg) // Msg는 트리거일 뿐
{//hk tlm 보내는 함수
    int i;

    /*
    ** Get command execution counters...
    */
    UANT_APP_Data.HkTlm.Payload.CommandErrorCounter = UANT_APP_Data.ErrCounter;
    UANT_APP_Data.HkTlm.Payload.CommandCounter      = UANT_APP_Data.CmdCounter;

    /*
    ** Send housekeeping telemetry packet...
    */
    CFE_SB_TimeStampMsg(CFE_MSG_PTR(UANT_APP_Data.HkTlm.TelemetryHeader));
    CFE_SB_TransmitMsg(CFE_MSG_PTR(UANT_APP_Data.HkTlm.TelemetryHeader), true);

    /*
    ** Manage any pending table loads, validations, etc.
    */
    for (i = 0; i < UANT_APP_NUMBER_OF_TABLES; i++)
    {
        CFE_TBL_Manage(UANT_APP_Data.TblHandles[i]);
    }

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* UANT NOOP commands                                                       */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
CFE_Status_t UANT_APP_NoopCmd(const UANT_APP_NoopCmd_t *Msg)
{
    UANT_APP_Data.CmdCounter++;

    CFE_EVS_SendEvent(UANT_APP_NOOP_INF_EID, CFE_EVS_EventType_INFORMATION, "UANT: NOOP command %s",
                      UANT_APP_VERSION);

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function resets all the global counter variables that are     */
/*         part of the task telemetry.                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
CFE_Status_t UANT_APP_ResetCountersCmd(const UANT_APP_ResetCountersCmd_t *Msg)
{
    UANT_APP_Data.CmdCounter = 0;
    UANT_APP_Data.ErrCounter = 0;

    CFE_EVS_SendEvent(UANT_APP_RESET_INF_EID, CFE_EVS_EventType_INFORMATION, "UANT: RESET command");

    return CFE_SUCCESS;
}


CFE_Status_t UANT_APP_Reset(const UANT_APP_ISIS_ResetCmd_t *Msg)
{
    CFE_Status_t     status;
    status = ISIS_UANT_Reset();
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(UANT_APP_RESET_ERR_EID, CFE_EVS_EventType_ERROR,
                        "UANT: Reset command failed, status = 0x%08X", status);
        UANT_APP_Data.ErrCounter++;
    }
    else
    {
        UANT_APP_Data.CmdCounter++;
    }
    return status;
}


CFE_Status_t UANT_APP_Arm(const UANT_APP_ISIS_ArmAntennaSystemsCmd_t *Msg)
{
    CFE_Status_t     status;
    status = ISIS_UANT_ArmAntennaSystems();
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(UANT_APP_ARM_ERR_EID, CFE_EVS_EventType_ERROR,
                        "UANT: ArmAntennaSystems failed, status = 0x%08X", status);
        UANT_APP_Data.ErrCounter++;
    }
    else
    {
        UANT_APP_Data.CmdCounter++;
    }
    return status;
}


CFE_Status_t UANT_APP_Disarm(const UANT_APP_ISIS_DisarmCmd_t *Msg)
{
    CFE_Status_t     status;
    status = ISIS_UANT_Disarm();
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(UANT_APP_DISARM_ERR_EID, CFE_EVS_EventType_ERROR,
                        "UANT: Disarm failed, status = 0x%08X", status);
        UANT_APP_Data.ErrCounter++;
    }
    else
    {
        UANT_APP_Data.CmdCounter++;
    }
    return status;
}


CFE_Status_t UANT_APP_AutomatedDeployment(const UANT_APP_ISIS_AutomatedDeploymentCmd_t *Msg)
{
    CFE_Status_t status;

    status = ISIS_UANT_AutomatedSequentialDeployment(Msg->Arg);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(UANT_APP_AUTO_DEPLOY_ERR_EID, CFE_EVS_EventType_ERROR,
                          "UANT: AutomatedSequentialDeployment failed, status = 0x%08X", status);
        UANT_APP_Data.ErrCounter++;
    }
    else
    {
        UANT_APP_Data.CmdCounter++;
    }

    return status;
}


CFE_Status_t UANT_APP_DeployAnt1(const UANT_APP_ISIS_DeployAnt1Cmd_t *Msg)
{
    CFE_Status_t status;

    status = ISIS_UANT_DeployAntenna1(Msg->Arg);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(UANT_APP_DEPLOY_ERR_EID, CFE_EVS_EventType_ERROR,
                                    "UANT: DeployAntennaX failed, status = 0x%08X", status);
        UANT_APP_Data.ErrCounter++;
    }
    else
    {
        UANT_APP_Data.CmdCounter++;
    }

    return status;
}

CFE_Status_t UANT_APP_DeployAnt2(const UANT_APP_ISIS_DeployAnt2Cmd_t *Msg)
{
    CFE_Status_t status;

    status = ISIS_UANT_DeployAntenna2(Msg->Arg);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(UANT_APP_DEPLOY_ERR_EID, CFE_EVS_EventType_ERROR,
                                    "UANT: DeployAntennaX failed, status = 0x%08X", status);
        UANT_APP_Data.ErrCounter++;
    }
    else
    {
        UANT_APP_Data.CmdCounter++;
    }

    return status;
}

CFE_Status_t UANT_APP_DeployAnt3(const UANT_APP_ISIS_DeployAnt3Cmd_t *Msg)
{
    CFE_Status_t status;

    status = ISIS_UANT_DeployAntenna3(Msg->Arg);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(UANT_APP_DEPLOY_ERR_EID, CFE_EVS_EventType_ERROR,
                                    "UANT: DeployAntennaX failed, status = 0x%08X", status);
        UANT_APP_Data.ErrCounter++;
    }
    else
    {
        UANT_APP_Data.CmdCounter++;
    }

    return status;
}

CFE_Status_t UANT_APP_DeployAnt4(const UANT_APP_ISIS_DeployAnt4Cmd_t *Msg)
{
    CFE_Status_t status;

    status = ISIS_UANT_DeployAntenna4(Msg->Arg);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(UANT_APP_DEPLOY_ERR_EID, CFE_EVS_EventType_ERROR,
                                    "UANT: DeployAntennaX failed, status = 0x%08X", status);
        UANT_APP_Data.ErrCounter++;
    }
    else
    {
        UANT_APP_Data.CmdCounter++;
    }

    return status;
}

CFE_Status_t UANT_APP_DeployAnt1_Override(const UANT_APP_ISIS_DeployAnt1OverrideCmd_t *Msg)
{
    CFE_Status_t status;

    status = ISIS_UANT_DeployAntenna1WithOverride(Msg->Arg);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(UANT_APP_DEPLOY_OVRD_ERR_EID, CFE_EVS_EventType_ERROR,
                                    "UANT: DeployAntennaXWithOverride failed, status = 0x%08X", status);
        UANT_APP_Data.ErrCounter++;
    }
    else
    {
        UANT_APP_Data.CmdCounter++;
    }

    return status;
}

CFE_Status_t UANT_APP_DeployAnt2_Override(const UANT_APP_ISIS_DeployAnt2OverrideCmd_t *Msg)
{
    CFE_Status_t status;

    status = ISIS_UANT_DeployAntenna2WithOverride(Msg->Arg);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(UANT_APP_DEPLOY_OVRD_ERR_EID, CFE_EVS_EventType_ERROR,
                                    "UANT: DeployAntennaXWithOverride failed, status = 0x%08X", status);
        UANT_APP_Data.ErrCounter++;
    }
    else
    {
        UANT_APP_Data.CmdCounter++;
    }

    return status;
}

CFE_Status_t UANT_APP_DeployAnt3_Override(const UANT_APP_ISIS_DeployAnt3OverrideCmd_t *Msg)
{
    CFE_Status_t status;

    status = ISIS_UANT_DeployAntenna3WithOverride(Msg->Arg);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(UANT_APP_DEPLOY_OVRD_ERR_EID, CFE_EVS_EventType_ERROR,
                                    "UANT: DeployAntennaXWithOverride failed, status = 0x%08X", status);
        UANT_APP_Data.ErrCounter++;
    }
    else
    {
        UANT_APP_Data.CmdCounter++;
    }

    return status;
}

CFE_Status_t UANT_APP_DeployAnt4_Override(const UANT_APP_ISIS_DeployAnt4OverrideCmd_t *Msg)
{
    CFE_Status_t status;

    status = ISIS_UANT_DeployAntenna4WithOverride(Msg->Arg);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(UANT_APP_DEPLOY_OVRD_ERR_EID, CFE_EVS_EventType_ERROR,
                                    "UANT: DeployAntennaXWithOverride failed, status = 0x%08X", status);
        UANT_APP_Data.ErrCounter++;
    }
    else
    {
        UANT_APP_Data.CmdCounter++;
    }

    return status;
}

CFE_Status_t UANT_APP_CancleDeployment(const UANT_APP_ISIS_CancelDeploymentActivationCmd_t *Msg)
{
    CFE_Status_t status;

    status = ISIS_UANT_CancelDeploymentSystemActivation();
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(UANT_APP_DEPLOY_CANCEL_ERR_EID, CFE_EVS_EventType_ERROR,
                        "UANT: CancelDeploymentSystemActivation failed, status = 0x%08X", status);
        UANT_APP_Data.ErrCounter++;
    }
    else
    {
        UANT_APP_Data.CmdCounter++;
    }

    return status;
}

CFE_Status_t UANT_APP_GetDeploymentStatus(const UANT_APP_ISIS_ReportDeploymentStatusCmd_t *Msg)
{
    CFE_Status_t status;

    uint16 deploy_status; //읽어온 payload를 저장할 저장소
    status=ISIS_UANT_ReportDeploymentStatus(&deploy_status); //저장하고자 할 변수의 주소를 넣으면 읽어와서 넣어줌
    
    
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(UANT_APP_GET_STATUS_ERR_EID, CFE_EVS_EventType_ERROR,
                        "UANT: Failed to get deployment status, status = 0x%08X", status);
        UANT_APP_Data.ErrCounter++;
    }
    else
    {
        UANT_APP_GET_DEPLOYMENT_STATUS_t tlm;
        CFE_MSG_Init(CFE_MSG_PTR(tlm.TelemetryHeader), CFE_SB_ValueToMsgId(UANT_APP_OP_TLM_MID), sizeof(tlm));
        tlm.Payload = deploy_status;
        CFE_SB_TimeStampMsg(CFE_MSG_PTR(tlm.TelemetryHeader));
        CFE_SB_TransmitMsg(CFE_MSG_PTR(tlm.TelemetryHeader), true);
        UANT_APP_Data.CmdCounter++;
        OS_printf("%x", deploy_status);
    }

    return status;
}

CFE_Status_t UANT_APP_MeasureAntSystemTemperature(const UANT_APP_ISIS_MeasureSystemTemperatureCmd_t *Msg)
{
    CFE_Status_t status;

    uint16 raw;
    status=ISIS_UANT_MeasureAntennaSystemTemperature(&raw);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(UANT_APP_MEASURE_TEMP_ERR_EID, CFE_EVS_EventType_ERROR,
                        "UANT: Failed to measure temperature, status = 0x%08X", status);
        UANT_APP_Data.ErrCounter++;
    }
    else
    {
        UANT_APP_MEASURE_SYSTEM_TEMPERATURE_t tlm;
        CFE_MSG_Init(CFE_MSG_PTR(tlm.TelemetryHeader), CFE_SB_ValueToMsgId(UANT_APP_OP_TLM_MID), sizeof(tlm));
        tlm.Payload = raw;
        CFE_SB_TimeStampMsg(CFE_MSG_PTR(tlm.TelemetryHeader));
        CFE_SB_TransmitMsg(CFE_MSG_PTR(tlm.TelemetryHeader), true);
        UANT_APP_Data.CmdCounter++;
        OS_printf("%x", raw);
    }

    return status;
}

CFE_Status_t UANT_APP_ReportAntActivationCnt(const UANT_APP_ISIS_ReportAntActivationCntCmd_t *Msg)
{
    CFE_Status_t status;

    uint8 count;     
    uint8 ant = Msg->Arg; 
    status=ISIS_UANT_ReportAntennaActivationCount(ant, &count);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(UANT_APP_GET_ACT_CNT_ERR_EID, CFE_EVS_EventType_ERROR,
                        "UANT: Failed to report activation count for ANT-%d, status = 0x%08X", ant, status);
        UANT_APP_Data.ErrCounter++;
    }
    else
    {
        UANT_APP_REPORT_ANT_ACTIVATION_CNT_t tlm;
        CFE_MSG_Init(CFE_MSG_PTR(tlm.TelemetryHeader), CFE_SB_ValueToMsgId(UANT_APP_OP_TLM_MID), sizeof(tlm));
        tlm.Payload = count;
        CFE_SB_TimeStampMsg(CFE_MSG_PTR(tlm.TelemetryHeader));
        CFE_SB_TransmitMsg(CFE_MSG_PTR(tlm.TelemetryHeader), true);
        UANT_APP_Data.CmdCounter++;
        OS_printf("%x", count);
    }


    return status;
}

CFE_Status_t UANT_APP_ReportAntActivationTime(const UANT_APP_ISIS_ReportAntActivationTimeCmd_t *Msg)
{
    CFE_Status_t status;

    uint16 time;
    uint8 ant = Msg->Arg; //받은명령에서 몇번 안테나를 원하는지 뽑아냄
    status=ISIS_UANT_ReportAntennaActivationTime(ant, &time); //해당 안테나 레지스터 읽기
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(UANT_APP_GET_ACT_TIME_ERR_EID, CFE_EVS_EventType_ERROR,
                        "UANT: Failed to report activation time for ANT-%d, status = 0x%08X", ant, status);
        UANT_APP_Data.ErrCounter++;
    }
    else
    {
        UANT_APP_REPORT_ANT_ACTIVATION_TIME_t tlm;
        CFE_MSG_Init(CFE_MSG_PTR(tlm.TelemetryHeader), CFE_SB_ValueToMsgId(UANT_APP_OP_TLM_MID), sizeof(tlm));
        tlm.Payload = time;
        CFE_SB_TimeStampMsg(CFE_MSG_PTR(tlm.TelemetryHeader));
        CFE_SB_TransmitMsg(CFE_MSG_PTR(tlm.TelemetryHeader), true);
        OS_printf("%x", time);

        UANT_APP_Data.CmdCounter++;
    }


    return status;
}