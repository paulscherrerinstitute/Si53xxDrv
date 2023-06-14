/* THIS FILE WAS AUTOMATICALLY GENERATED (reg_extract.py) -- DO NOT MODIFY */
#include <Si53xx.h>
namespace Si53xx {
SettingVec Si5395Settings = {
	Setting::mkSetting("PAGE","R/W",7,0,{0x0001}),
	Setting::mkSetting("PN_BASE","R/O",15,0,{0x0002,0x0003}),
	Setting::mkSetting("GRADE","R/O",7,0,{0x0004}),
	Setting::mkSetting("DEVICE_REV","R/O",7,0,{0x0005}),
	Setting::mkSetting("TEMP_GRADE","R/W",7,0,{0x0009}),
	Setting::mkSetting("PKG_ID","R/W",7,0,{0x000a}),
	Setting::mkSetting("I2C_ADDR","R/W",6,2,{0x000b}),
	Setting::mkSetting("SYSINCAL","R/O",0,0,{0x000c}),
	Setting::mkSetting("LOSXAXB","R/O",1,1,{0x000c}),
	Setting::mkSetting("XAXB_ERR","R/O",3,3,{0x000c}),
	Setting::mkSetting("SMBUS_TIMEOUT","R/O",5,5,{0x000c}),
	Setting::mkSetting("LOS","R/O",3,0,{0x000d}),
	Setting::mkSetting("OOF","R/O",7,4,{0x000d}),
	Setting::mkSetting("LOL","R/O",1,1,{0x000e}),
	Setting::mkSetting("HOLD","R/O",5,5,{0x000e}),
	Setting::mkSetting("CAL_PLL","R/O",5,5,{0x000f}),
	Setting::mkSetting("SYSINCAL_FLG","R/W",0,0,{0x0011}),
	Setting::mkSetting("LOSXAXB_FLG","R/W",1,1,{0x0011}),
	Setting::mkSetting("XAXB_ERR_FLG","R/W",3,3,{0x0011}),
	Setting::mkSetting("SMBUS_TIMEOUT_FLG","R/W",5,5,{0x0011}),
	Setting::mkSetting("LOS_FLG","R/W",3,0,{0x0012}),
	Setting::mkSetting("OOF_FLG","R/W",7,4,{0x0012}),
	Setting::mkSetting("LOL_FLG","R/W",1,1,{0x0013}),
	Setting::mkSetting("HOLD_FLG","R/W",5,5,{0x0013}),
	Setting::mkSetting("CAL_FLG_PLL","R/W",5,5,{0x0014}),
	Setting::mkSetting("LOL_ON_HOLD","R/W",1,1,{0x0016}),
	Setting::mkSetting("SYSINCAL_INTR_MSK","R/W",0,0,{0x0017}),
	Setting::mkSetting("LOSXAXB_INTR_MSK","R/W",1,1,{0x0017}),
	Setting::mkSetting("SMBUS_TIMEOUT_INTR_MSK","R/W",5,5,{0x0017}),
	Setting::mkSetting("LOS_INTR_MSK","R/W",3,0,{0x0018}),
	Setting::mkSetting("OOF_INTR_MSK","R/W",7,4,{0x0018}),
	Setting::mkSetting("LOL_INTR_MSK","R/W",1,1,{0x0019}),
	Setting::mkSetting("HOLD_INTR_MSK","R/W",5,5,{0x0019}),
	Setting::mkSetting("CAL_INTR_MSK","R/W",5,5,{0x001a}),
	Setting::mkSetting("SOFT_RST_ALL","S",0,0,{0x001c}),
	Setting::mkSetting("SOFT_RST","S",2,2,{0x001c}),
	Setting::mkSetting("FINC","S",0,0,{0x001d}),
	Setting::mkSetting("FDEC","S",1,1,{0x001d}),
	Setting::mkSetting("PDN","R/W",0,0,{0x001e}),
	Setting::mkSetting("HARD_RST","R/W",1,1,{0x001e}),
	Setting::mkSetting("SYNC","S",2,2,{0x001e}),
	Setting::mkSetting("SPI_3WIRE","R/W",3,3,{0x002b}),
	Setting::mkSetting("AUTO_NDIV_UPDATE","R/W",5,5,{0x002b}),
	Setting::mkSetting("LOS_EN","R/W",3,0,{0x002c}),
	Setting::mkSetting("LOSXAXB_DIS","R/W",4,4,{0x002c}),
	Setting::mkSetting("LOS0_VAL_TIME","R/W",1,0,{0x002d}),
	Setting::mkSetting("LOS1_VAL_TIME","R/W",3,2,{0x002d}),
	Setting::mkSetting("LOS2_VAL_TIME","R/W",5,4,{0x002d}),
	Setting::mkSetting("LOS3_VAL_TIME","R/W",7,6,{0x002d}),
	Setting::mkSetting("LOS0_TRG_THR","R/W",15,0,{0x002e,0x002f}),
	Setting::mkSetting("LOS1_TRG_THR","R/W",15,0,{0x0030,0x0031}),
	Setting::mkSetting("LOS2_TRG_THR","R/W",15,0,{0x0032,0x0033}),
	Setting::mkSetting("LOS3_TRG_THR","R/W",15,0,{0x0034,0x0035}),
	Setting::mkSetting("LOS0_CLR_THR","R/W",15,0,{0x0036,0x0037}),
	Setting::mkSetting("LOS1_CLR_THR","R/W",15,0,{0x0038,0x0039}),
	Setting::mkSetting("LOS2_CLR_THR","R/W",15,0,{0x003a,0x003b}),
	Setting::mkSetting("LOS3_CLR_THR","R/W",15,0,{0x003c,0x003d}),
	Setting::mkSetting("LOS_MIN_PERIOD_EN","R/W",7,4,{0x003e}),
	Setting::mkSetting("OOF_EN","R/W",3,0,{0x003f}),
	Setting::mkSetting("FAST_OOF_EN","R/W",7,4,{0x003f}),
	Setting::mkSetting("OOF_REF_SEL","R/W",2,0,{0x0040}),
	Setting::mkSetting("OOF0_DIV_SEL","R/W",4,0,{0x0041}),
	Setting::mkSetting("OOF1_DIV_SEL","R/W",4,0,{0x0042}),
	Setting::mkSetting("OOF2_DIV_SEL","R/W",4,0,{0x0043}),
	Setting::mkSetting("OOF3_DIV_SEL","R/W",4,0,{0x0044}),
	Setting::mkSetting("OOFXO_DIV_SEL","R/W",4,0,{0x0045}),
	Setting::mkSetting("OOF0_SET_THR","R/W",7,0,{0x0046}),
	Setting::mkSetting("OOF1_SET_THR","R/W",7,0,{0x0047}),
	Setting::mkSetting("OOF2_SET_THR","R/W",7,0,{0x0048}),
	Setting::mkSetting("OOF3_SET_THR","R/W",7,0,{0x0049}),
	Setting::mkSetting("OOF0_CLR_THR","R/W",7,0,{0x004a}),
	Setting::mkSetting("OOF1_CLR_THR","R/W",7,0,{0x004b}),
	Setting::mkSetting("OOF2_CLR_THR","R/W",7,0,{0x004c}),
	Setting::mkSetting("OOF3_CLR_THR","R/W",7,0,{0x004d}),
	Setting::mkSetting("OOF0_DETWIN_SEL","R/W",2,0,{0x004e}),
	Setting::mkSetting("OOF1_DETWIN_SEL","R/W",6,4,{0x004e}),
	Setting::mkSetting("OOF2_DETWIN_SEL","R/W",2,0,{0x004f}),
	Setting::mkSetting("OOF3_DETWIN_SEL","R/W",6,4,{0x004f}),
	Setting::mkSetting("OOF_ON_LOS","R/W",3,0,{0x0050}),
	Setting::mkSetting("FAST_OOF0_SET_THR","R/W",3,0,{0x0051}),
	Setting::mkSetting("FAST_OOF1_SET_THR","R/W",3,0,{0x0052}),
	Setting::mkSetting("FAST_OOF2_SET_THR","R/W",3,0,{0x0053}),
	Setting::mkSetting("FAST_OOF3_SET_THR","R/W",3,0,{0x0054}),
	Setting::mkSetting("FAST_OOF0_CLR_THR","R/W",3,0,{0x0055}),
	Setting::mkSetting("FAST_OOF1_CLR_THR","R/W",3,0,{0x0056}),
	Setting::mkSetting("FAST_OOF2_CLR_THR","R/W",3,0,{0x0057}),
	Setting::mkSetting("FAST_OOF3_CLR_THR","R/W",3,0,{0x0058}),
	Setting::mkSetting("FAST_OOF0_DETWIN_SEL","R/W",1,0,{0x0059}),
	Setting::mkSetting("FAST_OOF1_DETWIN_SEL","R/W",3,2,{0x0059}),
	Setting::mkSetting("FAST_OOF2_DETWIN_SEL","R/W",5,4,{0x0059}),
	Setting::mkSetting("FAST_OOF3_DETWIN_SEL","R/W",7,6,{0x0059}),
	Setting::mkSetting("OOF0_RATIO_REF","R/W",25,0,{0x005a,0x005b,0x005c,0x005d}),
	Setting::mkSetting("OOF1_RATIO_REF","R/W",25,0,{0x005e,0x005f,0x0060,0x0061}),
	Setting::mkSetting("OOF2_RATIO_REF","R/W",25,0,{0x0062,0x0063,0x0064,0x0065}),
	Setting::mkSetting("OOF3_RATIO_REF","R/W",25,0,{0x0066,0x0067,0x0068,0x0069}),
	Setting::mkSetting("LOL_FST_EN","R/W",1,1,{0x0092}),
	Setting::mkSetting("LOL_FST_DETWIN_SEL","R/W",7,4,{0x0093}),
	Setting::mkSetting("LOL_FST_VALWIN_SEL","R/W",3,2,{0x0095}),
	Setting::mkSetting("LOL_FST_SET_THR_SEL","R/W",7,4,{0x0096}),
	Setting::mkSetting("LOL_FST_CLR_THR_SEL","R/W",7,4,{0x0098}),
	Setting::mkSetting("LOL_SLOW_EN_PLL","R/W",1,1,{0x009a}),
	Setting::mkSetting("LOL_SLW_DETWIN_SEL","R/W",7,4,{0x009b}),
	Setting::mkSetting("LOL_SLW_VALWIN_SEL","R/W",3,2,{0x009d}),
	Setting::mkSetting("LOL_SLW_SET_THR","R/W",7,4,{0x009e}),
	Setting::mkSetting("LOL_SLW_CLR_THR","R/W",7,4,{0x00a0}),
	Setting::mkSetting("LOL_TIMER_EN","R/W",1,1,{0x00a2}),
	Setting::mkSetting("LOL_CLR_DELAY_DIV256","R/W",28,0,{0x00a9,0x00aa,0x00ab,0x00ac}),
	Setting::mkSetting("ACTIVE_NVM_BANK","R/O",7,0,{0x00e2}),
	Setting::mkSetting("NVM_WRITE","R/W",7,0,{0x00e3}),
	Setting::mkSetting("NVM_READ_BANK","S",0,0,{0x00e4}),
	Setting::mkSetting("FASTLOCK_EXTEND_EN","R/W",5,5,{0x00e5}),
	Setting::mkSetting("FASTLOCK_EXTEND","R/W",28,0,{0x00ea,0x00eb,0x00ec,0x00ed}),
	Setting::mkSetting("REG_0XF7_INTR","R/O",0,0,{0x00f6}),
	Setting::mkSetting("REG_0XF8_INTR","R/O",1,1,{0x00f6}),
	Setting::mkSetting("REG_0XF9_INTR","R/O",2,2,{0x00f6}),
	Setting::mkSetting("SYSINCAL_INTR","R/O",0,0,{0x00f7}),
	Setting::mkSetting("LOSXAXB_INTR","R/O",1,1,{0x00f7}),
	Setting::mkSetting("LOSREF_INTR","R/O",2,2,{0x00f7}),
	Setting::mkSetting("LOSVCO_INTR","R/O",4,4,{0x00f7}),
	Setting::mkSetting("SMBUS_TIME_OUT_INTR","R/O",5,5,{0x00f7}),
	Setting::mkSetting("LOS_INTR","R/O",3,0,{0x00f8}),
	Setting::mkSetting("OOF_INTR","R/O",7,4,{0x00f8}),
	Setting::mkSetting("LOL_INTR","R/O",1,1,{0x00f9}),
	Setting::mkSetting("HOLD_INTR","R/O",5,5,{0x00f9}),
	Setting::mkSetting("DEVICE_READY","R/O",7,0,{0x00fe}),
	Setting::mkSetting("OUTALL_DISABLE_LOW","R/W",0,0,{0x0102}),
	Setting::mkSetting("OUT0A_PDN","R/W",0,0,{0x0103}),
	Setting::mkSetting("OUT0A_OE","R/W",1,1,{0x0103}),
	Setting::mkSetting("OUT0A_RDIV_FORCE2","R/W",2,2,{0x0103}),
	Setting::mkSetting("OUT0A_FORMAT","R/W",2,0,{0x0104}),
	Setting::mkSetting("OUT0A_SYNC_EN","R/W",3,3,{0x0104}),
	Setting::mkSetting("OUT0A_DIS_STATE","R/W",5,4,{0x0104}),
	Setting::mkSetting("OUT0A_CMOS_DRV","R/W",7,6,{0x0104}),
	Setting::mkSetting("OUT0A_CM","R/W",3,0,{0x0105}),
	Setting::mkSetting("OUT0A_AMPL","R/W",6,4,{0x0105}),
	Setting::mkSetting("OUT0A_MUX_SEL","R/W",2,0,{0x0106}),
	Setting::mkSetting("OUT0A_VDD_SEL_EN","R/W",3,3,{0x0106}),
	Setting::mkSetting("OUT0A_VDD_SEL","R/W",5,4,{0x0106}),
	Setting::mkSetting("OUT0A_INV","R/W",7,6,{0x0106}),
	Setting::mkSetting("OUTX_ALWAYS_ON","R/W",11,0,{0x013f,0x0140}),
	Setting::mkSetting("OUT_DIS_MSK","R/W",1,1,{0x0141}),
	Setting::mkSetting("OUT_DIS_LOL_MSK","R/W",5,5,{0x0141}),
	Setting::mkSetting("OUT_DIS_LOSXAXB_MSK","R/W",6,6,{0x0141}),
	Setting::mkSetting("OUT_DIS_MSK_LOS_PFD","R/W",7,7,{0x0141}),
	Setting::mkSetting("OUT_DIS_MSK_LOL","R/W",1,1,{0x0142}),
	Setting::mkSetting("OUT_DIS_MSK_HOLD","R/W",5,5,{0x0142}),
	Setting::mkSetting("OUT_PDN_ALL","R/W",0,0,{0x0145}),
	Setting::mkSetting("PXAXB","R/W",1,0,{0x0206}),
	Setting::mkSetting("P0_NUM","R/W",47,0,{0x0208,0x0209,0x020a,0x020b,0x020c,0x020d}),
	Setting::mkSetting("P0_DEN","R/W",31,0,{0x020e,0x020f,0x0210,0x0211}),
	Setting::mkSetting("P0_UPDATE","S",0,0,{0x0230}),
	Setting::mkSetting("P1_UPDATE","S",1,1,{0x0230}),
	Setting::mkSetting("P2_UPDATE","S",2,2,{0x0230}),
	Setting::mkSetting("P3_UPDATE","S",3,3,{0x0230}),
	Setting::mkSetting("P0_FRACN_MODE","R/W",3,0,{0x0231}),
	Setting::mkSetting("P0_FRAC_EN","R/W",4,4,{0x0231}),
	Setting::mkSetting("P1_FRACN_MODE","R/W",3,0,{0x0232}),
	Setting::mkSetting("P1_FRAC_EN","R/W",4,4,{0x0232}),
	Setting::mkSetting("P2_FRACN_MODE","R/W",3,0,{0x0233}),
	Setting::mkSetting("P2_FRAC_EN","R/W",4,4,{0x0233}),
	Setting::mkSetting("P3_FRACN_MODE","R/W",3,0,{0x0234}),
	Setting::mkSetting("P3_FRAC_EN","R/W",4,4,{0x0234}),
	Setting::mkSetting("MXAXB_NUM","R/W",43,0,{0x0235,0x0236,0x0237,0x0238,0x0239,0x023a}),
	Setting::mkSetting("MXAXB_DEN","R/W",31,0,{0x023b,0x023c,0x023d,0x023e}),
	Setting::mkSetting("MXAXB_UPDATE","S",0,0,{0x023f}),
	Setting::mkSetting("R0A_REG","R/W",23,0,{0x0247,0x0248,0x0249}),
	Setting::mkSetting("DESIGN_ID","R/W",63,0,{0x026b,0x026c,0x026d,0x026e,0x026f,0x0270,0x0271,0x0272}),
	Setting::mkSetting("OPN_ID","R/W",39,0,{0x0278,0x0279,0x027a,0x027b,0x027c}),
	Setting::mkSetting("OPN_REVISION","R/W",7,0,{0x027d}),
	Setting::mkSetting("BASELINE_ID","R/W",7,0,{0x027e}),
	Setting::mkSetting("OOF0_TRG_THR_EXT","R/W",4,0,{0x028a}),
	Setting::mkSetting("OOF1_TRG_THR_EXT","R/W",4,0,{0x028b}),
	Setting::mkSetting("OOF2_TRG_THR_EXT","R/W",4,0,{0x028c}),
	Setting::mkSetting("OOF3_TRG_THR_EXT","R/W",4,0,{0x028d}),
	Setting::mkSetting("OOF0_CLR_THR_EXT","R/W",4,0,{0x028e}),
	Setting::mkSetting("OOF1_CLR_THR_EXT","R/W",4,0,{0x028f}),
	Setting::mkSetting("OOF2_CLR_THR_EXT","R/W",4,0,{0x0290}),
	Setting::mkSetting("OOF3_CLR_THR_EXT","R/W",4,0,{0x0291}),
	Setting::mkSetting("OOF_STOP_ON_LOS","R/W",3,0,{0x0292}),
	Setting::mkSetting("OOF_CLEAR_ON_LOS","R/W",3,0,{0x0293}),
	Setting::mkSetting("FASTLOCK_EXTEND_SCL","R/W",7,4,{0x0294}),
	Setting::mkSetting("LOL_SLW_VALWIN_SELX","R/W",1,1,{0x0296}),
	Setting::mkSetting("FASTLOCK_DLY_ONSW_EN","R/W",1,1,{0x0297}),
	Setting::mkSetting("FASTLOCK_DLY_ONLOL_EN","R/W",1,1,{0x0299}),
	Setting::mkSetting("FASTLOCK_DLY_ONLOL","R/W",19,0,{0x029d,0x029e,0x029f}),
	Setting::mkSetting("FASTLOCK_DLY_ONSW","R/W",19,0,{0x02a9,0x02aa,0x02ab}),
	Setting::mkSetting("LOL_NOSIG_TIME","R/W",3,2,{0x02b7}),
	Setting::mkSetting("LOL_LOS_REFCLK","R/W",1,1,{0x02b8}),
	Setting::mkSetting("LOS_CMOS_MIN_PER_EN","R/W",7,6,{0x02bc}),
	Setting::mkSetting("N0_NUM","R/W",43,0,{0x0302,0x0303,0x0304,0x0305,0x0306,0x0307}),
	Setting::mkSetting("N0_DEN","R/W",31,0,{0x0308,0x0309,0x030a,0x030b}),
	Setting::mkSetting("N0_UPDATE","S",0,0,{0x030c}),
	Setting::mkSetting("N_UPDATE_ALL","S",1,1,{0x0338}),
	Setting::mkSetting("N4_UPDATE","S",0,0,{0x0338}),
	Setting::mkSetting("N_FSTEP_MSK","R/W",4,0,{0x0339}),
	Setting::mkSetting("N0_FSTEPW","R/W",43,0,{0x033b,0x033c,0x033d,0x033e,0x033f,0x0340}),
	Setting::mkSetting("ZDM_EN","R/W",0,0,{0x0487}),
	Setting::mkSetting("ZDM_IN_SEL","R/W",2,1,{0x0487}),
	Setting::mkSetting("ZDM_AUTOSW_EN","R/W",4,4,{0x0487}),
	Setting::mkSetting("IN_ACTV","R/O",7,6,{0x0507}),
	Setting::mkSetting("BW0_PLL","R/W",5,0,{0x0508}),
	Setting::mkSetting("BW1_PLL","R/W",5,0,{0x0509}),
	Setting::mkSetting("BW2_PLL","R/W",5,0,{0x050a}),
	Setting::mkSetting("BW3_PLL","R/W",5,0,{0x050b}),
	Setting::mkSetting("BW4_PLL","R/W",5,0,{0x050c}),
	Setting::mkSetting("BW5_PLL","R/W",5,0,{0x050d}),
	Setting::mkSetting("FAST_BW0_PLL","R/W",5,0,{0x050e}),
	Setting::mkSetting("FAST_BW1_PLL","R/W",5,0,{0x050f}),
	Setting::mkSetting("FAST_BW2_PLL","R/W",5,0,{0x0510}),
	Setting::mkSetting("FAST_BW3_PLL","R/W",5,0,{0x0511}),
	Setting::mkSetting("FAST_BW4_PLL","R/W",5,0,{0x0512}),
	Setting::mkSetting("FAST_BW5_PLL","R/W",5,0,{0x0513}),
	Setting::mkSetting("BW_UPDATE_PLL","S",0,0,{0x0514}),
	Setting::mkSetting("M_NUM","R/W",55,0,{0x0515,0x0516,0x0517,0x0518,0x0519,0x051a,0x051b}),
	Setting::mkSetting("M_DEN","R/W",31,0,{0x051c,0x051d,0x051e,0x051f}),
	Setting::mkSetting("M_UPDATE","S",0,0,{0x0520}),
	Setting::mkSetting("M_FRAC_MODE","R/W",3,0,{0x0521}),
	Setting::mkSetting("M_FRAC_EN","R/W",4,4,{0x0521}),
	Setting::mkSetting("Reserved","R/W",5,5,{0x0521}),
	Setting::mkSetting("IN_SEL_REGCTRL","R/W",0,0,{0x052a}),
	Setting::mkSetting("IN_SEL","R/W",2,1,{0x052a}),
	Setting::mkSetting("FASTLOCK_AUTO_EN","R/W",0,0,{0x052b}),
	Setting::mkSetting("FASTLOCK_MAN","R/W",1,1,{0x052b}),
	Setting::mkSetting("HOLD_EN","R/W",0,0,{0x052c}),
	Setting::mkSetting("HOLD_RAMP_BYP","R/W",3,3,{0x052c}),
	Setting::mkSetting("HOLDEXIT_BW_SEL1","R/W",4,4,{0x052c}),
	Setting::mkSetting("RAMP_STEP_INTERVAL","R/W",7,5,{0x052c}),
	Setting::mkSetting("HOLD_RAMPBYP_NOH-","R/W",1,1,{0x052d}),
	Setting::mkSetting("HOLD_HIST_LEN","R/W",4,0,{0x052e}),
	Setting::mkSetting("HOLD_HIST_DELAY","R/W",4,0,{0x052f}),
	Setting::mkSetting("HOLD_REF_COUNT_FRC","R/W",4,0,{0x0531}),
	Setting::mkSetting("HOLD_15M_CYC_COUNT","R/W",23,0,{0x0532,0x0533,0x0534}),
	Setting::mkSetting("FORCE_HOLD","R/W",0,0,{0x0535}),
	Setting::mkSetting("CLK_SWTCH_MODE","R/W",1,0,{0x0536}),
	Setting::mkSetting("HSW_EN","R/W",2,2,{0x0536}),
	Setting::mkSetting("IN_LOS_MSK","R/W",3,0,{0x0537}),
	Setting::mkSetting("IN_OOF_MSK","R/W",7,4,{0x0537}),
	Setting::mkSetting("IN0_PRIORITY","R/W",2,0,{0x0538}),
	Setting::mkSetting("IN1_PRIORITY","R/W",6,4,{0x0538}),
	Setting::mkSetting("IN2_PRIORITY","R/W",2,0,{0x0539}),
	Setting::mkSetting("IN3_PRIORITY","R/W",6,4,{0x0539}),
	Setting::mkSetting("HSW_MODE","R/W",1,0,{0x053a}),
	Setting::mkSetting("HSW_PHMEAS_CTRL","R/W",3,2,{0x053a}),
	Setting::mkSetting("HSW_PHMEAS_THR","R/W",9,0,{0x053b,0x053c}),
	Setting::mkSetting("HSW_COARSE_PM_LEN","R/W",4,0,{0x053d}),
	Setting::mkSetting("HSW_COARSE_PM_DLY","R/W",4,0,{0x053e}),
	Setting::mkSetting("HOLD_HIST_VALID","R/O",1,1,{0x053f}),
	Setting::mkSetting("FASTLOCK_STATUS","R/O",2,2,{0x053f}),
	Setting::mkSetting("AMBLE2","R/W",7,0,{0x0540}),
	Setting::mkSetting("HSW_FINE_PM_LEN","R/W",3,0,{0x0588}),
	Setting::mkSetting("PFD_EN_DELAY","R/W",12,0,{0x0589,0x058a}),
	Setting::mkSetting("HSW_MEAS_SETTLE","R/W",19,0,{0x058b,0x058c,0x058d}),
	Setting::mkSetting("INIT_LP_CLOSE_HO","R/W",1,1,{0x059b}),
	Setting::mkSetting("HOLD_PRESERVE_HIST","R/W",4,4,{0x059b}),
	Setting::mkSetting("HOLD_FRZ_WITH_INTONLY","R/W",5,5,{0x059b}),
	Setting::mkSetting("HOLDEXIT_BW_SEL0","R/W",6,6,{0x059b}),
	Setting::mkSetting("HOLDEXIT_STD_BO","R/W",7,7,{0x059b}),
	Setting::mkSetting("HOLD_RAMPBP_NOHIST","R/W",7,7,{0x059c}),
	Setting::mkSetting("HOLDEXIT_ST_BO","R/W",6,6,{0x059c}),
	Setting::mkSetting("HOLDEXIT_BW0","R/W",5,0,{0x059d}),
	Setting::mkSetting("HOLDEXIT_BW1","R/W",5,0,{0x059e}),
	Setting::mkSetting("HOLDEXIT_BW2","R/W",5,0,{0x059f}),
	Setting::mkSetting("HOLDEXIT_BW3","R/W",5,0,{0x05a0}),
	Setting::mkSetting("HOLDEXIT_BW4","R/W",5,0,{0x05a1}),
	Setting::mkSetting("HOLDEXIT_BW5","R/W",5,0,{0x05a2}),
	Setting::mkSetting("HSW_LIMIT","R/W",7,0,{0x05a4}),
	Setting::mkSetting("HSW_LIMIT_ACTION","R/W",0,0,{0x05a5}),
	Setting::mkSetting("RAMP_STEP_SIZE","R/W",2,0,{0x05a6}),
	Setting::mkSetting("RAMP_SWITCH_EN","R/W",3,3,{0x05a6}),
	Setting::mkSetting("OUT_MAX_LIMIT_EN","R/W",0,0,{0x05ac}),
	Setting::mkSetting("HOLD_SETTLE_DET_EN","R/W",3,3,{0x05ac}),
	Setting::mkSetting("OUT_MAX_LIMIT_LMT","R/W",15,0,{0x05ad,0x05ae}),
	Setting::mkSetting("HOLD_SETTLE_TARGET","R/W",15,0,{0x05b1,0x05b2}),
	Setting::mkSetting("XAXB_EXTCLK_EN","R/W",0,0,{0x090e}),
	Setting::mkSetting("IO_VDD_SEL","R/W",0,0,{0x0943}),
	Setting::mkSetting("IN_EN","R/W",3,0,{0x0949}),
	Setting::mkSetting("IN_PULSED_CMOS_EN","R/W",7,4,{0x0949}),
	Setting::mkSetting("INX_TO_PFD_EN","R/W",3,0,{0x094a}),
	Setting::mkSetting("REFCLK_HYS_SEL","R/W",11,0,{0x094e,0x094f}),
	Setting::mkSetting("IN_CMOS_USE1P8","R/W",7,4,{0x094f}),
	Setting::mkSetting("MXAXB_INTEGER","R/W",0,0,{0x095e}),
	Setting::mkSetting("N_ADD_0P5","R/W",4,0,{0x0a02}),
	Setting::mkSetting("N_CLK_TO_OUTX_EN","R/W",4,0,{0x0a03}),
	Setting::mkSetting("N_PIBYP","R/W",4,0,{0x0a04}),
	Setting::mkSetting("N_PDNB","R/W",4,0,{0x0a05}),
	Setting::mkSetting("N0_HIGH_FREQ","R/W",3,3,{0x0a14}),
	Setting::mkSetting("N1_HIGH_FREQ","R/W",3,3,{0x0a1a}),
	Setting::mkSetting("N2_HIGH_FREQ","R/W",3,3,{0x0a20}),
	Setting::mkSetting("N3_HIGH_FREQ","R/W",3,3,{0x0a26}),
	Setting::mkSetting("N4_HIGH_FREQ","R/W",3,3,{0x0a2c}),
	Setting::mkSetting("N0_PHASE_STEP","R/W",7,0,{0x0a38}),
	Setting::mkSetting("N0_PHASE_COUNT","R/W",15,0,{0x0a39,0x0a3a}),
	Setting::mkSetting("N0_PHASE_INC","R/W",0,0,{0x0a3b}),
	Setting::mkSetting("N0_PHASE_DEC","R/W",1,1,{0x0a3b}),
	Setting::mkSetting("N1_PHASE_STEP","R/W",7,0,{0x0a3c}),
	Setting::mkSetting("N1_PHASE_COUNT","R/W",15,0,{0x0a3d,0x0a3e}),
	Setting::mkSetting("N1_PHASE_INC","R/W",0,0,{0x0a3f}),
	Setting::mkSetting("N1_PHASE_DEC","R/W",1,1,{0x0a3f}),
	Setting::mkSetting("N2_PHASE_STEP","R/W",7,0,{0x0a40}),
	Setting::mkSetting("N2_PHASE_COUNT","R/W",15,0,{0x0a41,0x0a42}),
	Setting::mkSetting("N2_PHASE_INC","R/W",0,0,{0x0a43}),
	Setting::mkSetting("N2_PHASE_DEC","R/W",1,1,{0x0a43}),
	Setting::mkSetting("N3_PHASE_STEP","R/W",7,0,{0x0a44}),
	Setting::mkSetting("N3_PHASE_COUNT","R/W",15,0,{0x0a45,0x0a46}),
	Setting::mkSetting("N3_PHASE_INC","R/W",0,0,{0x0a47}),
	Setting::mkSetting("N3_PHASE_DEC","R/W",1,1,{0x0a47}),
	Setting::mkSetting("N4_PHASE_STEP","R/W",7,0,{0x0a48}),
	Setting::mkSetting("N4_PHASE_COUNT","R/W",15,0,{0x0a49,0x0a4a}),
	Setting::mkSetting("N4_PHASE_INC","R/W",0,0,{0x0a4b}),
	Setting::mkSetting("N4_PHASE_DEC","R/W",1,1,{0x0a4b}),
	Setting::mkSetting("AMBLE0","R/W",7,0,{0x0b24}),
	Setting::mkSetting("AMBLE1","R/W",7,0,{0x0b25}),
	Setting::mkSetting("MS_OD_G_TIMEOUT","R/W",6,0,{0x0b2e}),
	Setting::mkSetting("MS_OD_G_TIMEOUT_EN","R/W",7,7,{0x0b2e}),
	Setting::mkSetting("PDIV_FRACN_CLK_DIS","R/W",3,0,{0x0b44}),
	Setting::mkSetting("FRACN_CLK_DIS_PLL","R/W",5,5,{0x0b44}),
	Setting::mkSetting("LOS_CLK_DIS","R/W",3,0,{0x0b46}),
	Setting::mkSetting("OOF_CLK_DIS","R/W",4,0,{0x0b47}),
	Setting::mkSetting("OOF_DIV_CLK_DIS","R/W",4,0,{0x0b48}),
	Setting::mkSetting("N_CLK_DIS","R/W",4,0,{0x0b4a}),
	Setting::mkSetting("VCO_RESET_CALCODE","R/W",11,0,{0x0b57,0x0b58}),
	Setting::mkSetting("VAL_DIV_CTL0","R/W",2,0,{0x0c02}),
	Setting::mkSetting("VAL_DIV_CTL1","R/W",4,4,{0x0c02}),
	Setting::mkSetting("IN_CLK_VAL_PWR_UP_DIS","R/W",3,0,{0x0c03}),
	Setting::mkSetting("IN_CLK_VAL_EN","R/W",0,0,{0x0c07}),
	Setting::mkSetting("IN_CLK_VAL_TIME","R/W",7,0,{0x0c08}),
	Setting::mkSetting("OUT0_PDN","R/W",0,0,{0x0108}),
	Setting::mkSetting("OUT0_OE","R/W",1,1,{0x0108}),
	Setting::mkSetting("OUT0_RDIV_FORCE2","R/W",2,2,{0x0108}),
	Setting::mkSetting("OUT0_FORMAT","R/W",2,0,{0x0109}),
	Setting::mkSetting("OUT0_SYNC_EN","R/W",3,3,{0x0109}),
	Setting::mkSetting("OUT0_DIS_STATE","R/W",5,4,{0x0109}),
	Setting::mkSetting("OUT0_CMOS_DRV","R/W",7,6,{0x0109}),
	Setting::mkSetting("OUT0_CM","R/W",3,0,{0x010a}),
	Setting::mkSetting("OUT0_AMPL","R/W",6,4,{0x010a}),
	Setting::mkSetting("OUT0_MUX_SEL","R/W",2,0,{0x010b}),
	Setting::mkSetting("OUT0_VDD_SEL_EN","R/W",3,3,{0x010b}),
	Setting::mkSetting("OUT0_VDD_SEL","R/W",5,4,{0x010b}),
	Setting::mkSetting("OUT0_INV","R/W",7,6,{0x010b}),
	Setting::mkSetting("OUT1_PDN","R/W",0,0,{0x010d}),
	Setting::mkSetting("OUT1_OE","R/W",1,1,{0x010d}),
	Setting::mkSetting("OUT1_RDIV_FORCE2","R/W",2,2,{0x010d}),
	Setting::mkSetting("OUT1_FORMAT","R/W",2,0,{0x010e}),
	Setting::mkSetting("OUT1_SYNC_EN","R/W",3,3,{0x010e}),
	Setting::mkSetting("OUT1_DIS_STATE","R/W",5,4,{0x010e}),
	Setting::mkSetting("OUT1_CMOS_DRV","R/W",7,6,{0x010e}),
	Setting::mkSetting("OUT1_CM","R/W",3,0,{0x010f}),
	Setting::mkSetting("OUT1_AMPL","R/W",6,4,{0x010f}),
	Setting::mkSetting("OUT1_MUX_SEL","R/W",2,0,{0x0110}),
	Setting::mkSetting("OUT1_VDD_SEL_EN","R/W",3,3,{0x0110}),
	Setting::mkSetting("OUT1_VDD_SEL","R/W",5,4,{0x0110}),
	Setting::mkSetting("OUT1_INV","R/W",7,6,{0x0110}),
	Setting::mkSetting("OUT2_PDN","R/W",0,0,{0x0112}),
	Setting::mkSetting("OUT2_OE","R/W",1,1,{0x0112}),
	Setting::mkSetting("OUT2_RDIV_FORCE2","R/W",2,2,{0x0112}),
	Setting::mkSetting("OUT2_FORMAT","R/W",2,0,{0x0113}),
	Setting::mkSetting("OUT2_SYNC_EN","R/W",3,3,{0x0113}),
	Setting::mkSetting("OUT2_DIS_STATE","R/W",5,4,{0x0113}),
	Setting::mkSetting("OUT2_CMOS_DRV","R/W",7,6,{0x0113}),
	Setting::mkSetting("OUT2_CM","R/W",3,0,{0x0114}),
	Setting::mkSetting("OUT2_AMPL","R/W",6,4,{0x0114}),
	Setting::mkSetting("OUT2_MUX_SEL","R/W",2,0,{0x0115}),
	Setting::mkSetting("OUT2_VDD_SEL_EN","R/W",3,3,{0x0115}),
	Setting::mkSetting("OUT2_VDD_SEL","R/W",5,4,{0x0115}),
	Setting::mkSetting("OUT2_INV","R/W",7,6,{0x0115}),
	Setting::mkSetting("OUT3_PDN","R/W",0,0,{0x0117}),
	Setting::mkSetting("OUT3_OE","R/W",1,1,{0x0117}),
	Setting::mkSetting("OUT3_RDIV_FORCE2","R/W",2,2,{0x0117}),
	Setting::mkSetting("OUT3_FORMAT","R/W",2,0,{0x0118}),
	Setting::mkSetting("OUT3_SYNC_EN","R/W",3,3,{0x0118}),
	Setting::mkSetting("OUT3_DIS_STATE","R/W",5,4,{0x0118}),
	Setting::mkSetting("OUT3_CMOS_DRV","R/W",7,6,{0x0118}),
	Setting::mkSetting("OUT3_CM","R/W",3,0,{0x0119}),
	Setting::mkSetting("OUT3_AMPL","R/W",6,4,{0x0119}),
	Setting::mkSetting("OUT3_MUX_SEL","R/W",2,0,{0x011a}),
	Setting::mkSetting("OUT3_VDD_SEL_EN","R/W",3,3,{0x011a}),
	Setting::mkSetting("OUT3_VDD_SEL","R/W",5,4,{0x011a}),
	Setting::mkSetting("OUT3_INV","R/W",7,6,{0x011a}),
	Setting::mkSetting("OUT4_PDN","R/W",0,0,{0x011c}),
	Setting::mkSetting("OUT4_OE","R/W",1,1,{0x011c}),
	Setting::mkSetting("OUT4_RDIV_FORCE2","R/W",2,2,{0x011c}),
	Setting::mkSetting("OUT4_FORMAT","R/W",2,0,{0x011d}),
	Setting::mkSetting("OUT4_SYNC_EN","R/W",3,3,{0x011d}),
	Setting::mkSetting("OUT4_DIS_STATE","R/W",5,4,{0x011d}),
	Setting::mkSetting("OUT4_CMOS_DRV","R/W",7,6,{0x011d}),
	Setting::mkSetting("OUT4_CM","R/W",3,0,{0x011e}),
	Setting::mkSetting("OUT4_AMPL","R/W",6,4,{0x011e}),
	Setting::mkSetting("OUT4_MUX_SEL","R/W",2,0,{0x011f}),
	Setting::mkSetting("OUT4_VDD_SEL_EN","R/W",3,3,{0x011f}),
	Setting::mkSetting("OUT4_VDD_SEL","R/W",5,4,{0x011f}),
	Setting::mkSetting("OUT4_INV","R/W",7,6,{0x011f}),
	Setting::mkSetting("OUT5_PDN","R/W",0,0,{0x0121}),
	Setting::mkSetting("OUT5_OE","R/W",1,1,{0x0121}),
	Setting::mkSetting("OUT5_RDIV_FORCE2","R/W",2,2,{0x0121}),
	Setting::mkSetting("OUT5_FORMAT","R/W",2,0,{0x0122}),
	Setting::mkSetting("OUT5_SYNC_EN","R/W",3,3,{0x0122}),
	Setting::mkSetting("OUT5_DIS_STATE","R/W",5,4,{0x0122}),
	Setting::mkSetting("OUT5_CMOS_DRV","R/W",7,6,{0x0122}),
	Setting::mkSetting("OUT5_CM","R/W",3,0,{0x0123}),
	Setting::mkSetting("OUT5_AMPL","R/W",6,4,{0x0123}),
	Setting::mkSetting("OUT5_MUX_SEL","R/W",2,0,{0x0124}),
	Setting::mkSetting("OUT5_VDD_SEL_EN","R/W",3,3,{0x0124}),
	Setting::mkSetting("OUT5_VDD_SEL","R/W",5,4,{0x0124}),
	Setting::mkSetting("OUT5_INV","R/W",7,6,{0x0124}),
	Setting::mkSetting("OUT6_PDN","R/W",0,0,{0x0126}),
	Setting::mkSetting("OUT6_OE","R/W",1,1,{0x0126}),
	Setting::mkSetting("OUT6_RDIV_FORCE2","R/W",2,2,{0x0126}),
	Setting::mkSetting("OUT6_FORMAT","R/W",2,0,{0x0127}),
	Setting::mkSetting("OUT6_SYNC_EN","R/W",3,3,{0x0127}),
	Setting::mkSetting("OUT6_DIS_STATE","R/W",5,4,{0x0127}),
	Setting::mkSetting("OUT6_CMOS_DRV","R/W",7,6,{0x0127}),
	Setting::mkSetting("OUT6_CM","R/W",3,0,{0x0128}),
	Setting::mkSetting("OUT6_AMPL","R/W",6,4,{0x0128}),
	Setting::mkSetting("OUT6_MUX_SEL","R/W",2,0,{0x0129}),
	Setting::mkSetting("OUT6_VDD_SEL_EN","R/W",3,3,{0x0129}),
	Setting::mkSetting("OUT6_VDD_SEL","R/W",5,4,{0x0129}),
	Setting::mkSetting("OUT6_INV","R/W",7,6,{0x0129}),
	Setting::mkSetting("OUT7_PDN","R/W",0,0,{0x012b}),
	Setting::mkSetting("OUT7_OE","R/W",1,1,{0x012b}),
	Setting::mkSetting("OUT7_RDIV_FORCE2","R/W",2,2,{0x012b}),
	Setting::mkSetting("OUT7_FORMAT","R/W",2,0,{0x012c}),
	Setting::mkSetting("OUT7_SYNC_EN","R/W",3,3,{0x012c}),
	Setting::mkSetting("OUT7_DIS_STATE","R/W",5,4,{0x012c}),
	Setting::mkSetting("OUT7_CMOS_DRV","R/W",7,6,{0x012c}),
	Setting::mkSetting("OUT7_CM","R/W",3,0,{0x012d}),
	Setting::mkSetting("OUT7_AMPL","R/W",6,4,{0x012d}),
	Setting::mkSetting("OUT7_MUX_SEL","R/W",2,0,{0x012e}),
	Setting::mkSetting("OUT7_VDD_SEL_EN","R/W",3,3,{0x012e}),
	Setting::mkSetting("OUT7_VDD_SEL","R/W",5,4,{0x012e}),
	Setting::mkSetting("OUT7_INV","R/W",7,6,{0x012e}),
	Setting::mkSetting("OUT8_PDN","R/W",0,0,{0x0130}),
	Setting::mkSetting("OUT8_OE","R/W",1,1,{0x0130}),
	Setting::mkSetting("OUT8_RDIV_FORCE2","R/W",2,2,{0x0130}),
	Setting::mkSetting("OUT8_FORMAT","R/W",2,0,{0x0131}),
	Setting::mkSetting("OUT8_SYNC_EN","R/W",3,3,{0x0131}),
	Setting::mkSetting("OUT8_DIS_STATE","R/W",5,4,{0x0131}),
	Setting::mkSetting("OUT8_CMOS_DRV","R/W",7,6,{0x0131}),
	Setting::mkSetting("OUT8_CM","R/W",3,0,{0x0132}),
	Setting::mkSetting("OUT8_AMPL","R/W",6,4,{0x0132}),
	Setting::mkSetting("OUT8_MUX_SEL","R/W",2,0,{0x0133}),
	Setting::mkSetting("OUT8_VDD_SEL_EN","R/W",3,3,{0x0133}),
	Setting::mkSetting("OUT8_VDD_SEL","R/W",5,4,{0x0133}),
	Setting::mkSetting("OUT8_INV","R/W",7,6,{0x0133}),
	Setting::mkSetting("OUT9_PDN","R/W",0,0,{0x0135}),
	Setting::mkSetting("OUT9_OE","R/W",1,1,{0x0135}),
	Setting::mkSetting("OUT9_RDIV_FORCE2","R/W",2,2,{0x0135}),
	Setting::mkSetting("OUT9_FORMAT","R/W",2,0,{0x0136}),
	Setting::mkSetting("OUT9_SYNC_EN","R/W",3,3,{0x0136}),
	Setting::mkSetting("OUT9_DIS_STATE","R/W",5,4,{0x0136}),
	Setting::mkSetting("OUT9_CMOS_DRV","R/W",7,6,{0x0136}),
	Setting::mkSetting("OUT9_CM","R/W",3,0,{0x0137}),
	Setting::mkSetting("OUT9_AMPL","R/W",6,4,{0x0137}),
	Setting::mkSetting("OUT9_MUX_SEL","R/W",2,0,{0x0138}),
	Setting::mkSetting("OUT9_VDD_SEL_EN","R/W",3,3,{0x0138}),
	Setting::mkSetting("OUT9_VDD_SEL","R/W",5,4,{0x0138}),
	Setting::mkSetting("OUT9_INV","R/W",7,6,{0x0138}),
	Setting::mkSetting("OUT9A_PDN","R/W",0,0,{0x013a}),
	Setting::mkSetting("OUT9A_OE","R/W",1,1,{0x013a}),
	Setting::mkSetting("OUT9A_RDIV_FORCE2","R/W",2,2,{0x013a}),
	Setting::mkSetting("OUT9A_FORMAT","R/W",2,0,{0x013b}),
	Setting::mkSetting("OUT9A_SYNC_EN","R/W",3,3,{0x013b}),
	Setting::mkSetting("OUT9A_DIS_STATE","R/W",5,4,{0x013b}),
	Setting::mkSetting("OUT9A_CMOS_DRV","R/W",7,6,{0x013b}),
	Setting::mkSetting("OUT9A_CM","R/W",3,0,{0x013c}),
	Setting::mkSetting("OUT9A_AMPL","R/W",6,4,{0x013c}),
	Setting::mkSetting("OUT9A_MUX_SEL","R/W",2,0,{0x013d}),
	Setting::mkSetting("OUT9A_VDD_SEL_EN","R/W",3,3,{0x013d}),
	Setting::mkSetting("OUT9A_VDD_SEL","R/W",5,4,{0x013d}),
	Setting::mkSetting("OUT9A_INV","R/W",7,6,{0x013d}),
	Setting::mkSetting("P1_NUM","R/W",47,0,{0x0212,0x0213,0x0214,0x0215,0x0216,0x0217}),
	Setting::mkSetting("P1_DEN","R/W",31,0,{0x0218,0x0219,0x021a,0x021b}),
	Setting::mkSetting("P2_NUM","R/W",47,0,{0x021c,0x021d,0x021e,0x021f,0x0220,0x0221}),
	Setting::mkSetting("P2_DEN","R/W",31,0,{0x0222,0x0223,0x0224,0x0225}),
	Setting::mkSetting("P3_NUM","R/W",47,0,{0x0226,0x0227,0x0228,0x0229,0x022a,0x022b}),
	Setting::mkSetting("P3_DEN","R/W",31,0,{0x022c,0x022d,0x022e,0x022f}),
	Setting::mkSetting("R0_REG","R/W",23,0,{0x024a,0x024b,0x024c}),
	Setting::mkSetting("R1_REG","R/W",23,0,{0x024d,0x024e,0x024f}),
	Setting::mkSetting("R2_REG","R/W",23,0,{0x0250,0x0251,0x0252}),
	Setting::mkSetting("R3_REG","R/W",23,0,{0x0253,0x0254,0x0255}),
	Setting::mkSetting("R4_REG","R/W",23,0,{0x0256,0x0257,0x0258}),
	Setting::mkSetting("R5_REG","R/W",23,0,{0x0259,0x025a,0x025b}),
	Setting::mkSetting("R6_REG","R/W",23,0,{0x025c,0x025d,0x025e}),
	Setting::mkSetting("R7_REG","R/W",23,0,{0x025f,0x0260,0x0261}),
	Setting::mkSetting("R8_REG","R/W",23,0,{0x0262,0x0263,0x0264}),
	Setting::mkSetting("R9_REG","R/W",23,0,{0x0265,0x0266,0x0267}),
	Setting::mkSetting("R9A_REG","R/W",23,0,{0x0268,0x0269,0x026a}),
	Setting::mkSetting("N1_NUM","R/W",43,0,{0x030d,0x030e,0x030f,0x0310,0x0311,0x0312}),
	Setting::mkSetting("N1_DEN","R/W",31,0,{0x0313,0x0314,0x0315,0x0316}),
	Setting::mkSetting("N1_UPDATE","S",0,0,{0x0317}),
	Setting::mkSetting("N2_NUM","R/W",43,0,{0x0318,0x0319,0x031a,0x031b,0x031c,0x031d}),
	Setting::mkSetting("N2_DEN","R/W",31,0,{0x031e,0x031f,0x0320,0x0321}),
	Setting::mkSetting("N2_UPDATE","S",0,0,{0x0322}),
	Setting::mkSetting("N3_NUM","R/W",43,0,{0x0323,0x0324,0x0325,0x0326,0x0327,0x0328}),
	Setting::mkSetting("N3_DEN","R/W",31,0,{0x0329,0x032a,0x032b,0x032c}),
	Setting::mkSetting("N3_UPDATE","S",0,0,{0x032d}),
	Setting::mkSetting("N4_NUM","R/W",43,0,{0x032e,0x032f,0x0330,0x0331,0x0332,0x0333}),
	Setting::mkSetting("N4_DEN","R/W",31,0,{0x0334,0x0335,0x0336,0x0337}),
	Setting::mkSetting("N1_FSTEPW","R/W",43,0,{0x0341,0x0342,0x0343,0x0344,0x0345,0x0346}),
	Setting::mkSetting("N2_FSTEPW","R/W",43,0,{0x0347,0x0348,0x0349,0x034a,0x034b,0x034c}),
	Setting::mkSetting("N3_FSTEPW","R/W",43,0,{0x034d,0x034e,0x034f,0x0350,0x0351,0x0352}),
	Setting::mkSetting("N4_FSTEPW","R/W",43,0,{0x0353,0x0354,0x0355,0x0356,0x0357,0x0358})
};
}
