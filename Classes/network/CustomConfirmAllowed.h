// CustomConfirmAllowed.h
#pragma once

#include "Common-cpp/inc/Helpers/ConfirmAllowed.h"
#include "Common-cpp/inc/JString.h" // ȷ��·����ȷ

namespace ExitGames
{
    namespace Common
    {
        namespace Helpers
        {
            // �ػ� ConfirmAllowed ��֧�� JString
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
