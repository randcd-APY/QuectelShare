#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dlfcn.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <hardware/hardware.h>
#include <iostream>
#include <iomanip>
#include "btheader.hpp"
#include "A2dp_Sink.hpp"
#include "Audio_Manager.hpp"
#include "osi/include/compat.h"
#include "Avrcp.hpp"

#include "utils.h"
#define LOGTAG  "MAIN "
#define LOCAL_SOCKET_NAME "/data/misc/bluetooth/btappsocket"
#define SOCKETNAME  "/data/misc/bluetooth/btprop"
static int bt_prop_socket;
bdstr_t bd_str="";//"00:ec:0A:9e:51:62";
extern Gap *g_gap;
extern A2dp_Sink *pA2dpSink;
extern BT_Audio_Manager *pBTAM;
extern ThreadInfo threadInfo[THREAD_ID_MAX];
bool a2dpsink_connected=false;
extern Avrcp *pAvrcp;
static alarm_t *opp_incoming_file_accept_timer = NULL;
#define USER_ACCEPTANCE_TIMEOUT 25000
struct hw_device_t *device_;
bluetooth_device_t *bt_device_;
const bt_interface_t *bt_interface;
bt_state_t bt_state;
config_t *config;
UiCommandStatus status;
bool ssp_notification;
bool pin_notification;
SSPReplyEvent   ssp_data;
PINReplyEvent   pin_reply;
ThreadInfo *main_thread;
/**
 * The Configuration options
 */
const char *BT_SOCKET_ENABLED      = "BtSockInputEnabled";
const char *BT_ENABLE_DEFAULT      = "BtEnableByDefault";
const char *BT_USER_INPUT          = "UserInteractionNeeded";
const char *BT_A2DP_SINK_ENABLED   = "BtA2dpSinkEnable";
const char *BT_A2DP_SOURCE_ENABLED = "BtA2dpSourceEnable";
const char *BT_HFP_CLIENT_ENABLED  = "BtHfClientEnable";
const char *BT_HFP_AG_ENABLED      = "BtHfpAGEnable";
const char *BT_AVRCP_ENABLED       = "BtAvrcpEnable";
const char *BT_ENABLE_EXT_POWER    = "BtEnableExtPower";
const char *BT_ENABLE_FW_SNOOP     = "BtEnableFWSnoop";
const char *BT_ENABLE_SOC_LOG      = "BtEnableSocLog";
const char *BT_HID_ENABLED         = "BtHidEnable";
/**
 * The Configuration file path
 */
const char *CONFIG_FILE_PATH       = "/etc/bluetooth/bt_app.conf";

bool LoadBtStack (void) {
	fprintf(stdout, "\n ########### LoadBtStack Enter ######### \n");
    hw_module_t *module;

    if (hw_get_module (BT_STACK_MODULE_ID, (hw_module_t const **) &module)) {
        ALOGE(LOGTAG " hw_get_module failed");
        return false;
    }

    if (module->methods->open (module, BT_STACK_MODULE_ID, &device_)) {
        return false;
    }

    bt_device_ = (bluetooth_device_t *) device_;
    bt_interface = bt_device_->get_bluetooth_interface ();
    if (!bt_interface) {
        bt_device_->common.close ((hw_device_t *) & bt_device_->common);
        bt_device_ = NULL;
        return false;
    }
    return true;
}


void UnLoadBtStack (void)
{
	fprintf(stdout, "\n ########### UnLoadBtStack Enter ######### \n");
    if (bt_interface) {
        bt_interface->cleanup ();
        bt_interface = NULL;
    }

    if (bt_device_) {
        bt_device_->common.close ((hw_device_t *) & bt_device_->common);
        bt_device_ = NULL;
    }
}

bool GAP_BT_ENABLE(){
	// Enable Bluetooth
	fprintf(stdout, "\n ########### GAP Bluetooth Enable  ######### \n");
    BtEvent *event = new BtEvent;
    event->event_id = GAP_API_ENABLE;
    ALOGV (LOGTAG "  Posting enable to GAP thread");
    PostMessage (THREAD_ID_GAP, event);
	fprintf(stdout, "\n ########### GAP Bluetooth Enable exit ######### \n");
	return true;
}
bool GAP_BT_DISABLE(){
    fprintf(stdout,"\n ########### send disable cmd to gap ########## \n");
    BtEvent *event = new BtEvent;
    event->event_id = GAP_API_DISABLE;
    ALOGV (LOGTAG " Posting disable to GAP thread");
    PostMessage (THREAD_ID_GAP, event);
    return true;
}
bool GAP_START_ENQUIRY(){
	BtEvent *event = new BtEvent;
    event->event_id = GAP_API_START_INQUIRY;
    ALOGV (LOGTAG " Posting inquiry to GAP thread");
    PostMessage (THREAD_ID_GAP, event);
	return true;
}
bool GAP_CANCEL_ENQUIRY(){
	BtEvent *event = new BtEvent;
    event->event_id = GAP_API_STOP_INQUIRY;
    ALOGV (LOGTAG " Posting stop inquiry to GAP thread");
    PostMessage (THREAD_ID_GAP, event);
	return true;
}
bool GAP_START_PAIR(){
	if (string_is_bdaddr(bd_str)) {
        BtEvent *event = new BtEvent;
        event->event_id = GAP_API_CREATE_BOND;
        string_to_bdaddr(bd_str, &event->bond_device.bd_addr);
        PostMessage (THREAD_ID_GAP, event);
    } else {
        fprintf( stdout, " BT address is NULL/Invalid \n");
    }
    return true;
}

#if 0
bool GAP_UNPAIR(){
    if (string_is_bdaddr(bd_str)) {
        bt_bdaddr_t bd_addr;
        string_to_bdaddr(bd_str, &bd_addr);
        g_bt_app->HandleUnPair(bd_addr);
    }else {
        fprintf( stdout, " BT address is NULL/Invalid \n");
    }
    return true;
}
#endif
bool GAP_GET_BT_NAME(){
    fprintf(stdout, "BT Name : %s\n", g_gap->GetBtName());
    return true;
}
bool GAP_GET_BT_ADDR(){
    bdstr_t bd_str;
    bt_bdaddr_t *bd_addr = g_gap->GetBtAddress();
    bdaddr_to_string(bd_addr, &bd_str[0], sizeof(bd_str));
    fprintf(stdout, " BT Address : %s\n", bd_str);
    return true;
}
void Inithandler()
{
	fprintf(stdout, "\n ########### Inithandler Enter ######### \n");
    if (!LoadBtStack())
        return;

    config = config_new (CONFIG_FILE_PATH);
    if (!config) {
        ALOGE (LOGTAG " Unable to open config file");
        return ;
    }

    // Starting GAP Thread
    threadInfo[THREAD_ID_GAP].thread_id = thread_new (
            threadInfo[THREAD_ID_GAP].thread_name);

    if (threadInfo[THREAD_ID_GAP].thread_id) {
        g_gap = new Gap (bt_interface, config);
    }

    threadInfo[THREAD_ID_BT_AM].thread_id = thread_new (
                    threadInfo[THREAD_ID_BT_AM].thread_name);
    if (threadInfo[THREAD_ID_BT_AM].thread_id) {
         pBTAM = new BT_Audio_Manager (bt_interface, config);
    }


    threadInfo[THREAD_ID_A2DP_SINK].thread_id = thread_new (
            threadInfo[THREAD_ID_A2DP_SINK].thread_name);

	fprintf(stdout, "\n ########### A2DPsink thread_id ######### \n");

    if (threadInfo[THREAD_ID_A2DP_SINK].thread_id) {
		fprintf(stdout, "\n ########### A2DPsink thread_id Not NULL ######### \n");
        pA2dpSink = new A2dp_Sink (bt_interface, config);
    }


    threadInfo[THREAD_ID_AVRCP].thread_id = thread_new (
            threadInfo[THREAD_ID_AVRCP].thread_name);

    if (threadInfo[THREAD_ID_AVRCP].thread_id) {
        pAvrcp = new Avrcp (bt_interface, config);
    }

    // Enable Bluetooth
#if 1
    BtEvent *event = new BtEvent;
    event->event_id = GAP_API_ENABLE;
    ALOGV (LOGTAG "  Posting enable to GAP thread");
    PostMessage (THREAD_ID_GAP, event);
	fprintf(stdout, "\n ########### Enable Bluetooth ######### \n");
#endif
	//GAP_BT_ENABLE();
}


#if 1
void HandleBondState(bt_bond_state_t new_state, const bt_bdaddr_t
                                        bd_addr, std::string bd_name ) {
	fprintf(stdout,"\n ########### HandleBondState Enter ########## \n");
    //bdstr_t bd_str;
    bdaddr_to_string(&bd_addr, &bd_str[0], sizeof(bd_str));
    std::string deviceAddress(bd_str);


    if(new_state == BT_BOND_STATE_BONDED) {

        fprintf(stdout, "\n*************************************************");
        fprintf(stdout, "\n Pairing state for %s is BONDED", bd_name.c_str());
        fprintf(stdout, "\n*************************************************\n");

#if 1
        bt_bdaddr_t address;
        string_to_bdaddr(bd_str, &address);
        if (!g_gap->IsDeviceBonded(address)) {
                fprintf( stdout, " Please pair with the device before A2DPSink connection\n");
                return;
            }
        BtEvent * event = new BtEvent;
        memset(event, 0, sizeof(BtEvent));
        event->a2dpSinkEvent.event_id = A2DP_SINK_API_CONNECT_REQ;
        string_to_bdaddr(bd_str, &event->a2dpSinkEvent.bd_addr);
        PostMessage (THREAD_ID_A2DP_SINK, event);
		a2dpsink_connected=true;
#endif
		//A2DPSink_CONNECT();
    } else if (new_state == BT_BOND_STATE_NONE) {
        fprintf(stdout, "\n*************************************************");
        fprintf(stdout, "\n Pairing state for %s is BOND NONE", bd_name.c_str());
        fprintf(stdout, "\n*************************************************\n");

    }else if(new_state == BT_BOND_STATE_BONDING) {

        fprintf(stdout, "\n*************************************************");
        fprintf(stdout, "\n Pairing state for %s is BONDING", bd_name.c_str());
        fprintf(stdout, "\n*************************************************\n");
	}
	fprintf(stdout, "\n ########### HandleBondState Exit ######### \n");
}

#endif
void ProcessEvent (BtEvent * event) {
	fprintf(stdout, "\n ########### ProcessEvent Enter ######### \n");
    ALOGD (LOGTAG " Processing event %d", event->event_id);
    const uint8_t *ptr = NULL;
	bdstr_t bd_str_local;
    switch (event->event_id) {
     case MAIN_EVENT_ENABLED:
			fprintf(stdout, "\n ########### MAIN_EVENT_ENABLED ######### \n");
            bt_state = event->state_event.status;
            if (event->state_event.status == BT_STATE_OFF) {
                fprintf(stdout," Error in Enabling BT\n");
            } else {
               fprintf(stdout," BT State is ON\n");
			}
			status.enable_cmd = COMMAND_COMPLETE;
		    break;
    case MAIN_EVENT_DISABLED:
			fprintf(stdout, "\n ########### MAIN_EVENT_DISABLED ######### \n");
            bt_state = event->state_event.status;
            if (event->state_event.status == BT_STATE_ON) {
                fprintf(stdout, " Error in disabling BT\n");
            } else {
                // clear the inquiry related cmds
                status.enquiry_cmd = COMMAND_COMPLETE;
                status.stop_enquiry_cmd = COMMAND_COMPLETE;
                //bt_discovery_state = BT_DISCOVERY_STOPPED;
                // clearing bond_devices list and inquiry_list
                //bonded_devices.clear();
                //inquiry_list.clear();
                system("killall -KILL wcnssfilter");
                usleep(200);
                fprintf(stdout, " BT State is OFF\n");
            }
            status.disable_cmd = COMMAND_COMPLETE;
            break;
    case MAIN_EVENT_BOND_STATE:{
			fprintf(stdout, "\n ########### MAIN_EVENT_BOND_STATE ######### \n");
            char str[18];
            std::string bd_name((const char*)event->bond_state_event.bd_name.name);
            HandleBondState(event->bond_state_event.state,
                                    event->bond_state_event.bd_addr, bd_name);
            }
            break;
#if 1
	case MAIN_EVENT_SSP_REQUEST:
			fprintf(stdout, "\n ########### MAIN_EVENT_SSP_REQUEST ######### \n");
            memcpy(&ssp_data.bd_addr, &event->ssp_request_event.bd_addr,
                                            sizeof(bt_bdaddr_t));
            memcpy(&ssp_data.bd_name, &event->ssp_request_event.bd_name,
                                                    sizeof(bt_bdname_t));
            ssp_data.cod = event->ssp_request_event.cod;
            ssp_data.pairing_variant = event->ssp_request_event.pairing_variant;
            ssp_data.pass_key = event->ssp_request_event.pass_key;
            // instruct the cmd handler to treat the next inputs for SSP
            fprintf(stdout, "\n*************************************************");
            fprintf(stdout, "\n BT pairing request::Device %s::Pairing Code:: %d",
                                    ssp_data.bd_name.name, ssp_data.pass_key);
            fprintf(stdout, "\n*************************************************\n");
            fprintf(stdout, " ** Please enter yes / no **\n");
            ssp_notification = true;
            break;
    case MAIN_EVENT_PIN_REQUEST:

            memcpy(&pin_reply.bd_addr, &event->pin_request_event.bd_addr,
                                            sizeof(bt_bdaddr_t));
            memcpy(&pin_reply.bd_name, &event->pin_request_event.bd_name,
                                            sizeof(bt_bdname_t));
            fprintf(stdout, "\n*************************************************");
            fprintf(stdout, "\n BT Legacy pairing request::Device %s::",
                                    pin_reply.bd_name.name);
            fprintf(stdout, "\n*************************************************\n");
            fprintf(stdout, " ** Please enter valid PIN key **\n");
            pin_reply.secure = event->pin_request_event.secure;
            // instruct the cmd handler to treat the next inputs for PIN
            pin_notification = true;
            break;
#endif
     default:
            ALOGD (LOGTAG " Default Case");
            break;


    }
}


void BtMainMsgHandler (void *context) {

    BtEvent *event = NULL;
    if (!context) {
        ALOGI (LOGTAG " Msg is null, return.\n");
        return;
    }
    event = (BtEvent *) context;
    ProcessEvent ((BtEvent *) context);
    delete event;
}

bool A2DPSink_CONNECT(){
            bt_bdaddr_t address;
            string_to_bdaddr(bd_str, &address);
            if (!g_gap->IsDeviceBonded(address)) {
                fprintf( stdout, " Please pair with the device before A2DPSink connection\n");
                return false;
            }
            BtEvent *event = new BtEvent;
            memset(event, 0, sizeof(BtEvent));
            event->a2dpSinkEvent.event_id = A2DP_SINK_API_CONNECT_REQ;
            string_to_bdaddr(bd_str, &event->a2dpSinkEvent.bd_addr);
            PostMessage (THREAD_ID_A2DP_SINK, event);
            return true;
        }
bool A2DPSink_DISCONNECT(){
            BtEvent *event = new BtEvent;
            memset(event, 0, sizeof(BtEvent));
            event->a2dpSinkEvent.event_id = A2DP_SINK_API_DISCONNECT_REQ;
            string_to_bdaddr(bd_str, &event->a2dpSinkEvent.bd_addr);
            PostMessage (THREAD_ID_A2DP_SINK, event);
            return true;
}
bool AVRCP_PLAY(){
			fprintf( stdout, " AVRCP PLAY \n");
            BtEvent *event = new BtEvent;
            memset(event, 0, sizeof(BtEvent));
            event->avrcpCtrlPassThruEvent.event_id = AVRCP_CTRL_PASS_THRU_CMD_REQ;
            event->avrcpCtrlPassThruEvent.key_id = CMD_ID_PLAY;
            string_to_bdaddr(bd_str, &event->avrcpCtrlPassThruEvent.bd_addr);
            PostMessage (THREAD_ID_AVRCP, event);
            return true;
}
bool AVRCP_PAUSE(){
			fprintf( stdout, " AVRCP PAUSE \n");
            BtEvent *event = new BtEvent;
            memset(event, 0, sizeof(BtEvent));
            event->avrcpCtrlPassThruEvent.event_id = AVRCP_CTRL_PASS_THRU_CMD_REQ;
            event->avrcpCtrlPassThruEvent.key_id = CMD_ID_PAUSE;
            string_to_bdaddr(bd_str, &event->avrcpCtrlPassThruEvent.bd_addr);
            PostMessage (THREAD_ID_AVRCP, event);
            return true;
}
bool AVRCP_STOP(){
			fprintf( stdout, " AVRCP STOP \n");
            BtEvent *event = new BtEvent;
            memset(event, 0, sizeof(BtEvent));
            event->avrcpCtrlPassThruEvent.event_id = AVRCP_CTRL_PASS_THRU_CMD_REQ;
            event->avrcpCtrlPassThruEvent.key_id = CMD_ID_STOP;
            string_to_bdaddr(bd_str, &event->avrcpCtrlPassThruEvent.bd_addr);
            PostMessage (THREAD_ID_AVRCP, event);
			return true;
}
bool AVRCP_FASTFORWARD(){
            BtEvent *event = new BtEvent;
            memset(event, 0, sizeof(BtEvent));
            event->avrcpCtrlPassThruEvent.event_id = AVRCP_CTRL_PASS_THRU_CMD_REQ;
            event->avrcpCtrlPassThruEvent.key_id = CMD_ID_FF;
            string_to_bdaddr(bd_str, &event->avrcpCtrlPassThruEvent.bd_addr);
            PostMessage (THREAD_ID_AVRCP, event);
            return true;;
}
bool AVRCP_REWIND(){
            BtEvent *event = new BtEvent;
            memset(event, 0, sizeof(BtEvent));
            event->avrcpCtrlPassThruEvent.event_id = AVRCP_CTRL_PASS_THRU_CMD_REQ;
            event->avrcpCtrlPassThruEvent.key_id = CMD_ID_REWIND;
            string_to_bdaddr(bd_str, &event->avrcpCtrlPassThruEvent.bd_addr);
            PostMessage (THREAD_ID_AVRCP, event);
            return true;;
}
bool AVRCP_FORWARD(){
            BtEvent *event = new BtEvent;
            memset(event, 0, sizeof(BtEvent));
            event->avrcpCtrlPassThruEvent.event_id = AVRCP_CTRL_PASS_THRU_CMD_REQ;
            event->avrcpCtrlPassThruEvent.key_id = CMD_ID_FORWARD;
            string_to_bdaddr(bd_str, &event->avrcpCtrlPassThruEvent.bd_addr);
            PostMessage (THREAD_ID_AVRCP, event);
            return true;;
}
bool AVRCP_BACKWARD(){
            BtEvent *event = new BtEvent;
            memset(event, 0, sizeof(BtEvent));
            event->avrcpCtrlPassThruEvent.event_id = AVRCP_CTRL_PASS_THRU_CMD_REQ;
            event->avrcpCtrlPassThruEvent.key_id = CMD_ID_BACKWARD;
            string_to_bdaddr(bd_str, &event->avrcpCtrlPassThruEvent.bd_addr);
            PostMessage (THREAD_ID_AVRCP, event);
            return true;;
}
bool AVRCP_VOL_UP(){
			fprintf( stdout, " \n AVRCP volume up \n");
            BtEvent *event = new BtEvent;
            memset(event, 0, sizeof(BtEvent));
            event->avrcpCtrlPassThruEvent.event_id = AVRCP_CTRL_PASS_THRU_CMD_REQ;
            event->avrcpCtrlPassThruEvent.key_id = CMD_ID_VOL_UP;
            string_to_bdaddr(bd_str, &event->avrcpCtrlPassThruEvent.bd_addr);
            PostMessage (THREAD_ID_AVRCP, event);
            return true;;
}
bool AVRCP_VOL_DOWN(){
			fprintf( stdout, " \n AVRCP volume down \n");
            BtEvent *event = new BtEvent;
            memset(event, 0, sizeof(BtEvent));
            event->avrcpCtrlPassThruEvent.event_id = AVRCP_CTRL_PASS_THRU_CMD_REQ;
            event->avrcpCtrlPassThruEvent.key_id = CMD_ID_VOL_DOWN;
            string_to_bdaddr(bd_str, &event->avrcpCtrlPassThruEvent.bd_addr);
            PostMessage (THREAD_ID_AVRCP, event);
            return true;;
}

int BtInit(void) {
 	fprintf(stdout, "\n ########### MAIN/BtInit Function ######### \n");
    main_thread = &threadInfo[THREAD_ID_MAIN];
    Inithandler();
    main_thread->thread_id = thread_new (main_thread->thread_name);
    if (main_thread->thread_id) {
#if 1
        BtEvent *event = new BtEvent;
        event->event_id = MAIN_API_INIT;
        ALOGV (LOGTAG " Posting init to Main thread\n");
        PostMessage (THREAD_ID_MAIN, event);
#endif
}
	fprintf(stdout, "\n ########### MAIN/BtInit Function Exit######### \n");
    return 0;
}

int BtDeinit(){
	a2dpsink_connected=false;
	printf("\n######### BtDeinit ##########\n");
	printf("\n######### UnLoadBtStack ##########\n");
	UnLoadBtStack ();
	printf("\n######### free thread A2DP Sink ##########\n");
	if (threadInfo[THREAD_ID_A2DP_SINK].thread_id != NULL) {
            thread_free (threadInfo[THREAD_ID_A2DP_SINK].thread_id);
            if ( pA2dpSink != NULL)
                delete pA2dpSink;
        }
	printf("\n######### free thread AVRCP ##########\n");
	if (threadInfo[THREAD_ID_AVRCP].thread_id != NULL) {
            thread_free (threadInfo[THREAD_ID_AVRCP].thread_id);
            if ( pAvrcp != NULL)
                delete pAvrcp;
        }
	printf("\n######### free thread GAP ##########\n");
    if (threadInfo[THREAD_ID_GAP].thread_id != NULL) {
        thread_free (threadInfo[THREAD_ID_GAP].thread_id);
        if ( g_gap != NULL)
            delete g_gap;
    }

	 // wait for Main thread to exit
	printf("\n######### waiting for main thread to join ##########\n");
//    thread_join (main_thread->thread_id);
//    thread_free (main_thread->thread_id);
	return 0;
}
