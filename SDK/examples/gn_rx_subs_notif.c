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
#include <cms_v2x/common_types.h>
#include <cms_v2x/gn.h>

/** @file
@brief Subscribe to received GeoNet messages.
@ingroup ex
*/


/* Context type for the notification callback */
typedef struct notif_ctx {
    uint32_t param;
    uint32_t cnt;
} notif_ctx_t;


/* Notification callback to print received message details */
static void gn_notif_cb(cms_btp_port_t btp_port,
                        const cms_gn_rx_notif_data_t* notif,
                        cms_buffer_view_t msg,
                        void* ctx)
{
    if((NULL == notif) || (NULL == msg.data) || (0UL == msg.length) || (NULL == ctx)) {
        fprintf(stderr, "%s NULL argument\n", __func__);
    } else {

        notif_ctx_t* notif_ctx = (notif_ctx_t*)ctx;

        printf("Context: %lu\n", (unsigned long)notif_ctx->param);
        printf("Receive counter: %lu\n", (unsigned long)notif_ctx->cnt);
        printf("BTP port key: %u\n", btp_port);
        printf("Timestamp: %llu [ms]\n", (unsigned long long)notif->radio.timestamp);
        printf("Datarate: %u [kbps]\n", notif->radio.datarate);
        printf("Destination address: %02X:%02X:%02X:%02X:%02X:%02X\n",
               notif->radio.dest_address[0],
               notif->radio.dest_address[1],
               notif->radio.dest_address[2],
               notif->radio.dest_address[3],
               notif->radio.dest_address[4],
               notif->radio.dest_address[5]);
        printf("Source address: %02X:%02X:%02X:%02X:%02X:%02X\n",
               notif->radio.source_address[0],
               notif->radio.source_address[1],
               notif->radio.source_address[2],
               notif->radio.source_address[3],
               notif->radio.source_address[4],
               notif->radio.source_address[5]);
        printf("Interface ID: %lu\n", (unsigned long)notif->radio.interface_id);
        printf("RSSI: %d [dBm]\n", notif->radio.rssi);
        printf("User Priority: %u\n", notif->radio.user_prio);
        printf("BTP port: %lu\n", (unsigned long)notif->gn_params.btp_params.btp_port);
        printf("Verification result: %lu\n", (unsigned long)notif->security.verify_result);
        printf("=====================================================\n");

        ++notif_ctx->cnt;
    }
}


int main(int argc, char* argv[])
{
    const char* host = (argc > 1) ? argv[1] : "127.0.0.1";

    /* Create a session */
    cms_session_t session = cms_get_session();

    /* Connect to the host */
    bool error = cms_api_connect_easy(&session, host);

    /* Create a context for the subscription callback */
    notif_ctx_t filtered_ctx = {
        .param = 1U,        /* just a random parameter */
        .cnt = 0U
    };

    /* Subscribe to a specific BTP */
    static const uint64_t FILTER_BTP = 2001U;
    cms_subs_id_t filtered_subs_id = CMS_SUBS_ID_INVALID;
    error = error || cms_gn_rx_subscribe(&session,
                                         FILTER_BTP,
                                         &gn_notif_cb,
                                         &filtered_ctx,
                                         &filtered_subs_id);
    if(error) {
        printf("Unable to subscribe to GeoNet Rx for BTP %llu\n", (unsigned long long)FILTER_BTP);
    }

    /* Create a context for the subscription callback */
    notif_ctx_t wildcard_ctx = {
        .param = 2U,        /* just a random parameter */
        .cnt = 0U
    };

    /* Subscribe to all messages */
    cms_subs_id_t wildcard_subs_id = CMS_SUBS_ID_INVALID;
    error = error || cms_gn_rx_subscribe(&session,
                                         CMS_GN_SUBSCRIBE_WILDCARD,
                                         gn_notif_cb,
                                         &wildcard_ctx,
                                         &wildcard_subs_id);
    if(error) {
        printf("Unable to subscribe to all GeoNet Rx notifications\n");
    } else {

        /* Wait some messages on the wildcard subscription */
        static const uint32_t EXIT_ON_RECV_COUNT = 50UL;
        while(wildcard_ctx.cnt < EXIT_ON_RECV_COUNT) {
            sleep(1);
        }
    }

    /* Unsubscribe */
    error = error || cms_gn_rx_unsubscribe(&session, filtered_subs_id);
    error = error || cms_gn_rx_unsubscribe(&session, wildcard_subs_id);

    /* Close connection and cleanup */
    cms_api_disconnect(&session);
    cms_api_clean();

    return (int)error;
}

