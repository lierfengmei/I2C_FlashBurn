5509a使用I2C模式烧写eeprom步骤

step1：build语音程序，生成.out文件；

step2:确定该工作目录，好像有中文的时候会有问题，可以直接把这个目录都复制到其他根目录下即可运行良好；

step3:将编译好的语音的.out文件，拷贝到本目录下，并更名为USER_OUT.out；

step4:双击FLASH5509BGA.bat,生成对应的USER_OUT.bin文件；

step5:双击bin2h.exe，生成flash_data.h头文件；

step6:将flash_data.h头文件替换到I2C_FLASHBURN工程中已存在的flash_data.h文件，打开CCS重新编译该I2C烧写程序，debug模式下运行程序。完成烧写后将gpio[3:0]配置为0110即可。
