#include "stm8s.h"
#include "a4964.h"
#include "spi.h"

#define A4964_WRITE_CMD				(u16)( 1 << 10 )
#define A4964_MOTOR_FUNCTION		(u8)27
#define A4964_DEMANG_INPUT			(u8)30

static u8 Parity_Cal_Msb15Bits( u16 data );

EEPROM u16 A4964InitData[ ] = {
	0x0CD, 0x001, 0x029,	0x001, 0x001,
	0x001, 0x0FE, 0x07F, 0x07F, 0x001,
	0x001, 0x0C8, 0x0C8, 0x007, 0x043,
	0x282, 0x038, 0x04F, 0x080, 0x064,
	0x025, 0x07F, 0x080, 0x01F, 0x001,
	0x100, 0x001, 0x103, 0x200, 0x3EF
}; 

void A4964_Init( void )
{
	u8 i;
	u16 temp;
	
	for( i = 0; i < 30; i++ )
	{
		temp = A4964InitData[ i ] >> 1;
		A4964_Write( i, temp );
	}
}

u16 A4964_Read( u8 reg )
{
	u8 parity;	
	u16 temp;
	
	temp = ( reg & 0x1f ) << 11;
	parity = Parity_Cal_Msb15Bits( temp );
	temp |= parity;
	
	return Spi_Read2Byte( temp );
}

void A4964_Write( u8 reg, u16 data )
{
	u8 parity;
	u16 temp;
	
	temp = (( reg & 0x1f ) << 11 ) | A4964_WRITE_CMD;
	temp |= (( data & 0x1ff ) << 1 );
	parity = Parity_Cal_Msb15Bits( temp );
	temp |= parity;
	
	Spi_Write2Byte( temp );
}

void A4964_SetDemand( u16 data )
{
	u8 parity;
	u16 temp;
	
	temp = ( A4964_DEMANG_INPUT & 0x1f ) << 11;
	temp |= (( data & 0x3ff ) << 1 );
	parity = Parity_Cal_Msb15Bits( temp );
	temp |= parity;
	
	Spi_Write2Byte( temp );	
}

static u8 Parity_Cal_Msb15Bits( u16 data )
{
	u8 i;
	u8 parity = 0;
	u16 temp = data;
	
	temp = temp >> 1;
	for( i = 0; i < 15; i++ )
	{
		parity ^= (u8)(( data >> i ) & 0x01 );
		temp = temp >> 1;
	}
	
	if( parity & 0x01 )
	{
		return 0;
	}
	
	return 1;
}
