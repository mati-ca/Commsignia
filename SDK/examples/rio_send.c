/** @file
@copyright
(C) Commsignia Ltd. - All Rights Reserved.
Unauthorised copying of this file, via any medium is strictly prohibited.
Proprietary and confidential.
@date 2021
*/

#include <stdio.h>
#include <stdlib.h>

#include <cms_v2x/api.h>
#include <cms_v2x/radio.h>

/** @file
@brief Send a radio message.
@ingroup ex
*/


static void fill_send_data(cms_rio_send_data_t* send_data, cms_rio_packet_t packet_type)
{
    /* Radio parameters */
    send_data->radio_params.datarate = CMS_DATARATE_NA;
    send_data->radio_params.expiry_time = 1000U;
    send_data->radio_params.interface_id = 1U;
    send_data->radio_params.sps_index = 0U;
    send_data->radio_params.tx_power = CMS_POWER_LEVEL_NA;
    send_data->radio_params.user_prio = 1U;

    /* Packet type */
    send_data->packet_type = packet_type;
}

static void log_rio_send_result(cms_rio_result_t result, const cms_rio_send_data_t* send_data)
{
    switch(result) {
    case CMS_RIO_RESULT_OK:
        printf("Radio message sent\n");
        break;
    case CMS_RIO_RESULT_NO_IFACE_ERROR:
        printf("Unable to get radio device on interface %u.\n", send_data->radio_params.interface_id);
        break;
    case CMS_RIO_RESULT_DISABLED_IFACE_ERROR:
        printf("Interface %u is disabled.\n", send_data->radio_params.interface_id);
        break;
    default:
        fprintf(stderr, "Cannot send out radio message, error code: %d\n", (int)result);
        break;
    }
}

int main(int argc, char* argv[])
{
    cms_rio_packet_t packet_type = (argc > 1) ?
                                   (cms_rio_packet_t)atoi(argv[1]) :
                                   CMS_RIO_GNP_PACKET;
    const char* host = (argc > 2) ? argv[2] : "127.0.0.1";

    /* Create a session */
    cms_session_t session = cms_get_session();

    /* Connect to the host */
    bool error = cms_api_connect_easy(&session, host);

    if(!error) {

        /* Create send header */
        cms_rio_send_data_t send_data = {0};
        fill_send_data(&send_data, packet_type);

        uint8_t msg_payload[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

        /* Create a buffer view as a handle to the actual payload buffer */
        cms_buffer_view_t payload = {
            .data = msg_payload,
            .length = sizeof(msg_payload)
        };

        /* Send radio message with the given data and payload */
        cms_rio_result_t result = cms_rio_send(&session, &send_data, payload, NULL);
        log_rio_send_result(result, &send_data);
    }

    /* Close connection and cleanup */
    cms_api_disconnect(&session);
    cms_api_clean();

    return (int)error;
}
