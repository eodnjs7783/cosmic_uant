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
 *   This file contains the source code for the Sample App.
 */

/*
** Include Files:
*/
#include "uant.h" 
#include "uant_app.h"
#include "uant_app_dispatch.h"
#include "uant_app_cmds.h"
#include "uant_app_eventids.h"
#include "uant_app_msgids.h"
#include "uant_app_msg.h"
#include "cfe_msg.h"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* Verify command packet length                                               */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
bool UANT_APP_VerifyCmdLength(const CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength)
{
    bool              result       = true;
    size_t            ActualLength = 0;
    CFE_SB_MsgId_t    MsgId        = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t FcnCode      = 0;

    CFE_MSG_GetSize(MsgPtr, &ActualLength);

    /*
    ** Verify the command packet length.
    */
    if (ExpectedLength != ActualLength)
    {
        CFE_MSG_GetMsgId(MsgPtr, &MsgId);
        CFE_MSG_GetFcnCode(MsgPtr, &FcnCode);

        CFE_EVS_SendEvent(UANT_APP_CMD_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Invalid Msg length: ID = 0x%X,  CC = %u, Len = %u, Expected = %u",
                          (unsigned int)CFE_SB_MsgIdToValue(MsgId), (unsigned int)FcnCode, (unsigned int)ActualLength,
                          (unsigned int)ExpectedLength);

        result = false;

        UANT_APP_Data.ErrCounter++;
    }

    return result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* UANT ground commands                                                     */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void UANT_APP_ProcessGroundCommand(const CFE_SB_Buffer_t *SBBufPtr)
{   CFE_Status_t     status; // 내부 판단용
    CFE_MSG_FcnCode_t CommandCode = 0;
    CFE_MSG_GetFcnCode(&SBBufPtr->Msg, &CommandCode);

    switch (CommandCode)
    {
        /* ───────── NOOP ───────── */
        case UANT_APP_NOOP_CC:
            if (UANT_APP_VerifyCmdLength(&SBBufPtr->Msg, sizeof(UANT_APP_NoopCmd_t))) //지상에서 보내준 것과 정의한 구조가 일치하는지 검증
            {
                UANT_APP_NoopCmd((const UANT_APP_NoopCmd_t *)SBBufPtr);
            }
            break;

        /* ───────── 카운터 리셋 ───────── */
        case UANT_APP_RESET_COUNTERS_CC:
            if (UANT_APP_VerifyCmdLength(&SBBufPtr->Msg, sizeof(UANT_APP_ResetCountersCmd_t)))
            {
                UANT_APP_ResetCountersCmd((const UANT_APP_ResetCountersCmd_t *)SBBufPtr);
            }
            break;

        /* ───────── 장치 리셋 ───────── */
        case UANT_APP_RESET_CC:
            if (UANT_APP_VerifyCmdLength(&SBBufPtr->Msg, sizeof(UANT_APP_ISIS_ResetCmd_t)))
            {
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
            }
            break;

        /* ───────── ARM / DISARM ───────── */
        case UANT_APP_ARM_ANTENNA_SYSTEMS_CC:
            if (UANT_APP_VerifyCmdLength(&SBBufPtr->Msg, sizeof(UANT_APP_ISIS_ArmAntennaSystemsCmd_t)))
            {
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
                
            }
            break;

        case UANT_APP_DISARM_CC:
            if (UANT_APP_VerifyCmdLength(&SBBufPtr->Msg, sizeof(UANT_APP_ISIS_DisarmCmd_t)))
            {
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
            }
            break;

        /* ───────── 자동 전개 ───────── */
        case UANT_APP_AUTOMATED_DEPLOYMENT_CC:
            if (UANT_APP_VerifyCmdLength(&SBBufPtr->Msg, sizeof(UANT_APP_ISIS_AutomatedDeploymentCmd_t)))
            {
                const UANT_APP_ISIS_AutomatedDeploymentCmd_t *cmd = (const UANT_APP_ISIS_AutomatedDeploymentCmd_t *)SBBufPtr;
                status = ISIS_UANT_AutomatedSequentialDeployment(cmd->Arg);
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
            }
            break;

        /* ───────── 단일 안테나 전개 ───────── */
        case UANT_APP_DEPLOY_ANT1_CC:
        case UANT_APP_DEPLOY_ANT2_CC:
        case UANT_APP_DEPLOY_ANT3_CC:
        case UANT_APP_DEPLOY_ANT4_CC:
            if (UANT_APP_VerifyCmdLength(&SBBufPtr->Msg, sizeof(UANT_APP_ISIS_DeployAnt1Cmd_t)))
            {
                const UANT_APP_ISIS_DeployAnt1Cmd_t *cmd = (const UANT_APP_ISIS_DeployAnt1Cmd_t *)SBBufPtr;
                switch (CommandCode)
                {
                    case UANT_APP_DEPLOY_ANT1_CC: status = ISIS_UANT_DeployAntenna1(cmd->Arg); break;
                    case UANT_APP_DEPLOY_ANT2_CC: status = ISIS_UANT_DeployAntenna2(cmd->Arg); break;
                    case UANT_APP_DEPLOY_ANT3_CC: status = ISIS_UANT_DeployAntenna3(cmd->Arg); break;
                    case UANT_APP_DEPLOY_ANT4_CC: status = ISIS_UANT_DeployAntenna4(cmd->Arg); break;
                }
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
            }
            break;

        /* ───────── Override 전개 ───────── */
        case UANT_APP_DEPLOY_ANT1_OVERRIDE_CC:
        case UANT_APP_DEPLOY_ANT2_OVERRIDE_CC:
        case UANT_APP_DEPLOY_ANT3_OVERRIDE_CC:
        case UANT_APP_DEPLOY_ANT4_OVERRIDE_CC:
            if (UANT_APP_VerifyCmdLength(&SBBufPtr->Msg, sizeof(UANT_APP_ISIS_DeployAnt1OverrideCmd_t)))
            {
                const UANT_APP_ISIS_DeployAnt1OverrideCmd_t *cmd = (const UANT_APP_ISIS_DeployAnt1OverrideCmd_t *)SBBufPtr; //MSG에 딸려오는 건 arg임 메시지 구조상 그렇다
                switch (CommandCode)
                {
                    case UANT_APP_DEPLOY_ANT1_OVERRIDE_CC:
                        status = ISIS_UANT_DeployAntenna1WithOverride(cmd->Arg);
                        break;
                    case UANT_APP_DEPLOY_ANT2_OVERRIDE_CC:
                        status = ISIS_UANT_DeployAntenna2WithOverride(cmd->Arg);
                        break;
                    case UANT_APP_DEPLOY_ANT3_OVERRIDE_CC:
                        status = ISIS_UANT_DeployAntenna3WithOverride(cmd->Arg);
                        break;
                    case UANT_APP_DEPLOY_ANT4_OVERRIDE_CC:
                        status = ISIS_UANT_DeployAntenna4WithOverride(cmd->Arg);
                        break;
                }

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
            }
            break;

        /* ───────── 전개 취소 ───────── */
        case UANT_APP_CANCEL_DEPLOYMENT_ACTIVATION_CC:
            if (UANT_APP_VerifyCmdLength(&SBBufPtr->Msg, sizeof(UANT_APP_ISIS_CancelDeploymentActivationCmd_t)))
            {
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
                    }
                    break;

        /* ───────── 상태 조회 ───────── */
        case UANT_APP_GET_DEPLOYMENT_STATUS_CC:
            if (UANT_APP_VerifyCmdLength(&SBBufPtr->Msg, sizeof(UANT_APP_ISIS_ReportDeploymentStatusCmd_t)))
            {
                uint16 deploy_status; //읽어온 payload를 저장할 저장
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
                }
            }
            break;

        /* ───────── 온도 조회 ───────── */
        case UANT_APP_MEASURE_SYSTEM_TEMPERATURE_CC:
            if (UANT_APP_VerifyCmdLength(&SBBufPtr->Msg, sizeof(UANT_APP_ISIS_MeasureSystemTemperatureCmd_t)))
            {
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
                }
            }
            break;

        /* ───────── 활성화 횟수 보고 ───────── */
        case UANT_APP_REPORT_ANT_ACTIVATION_CNT_CC:
            if (UANT_APP_VerifyCmdLength(&SBBufPtr->Msg, sizeof(UANT_APP_ISIS_ReportAntActivationCntCmd_t)))
            {
                uint8 count; //buffer 즉 일정크기의 임시저장소
                
                uint8 ant = ((UANT_APP_ISIS_ReportAntActivationCntCmd_t *)SBBufPtr)->Arg; //arg가 딸려서 온 텔레메트리 요구 명령
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
                }
            }
            break;

        /* ───────── 활성화 시간 보고 ───────── */
        case UANT_APP_REPORT_ANT_ACTIVATION_TIME_CC:
            if (UANT_APP_VerifyCmdLength(&SBBufPtr->Msg, sizeof(UANT_APP_ISIS_ReportAntActivationTimeCmd_t)))
            {
                uint16 time;
                uint8 ant = ((UANT_APP_ISIS_ReportAntActivationTimeCmd_t *)SBBufPtr)->Arg; //받은명령에서 몇번 안테나를 원하는지 뽑아냄
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

                    UANT_APP_Data.CmdCounter++;
                }
            }
            break;

        default:
            CFE_EVS_SendEvent(UANT_APP_CC_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Invalid ground command code: CC = %d", CommandCode);
            break;
    }

   
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/*  Purpose:                                                                  */
/*     This routine will process any packet that is received on the UANT    */
/*     command pipe.                                                          */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void UANT_APP_TaskPipe(const CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;

    CFE_MSG_GetMsgId(&SBBufPtr->Msg, &MsgId);

    switch (CFE_SB_MsgIdToValue(MsgId))
    {
        case UANT_APP_CMD_MID:
            UANT_APP_ProcessGroundCommand(SBBufPtr);
            break;
        //sch 추가?
        case UANT_APP_SEND_HK_MID:
            UANT_APP_SendHkCmd((const UANT_APP_SendHkCmd_t *)SBBufPtr);
            break;

        default:
            CFE_EVS_SendEvent(UANT_APP_MID_ERR_EID, CFE_EVS_EventType_ERROR,
                              "UANT: invalid command packet,MID = 0x%x", (unsigned int)CFE_SB_MsgIdToValue(MsgId));
            break;
    }
}
