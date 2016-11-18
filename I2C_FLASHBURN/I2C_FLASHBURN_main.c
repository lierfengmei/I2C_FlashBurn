/******************************************************************************/
/*  title: I2C��д����				         */
/*  Copyright 2015 by MM					 */
/*  All rights reserved.                     */
/******************************************************************************/

#include <csl.h>
#include <csl_pll.h>
#include <csl_gpio.h>
#include <csl_mcbsp.h>
#include <csl_i2c.h>
#include "stdio.h"
#include "string.h"
#include "flash_data.h"

void MCBSP_config_SPI_0(void);
void GPIO_init(void);
void delay(Uint32 k);


/*���໷������*/
PLL_Config  myConfig      = {
  0,    		//IAI: the PLL locks using the same process that was underway 
                //before the idle mode was entered
  1,    		//IOB: If the PLL indicates a break in the phase lock, 
                //it switches to its bypass mode and restarts the PLL phase-locking 
                //sequence
  12,    		//PLL multiply value; ��ʱʱ��=12*12/2=72Mhz
  1             //Divide by 2 PLL divide value; it can be either PLL divide value 
                //(when PLL is enabled), or Bypass-mode divide value
                //(PLL in bypass mode, if PLL multiply value is set to 1)
};


I2C_Setup I2Cinit = {
        0,              /* 7 bit address mode */
        0,         		/* own address - don't care if master */
       // 84,            	/* clkout value (Mhz)  */
		72,				//����ʹ�õľ�����12MHz���ʶ�ʱ�����Ϊ12MHz�ı������ɣ���ϵͳѡ��72MHz
        100,            	/* a number between 10 and 400*/
						//��ΪI2C���ߵ���Ϣ����������10~400֮�ڼ���
        0,              /* number of bits/byte to be received or transmitted (8)*/
        0,              /* DLB mode on*/
        1               /* FREE mode of operation on*/
};

int main(void)
{	
	int i,j,count;
	Uint16 page,page1;
	char len;
	Uint16 Wtemp[130];			//��Ϊÿ��ֻ��д��128byte,��������Ϊ��ַ
	Uint32 flash_data_count;
	CSL_init();					/*��ʼ��CSL��*/
	PLL_config(&myConfig);  	/*����ϵͳ�������ٶ�Ϊ_MHz*/
	MCBSP_config_SPI_0();		//����MCBSP MCBSP0 ��AIC23	

	i = CHIP_RGET(XBSR);		//�����20150616
	i = i&0xFFFE;				//���һλ��0
	i = i|0x0002;				//�����ڶ�λ��1  �������λΪ10 ����ʹ��GPIO8~13
	CHIP_RSET(XBSR,i);
	GPIO_init();						

/***************step1:��ʼ��codec************************************/	
	I2C_FSET(I2CSTR,BB,0x1); // Writing a 1 to BB Bus busy bit is supposed to clear it
							// Initializes I2C registers using initialization structure   
							//Need to calcualte I2CCLKL & I2CCLKH manually has functionnality not fully miplmented in CSL
							//Ҳ����������I2Cģ��֮ǰ����Ҫ��I2CSTR�Ĵ����е�BBλ��1��ͬʱ���ڴ������I2C_setup����������������һ��I2CCLKL��I2CCLKH�Ĵ����� 
	I2C_setup(&I2Cinit);	
	I2C_RSET(I2CCLKL,100); /* Clock Divider Low register */
	I2C_RSET(I2CCLKH,100); /* Clock Divider High register */

	len = sizeof(flash_data);
	page = len/128;	//Ҫд�����ҳ��
	page1 = len%128;

	printf("\nStart Burning Flash !\n");
	flash_data_count = 0;
	for(i=0;i<page;i++)					//ÿ��д128byte
	{
		if(i%2==0)	Wtemp[1] =0;		//���õ�ַ
		else Wtemp[1] = 0x0080;
		Wtemp[0] = i/2;
		j=2;
		for(;j<130;j++,flash_data_count++)
		{
			Wtemp[j] = flash_data[flash_data_count];	
		}
		I2C_write(Wtemp,130,1,0x50,1,30000); 
		printf(".");	
	}

	if(i%2==0)	Wtemp[1] =0;		//дʣ���page1 byge
	else Wtemp[1] = 0x0080;
	Wtemp[0] = i/2;
	j = 2;
	for(;j<page1+2;j++,flash_data_count++)
	{
		Wtemp[j] = flash_data[flash_data_count];	
	}
	I2C_write(Wtemp,page1+2,1,0x50,1,30000);

	printf("\nFinished Burning Flash ! , Verifying now! \n");
	flash_data_count=0;
	count = 0;
	for(i=0;i<page;i++)
	{
		if(i%2==0)	Wtemp[1] =0;		//��eeprom�ж���128byte����
		else Wtemp[1] = 0x0080;
		Wtemp[0] = i/2;
		I2C_write(Wtemp,2,1,0x50,1,30000); 
		delay(1800);
		I2C_read(Wtemp,128,1,0x50,1,30000,0);
		I2C_sendStop();

		for(j=0;j<128;j++,flash_data_count++)		//���жԱ�
			if(flash_data[flash_data_count]!=Wtemp[j])  count++;
		printf(".");	
	}

		if(i%2==0)	Wtemp[1] =0;		//��eeprom�ж���page1 byte����
		else Wtemp[1] = 0x0080;
		Wtemp[0] = i/2;
		I2C_write(Wtemp,2,1,0x50,1,30000); 
		delay(1800);
		I2C_read(Wtemp,page1,1,0x50,1,30000,0);
		I2C_sendStop();
		for(j=0;j<page1;j++,flash_data_count++)		//���жԱ�
			if(flash_data[flash_data_count]!=Wtemp[j])  count++;
		printf("."); 

	printf("\nFinished checking,  And %04d words wrong!\n",count);
	for(;;){}
}//end main



//����MCBSP0����AIC23
 MCBSP_Config ConfigLoopBack32_0= {
  MCBSP_SPCR1_RMK(
    MCBSP_SPCR1_DLB_OFF,                   /* DLB    = 0,��ֹ�Աջ���ʽ */
    MCBSP_SPCR1_RJUST_LZF,                 /* RJUST  = 2 */
    MCBSP_SPCR1_CLKSTP_DISABLE,            /* CLKSTP = 0 */
    MCBSP_SPCR1_DXENA_ON,                  /* DXENA  = 1 */
    0,                                     /* ABIS   = 0 */
    MCBSP_SPCR1_RINTM_RRDY,                /* RINTM  = 0 */
    0,                                     /* RSYNCER = 0 */
    MCBSP_SPCR1_RRST_DISABLE               /* RRST   = 0 */
   ),
    MCBSP_SPCR2_RMK(
    MCBSP_SPCR2_FREE_NO,                   /* FREE   = 0 */
    MCBSP_SPCR2_SOFT_NO,                   /* SOFT   = 0 */
    MCBSP_SPCR2_FRST_FSG,                  /* FRST   = 0 */
    MCBSP_SPCR2_GRST_CLKG,                 /* GRST   = 0 */
    MCBSP_SPCR2_XINTM_XRDY,                /* XINTM  = 0 */
    0,                                     /* XSYNCER = N/A */           
    MCBSP_SPCR2_XRST_DISABLE               /* XRST   = 0 */
   ),
   /*�������࣬�������ݳ���Ϊ16λ,ÿ��2������*/
  MCBSP_RCR1_RMK( 
   MCBSP_RCR1_RFRLEN1_OF(1),              /* RFRLEN1 = 1 */
   MCBSP_RCR1_RWDLEN1_16BIT               /* RWDLEN1 = 2 */
  ),
  MCBSP_RCR2_RMK(    
    MCBSP_RCR2_RPHASE_SINGLE,              /* RPHASE  = 0 */
    MCBSP_RCR2_RFRLEN2_OF(0),              /* RFRLEN2 = 0 */
    MCBSP_RCR2_RWDLEN2_8BIT,               /* RWDLEN2 = 0 */
    MCBSP_RCR2_RCOMPAND_MSB,               /* RCOMPAND = 0 */
    MCBSP_RCR2_RFIG_YES,                   /* RFIG    = 0 */
    MCBSP_RCR2_RDATDLY_1BIT                /* RDATDLY = 1 */
    ),  
   MCBSP_XCR1_RMK(    
    MCBSP_XCR1_XFRLEN1_OF(1),              /* XFRLEN1 = 1 */ 
    MCBSP_XCR1_XWDLEN1_16BIT               /* XWDLEN1 = 2 */
    
 ),   
 MCBSP_XCR2_RMK(   
    MCBSP_XCR2_XPHASE_SINGLE,              /* XPHASE  = 0 */
    MCBSP_XCR2_XFRLEN2_OF(0),              /* XFRLEN2 = 0 */
    MCBSP_XCR2_XWDLEN2_8BIT,               /* XWDLEN2 = 0 */
    MCBSP_XCR2_XCOMPAND_MSB,               /* XCOMPAND = 0 */
    MCBSP_XCR2_XFIG_YES,                   /* XFIG    = 0 */
    MCBSP_XCR2_XDATDLY_1BIT                /* XDATDLY = 1 */
  ),            
 MCBSP_SRGR1_DEFAULT,
 MCBSP_SRGR2_DEFAULT,  
 MCBSP_MCR1_DEFAULT,
 MCBSP_MCR2_DEFAULT, 
 MCBSP_PCR_RMK(
   MCBSP_PCR_IDLEEN_RESET,                 /* IDLEEN   = 0   */
   MCBSP_PCR_XIOEN_SP,                     /* XIOEN    = 0   */
   MCBSP_PCR_RIOEN_SP,                     /* RIOEN    = 0   */
   MCBSP_PCR_FSXM_EXTERNAL,                /* FSXM     = 0   */
   MCBSP_PCR_FSRM_EXTERNAL,                /* FSRM     = 0   */
   0,                                      /* DXSTAT = N/A   */
   MCBSP_PCR_CLKXM_INPUT,                  /* CLKXM    = 0   */
   MCBSP_PCR_CLKRM_INPUT,                  /* CLKRM    = 0   */
   MCBSP_PCR_SCLKME_NO,                    /* SCLKME   = 0   */
   MCBSP_PCR_FSXP_ACTIVEHIGH,              /* FSXP     = 0   */
   MCBSP_PCR_FSRP_ACTIVEHIGH,              /* FSRP     = 1   */
   MCBSP_PCR_CLKXP_FALLING,                /* CLKXP    = 1   */
   MCBSP_PCR_CLKRP_RISING                  /* CLKRP    = 1   */
 ),
 MCBSP_RCERA_DEFAULT, 
 MCBSP_RCERB_DEFAULT, 
 MCBSP_RCERC_DEFAULT, 
 MCBSP_RCERD_DEFAULT, 
 MCBSP_RCERE_DEFAULT, 
 MCBSP_RCERF_DEFAULT, 
 MCBSP_RCERG_DEFAULT, 
 MCBSP_RCERH_DEFAULT, 
 MCBSP_XCERA_DEFAULT,
 MCBSP_XCERB_DEFAULT,
 MCBSP_XCERC_DEFAULT,
 MCBSP_XCERD_DEFAULT,  
 MCBSP_XCERE_DEFAULT,
 MCBSP_XCERF_DEFAULT,  
 MCBSP_XCERG_DEFAULT,
 MCBSP_XCERH_DEFAULT
};
MCBSP_Handle mhMcbsp0;

//*����MCBSPΪSPI master*/
void MCBSP_config_SPI_0(void)
	{
		//����MCBSP0����AIC23
  mhMcbsp0 = MCBSP_open(MCBSP_PORT0, MCBSP_OPEN_RESET);   

  MCBSP_config(mhMcbsp0, &ConfigLoopBack32_0); 
   MCBSP_start(mhMcbsp0, 																								//MM��������������������������ʲô��˼����û�����
               MCBSP_SRGR_START | MCBSP_SRGR_FRAMESYNC, 
               0x300
               );
   MCBSP_start(mhMcbsp0, 
                MCBSP_RCV_START | MCBSP_XMIT_START, 
                0
               );              
		}


/****************************��������*****************************************************/

		//��ʼ��GPIO
void GPIO_init(void)					//��ʼ��GPIO
{
	GPIO_RSET(IODIR,0x04);				
		/*	0Ϊinput ,1 Ϊ output
		GPIO2 Ϊ���CON		=1
			�ʶ�Ϊ	0000_0000_0000_1000=0x0004;	*/
	GPIO_RSET(IODATA,0x00);
	GPIO_RSET(GPIOEN,0x001A);		//gpio9��
	GPIO_RSET(GPIODIR,0x0008);	
	GPIO_RSET(GPIODATA,0x0000);
	delay(100);	
}	


void delay(Uint32 k)
{
   while(k--);
}

