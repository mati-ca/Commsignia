/** @file
@copyright
(C) Commsignia Ltd. - All Rights Reserved.
Unauthorised copying of this file, via any medium is strictly prohibited.
Proprietary and confidential.
@date 2020
*/

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include <cms_v2x/api.h>
#include <cms_v2x/cam.h>

/** @file
@brief Enable CAM sending
@ingroup ex
*/

int main(int argc, char* argv[])
{
    const char* host = (argc > 1) ? argv[1] : "127.0.0.1";

    /* Create a session */
    cms_session_t session = cms_get_session();

    /* Connect to the host */
    bool error = cms_api_connect_easy(&session, host);

    cms_cam_tx_state_t cam_tx_enable = {
        .enable = true      /* CAM send enable */
    };

    /* Enable CAM sending */
    error = error || cms_cam_tx_enable(&session, &cam_tx_enable, NULL);

    if(error) {
        printf("Unable to %s CAM sending\n", (cam_tx_enable.enable == true) ? "enable" : "disable");
    } else {
        printf("CAM Tx successfully %s.\n", (cam_tx_enable.enable == true) ? "enabled" : "disabled");
    }

    /* Wait some time */
    sleep(1);

    /* Disable CAM sending */
    cam_tx_enable.enable = false;
    error = error || cms_cam_tx_enable(&session, &cam_tx_enable, NULL);

    if(error) {
        printf("Unable to %s CAM sending\n", (cam_tx_enable.enable == true) ? "enable" : "disable");
    } else {
        printf("CAM Tx successfully %s.\n", (cam_tx_enable.enable == true) ? "enabled" : "disabled");
    }

    /* Close connection and cleanup */
    cms_api_disconnect(&session);
    cms_api_clean();

    return (int)error;
}

