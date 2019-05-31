/******************************************************************************
 *
 *  Copyright (C) 2004-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/******************************************************************************
 *
 *  This is the stream state machine for the BTA advanced audio/video.
 *
 ******************************************************************************/

#include "bt_target.h"
#if defined(BTA_AV_INCLUDED) && (BTA_AV_INCLUDED == TRUE)

#include <string.h>
#include "bta_avk_co.h"
#include "bta_avk_int.h"

/*****************************************************************************
** Constants and types
*****************************************************************************/

/* state machine states */
enum
{
    BTA_AVK_INIT_SST,
    BTA_AVK_INCOMING_SST,
    BTA_AVK_OPENING_SST,
    BTA_AVK_OPEN_SST,
    BTA_AVK_RCFG_SST,
    BTA_AVK_CLOSING_SST
};


/* state machine action enumeration list */
enum
{
    BTA_AVK_DO_DISC,
    BTA_AVK_CLEANUP,
    BTA_AVK_FREE_SDB,
    BTA_AVK_CONFIG_IND,
    BTA_AVK_DISCONNECT_REQ,
    BTA_AVK_SECURITY_REQ,
    BTA_AVK_SECURITY_RSP,
    BTA_AVK_SETCONFIG_RSP,
    BTA_AVK_ST_RC_TIMER,
    BTA_AVK_STR_OPENED,
    BTA_AVK_SECURITY_IND,
    BTA_AVK_SECURITY_CFM,
    BTA_AVK_DO_CLOSE,
    BTA_AVK_CONNECT_REQ,
    BTA_AVK_SDP_FAILED,
    BTA_AVK_DISC_RESULTS,
    BTA_AVK_DISC_RES_AS_ACP,
    BTA_AVK_OPEN_FAILED,
    BTA_AVK_GETCAP_RESULTS,
    BTA_AVK_SETCONFIG_REJ,
    BTA_AVK_DISCOVER_REQ,
    BTA_AVK_CONN_FAILED,
    BTA_AVK_DO_START,
    BTA_AVK_STR_STOPPED,
    BTA_AVK_RECONFIG,
    BTA_AVK_DATA_PATH,
    BTA_AVK_START_OK,
    BTA_AVK_START_FAILED,
    BTA_AVK_STR_CLOSED,
    BTA_AVK_CLR_CONG,
    BTA_AVK_SUSPEND_CFM,
    BTA_AVK_RCFG_STR_OK,
    BTA_AVK_RCFG_FAILED,
    BTA_AVK_RCFG_CONNECT,
    BTA_AVK_RCFG_DISCNTD,
    BTA_AVK_SUSPEND_CONT,
    BTA_AVK_RCFG_CFM,
    BTA_AVK_RCFG_OPEN,
    BTA_AVK_SECURITY_REJ,
    BTA_AVK_OPEN_RC,
    BTA_AVK_CHK_2ND_START,
    BTA_AVK_SAVE_CAPS,
    BTA_AVK_SET_USE_RC,
    BTA_AVK_CCO_CLOSE,
    BTA_AVK_SWITCH_ROLE,
    BTA_AVK_ROLE_RES,
    BTA_AVK_DELAY_CO,
    BTA_AVK_OPEN_AT_INC,
    BTA_AVK_NUM_SACTIONS
};

#define BTA_AVK_SIGNORE       BTA_AVK_NUM_SACTIONS


/* state table information */
/* #define BTA_AVK_SACTION_COL           0       position of actions */
#define BTA_AVK_SACTIONS              2       /* number of actions */
#define BTA_AVK_SNEXT_STATE           2       /* position of next state */
#define BTA_AVK_NUM_COLS              3       /* number of columns in state tables */

/* state table for init state */
static const UINT8 bta_avk_sst_init[][BTA_AVK_NUM_COLS] =
{
/* Event                     Action 1               Action 2               Next state */
/* AP_OPEN_EVT */           {BTA_AVK_DO_DISC,        BTA_AVK_SIGNORE,        BTA_AVK_OPENING_SST },
/* AP_CLOSE_EVT */          {BTA_AVK_CLEANUP,        BTA_AVK_SIGNORE,        BTA_AVK_INIT_SST },
/* AP_START_EVT */          {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_INIT_SST },
/* AP_STOP_EVT */           {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_INIT_SST },
/* API_RECONFIG_EVT */      {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_INIT_SST },
/* API_PROTECT_REQ_EVT */   {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_INIT_SST },
/* API_PROTECT_RSP_EVT */   {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_INIT_SST },
/* API_RC_OPEN_EVT  */      {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_INIT_SST },
/* SRC_DATA_READY_EVT */    {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_INIT_SST },
/* CI_SETCONFIG_OK_EVT */   {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_INIT_SST },
/* CI_SETCONFIG_FAIL_EVT */ {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_INIT_SST },
/* SDP_DISC_OK_EVT */       {BTA_AVK_FREE_SDB,       BTA_AVK_SIGNORE,        BTA_AVK_INIT_SST },
/* SDP_DISC_FAIL_EVT */     {BTA_AVK_FREE_SDB,       BTA_AVK_SIGNORE,        BTA_AVK_INIT_SST },
/* STR_DISC_OK_EVT */       {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_INIT_SST },
/* STR_DISC_FAIL_EVT */     {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_INIT_SST },
/* STR_GETCAP_OK_EVT */     {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_INIT_SST },
/* STR_GETCAP_FAIL_EVT */   {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_INIT_SST },
/* STR_OPEN_OK_EVT */       {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_INIT_SST },
/* STR_OPEN_FAIL_EVT */     {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_INIT_SST },
/* STR_START_OK_EVT */      {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_INIT_SST },
/* STR_START_FAIL_EVT */    {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_INIT_SST },
/* STR_CLOSE_EVT */         {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_INIT_SST },
/* STR_CONFIG_IND_EVT */    {BTA_AVK_SETCONFIG_REJ,  BTA_AVK_SIGNORE,        BTA_AVK_INIT_SST },
/* STR_SECURITY_IND_EVT */  {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_INIT_SST },
/* STR_SECURITY_CFM_EVT */  {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_INIT_SST },
/* STR_WRITE_CFM_EVT */     {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_INIT_SST },
/* STR_SUSPEND_CFM_EVT */   {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_INIT_SST },
/* STR_RECONFIG_CFM_EVT */  {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_INIT_SST },
/* AVRC_TIMER_EVT */        {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_INIT_SST },
/* AVDT_CONNECT_EVT */      {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_INIT_SST },
/* AVDT_DISCONNECT_EVT */   {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_INIT_SST },
/* ROLE_CHANGE_EVT*/        {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_INIT_SST },
/* AVDT_DELAY_RPT_EVT */    {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_INIT_SST },
/* ACP_CONNECT_EVT */       {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_INCOMING_SST }
};

/* state table for incoming state */
static const UINT8 bta_avk_sst_incoming[][BTA_AVK_NUM_COLS] =
{
/* Event                     Action 1               Action 2               Next state */
/* AP_OPEN_EVT */           {BTA_AVK_OPEN_AT_INC,    BTA_AVK_SIGNORE,        BTA_AVK_INCOMING_SST },
/* AP_CLOSE_EVT */          {BTA_AVK_CCO_CLOSE,      BTA_AVK_DISCONNECT_REQ, BTA_AVK_CLOSING_SST },
/* AP_START_EVT */          {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_INCOMING_SST },
/* AP_STOP_EVT */           {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_INCOMING_SST },
/* API_RECONFIG_EVT */      {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_INCOMING_SST },
/* API_PROTECT_REQ_EVT */   {BTA_AVK_SECURITY_REQ,   BTA_AVK_SIGNORE,        BTA_AVK_INCOMING_SST },
/* API_PROTECT_RSP_EVT */   {BTA_AVK_SECURITY_RSP,   BTA_AVK_SIGNORE,        BTA_AVK_INCOMING_SST },
/* API_RC_OPEN_EVT  */      {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_INCOMING_SST },
/* SRC_DATA_READY_EVT */    {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_INCOMING_SST },
/* CI_SETCONFIG_OK_EVT */   {BTA_AVK_SETCONFIG_RSP,  BTA_AVK_ST_RC_TIMER,    BTA_AVK_INCOMING_SST },
/* CI_SETCONFIG_FAIL_EVT */ {BTA_AVK_SETCONFIG_REJ,  BTA_AVK_CLEANUP,        BTA_AVK_INIT_SST },
/* SDP_DISC_OK_EVT */       {BTA_AVK_FREE_SDB,       BTA_AVK_SIGNORE,        BTA_AVK_INCOMING_SST },
/* SDP_DISC_FAIL_EVT */     {BTA_AVK_FREE_SDB,       BTA_AVK_SIGNORE,        BTA_AVK_INCOMING_SST },
/* STR_DISC_OK_EVT */       {BTA_AVK_DISC_RES_AS_ACP,BTA_AVK_SIGNORE,        BTA_AVK_INCOMING_SST },
/* STR_DISC_FAIL_EVT */     {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_INCOMING_SST },
/* STR_GETCAP_OK_EVT */     {BTA_AVK_SAVE_CAPS,      BTA_AVK_SIGNORE,        BTA_AVK_INCOMING_SST },
/* STR_GETCAP_FAIL_EVT */   {BTA_AVK_OPEN_FAILED,    BTA_AVK_SIGNORE,        BTA_AVK_CLOSING_SST },
/* STR_OPEN_OK_EVT */       {BTA_AVK_STR_OPENED,     BTA_AVK_SIGNORE,        BTA_AVK_OPEN_SST },
/* STR_OPEN_FAIL_EVT */     {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_INCOMING_SST },
/* STR_START_OK_EVT */      {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_INCOMING_SST },
/* STR_START_FAIL_EVT */    {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_INCOMING_SST },
/* STR_CLOSE_EVT */         {BTA_AVK_CCO_CLOSE,      BTA_AVK_CLEANUP,        BTA_AVK_INIT_SST },
/* STR_CONFIG_IND_EVT */    {BTA_AVK_CONFIG_IND,     BTA_AVK_SIGNORE,        BTA_AVK_INCOMING_SST },
/* STR_SECURITY_IND_EVT */  {BTA_AVK_SECURITY_IND,   BTA_AVK_SIGNORE,        BTA_AVK_INCOMING_SST },
/* STR_SECURITY_CFM_EVT */  {BTA_AVK_SECURITY_CFM,   BTA_AVK_SIGNORE,        BTA_AVK_INCOMING_SST },
/* STR_WRITE_CFM_EVT */     {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_INCOMING_SST },
/* STR_SUSPEND_CFM_EVT */   {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_INCOMING_SST },
/* STR_RECONFIG_CFM_EVT */  {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_INCOMING_SST },
/* AVRC_TIMER_EVT */        {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_INCOMING_SST },
/* AVDT_CONNECT_EVT */      {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_INCOMING_SST },
/* AVDT_DISCONNECT_EVT */   {BTA_AVK_CCO_CLOSE,      BTA_AVK_DISCONNECT_REQ, BTA_AVK_CLOSING_SST },
/* ROLE_CHANGE_EVT*/        {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_INCOMING_SST },
/* AVDT_DELAY_RPT_EVT */    {BTA_AVK_DELAY_CO,       BTA_AVK_SIGNORE,        BTA_AVK_INCOMING_SST },
/* ACP_CONNECT_EVT */       {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_INCOMING_SST }
};

/* state table for opening state */
static const UINT8 bta_avk_sst_opening[][BTA_AVK_NUM_COLS] =
{
/* Event                     Action 1               Action 2               Next state */
/* AP_OPEN_EVT */           {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_OPENING_SST },
/* AP_CLOSE_EVT */          {BTA_AVK_DO_CLOSE,       BTA_AVK_SIGNORE,        BTA_AVK_CLOSING_SST },
/* AP_START_EVT */          {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_OPENING_SST },
/* AP_STOP_EVT */           {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_OPENING_SST },
/* API_RECONFIG_EVT */      {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_OPENING_SST },
/* API_PROTECT_REQ_EVT */   {BTA_AVK_SECURITY_REQ,   BTA_AVK_SIGNORE,        BTA_AVK_OPENING_SST },
/* API_PROTECT_RSP_EVT */   {BTA_AVK_SECURITY_RSP,   BTA_AVK_SIGNORE,        BTA_AVK_OPENING_SST },
/* API_RC_OPEN_EVT  */      {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_OPENING_SST },
/* SRC_DATA_READY_EVT */    {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_OPENING_SST },
/* CI_SETCONFIG_OK_EVT */   {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_OPENING_SST },
/* CI_SETCONFIG_FAIL_EVT */ {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_OPENING_SST },
/* SDP_DISC_OK_EVT */       {BTA_AVK_CONNECT_REQ,    BTA_AVK_SIGNORE,        BTA_AVK_OPENING_SST },
/* SDP_DISC_FAIL_EVT */     {BTA_AVK_SDP_FAILED,    BTA_AVK_SIGNORE,        BTA_AVK_INIT_SST },
/* STR_DISC_OK_EVT */       {BTA_AVK_DISC_RESULTS,   BTA_AVK_SIGNORE,        BTA_AVK_OPENING_SST },
/* STR_DISC_FAIL_EVT */     {BTA_AVK_OPEN_FAILED,    BTA_AVK_SIGNORE,        BTA_AVK_CLOSING_SST },
/* STR_GETCAP_OK_EVT */     {BTA_AVK_GETCAP_RESULTS, BTA_AVK_SIGNORE,        BTA_AVK_OPENING_SST },
/* STR_GETCAP_FAIL_EVT */   {BTA_AVK_OPEN_FAILED,    BTA_AVK_SIGNORE,        BTA_AVK_CLOSING_SST },
/* STR_OPEN_OK_EVT */       {BTA_AVK_ST_RC_TIMER,    BTA_AVK_STR_OPENED,     BTA_AVK_OPEN_SST },
/* STR_OPEN_FAIL_EVT */     {BTA_AVK_OPEN_FAILED,    BTA_AVK_SIGNORE,        BTA_AVK_CLOSING_SST },
/* STR_START_OK_EVT */      {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_OPENING_SST },
/* STR_START_FAIL_EVT */    {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_OPENING_SST },
/* STR_CLOSE_EVT */         {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_OPENING_SST },
/* STR_CONFIG_IND_EVT */    {BTA_AVK_CONFIG_IND,     BTA_AVK_SIGNORE,        BTA_AVK_INCOMING_SST },
/* STR_SECURITY_IND_EVT */  {BTA_AVK_SECURITY_IND,   BTA_AVK_SIGNORE,        BTA_AVK_OPENING_SST },
/* STR_SECURITY_CFM_EVT */  {BTA_AVK_SECURITY_CFM,   BTA_AVK_SIGNORE,        BTA_AVK_OPENING_SST },
/* STR_WRITE_CFM_EVT */     {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_OPENING_SST },
/* STR_SUSPEND_CFM_EVT */   {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_OPENING_SST },
/* STR_RECONFIG_CFM_EVT */  {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_OPENING_SST },
/* AVRC_TIMER_EVT */        {BTA_AVK_SWITCH_ROLE,    BTA_AVK_SIGNORE,        BTA_AVK_OPENING_SST },
/* AVDT_CONNECT_EVT */      {BTA_AVK_DISCOVER_REQ,   BTA_AVK_SIGNORE,        BTA_AVK_OPENING_SST },
/* AVDT_DISCONNECT_EVT */   {BTA_AVK_CONN_FAILED,    BTA_AVK_SIGNORE,        BTA_AVK_INIT_SST },
/* ROLE_CHANGE_EVT*/        {BTA_AVK_ROLE_RES,       BTA_AVK_SIGNORE,        BTA_AVK_OPENING_SST },
/* AVDT_DELAY_RPT_EVT */    {BTA_AVK_DELAY_CO,       BTA_AVK_SIGNORE,        BTA_AVK_OPENING_SST },
/* ACP_CONNECT_EVT */       {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_OPENING_SST }
};

/* state table for open state */
static const UINT8 bta_avk_sst_open[][BTA_AVK_NUM_COLS] =
{
/* Event                     Action 1               Action 2               Next state */
/* AP_OPEN_EVT */           {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_OPEN_SST },
/* AP_CLOSE_EVT */          {BTA_AVK_DO_CLOSE,       BTA_AVK_SIGNORE,        BTA_AVK_CLOSING_SST },
/* AP_START_EVT */          {BTA_AVK_DO_START,       BTA_AVK_SIGNORE,        BTA_AVK_OPEN_SST },
/* AP_STOP_EVT */           {BTA_AVK_STR_STOPPED,    BTA_AVK_SIGNORE,        BTA_AVK_OPEN_SST },
/* API_RECONFIG_EVT */      {BTA_AVK_RECONFIG,       BTA_AVK_SIGNORE,        BTA_AVK_RCFG_SST },
/* API_PROTECT_REQ_EVT */   {BTA_AVK_SECURITY_REQ,   BTA_AVK_SIGNORE,        BTA_AVK_OPEN_SST },
/* API_PROTECT_RSP_EVT */   {BTA_AVK_SECURITY_RSP,   BTA_AVK_SIGNORE,        BTA_AVK_OPEN_SST },
/* API_RC_OPEN_EVT  */      {BTA_AVK_SET_USE_RC,     BTA_AVK_OPEN_RC,        BTA_AVK_OPEN_SST },
/* SRC_DATA_READY_EVT */    {BTA_AVK_DATA_PATH,      BTA_AVK_SIGNORE,        BTA_AVK_OPEN_SST },
/* CI_SETCONFIG_OK_EVT */   {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_OPEN_SST },
/* CI_SETCONFIG_FAIL_EVT */ {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_OPEN_SST },
/* SDP_DISC_OK_EVT */       {BTA_AVK_FREE_SDB,       BTA_AVK_SIGNORE,        BTA_AVK_OPEN_SST },
/* SDP_DISC_FAIL_EVT */     {BTA_AVK_FREE_SDB,       BTA_AVK_SIGNORE,        BTA_AVK_OPEN_SST },
/* STR_DISC_OK_EVT */       {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_OPEN_SST },
/* STR_DISC_FAIL_EVT */     {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_OPEN_SST },
/* STR_GETCAP_OK_EVT */     {BTA_AVK_SAVE_CAPS,      BTA_AVK_SIGNORE,        BTA_AVK_OPEN_SST },
/* STR_GETCAP_FAIL_EVT */   {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_OPEN_SST },
/* STR_OPEN_OK_EVT */       {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_OPEN_SST },
/* STR_OPEN_FAIL_EVT */     {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_OPEN_SST },
/* STR_START_OK_EVT */      {BTA_AVK_START_OK,       BTA_AVK_SIGNORE,        BTA_AVK_OPEN_SST },
/* STR_START_FAIL_EVT */    {BTA_AVK_START_FAILED,   BTA_AVK_SIGNORE,        BTA_AVK_OPEN_SST },
/* STR_CLOSE_EVT */         {BTA_AVK_STR_CLOSED,     BTA_AVK_SIGNORE,        BTA_AVK_INIT_SST },
/* STR_CONFIG_IND_EVT */    {BTA_AVK_SETCONFIG_REJ,  BTA_AVK_SIGNORE,        BTA_AVK_OPEN_SST },
/* STR_SECURITY_IND_EVT */  {BTA_AVK_SECURITY_IND,   BTA_AVK_SIGNORE,        BTA_AVK_OPEN_SST },
/* STR_SECURITY_CFM_EVT */  {BTA_AVK_SECURITY_CFM,   BTA_AVK_SIGNORE,        BTA_AVK_OPEN_SST },
/* STR_WRITE_CFM_EVT */     {BTA_AVK_CLR_CONG,       BTA_AVK_DATA_PATH,      BTA_AVK_OPEN_SST },
/* STR_SUSPEND_CFM_EVT */   {BTA_AVK_SUSPEND_CFM,    BTA_AVK_SIGNORE,        BTA_AVK_OPEN_SST },
/* STR_RECONFIG_CFM_EVT */  {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_OPEN_SST },
/* AVRC_TIMER_EVT */        {BTA_AVK_OPEN_RC,        BTA_AVK_CHK_2ND_START,  BTA_AVK_OPEN_SST },
/* AVDT_CONNECT_EVT */      {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_OPEN_SST },
/* AVDT_DISCONNECT_EVT */   {BTA_AVK_STR_CLOSED,     BTA_AVK_SIGNORE,        BTA_AVK_INIT_SST },
/* ROLE_CHANGE_EVT*/        {BTA_AVK_ROLE_RES,       BTA_AVK_SIGNORE,        BTA_AVK_OPEN_SST },
/* AVDT_DELAY_RPT_EVT */    {BTA_AVK_DELAY_CO,       BTA_AVK_SIGNORE,        BTA_AVK_OPEN_SST },
/* ACP_CONNECT_EVT */       {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_OPEN_SST }
};

/* state table for reconfig state */
static const UINT8 bta_avk_sst_rcfg[][BTA_AVK_NUM_COLS] =
{
/* Event                     Action 1               Action 2               Next state */
/* AP_OPEN_EVT */           {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_RCFG_SST },
/* AP_CLOSE_EVT */          {BTA_AVK_DISCONNECT_REQ, BTA_AVK_SIGNORE,        BTA_AVK_CLOSING_SST },
/* AP_START_EVT */          {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_RCFG_SST },
/* AP_STOP_EVT */           {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_RCFG_SST },
/* API_RECONFIG_EVT */      {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_RCFG_SST },
/* API_PROTECT_REQ_EVT */   {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_RCFG_SST },
/* API_PROTECT_RSP_EVT */   {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_RCFG_SST },
/* API_RC_OPEN_EVT  */      {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_RCFG_SST },
/* SRC_DATA_READY_EVT */    {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_RCFG_SST },
/* CI_SETCONFIG_OK_EVT */   {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_RCFG_SST },
/* CI_SETCONFIG_FAIL_EVT */ {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_RCFG_SST },
/* SDP_DISC_OK_EVT */       {BTA_AVK_FREE_SDB,       BTA_AVK_SIGNORE,        BTA_AVK_RCFG_SST },
/* SDP_DISC_FAIL_EVT */     {BTA_AVK_FREE_SDB,       BTA_AVK_SIGNORE,        BTA_AVK_RCFG_SST },
/* STR_DISC_OK_EVT */       {BTA_AVK_DISC_RESULTS,   BTA_AVK_SIGNORE,        BTA_AVK_RCFG_SST },
/* STR_DISC_FAIL_EVT */     {BTA_AVK_STR_CLOSED,     BTA_AVK_SIGNORE,        BTA_AVK_INIT_SST },
/* STR_GETCAP_OK_EVT */     {BTA_AVK_GETCAP_RESULTS, BTA_AVK_SIGNORE,        BTA_AVK_RCFG_SST },
/* STR_GETCAP_FAIL_EVT */   {BTA_AVK_STR_CLOSED,     BTA_AVK_SIGNORE,        BTA_AVK_INIT_SST },
/* STR_OPEN_OK_EVT */       {BTA_AVK_RCFG_STR_OK,    BTA_AVK_SIGNORE,        BTA_AVK_OPEN_SST },
/* STR_OPEN_FAIL_EVT */     {BTA_AVK_RCFG_FAILED,    BTA_AVK_SIGNORE,        BTA_AVK_RCFG_SST },
/* STR_START_OK_EVT */      {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_RCFG_SST },
/* STR_START_FAIL_EVT */    {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_RCFG_SST },
/* STR_CLOSE_EVT */         {BTA_AVK_RCFG_CONNECT,   BTA_AVK_SIGNORE,        BTA_AVK_RCFG_SST },
/* STR_CONFIG_IND_EVT */    {BTA_AVK_SETCONFIG_REJ,  BTA_AVK_SIGNORE,        BTA_AVK_RCFG_SST },
/* STR_SECURITY_IND_EVT */  {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_RCFG_SST },
/* STR_SECURITY_CFM_EVT */  {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_RCFG_SST },
/* STR_WRITE_CFM_EVT */     {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_RCFG_SST },
/* STR_SUSPEND_CFM_EVT */   {BTA_AVK_SUSPEND_CONT,   BTA_AVK_SIGNORE,        BTA_AVK_RCFG_SST },
/* STR_RECONFIG_CFM_EVT */  {BTA_AVK_RCFG_CFM,       BTA_AVK_SIGNORE,        BTA_AVK_RCFG_SST },
/* AVRC_TIMER_EVT */        {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_RCFG_SST },
/* AVDT_CONNECT_EVT */      {BTA_AVK_RCFG_OPEN,      BTA_AVK_SIGNORE,        BTA_AVK_RCFG_SST },
/* AVDT_DISCONNECT_EVT */   {BTA_AVK_RCFG_DISCNTD,   BTA_AVK_SIGNORE,        BTA_AVK_RCFG_SST },
/* ROLE_CHANGE_EVT*/        {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_RCFG_SST },
/* AVDT_DELAY_RPT_EVT */    {BTA_AVK_DELAY_CO,       BTA_AVK_SIGNORE,        BTA_AVK_RCFG_SST },
/* ACP_CONNECT_EVT */       {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_RCFG_SST }
};

/* state table for closing state */
static const UINT8 bta_avk_sst_closing[][BTA_AVK_NUM_COLS] =
{
/* Event                     Action 1               Action 2               Next state */
/* AP_OPEN_EVT */           {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_CLOSING_SST },
/* AP_CLOSE_EVT */          {BTA_AVK_DISCONNECT_REQ, BTA_AVK_SIGNORE,        BTA_AVK_CLOSING_SST },
/* AP_START_EVT */          {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_CLOSING_SST },
/* AP_STOP_EVT */           {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_CLOSING_SST },
/* API_RECONFIG_EVT */      {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_CLOSING_SST },
/* API_PROTECT_REQ_EVT */   {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_CLOSING_SST },
/* API_PROTECT_RSP_EVT */   {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_CLOSING_SST },
/* API_RC_OPEN_EVT  */      {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_CLOSING_SST },
/* SRC_DATA_READY_EVT */    {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_CLOSING_SST },
/* CI_SETCONFIG_OK_EVT */   {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_CLOSING_SST },
/* CI_SETCONFIG_FAIL_EVT */ {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_CLOSING_SST },
/* SDP_DISC_OK_EVT */       {BTA_AVK_SDP_FAILED,     BTA_AVK_SIGNORE,        BTA_AVK_INIT_SST },
/* SDP_DISC_FAIL_EVT */     {BTA_AVK_SDP_FAILED,     BTA_AVK_SIGNORE,        BTA_AVK_INIT_SST },
/* STR_DISC_OK_EVT */       {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_CLOSING_SST },
/* STR_DISC_FAIL_EVT */     {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_CLOSING_SST },
/* STR_GETCAP_OK_EVT */     {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_CLOSING_SST },
/* STR_GETCAP_FAIL_EVT */   {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_CLOSING_SST },
/* STR_OPEN_OK_EVT */       {BTA_AVK_DO_CLOSE,       BTA_AVK_SIGNORE,        BTA_AVK_CLOSING_SST },
/* STR_OPEN_FAIL_EVT */     {BTA_AVK_DISCONNECT_REQ, BTA_AVK_SIGNORE,        BTA_AVK_CLOSING_SST },
/* STR_START_OK_EVT */      {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_CLOSING_SST },
/* STR_START_FAIL_EVT */    {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_CLOSING_SST },
/* STR_CLOSE_EVT */         {BTA_AVK_DISCONNECT_REQ, BTA_AVK_SIGNORE,        BTA_AVK_CLOSING_SST },
/* STR_CONFIG_IND_EVT */    {BTA_AVK_SETCONFIG_REJ,  BTA_AVK_SIGNORE,        BTA_AVK_CLOSING_SST },
/* STR_SECURITY_IND_EVT */  {BTA_AVK_SECURITY_REJ,   BTA_AVK_SIGNORE,        BTA_AVK_CLOSING_SST },
/* STR_SECURITY_CFM_EVT */  {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_CLOSING_SST },
/* STR_WRITE_CFM_EVT */     {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_CLOSING_SST },
/* STR_SUSPEND_CFM_EVT */   {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_CLOSING_SST },
/* STR_RECONFIG_CFM_EVT */  {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_CLOSING_SST },
/* AVRC_TIMER_EVT */        {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_CLOSING_SST },
/* AVDT_CONNECT_EVT */      {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_CLOSING_SST },
/* AVDT_DISCONNECT_EVT */   {BTA_AVK_STR_CLOSED,     BTA_AVK_SIGNORE,        BTA_AVK_INIT_SST },
/* ROLE_CHANGE_EVT*/        {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_CLOSING_SST },
/* AVDT_DELAY_RPT_EVT */    {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_CLOSING_SST },
/* ACP_CONNECT_EVT */       {BTA_AVK_SIGNORE,        BTA_AVK_SIGNORE,        BTA_AVK_CLOSING_SST }
};

/* type for state table */
typedef const UINT8 (*tBTA_AVK_SST_TBL)[BTA_AVK_NUM_COLS];

/* state table */
static const tBTA_AVK_SST_TBL bta_avk_sst_tbl[] =
{
    bta_avk_sst_init,
    bta_avk_sst_incoming,
    bta_avk_sst_opening,
    bta_avk_sst_open,
    bta_avk_sst_rcfg,
    bta_avk_sst_closing
};



#if (defined(BTA_AVK_DEBUG) && BTA_AVK_DEBUG == TRUE)
static char *bta_avk_sst_code(UINT8 state);
#endif

/*******************************************************************************
**
** Function         bta_avk_is_rcfg_sst
**
** Description      Check if stream state machine is in reconfig state.
**
**
** Returns          TRUE if stream state machine is in reconfig state.
**
*******************************************************************************/
BOOLEAN bta_avk_is_rcfg_sst (tBTA_AVK_SCB *p_scb)
{
    BOOLEAN is_rcfg_sst = FALSE;

    if (p_scb != NULL)
    {
        if (p_scb->state == BTA_AVK_RCFG_SST)
            is_rcfg_sst = TRUE;
    }

    return is_rcfg_sst;
}

/*******************************************************************************
**
** Function         bta_avk_ssm_execute
**
** Description      Stream state machine event handling function for AV
**
**
** Returns          void
**
*******************************************************************************/
void bta_avk_ssm_execute(tBTA_AVK_SCB *p_scb, UINT16 event, tBTA_AVK_DATA *p_data)
{
    tBTA_AVK_SST_TBL     state_table;
    UINT8               action;
    int                 i, xx;

    if(p_scb == NULL)
    {
        /* this stream is not registered */
        APPL_TRACE_EVENT("bta_avk_ssm_execute AVK channel not registered");
        return;
    }

    /* In case incoming connection is for VDP, we need to swap scb.        */
    /* When ACP_CONNECT_EVT was received, we put first available scb to    */
    /* to Incoming state. Later, when STR_CONFIG_IND_EVT is coming, we     */
    /* know if it is A2DP or VDP.                                          */
    if ((p_scb->state == BTA_AVK_INIT_SST) && (event == BTA_AVK_STR_CONFIG_IND_EVT))
    {
        for (xx = 0; xx < BTA_AVK_NUM_STRS; xx++)
        {
            if (bta_avk_cb.p_scb[xx])
            {
                if (bta_avk_cb.p_scb[xx]->state == BTA_AVK_INCOMING_SST)
                {
                    bta_avk_cb.p_scb[xx]->state = BTA_AVK_INIT_SST;
                    bta_avk_cb.p_scb[xx]->coll_mask = 0;
                    p_scb->state = BTA_AVK_INCOMING_SST;
                    break;
                }
            }
        }
    }

    if ((event != BTA_AVK_STR_WRITE_CFM_EVT) && (event != BTA_AVK_SRC_DATA_READY_EVT))
    {
        #if (defined(BTA_AVK_DEBUG) && BTA_AVK_DEBUG == TRUE)
            APPL_TRACE_IMP("AVK Sevent(0x%x)=0x%x(%s) state=%d(%s)",
               p_scb->hndl, event, bta_avk_evt_code(event), p_scb->state, bta_avk_sst_code(p_scb->state));
        #else
            APPL_TRACE_IMP("AVK Sevent=0x%x state=%d", event, p_scb->state);
        #endif
    }

    /* look up the state table for the current state */
    state_table = bta_avk_sst_tbl[p_scb->state];

    event -= BTA_AVK_FIRST_SSM_EVT;
    APPL_TRACE_IMP("Event =0x%x", event);
    if((p_scb->state != BTA_AVK_OPENING_SST) &&
        (state_table[event][BTA_AVK_SNEXT_STATE] == BTA_AVK_OPENING_SST))
    {
        AVDT_UpdateServiceBusyState(TRUE);
    }
    else if(AVDT_GetServiceBusyState() == TRUE)
    {
        BOOLEAN keep_busy = TRUE;

        for (xx = 0; xx < BTA_AVK_NUM_STRS; xx++)
        {
            if (bta_avk_cb.p_scb[xx])
            {
                if ((bta_avk_cb.p_scb[xx]->state == BTA_AVK_OPENING_SST) &&
                    (bta_avk_cb.p_scb[xx] != p_scb))
                {
                    /* There is other SCB in opening state
                     * keep the service state in progress
                     */
                    APPL_TRACE_VERBOSE("bta_avk_ssm_execute SCB in opening state. Keep Busy");
                    keep_busy = TRUE;
                    break;
                }
                else if ((bta_avk_cb.p_scb[xx]->state == BTA_AVK_OPENING_SST) &&
                    (bta_avk_cb.p_scb[xx] == p_scb) &&
                    (state_table[event][BTA_AVK_SNEXT_STATE] != BTA_AVK_OPENING_SST))
                {
                    keep_busy = FALSE;
                }
            }
        }
        if (keep_busy == FALSE)
        {
            AVDT_UpdateServiceBusyState(FALSE);
        }
    }

    APPL_TRACE_IMP("bta_avk_ssm_execute AVK Current State==%d", p_scb->state);
    /* set next state */
    p_scb->state = state_table[event][BTA_AVK_SNEXT_STATE];
    APPL_TRACE_IMP("bta_avk_ssm_execute AVK Next State==%d", p_scb->state);

    /* execute action functions */
    for(i=0; i< BTA_AVK_SACTIONS; i++)
    {
        APPL_TRACE_IMP("bta_avk_ssm_execute action==%d", state_table[event][i]);
        if ((action = state_table[event][i]) != BTA_AVK_SIGNORE)
        {
            (*p_scb->p_act_tbl[action])(p_scb, p_data);
        }
        else
            break;
    }

}

/*******************************************************************************
**
** Function         bta_avk_is_scb_opening
**
** Description      Returns TRUE is scb is in opening state.
**
**
** Returns          TRUE if scb is in opening state.
**
*******************************************************************************/
BOOLEAN bta_avk_is_scb_opening (tBTA_AVK_SCB *p_scb)
{
    BOOLEAN is_opening = FALSE;

    if (p_scb)
    {
        if (p_scb->state == BTA_AVK_OPENING_SST)
            is_opening = TRUE;
    }

    return is_opening;
}

/*******************************************************************************
**
** Function         bta_avk_is_scb_incoming
**
** Description      Returns TRUE is scb is in incoming state.
**
**
** Returns          TRUE if scb is in incoming state.
**
*******************************************************************************/
BOOLEAN bta_avk_is_scb_incoming (tBTA_AVK_SCB *p_scb)
{
    BOOLEAN is_incoming = FALSE;

    if (p_scb)
    {
        if (p_scb->state == BTA_AVK_INCOMING_SST)
            is_incoming = TRUE;
    }

    return is_incoming;
}

/*******************************************************************************
**
** Function         bta_avk_set_scb_sst_init
**
** Description      Set SST state to INIT.
**                  Use this function to change SST outside of state machine.
**
** Returns          None
**
*******************************************************************************/
void bta_avk_set_scb_sst_init (tBTA_AVK_SCB *p_scb)
{
    if (p_scb)
    {
        p_scb->state = BTA_AVK_INIT_SST;
    }
}

/*******************************************************************************
**
** Function         bta_avk_is_scb_init
**
** Description      Returns TRUE is scb is in init state.
**
**
** Returns          TRUE if scb is in incoming state.
**
*******************************************************************************/
BOOLEAN bta_avk_is_scb_init (tBTA_AVK_SCB *p_scb)
{
    BOOLEAN is_init = FALSE;

    if (p_scb)
    {
        if (p_scb->state == BTA_AVK_INIT_SST)
            is_init = TRUE;
    }

    return is_init;
}

/*******************************************************************************
**
** Function         bta_avk_set_scb_sst_incoming
**
** Description      Set SST state to incoming.
**                  Use this function to change SST outside of state machine.
**
** Returns          None
**
*******************************************************************************/
void bta_avk_set_scb_sst_incoming (tBTA_AVK_SCB *p_scb)
{
    if (p_scb)
    {
        p_scb->state = BTA_AVK_INCOMING_SST;
    }
}

/*****************************************************************************
**  Debug Functions
*****************************************************************************/
#if (defined(BTA_AVK_DEBUG) && BTA_AVK_DEBUG == TRUE)
/*******************************************************************************
**
** Function         bta_avk_sst_code
**
** Description
**
** Returns          char *
**
*******************************************************************************/
static char *bta_avk_sst_code(UINT8 state)
{
    switch(state)
    {
    case BTA_AVK_INIT_SST: return "INIT";
    case BTA_AVK_INCOMING_SST: return "INCOMING";
    case BTA_AVK_OPENING_SST: return "OPENING";
    case BTA_AVK_OPEN_SST: return "OPEN";
    case BTA_AVK_RCFG_SST: return "RCFG";
    case BTA_AVK_CLOSING_SST: return "CLOSING";
    default:             return "unknown";
    }
}

#endif
#endif /* BTA_AV_INCLUDED */
