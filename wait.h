#ifndef WAIT_H
#define WAIT_H

namespace SGDK
{
    void wait_until_true(bool *condition, int max_wait_count=-1);
    void wait_until_false(bool *condition, int max_wait_count=-1);


}  // namespace SGDK

#endif // WAIT_H
