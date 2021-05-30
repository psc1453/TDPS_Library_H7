//
// Created by 彭世辰 on 2021/5/29.
//

#include "TCS34725.h"

/*******************************************************************************
 * @brief Writes data into TCS34725 registers, starting from the selected
 *        register address pointer.
 *
 * @param subAddr - The selected register address pointer.
 * @param dataBuffer - Pointer to a buffer storing the transmission data.
 * @param bytesNumber - Number of bytes that will be sent.
 *
 * @return None.
*******************************************************************************/
void TCS34725_Write(I2C_HandleTypeDef* hi2c, u8 subAddr, u8* dataBuffer, u8 bytesNumber)
{
    u8 sendBuffer[10] = {0, };
    u8 byte = 0;

    sendBuffer[0] = subAddr | TCS34725_COMMAND_BIT;
    for(byte = 1; byte <= bytesNumber; byte++)
    {
        sendBuffer[byte] = dataBuffer[byte - 1];
    }
    HAL_I2C_Master_Transmit(hi2c, TCS34725_ADDRESS<<1, sendBuffer, bytesNumber + 1, 10);
}
/*******************************************************************************
 * @brief Reads data from TCS34725 registers, starting from the selected
 *        register address pointer.
 *
 * @param subAddr - The selected register address pointer.
 * @param dataBuffer - Pointer to a buffer that will store the received data.
 * @param bytesNumber - Number of bytes that will be read.
 *
 * @return None.
*******************************************************************************/
void TCS34725_Read(I2C_HandleTypeDef* hi2c, u8 subAddr, u8* dataBuffer, u8 bytesNumber)
{
    subAddr |= TCS34725_COMMAND_BIT;

    HAL_I2C_Master_Transmit(hi2c, TCS34725_ADDRESS<<1, &subAddr, 1, 10);
    HAL_I2C_Master_Receive(hi2c, TCS34725_ADDRESS<<1, dataBuffer, bytesNumber, 10);
}
/*******************************************************************************
 * @brief TCS34725设置积分时间
 *
 * @return None
*******************************************************************************/
void TCS34725_SetIntegrationTime(I2C_HandleTypeDef* hi2c, u8 time)
{
    TCS34725_Write(hi2c, TCS34725_ATIME, &time, 1);
}
/*******************************************************************************
 * @brief TCS34725设置增益
 *
 * @return None
*******************************************************************************/
void TCS34725_SetGain(I2C_HandleTypeDef* hi2c, u8 gain)
{
    TCS34725_Write(hi2c, TCS34725_CONTROL, &gain, 1);
}
/*******************************************************************************
 * @brief TCS34725使能
 *
 * @return None
*******************************************************************************/
void TCS34725_Enable(I2C_HandleTypeDef* hi2c)
{
    u8 cmd = TCS34725_ENABLE_PON;

    TCS34725_Write(hi2c, TCS34725_ENABLE, &cmd, 1);
    cmd = TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN;
    TCS34725_Write(hi2c, TCS34725_ENABLE, &cmd, 1);
    //delay_us(600000);//delay_ms(3);//延时应该放在设置AEN之后
}
/*******************************************************************************
 * @brief TCS34725失能
 *
 * @return None
*******************************************************************************/
void TCS34725_Disable(I2C_HandleTypeDef* hi2c)
{
    u8 cmd = 0;

    TCS34725_Read(hi2c, TCS34725_ENABLE, &cmd, 1);
    cmd = cmd & ~(TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN);
    TCS34725_Write(hi2c, TCS34725_ENABLE, &cmd, 1);
}
/*******************************************************************************
 * @brief TCS34725初始化
 *
 * @return ID - ID寄存器中的值
*******************************************************************************/
u8 TCS34725_Init(I2C_HandleTypeDef* hi2c)
{
    u8 id=0;

    TCS34725_Read(hi2c, TCS34725_ID, &id, 1);  //TCS34725 的 ID 是 0x44 可以根据这个来判断是否成功连接,0x4D是TCS34727;
    if(id==0x4D | id==0x44)
    {
        TCS34725_SetIntegrationTime(hi2c, TCS34725_INTEGRATIONTIME_50MS);
        TCS34725_SetGain(hi2c, TCS34725_GAIN_1X);
        TCS34725_Enable(hi2c);
        return 1;
    }
    return 0;
}
/*******************************************************************************
 * @brief TCS34725获取单个通道数据
 *
 * @return data - 该通道的转换值
*******************************************************************************/
u16 TCS34725_GetChannelData(I2C_HandleTypeDef* hi2c, u8 reg)
{
    u8 tmp[2] = {0,0};
    u16 data;

    TCS34725_Read(hi2c, reg, tmp, 2);
    data = (tmp[1] << 8) | tmp[0];

    return data;
}
/*******************************************************************************
 * @brief TCS34725获取各个通道数据
 *
 * @return 1 - 转换完成，数据可用
 *   	   0 - 转换未完成，数据不可用
*******************************************************************************/
u8 TCS34725_GetRawData(I2C_HandleTypeDef* hi2c, COLOR_RGBC_RAW *rgbc)
{
    u8 status = TCS34725_STATUS_AVALID;

    TCS34725_Read(hi2c, TCS34725_STATUS, &status, 1);

    if(status & TCS34725_STATUS_AVALID)
    {
        rgbc->c = TCS34725_GetChannelData(hi2c, TCS34725_CDATAL);
        rgbc->r = TCS34725_GetChannelData(hi2c, TCS34725_RDATAL);
        rgbc->g = TCS34725_GetChannelData(hi2c, TCS34725_GDATAL);
        rgbc->b = TCS34725_GetChannelData(hi2c, TCS34725_BDATAL);
        return 1;
    }
    return 0;
}

void TCS34725_RAW_To_RGB(COLOR_RGBC_RAW *rgbc, COLOR_RGB *rgb)
{
    u8 r = (rgbc->r*255)/(rgbc->c);
    u8 g = (rgbc->g*255)/(rgbc->c);
    u8 b = (rgbc->b*255)/(rgbc->c);

    rgb->r = r;
    rgb->g = g;
    rgb->b = b;
}

/******************************************************************************/
//RGB转HSL
void TCS34725_RAW_To_HSL(COLOR_RGBC_RAW *rgbc, COLOR_HSL *hsl)
{
    u8 maxVal,minVal,difVal;
    u8 r = rgbc->r*100/rgbc->c;   //[0-100]
    u8 g = rgbc->g*100/rgbc->c;
    u8 b = rgbc->b*100/rgbc->c;

    maxVal = max3v(r,g,b);
    minVal = min3v(r,g,b);
    difVal = maxVal-minVal;

    //计算亮度
    hsl->l = (maxVal+minVal)/2;   //[0-100]

    if(maxVal == minVal)//若r=g=b,灰度
    {
        hsl->h = 0;
        hsl->s = 0;
    }
    else
    {
        //计算色调
        if(maxVal==r)
        {
            if(g>=b)
                hsl->h = 60*(g-b)/difVal;
            else
                hsl->h = 60*(g-b)/difVal+360;
        }
        else
        {
            if(maxVal==g)hsl->h = 60*(b-r)/difVal+120;
            else
            if(maxVal==b)hsl->h = 60*(r-g)/difVal+240;
        }

        //计算饱和度
        if(hsl->l<=50)hsl->s=difVal*100/(maxVal+minVal);  //[0-100]
        else
            hsl->s=difVal*100/(200-(maxVal+minVal));
    }
}

void TCS34725_RAW_To_CMYK(COLOR_RGBC_RAW *rgbc, COLOR_CMYK *cmyk)
{
    float maxVal;
    float r_temp = rgbc->r/(float)rgbc->c;   //[0-100]
    float g_temp = rgbc->g/(float)rgbc->c;
    float b_temp = rgbc->b/(float)rgbc->c;

    maxVal = max3v(r_temp,g_temp,b_temp);

    cmyk->k = 1.0 - maxVal;

    cmyk->c = (1.0-r_temp-cmyk->k) / (1-cmyk->k);
    cmyk->m = (1.0-g_temp-cmyk->k) / (1-cmyk->k);
    cmyk->y = (1.0-b_temp-cmyk->k) / (1-cmyk->k);
//    printf("%f %f\n", g_temp, cmyk->k);
}
/******************************************************************************/


