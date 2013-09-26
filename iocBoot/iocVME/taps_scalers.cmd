#!../../bin/linux-x86/VME

## You may have to change VME to something else
## everywhere it appears in this file

< envPaths

cd ${TOP}

## Register all support components
dbLoadDatabase "dbd/VME.dbd"
VME_registerRecordDeviceDriver pdbbase

## Load record instances
dbLoadRecords("db/vme_scaler.db","user=steffenHost")

cd ${TOP}/iocBoot/${IOC}
iocInit

## Start any sequence programs
#seq sncxxx,"user=steffenHost"