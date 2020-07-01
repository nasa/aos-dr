/************************************************************************
** File:
**   $Id: dr_msgids.h  $
**
** Purpose: 
**  Define Message ID numbers for the DR application
**
** Notes:
**  These MID numbers must be unique in the running system
**  and should be unique within the project.
**
**  Projects have in the past defined other schemes for defining
**  the MIDs used in that project, such as generation from a 
**  master .csv file or from a database. In such a case, this file
**  should be replaced by  
**
**
*************************************************************************/
#ifndef DR_MSGIDS_H
#define DR_MSGIDS_H

/*
** DR Commands
**
** The command code in the secondary header of these
** packets is interpreted per the DR_*_CC macros
** defined in the dr_msg.h header.
*/
#ifndef DR_CMD_MID
#define DR_CMD_MID   0x1911
#else
#error "Message ID macro DR_CMD_MID already defined!"
#endif

/*
** DR Housekeeping Trigger
**
** When DR sees this message, it will trigger transmission
** of its housekeeping packet.
**
** The command code in the secondary header of these
** packets (and the content of the payload) is ignored.
*/
#ifndef DR_SEND_HK_MID
#define DR_SEND_HK_MID   0x1912
#else
#error "Message ID macro DR_SEND_HK_MID already defined!"
#endif

/*
** DR Wakeup Trigger
**
** Arrival of packets on this MID triggers execution
** of the DR DIAGNOISE process.
**
** The command code in the secondary header of these
** packets (and the content of the payload) is ignored.
*/
#ifndef DR_WAKEUP_MID
#define DR_WAKEUP_MID   0x1913
#else
#error "Message ID macro DR_WAKEUP_MID already defined!"
#endif


/*
** DR Housekeeping Telemetry
**
** DR generates housekeeping telemetry with this MID
** containing metadata relating to the application.
*/
#ifndef DR_HK_TLM_MID
#define DR_HK_TLM_MID   0x0912
#else
#error "Message ID macro DR_HK_TLM_MID already defined!"
#endif

  
/*
** DR Diagnosis
**
** DR sends diagnois results with this MID.
*/
#ifndef DR_DIAGNOSIS_MID
#define DR_DIAGNOSIS_MID   0x0913
#else
#error "Message ID macro DR_DIAGNOSIS_MID already defined!"
#endif

  
#endif /* DR_MSGIDS_H */

/************************/
/*  End of File Comment */
/************************/
