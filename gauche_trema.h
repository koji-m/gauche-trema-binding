/*
 * gauche_trema.h
 */

/* Prologue */
#ifndef GAUCHE_GAUCHE_TREMA_H
#define GAUCHE_GAUCHE_TREMA_H

#include <gauche.h>
#include <gauche/extend.h>
#include <gauche/class.h>
#include "trema.h"

SCM_DECL_BEGIN

/*
 * The following entry is a dummy one.
 * Replace it for your declarations.
 */

extern ScmObj test_gauche_trema(void);

#define STRLEN (512)

/*
 * Trema Class entries
 */

typedef struct ScmTrmPacketInRec {
  SCM_HEADER;
  packet_in *message;
  packet_info pinfo;
} ScmTrmPacketIn;

typedef struct ScmOpenFlowActionsRec {
  SCM_HEADER;
  openflow_actions *actions;
} ScmOpenFlowActions;

typedef struct ScmOpenFlowMatchRec {
  SCM_HEADER;
  struct ofp_match *match;
} ScmOpenFlowMatch;

SCM_CLASS_DECL(Scm_TrmPacketInClass);
SCM_CLASS_DECL(Scm_OpenFlowActionsClass);
SCM_CLASS_DECL(Scm_OpenFlowMatchClass);

#define SCM_CLASS_TRM_PACKET_IN  (&Scm_TrmPacketInClass)
#define SCM_TRM_PACKET_IN(obj)   ((ScmTrmPacketIn*)obj)
#define SCM_TRM_PACKET_IN_P(obj) SCM_XTYPEP(obj, &Scm_TrmPacketInClass)

#define SCM_CLASS_OPENFLOW_ACTIONS  (&Scm_OpenFlowActionsClass)
#define SCM_OPEN_FLOW_ACTIONS(obj)   ((ScmOpenFlowActions*)obj)
#define SCM_OPEN_FLOW_ACTIONS_P(obj) SCM_XTYPEP(obj, &Scm_OpenFlowActionsClass)

#define SCM_CLASS_OPENFLOW_MATCH  (&Scm_OpenFlowMatchClass)
#define SCM_OPEN_FLOW_MATCH(obj)   ((ScmOpenFlowMatch*)obj)
#define SCM_OPEN_FLOW_MATCH_P(obj) SCM_XTYPEP(obj, &Scm_OpenFlowMatchClass)

//ScmObj Scm_MakeTrmPacketIn(void);
//ScmObj Scm_MakeOpenFlowActions(void);

ScmObj test_gauche_trema(void);
static ScmObj trm_packet_in_allocate(ScmClass *, ScmObj);
static char *addr2str(uint8_t *, int);
static ScmObj scm_make_packet_in(packet_in *);
static ScmObj get_packet_in_in_port(ScmTrmPacketIn *);
static ScmObj get_packet_in_macsa(ScmTrmPacketIn *);
static ScmObj get_packet_in_macda(ScmTrmPacketIn *);
static ScmObj openflow_actions_allocate(ScmClass *, ScmObj);
ScmObj Scm_MakeActionOutput(uint16_t);
static ScmObj openflow_match_allocate(ScmClass *, ScmObj);
ScmOpenFlowMatch *Scm_MatchFrom(ScmTrmPacketIn *, ScmObj);
static char **scm_list2cstring(ScmObj);
static char *addr2str(uint8_t *, int);
static void scm_handle_switch_ready(uint64_t, void *);
static void scm_handle_switch_disconnected(uint64_t, void *);
static void scm_handle_packet_in(uint64_t, packet_in);
ScmObj Scm_set_switch_ready_handler(void);
ScmObj Scm_set_switch_disconnected_handler(void);
ScmObj Scm_set_packet_in_handler(void);
ScmObj Scm_SendPacketOut(ScmObj, ScmTrmPacketIn *, ScmOpenFlowActions *);
ScmObj Scm_SendFlowMod(ScmObj, ScmObj, ScmOpenFlowMatch *, ScmOpenFlowActions *, ScmObj, ScmObj, ScmObj);
ScmObj Scm_InitTrema(ScmObj);
ScmObj Scm_StartTrema(void);
ScmObj Scm_DebugPrint(ScmTrmPacketIn *);


/* Epilogue */
SCM_DECL_END

#endif  /* GAUCHE_GAUCHE_TREMA_H */
