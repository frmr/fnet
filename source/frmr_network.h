#ifndef FRMR_NETWORK_H
#define FRMR_NETWORK_H

#include <string>

using std::string;

namespace frmr
{
    string      EncodeUINT16( const uint16_t num );
    uint16_t    DecodeUINT16( const string encodedNum );
    string      EncodeFloat( const float num );
    float       DecodeFloat( const string encodedNum );
}

#endif
