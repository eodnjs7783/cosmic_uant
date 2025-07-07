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
