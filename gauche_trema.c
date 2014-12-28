/*
 * gauche_trema.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>
#include "gauche.h"
#include "gauche_trema.h"

/*
 * The following function is a dummy one; replace it for
 * your C function definitions.
 */

ScmObj test_gauche_trema(void)
{
    return SCM_MAKE_STR("gauche_trema is working");
}

/*
 * Define Trema Class
 */

//Packet-In class

static ScmObj trm_packet_in_allocate(ScmClass *klass, ScmObj initargs);
static char *addr2str(uint8_t *addr, int len);

SCM_DEFINE_BUILTIN_CLASS(Scm_TrmPacketInClass,
			 NULL,NULL,NULL,
			 trm_packet_in_allocate,
			 NULL);

static ScmObj trm_packet_in_allocate(ScmClass *klass, ScmObj initargs){
  ScmTrmPacketIn *p = SCM_NEW(ScmTrmPacketIn);
  SCM_SET_CLASS(p, SCM_CLASS_TRM_PACKET_IN);

  // here is default slot initialization.

  return SCM_OBJ(p);
}

static ScmObj scm_make_packet_in(packet_in *message){
  ScmTrmPacketIn *p = SCM_NEW(ScmTrmPacketIn);
  SCM_SET_CLASS(p, SCM_CLASS_TRM_PACKET_IN);
  p->message = message;
  p->pinfo = get_packet_info(message->data);

  return SCM_OBJ(p);
}

static ScmObj get_packet_in_in_port(ScmTrmPacketIn *p){
  return Scm_MakeIntegerU( (u_long)((p->message)->in_port) );
}

static ScmObj get_packet_in_macsa(ScmTrmPacketIn *p){
  return SCM_MAKE_STR(addr2str((p->pinfo).eth_macsa, ETH_ADDRLEN));
}

static ScmObj get_packet_in_macda(ScmTrmPacketIn *p){
  return SCM_MAKE_STR(addr2str((p->pinfo).eth_macda, ETH_ADDRLEN));
}

static ScmClassStaticSlotSpec packet_in_slots[] = {
  SCM_CLASS_SLOT_SPEC("in-port", get_packet_in_in_port, NULL),
  SCM_CLASS_SLOT_SPEC("macsa", get_packet_in_macsa, NULL),
  SCM_CLASS_SLOT_SPEC("macda", get_packet_in_macda, NULL),
  {NULL}
};

//OpenFlow Actions class

static ScmObj openflow_actions_allocate(ScmClass *klass, ScmObj initargs);

SCM_DEFINE_BUILTIN_CLASS(Scm_OpenFlowActionsClass,
			 NULL,NULL,NULL,
			 openflow_actions_allocate,
			 NULL);

static ScmObj openflow_actions_allocate(ScmClass *klass, ScmObj initargs){
  ScmOpenFlowActions *p = SCM_NEW(ScmOpenFlowActions);
  SCM_SET_CLASS(p, SCM_CLASS_OPENFLOW_ACTIONS);

  // here is default slot initialization.

  return SCM_OBJ(p);
}

ScmObj Scm_MakeActionOutput(uint16_t outport){
  ScmOpenFlowActions *p = SCM_NEW(ScmOpenFlowActions);
  SCM_SET_CLASS(p, SCM_CLASS_OPENFLOW_ACTIONS);

  openflow_actions *acts = create_actions();

  append_action_output(acts, outport, UINT16_MAX);

  p->actions = acts;

  return SCM_OBJ(p);
}

static ScmClassStaticSlotSpec openflow_actions_slots[] = {
  SCM_CLASS_SLOT_SPEC("actions", NULL, NULL),
  {NULL}
};

//OpenFlow Match class

static ScmObj openflow_match_allocate(ScmClass *klass, ScmObj initargs);

SCM_DEFINE_BUILTIN_CLASS(Scm_OpenFlowMatchClass,
			 NULL,NULL,NULL,
			 openflow_match_allocate,
			 NULL);

static ScmObj openflow_match_allocate(ScmClass *klass, ScmObj initargs){
  ScmOpenFlowMatch *p = SCM_NEW(ScmOpenFlowMatch);
  SCM_SET_CLASS(p, SCM_CLASS_OPENFLOW_MATCH);

  // here is default slot initialization.

  return SCM_OBJ(p);
}

ScmOpenFlowMatch *Scm_MatchFrom(ScmTrmPacketIn *message, ScmObj wildcard){
  ScmOpenFlowMatch *scm_ofp_match = SCM_NEW(ScmOpenFlowMatch);
  SCM_SET_CLASS(scm_ofp_match, SCM_CLASS_OPENFLOW_MATCH);

  struct ofp_match *match = (struct ofp_match *)malloc(sizeof(struct ofp_match));
  memset(match, 0, sizeof(struct ofp_match));

  packet_in *msg = message->message;

  set_match_from_packet(match, msg->in_port, (uint32_t)Scm_GetIntegerU(wildcard), msg->data);

  scm_ofp_match->match = match;

  return scm_ofp_match;
}

static ScmClassStaticSlotSpec openflow_match_slots[] = {
  SCM_CLASS_SLOT_SPEC("match", NULL, NULL),
  {NULL}
};


/*
 * Module initialization function.
 */
extern void Scm_Init_gauche_tremalib(ScmModule*);

void Scm_Init_gauche_trema(void)
{
    ScmModule *mod;

    /* Register this DSO to Gauche */
    SCM_INIT_EXTENSION(gauche_trema);

    /* Create the module if it doesn't exist yet. */
    mod = SCM_MODULE(SCM_FIND_MODULE("net.trema", TRUE));

    /* Initialize Classes. */
    Scm_InitStaticClass(SCM_CLASS_TRM_PACKET_IN, "<packet-in>", mod, packet_in_slots, 0);
    Scm_InitStaticClass(SCM_CLASS_OPENFLOW_ACTIONS, "<openflow_actions>", mod, openflow_actions_slots, 0);
    Scm_InitStaticClass(SCM_CLASS_OPENFLOW_MATCH, "<openflow_match>", mod, openflow_match_slots, 0);

    /* Register stub-generated procedures */
    Scm_Init_gauche_tremalib(mod);
}


/*
 * Scheme List to C String
 */
static char **scm_list2cstring(ScmObj list){
  ScmObj lp;
  char **strings;
  int len = Scm_Length(list), i;

  if(len<0) Scm_Error("proper list required!");

  strings = (char **)malloc((size_t)((sizeof(char*))*len));

  for(i=0, lp=list; i<len; i++, lp=SCM_CDR(lp)){
    strings[i] = (char *)SCM_STRING_CONST_CSTRING(SCM_CAR(lp));
  }

  return strings;
}

/*
 * MAC address to string
 */
static char *addr2str(uint8_t *addr, int len){
  static char str[STRLEN];
  int i, offset = 0;

  memset(str, 0, STRLEN);

  for(i = 0; i < len; i++){
    if(i == (len - 1)){
      offset += sprintf(str + offset, "%02x", addr[i]);
    }else{
      offset += sprintf(str + offset, "%02x:", addr[i]);
    }
  }

  return str;
}

/*
 * scm_handle_switch_ready
 */
static void scm_handle_switch_ready(uint64_t datapath_id, void *user_data){
  ScmObj callback;
  ScmEvalPacket eval_packet;

  callback = Scm_GlobalVariableRef(Scm_UserModule(),
                                   SCM_SYMBOL(SCM_INTERN("switch-ready")),
                                   SCM_BINDING_STAY_IN_MODULE);

  if(Scm_Apply(callback, SCM_LIST1(Scm_MakeIntegerU64(datapath_id)), &eval_packet) < 0){
    printf("[%s] %s\n",
           SCM_STRING_CONST_CSTRING(Scm_ConditionTypeName(eval_packet.exception)),
           SCM_STRING_CONST_CSTRING(Scm_ConditionMessage(eval_packet.exception)));
  }
}

/*
 * scm_handle_switch_disconnected
 */
static void scm_handle_switch_disconnected(uint64_t datapath_id, void *user_data){
  ScmObj callback;
  ScmEvalPacket eval_packet;

  callback = Scm_GlobalVariableRef(Scm_UserModule(),
                                   SCM_SYMBOL(SCM_INTERN("switch-disconnected")),
				   SCM_BINDING_STAY_IN_MODULE);

  if(Scm_Apply(callback, SCM_LIST1(Scm_MakeIntegerU64(datapath_id)), &eval_packet) < 0){
    printf("[%s] %s\n",
           SCM_STRING_CONST_CSTRING(Scm_ConditionTypeName(eval_packet.exception)),
           SCM_STRING_CONST_CSTRING(Scm_ConditionMessage(eval_packet.exception)));
  }
}

/*
 * scm_handle_packet_in
 */
static void scm_handle_packet_in(uint64_t datapath_id, packet_in message){
  ScmObj callback;
  ScmEvalPacket eval_packet;

  ScmTrmPacketIn pkt_in = *(ScmTrmPacketIn*)scm_make_packet_in(&message);

  callback = Scm_GlobalVariableRef(Scm_UserModule(),
                                   SCM_SYMBOL(SCM_INTERN("packet-in")),
				   SCM_BINDING_STAY_IN_MODULE);

  if(Scm_Apply(callback, SCM_LIST2(Scm_MakeIntegerU64(datapath_id), SCM_OBJ(&pkt_in)), &eval_packet) < 0){
    printf("[%s] %s\n",
           SCM_STRING_CONST_CSTRING(Scm_ConditionTypeName(eval_packet.exception)),
           SCM_STRING_CONST_CSTRING(Scm_ConditionMessage(eval_packet.exception)));
  }
}

/*
 * Scm_set_switch_ready_handler
 */
ScmObj Scm_set_switch_ready_handler(void){

  set_switch_ready_handler(scm_handle_switch_ready, NULL);

  return SCM_MAKE_STR("switch ready handler is set!");
}

/*
 * Scm_set_switch_disconnected_handler
 */
ScmObj Scm_set_switch_disconnected_handler(void){

  set_switch_disconnected_handler(scm_handle_switch_disconnected, NULL);

  return SCM_MAKE_STR("switch disconnected handler is set!");
}

/*
 * Scm_set_packet_in_handler
 */
ScmObj Scm_set_packet_in_handler(void){

  set_packet_in_handler(scm_handle_packet_in, NULL);

  return SCM_MAKE_STR("packet in  handler is set!");
}

/*
 * Send Packet-Out
 */
ScmObj Scm_SendPacketOut(ScmObj datapath_id, ScmTrmPacketIn *pktin, ScmOpenFlowActions *actions){

  packet_in *msg = pktin->message;

  const buffer *data = (msg->buffer_id == -1) ? msg->data : NULL;

  buffer *packet_out = create_packet_out(get_transaction_id(),
					 msg->buffer_id, msg->in_port,
					 actions->actions, data);

  send_openflow_message(Scm_GetIntegerU64(datapath_id), packet_out);

  //delete_actions(actions->actions);

  free_buffer(packet_out);

  return Scm_MakeInteger(0);
}

/*
 * Send Flow_Mod
 */
ScmObj Scm_SendFlowMod(ScmObj command, ScmObj datapath_id, ScmOpenFlowMatch *match, ScmOpenFlowActions *actions, ScmObj idle_timeout, ScmObj priority, ScmObj buffer_id){

  buffer *flow_mod = create_flow_mod(get_transaction_id(),
				     *(match->match),
				     get_cookie(),
				     Scm_GetIntegerU(command),
				     Scm_GetIntegerU(idle_timeout),
				     0,
				     Scm_GetIntegerU(priority),
				     Scm_GetInteger(buffer_id),
				     OFPP_NONE,
				     OFPFF_SEND_FLOW_REM,
				     actions->actions);

  send_openflow_message(Scm_GetIntegerU64(datapath_id), flow_mod);

  free_buffer(flow_mod);

  return Scm_MakeInteger(0);
}

/*
 * Initialize Trema
 */
ScmObj Scm_InitTrema(ScmObj args){
  int argc = Scm_Length(args);
  char **argv = scm_list2cstring(args);
  struct stat buf;

  init_trema(&argc, &argv);

  return Scm_MakeInteger(0);
}

/*
 * Start Trema
 */
ScmObj Scm_StartTrema(void){
 
  start_trema();

  return Scm_MakeInteger(0);
}

ScmObj Scm_DebugPrint(ScmTrmPacketIn *p){
  printf("<--- print debug start --->\n");
  printf("ScmTrmPacketIn message eth_macsa: %s\n", addr2str((p->pinfo).eth_macsa, ETH_ADDRLEN));
  printf("<--- print debug end --->\n");

  return SCM_MAKE_INT(48);
}
