#include "frmr_network.h"
#include <iostream>
#include "stdlib.h"

using std::cout;
using std::endl;

string frmr::EncodeUINT16( const uint16_t num )
{
    unsigned char unsignedCode[2];
    unsignedCode[0] = (unsigned char) ( num >> 8 );
    unsignedCode[1] = (unsigned char) ( num );

    string stringCode;
    stringCode += *reinterpret_cast<char*>( &unsignedCode[0] );
    stringCode += *reinterpret_cast<char*>( &unsignedCode[1] );

    return stringCode;
}

uint16_t frmr::DecodeUINT16( const string stringCode )
{
    unsigned char unsignedCode[2];

    unsignedCode[0] = *reinterpret_cast<const unsigned char*>( &stringCode[0] );
    unsignedCode[1] = *reinterpret_cast<const unsigned char*>( &stringCode[1] );

    uint16_t num = 0;
    num += (uint16_t) ( unsignedCode[0] << 8 );
    num += (uint16_t) ( unsignedCode[1] );

    return num;
}

string frmr::EncodeFloat( const float num )
{
    unsigned int floatAsInt = *reinterpret_cast<const unsigned int*>( &num );

    unsigned char unsignedCode[4];
    unsignedCode[0] = floatAsInt >> 24;
    unsignedCode[1] = floatAsInt >> 16;
    unsignedCode[2] = floatAsInt >> 8;
    unsignedCode[3] = floatAsInt;

    string stringCode;
    stringCode += *reinterpret_cast<char*>( &unsignedCode[0] );
    stringCode += *reinterpret_cast<char*>( &unsignedCode[1] );
    stringCode += *reinterpret_cast<char*>( &unsignedCode[2] );
    stringCode += *reinterpret_cast<char*>( &unsignedCode[3] );

    if ( stringCode.size() != 4 )
    {
        cout << "ERROR: Float is less than four chars" << endl;
        exit( 0 );
    }

    return stringCode;
}

float frmr::DecodeFloat( const string encodedNum )
{
    unsigned char unsignedCode[4];

    unsignedCode[0] = *reinterpret_cast<const unsigned char*>( &encodedNum[0] );
    unsignedCode[1] = *reinterpret_cast<const unsigned char*>( &encodedNum[1] );
    unsignedCode[2] = *reinterpret_cast<const unsigned char*>( &encodedNum[2] );
    unsignedCode[3] = *reinterpret_cast<const unsigned char*>( &encodedNum[3] );

    unsigned floatAsInt = 0;
    floatAsInt += (unsigned int) ( unsignedCode[0] << 24 );
    floatAsInt += (unsigned int) ( unsignedCode[1] << 16 );
    floatAsInt += (unsigned int) ( unsignedCode[2] << 8 );
    floatAsInt += (unsigned int) ( unsignedCode[3] );

    return *reinterpret_cast<float*>( &floatAsInt );
}
