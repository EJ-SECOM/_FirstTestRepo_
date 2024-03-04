/*  
-------------------------------------------------------------------------
COPYRIGHT (C) BERGER LAHR GmbH 2003. All rights reserved.
-------------------------------------------------------------------------
$Id: //servo3/code/main/prog/PR0912-00/main.cpp#12 $

$Date: 2012/05/15 $

-------------------------------------------------------------------------
DESCRIPTION

   start up after bootloader and mainloop.

-------------------------------------------------------------------------
DATE     WHO   WHAT
28082003 herrh initial revision
23122004 wurtr order of calling dco-task and can-task changed in main loop
05112006 wurtr Moved the detailed functionality to basMain.cpp
23122008 petrj swinfo.h added
-------------------------------------------------------------------------
*/

#include "sysconf.h"
#include "ctarget.h"
#include "cpu.h" 

#include "bas_bgf.h"
#include "0version.h"
#include "glbapicnst.h"
#include "pms.hpp"
#include "ioc.hpp"
#include "param_user.h"
#include "objdefs.h"
#include "adm.hpp"

#define MEML_CRC_ADR    (UINT32) &ChecksumDummy
#define MEML_FW_ADR     0x0033FFF6
#define MEML_NO_MEM     2
#define MEML_CONF_BITS  (CONF_BIT_HMI BOR CONF_BIT_EXTFLASH)

#include "swinfo.h"


/*----------------------------------------------------------------------*/
/*CFUNCTION*/ COMPGLOBAL INT16   main
(
)
/* FUNCTIONAL_DESCRIPTION

   This function is called by the the library function c_int00 which is
   called after reset CPU.

   The complete startup according to the configuration in sysconf.h
   and also the mainloop are implemented in basMain.cpp

   The different startup steps can be extended for any application's
   special requirements:
   - Insert your functionality between the functions below
   - or copy the needed function from basMain.cpp to here and modify it
*/
{ 

    bas_mainInitHardware();       // Initialize the hardware drivers

    bas_mainCreateComponents();   // create all needed components with the new operator

    bas_mainInitComponents();     // initialze the components calling Component::init()

    bas_mainStartScheduler();     // start the scheduler by enabling the interrupt system

    bas_mainStartComponents();    // start the components calling Component::start()

    bas_mainFinishStartup();      // do last initialization things before entering the main loop

    // Set device control to predefined value and activate corresponding Flexible IO processing
    if (theParamUserNoReset.uiDeviceDevControl EQ DEVICE_DEVCONTROL_NONE)
    {
       UINT32 ulValue = DEVICE_DEVCONTROL_FIELDBUS;
       theAdm->writeNoPmsDevControl(ulValue);   // Use this function to avoid switching into state 2
       //thePms->executeParamWrite(CH_INTERNAL,O_DEVICE_DEVCONTROL,&ulValue);
       theIoc->BootInitIoFunctions();
    }

    // main loop
    while ( TRUE )
    {  
       bas_mainLoopFunction();    // main loop functions of all configured components
    }
    // never reached
}


/*-END-OF-FILE-*/


