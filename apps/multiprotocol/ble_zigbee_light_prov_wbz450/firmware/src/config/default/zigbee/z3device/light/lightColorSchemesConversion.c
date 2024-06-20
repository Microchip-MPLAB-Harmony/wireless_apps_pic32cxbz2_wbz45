/*******************************************************************************
  Light device color schemes conversion Source File

  Company:
    Microchip Technology Inc.

  File Name:
    lightColorSchemesConversion.c

  Summary:
    This file contains the Light device color schemes conversion interface.

  Description:
    This file contains the Light device color schemes conversion interface.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/
// DOM-IGNORE-END

#if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_COLOR_LIGHT

/******************************************************************************
                    Includes section
******************************************************************************/
#include <z3device/light/include/lightColorSchemesConversion.h>

/******************************************************************************
                    Definitions section
******************************************************************************/
#define TEMPERATURE_TO_X_TEMPERATURE_TRESHOLD           4000U

#define TEMPERATURE_TO_Y_FIRST_TEMPERATURE_TRESHOLD     2222U
#define TEMPERATURE_TO_Y_SECOND_TEMPERATURE_TRESHOLD    4000U

#define TEMPERATURE_TO_X_FIRST_FACTOR_FIRST_EQUATION    17440695910400U
#define TEMPERATURE_TO_X_SECOND_FACTOR_FIRST_EQUATION   15358885888U
#define TEMPERATURE_TO_X_THIRD_FACTOR_FIRST_EQUATION    57520658U
#define TEMPERATURE_TO_X_FOURTH_FACTOR_FIRST_EQUATION   11790U

#define TEMPERATURE_TO_X_FIRST_FACTOR_SECOND_EQUATION   198301902438400U
#define TEMPERATURE_TO_X_SECOND_FACTOR_SECOND_EQUATION  138086835814U
#define TEMPERATURE_TO_X_THIRD_FACTOR_SECOND_EQUATION   14590587U
#define TEMPERATURE_TO_X_FOURTH_FACTOR_SECOND_EQUATION  15754U

#define TEMPERATURE_TO_Y_FIRST_FACTOR_FIRST_EQUATION    18126U
#define TEMPERATURE_TO_Y_SECOND_FACTOR_FIRST_EQUATION   22087U
#define TEMPERATURE_TO_Y_THIRD_FACTOR_FIRST_EQUATION    35808U
#define TEMPERATURE_TO_Y_FOURTH_FACTOR_FIRST_EQUATION   3312U

#define TEMPERATURE_TO_Y_FIRST_FACTOR_SECOND_EQUATION   15645U
#define TEMPERATURE_TO_Y_SECOND_FACTOR_SECOND_EQUATION  22514U
#define TEMPERATURE_TO_Y_THIRD_FACTOR_SECOND_EQUATION   34265U
#define TEMPERATURE_TO_Y_FOURTH_FACTOR_SECOND_EQUATION  2744U

#define TEMPERATURE_TO_Y_FIRST_FACTOR_THIRD_EQUATION    50491U
#define TEMPERATURE_TO_Y_SECOND_FACTOR_THIRD_EQUATION   96229U
#define TEMPERATURE_TO_Y_THIRD_FACTOR_THIRD_EQUATION    61458U
#define TEMPERATURE_TO_Y_FOURTH_FACTOR_THIRD_EQUATION   6062U

#define XY_TO_TEMPERATURE_X_EPICENTER                   21757U
#define XY_TO_TEMPERATURE_Y_EPICENTER                   12176U

#define XY_TO_TEMPERATURE_FIRST_FACTOR                  44900ULL
#define XY_TO_TEMPERATURE_SECOND_FACTOR                 352500ULL
#define XY_TO_TEMPERATURE_THIRD_FACTOR                  682330ULL
#define XY_TO_TEMPERATURE_FOURTH_FACTOR                 552033ULL

/******************************************************************************
                    Implementation section
******************************************************************************/
/**************************************************************************//**
\brief Converts color temperature to appropriate XY coordinates

\param[in] temperature - color temperature (attribute value);
\param[out] x          - x coordinate (attribute value);
\param[out] y          - y coordinate (attribute value)
******************************************************************************/
void lightConvertColorToXY(uint16_t temperature, uint16_t *x, uint16_t *y)
{
  uint64_t localX = 0, localY = 0;
  uint16_t temp=0;
  if (temperature != 0U)
  {
    temp = (uint16_t)(1000000U / temperature);
  }
  if (TEMPERATURE_TO_X_TEMPERATURE_TRESHOLD > temp)
  {
    if(temp != 0U)
    {
    localX = (uint64_t)((uint32_t)(TEMPERATURE_TO_X_THIRD_FACTOR_FIRST_EQUATION / temp + 
      TEMPERATURE_TO_X_FOURTH_FACTOR_FIRST_EQUATION) -
      TEMPERATURE_TO_X_SECOND_FACTOR_FIRST_EQUATION / temp / temp -
      TEMPERATURE_TO_X_FIRST_FACTOR_FIRST_EQUATION / temp / temp/ temp);
    }
  }
  else
  {
    localX = (uint64_t)(TEMPERATURE_TO_X_SECOND_FACTOR_SECOND_EQUATION / temp / temp +
      TEMPERATURE_TO_X_THIRD_FACTOR_SECOND_EQUATION / temp +
      TEMPERATURE_TO_X_FOURTH_FACTOR_SECOND_EQUATION -
      TEMPERATURE_TO_X_FIRST_FACTOR_SECOND_EQUATION / temp / temp / temp);
  }
  if (TEMPERATURE_TO_Y_FIRST_TEMPERATURE_TRESHOLD > temp)
  {
    localY = TEMPERATURE_TO_Y_THIRD_FACTOR_FIRST_EQUATION * localX / 65536U -
      TEMPERATURE_TO_Y_FIRST_FACTOR_FIRST_EQUATION * localX * localX * localX / 281474976710656U -
      TEMPERATURE_TO_Y_SECOND_FACTOR_FIRST_EQUATION * localX * localX / 4294967296U -
      TEMPERATURE_TO_Y_FOURTH_FACTOR_FIRST_EQUATION;
  }
  else if (TEMPERATURE_TO_Y_SECOND_TEMPERATURE_TRESHOLD > temp)
  {
    localY = TEMPERATURE_TO_Y_THIRD_FACTOR_SECOND_EQUATION * localX / 65536U -
      TEMPERATURE_TO_Y_FIRST_FACTOR_SECOND_EQUATION * localX * localX * localX / 281474976710656U -
      TEMPERATURE_TO_Y_SECOND_FACTOR_SECOND_EQUATION * localX * localX / 4294967296U -
      TEMPERATURE_TO_Y_FOURTH_FACTOR_SECOND_EQUATION;
  }
  else
  {
    localY = TEMPERATURE_TO_Y_THIRD_FACTOR_THIRD_EQUATION * localX / 65536U +
      TEMPERATURE_TO_Y_FIRST_FACTOR_THIRD_EQUATION * localX * localX * localX / 281474976710656U -
      TEMPERATURE_TO_Y_SECOND_FACTOR_THIRD_EQUATION * localX * localX / 4294967296U -
      TEMPERATURE_TO_Y_FOURTH_FACTOR_THIRD_EQUATION;
  }

  localY *= 4U;

  *x = (uint16_t)localX;
  *y = (uint16_t)localY;
}

/**************************************************************************//**
\brief Converts XY coordinates to appropriate color temperature

\param[in] x - x coordinate (attribute value);
\param[in] y - y coordinate (attribute value)

\returns color temperature (attribute value)
******************************************************************************/
uint16_t lightConvertXYToColor(uint16_t x, uint16_t y)
{
  uint32_t nX;
  uint32_t nY = (uint16_t)(y - XY_TO_TEMPERATURE_Y_EPICENTER);
  bool negativeNx = x > XY_TO_TEMPERATURE_X_EPICENTER ? false : true;
  uint64_t temperature;

  if (negativeNx)
  {
    nX = (uint16_t)(XY_TO_TEMPERATURE_X_EPICENTER - x);
    temperature = XY_TO_TEMPERATURE_SECOND_FACTOR * nX * nX / nY / nY +
      XY_TO_TEMPERATURE_FOURTH_FACTOR +
      XY_TO_TEMPERATURE_FIRST_FACTOR * nX * nX * nX / nY / nY / nY +
      XY_TO_TEMPERATURE_THIRD_FACTOR * nX / nY;
  }
  else
  {
    nX = (uint16_t)(x - XY_TO_TEMPERATURE_X_EPICENTER);
    temperature = XY_TO_TEMPERATURE_SECOND_FACTOR * nX * nX / nY / nY +
      XY_TO_TEMPERATURE_FOURTH_FACTOR -
      XY_TO_TEMPERATURE_FIRST_FACTOR * nX * nX * nX / nY / nY / nY -
      XY_TO_TEMPERATURE_THIRD_FACTOR * nX / nY;
  }

  return (uint16_t)(1000000U / (temperature / 100U));
}

#endif // APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_COLOR_LIGHT

// eof lightColorSchemesConversion.c


