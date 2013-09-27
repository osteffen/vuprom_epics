/**
 * @brief A2 VME scaler read out for EPICS
 * @author Oliver Steffen <steffen@kph.uni-mainz.de>
 * @date Aug 30, 2013
 */

static char what[] =
"A2 VUPROM VME Scalers - O.Steffen <steffen@kph.uni-mainz.de>, 27. Sep 2013";

#define DEBUG_ON

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "alarm.h"
#include "cvtTable.h"
#include "dbDefs.h"
#include "dbAccess.h"
#include "recGbl.h"
#include "recSup.h"
#include "devSup.h"
#include "aiRecord.h"
#include "link.h"
#include "epicsExport.h"
#include "dbScan.h"
#include "devvme_scaler.h"
#include <sys/types.h>

#include "drv.h"

// @todo deinit driver?

/* Create the dset for devvme_second */
static long init_record();
static long init_ai();
static long read_ai();
static long get_ioint_info();

struct {
    long		number;
    DEVSUPFUN	report;
    DEVSUPFUN	init;
    DEVSUPFUN	init_record;
    DEVSUPFUN	get_ioint_info;
    DEVSUPFUN	read_ai;
    DEVSUPFUN	special_linconv;
} devvme_scaler = {
	6,
	NULL,
	init_ai,
	init_record,
    get_ioint_info,
	read_ai,
	NULL
};
epicsExportAddress(dset,devvme_scaler);


/************************************************************************/
/* vme_scaler Record								*/
/*  INP = "hostname-or-ipaddress:data-number"				*/
/************************************************************************/

/* init_ai for debug */

static long init_ai(int after)
{
    if( after == 0 ) {
        printf("%s\n", what);
        drv_init();
    } else if ( after == 1 ) {
        drv_start();
    }

    return(0);
}

static u_int32_t parseRecNumber( char* str ) {
    u_int32_t addr;
    int ret = sscanf( str,"Addr:%x", &addr);

    if( ret == 1 )
        return addr;
    else
        return 0;
}

static long init_record(struct aiRecord *pai)
{

    const u_int32_t addr = parseRecNumber( pai->inp.text );

    if( addr == 0 ) {
        printf("Invalud address: %#010x for %s\n", addr, pai->name);
        return 1;
    }

    u_int32_t* ptr = drv_AddRecord(addr);

    if( ptr ) {
        pai->dpvt = (void*) ptr;
        pai->udf = FALSE;
        printf("%s: Addr= %#010x\n", pai->name, addr);
        return 0;
    } else
        return 1;
}


static long read_ai(struct aiRecord *pai)
{
    if( pai->dpvt ) {
        pai->val = *((u_int32_t*) pai->dpvt);
        pai->udf = FALSE;
        return TRUE;
    } else
        return FALSE;
}

static int n=0;

static long get_ioint_info(int cmd,struct dbCommon *precord, IOSCANPVT *ppvt) {

    IOSCANPVT* ioinfo = NULL;

    switch (cmd) {
    case 0:

        ioinfo = drv_getioinfo();

        if( ioinfo ) {
            if (n==0 ) scanIoInit(ioinfo);  //only once! not for every record!
            ++n;
            *ppvt = *ioinfo;
            drv_enable_iointr();
            printf("I/O Intr enabled for %s\n", precord->name);
        } else
            puts("Error settinf I/O Intr\n");
        break;
    case 1:
        drv_disable_iointr();
        printf("I/O Intr disabled\n");
        break;
    default:
        printf("Error: unknown command %d\n", cmd);
    }

    return 0;
}
