```c
#include "calcEcuTemp.h"

static sint16 gNtcTemp_s16;
static sint16 gSysTemp_s16;
static bool   gNtcErr_b;

/**
 * @brief Set NTC temperature measurement.
 * @param ntcTemp Temperature from NTC sensor [°C], valid range [-40,140].
 */
void CalcEcuTemp_SetNtcTemp(const sint16 ntcTemp)
{
    gNtcTemp_s16 = ntcTemp;
}

/**
 * @brief Set system temperature measurement.
 * @param sysTemp Temperature from system sensor [°C], valid range [-40,140].
 */
void CalcEcuTemp_SetSysTemp(const sint16 sysTemp)
{
    gSysTemp_s16 = sysTemp;
}

/**
 * @brief Set NTC error flag.
 * @param ntcError True if NTC sensor error detected, false otherwise.
 */
void CalcEcuTemp_SetNtcError(const bool ntcError)
{
    gNtcErr_b = ntcError;
}

sint16 Calc_ecuTemperature_s16(void)
{
    sint32 numerator;
    sint16  l_ecuTemp_s16;
    sint16  ntcTemp;
    sint16  sysTemp;
    bool    ntcErr;
    sint16  lowThr;
    sint16  upThr;
    sint16  denom;

    /* Assign local copies of inputs */
    ntcTemp = gNtcTemp_s16;
    sysTemp = gSysTemp_s16;
    ntcErr = gNtcErr_b;

    lowThr = (sint16)PAR_ECU_TEMP_LOW_THR;
    upThr = (sint16)PAR_ECU_TEMP_UP_THR;
    denom = (sint16)(upThr - lowThr);

    /* Ensure denom > 0 to comply with calculation */
    if (denom <= 0)
    {
        /* Invalid configuration, fallback to sysTemp */
        l_ecuTemp_s16 = sysTemp;
    }
    else
    {
        if ((ntcTemp <= lowThr) || (true == ntcErr))
        {
            l_ecuTemp_s16 = sysTemp;
        }
        else if (ntcTemp >= upThr)
        {
            l_ecuTemp_s16 = ntcTemp;
        }
        else
        {
            /* Perform linear interpolation with 32-bit intermediate to avoid overflow */
            numerator = (sint32)ntcTemp * (ntcTemp - lowThr);
            numerator += (sint32)sysTemp * (upThr - ntcTemp);

            /* Arithmetic rounding */
            if (numerator >= 0)
            {
                l_ecuTemp_s16 = (sint16)((numerator + (denom / 2)) / denom);
            }
            else
            {
                l_ecuTemp_s16 = (sint16)((numerator - (denom / 2)) / denom);
            }
        }
    }

    return l_ecuTemp_s16;
}
```