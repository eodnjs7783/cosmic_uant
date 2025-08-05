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
 * @file
 *   UANT_APP Application Public Definitions
 *
 * This provides default values for configurable items that affect
 * the interface(s) of this module.  This includes the CMD/TLM message
 * interface, tables definitions, and any other data products that
 * serve to exchange information with other entities. 외부와도 공유되는 값
 *
 * @note This file may be overridden/superceded by mission-provided defintions
 * either by overriding this header or by generating definitions from a command/data
 * dictionary tool.
 */

#include "common_types.h"
#include "cfe.h"
#ifndef UANT_APP_INTERFACE_CFG_H
#define UANT_APP_INTERFACE_CFG_H

/**
 * \brief Length of string buffer in the Display Value command
 *
 * The Display Value command offers an example of how to use command
 * parameters of different types.  This macro controls the length
 * of the string parameter.
 */
#define UANT_APP_STRING_VAL_LEN 10

/*
#define MAX_TM_DATASIZE     170
#define MAX_RAW_DATASIZE    200
#define MAX_TC_DATASIZE     170
*/

#define RPT_REPORT_QUEUE_LEN    10
#define RPT_CRITICAL_QUEUE_LEN  40

/**
 * Report Buffer Size
 */
#define RPT_RET_VALUE_BUF_SIZE  60


/**
 * RPT Subscribe Table Entry size
 */
#define RPT_MAX_TBL_ENTRY       30

typedef struct RPT_Report {

    uint16 MsgID;
    uint8 CommandCode;

    /**
     * `enum RPT_ReturnType_t`
     */
    uint8 ReturnType;
    int32 ReturnCode;

    /**
     * If cmd acquires **any kind of data**, use this member.
     * Ex 1) Get CMD -> earned values 
     *      (In this case, `ReturnType` must be `RPT_RETTYPE_SUCCESS`)
     * 
     * Ex 2) HW's Error packet -> that error values
     *      (In this case, `ReturnType` must be `RPT_RETTYPE_SUCCESS`)
     * 
     * Ex 3) Partially readed packet -> segmented packet
     *      (In this case, `ReturnType` must be `RPT_RETTYPE_CFE`
     *       and `ReturnCode` must be `CFE_SRL_PARTIAL_READ_ERR`)
     */
    uint16 ReturnDataSize;
    uint8 ReturnValue[RPT_RET_VALUE_BUF_SIZE];

} RPT_Report_t;



typedef struct RPT_Critical {

    RPT_Report_t Report;
    CFE_TIME_SysTime_t Time;

} RPT_Critical_t;


/*****************************************************
 * Return Type definition
 * Should matched with return function
 * If RETTYPE is CFE core error, `RPT_RETTYPE_CFE`
 * If RETTYPE is App error, `RPT_RETTYPE_APP`
 ****************************************************/
typedef enum {

    RPT_RETTYPE_SUCCESS,

    /**
     * `osapi-error.h`
     */
    RPT_RETTYPE_OSAL,

    /**
     * `cfe_error.h`
     */
    RPT_RETTYPE_CFE,

    /**
     * App layer library
     */
    RPT_RETTYPE_LIB,

    /**
     * App layer Application
     * Also `cfe_error.h`
     */
    RPT_RETTYPE_APP,

    /**
     * H/W error packet case
     * This means the **serial communication is successfully (or partially) done**
     */
    RPT_RETTYPE_HW,

} RPT_ReturnType_t;

#endif
