#include "wait.h"

namespace SGDK
{
// ждет пока не true
void wait_until_true(bool *condition, int max_wait_count)
{
    do
    {
       if (max_wait_count-- == 0) *condition = true;
    }while ( !(*condition) );
}

void wait_until_false(bool *condition, int max_wait_count)
{
    do
    {
       if (max_wait_count-- == 0) *condition = true;
    }while ( *condition );
}



}   // namespace SGDK
