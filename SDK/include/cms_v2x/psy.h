/** @file
@copyright
(C) Commsignia Ltd. - All Rights Reserved.
Unauthorised copying of this file, via any medium is strictly prohibited.
Proprietary and confidential.
@date 2020-2021
*/

#ifndef CMS_V2X_PSY_H_
#define CMS_V2X_PSY_H_

/** @file
@brief Station Info
*/

#include <stdbool.h>

#include <cms_v2x/fwd.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
@defgroup psy Pseudonimity
@ingroup api
*/

/**
Trigger pseudonym certificate change
@ingroup psy
Trigger a pseudonymity change immediately,
without taking into account any of the pseudonymity change criteria.
The function returns immediately, and does not wait for the actual change to happen.

@experimentalapi
@param session        Client session
@param unused_in      Unused parameter
@param unused_out     Unused parameter
@return true in case of error
*/
bool cms_psy_trigger(const cms_session_t* session,
                     const void* unused_in,
                     void* unused_out);

#ifdef __cplusplus
}
#endif

#endif /* CMS_V2X_PSY_H_ */
