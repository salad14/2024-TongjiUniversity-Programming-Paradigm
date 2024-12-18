// CustomConfirmAllowed.h
#pragma once

#include "Common-cpp/inc/Helpers/ConfirmAllowed.h"
#include "Common-cpp/inc/JString.h" // 确保路径正确

namespace ExitGames
{
    namespace Common
    {
        namespace Helpers
        {
            // 特化 ConfirmAllowed 以支持 JString
            template<>
            struct ConfirmAllowed<JString>
            {
                typedef JString type;
                typedef JString scalarType;
                static const unsigned int dimensions = 0;
                static const nByte typeName = TypeCode::STRING;
                static const nByte customTypeName = 0;
            };
        }
    }
}
