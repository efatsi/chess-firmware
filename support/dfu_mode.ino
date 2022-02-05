void DFUMode() {
  // From https://github.com/spark/cc3000-patch-programmer/blob/master/src/main.cpp
  //RESET INTO DFU MODE PLEASE!
  FLASH_OTA_Update_SysFlag = 0xFFFF;
  Save_SystemFlags();
  //THE BACKUP REGISTERS, THEY DO NOTHING!!
  //firmware state flag
  BKP_WriteBackupRegister(BKP_DR10, 0xFFFF);
  //system health -- SET_SYS_HEALTH(0xFFFF);
  BKP_WriteBackupRegister(BKP_DR1, 0xFFFF);
  //OTA_FLASHED_Status_SysFlag
  USB_Cable_Config(DISABLE);
  NVIC_SystemReset();
}
