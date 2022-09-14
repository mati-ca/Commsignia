/** @file
@copyright
(C) Commsignia Ltd. - All Rights Reserved.
Unauthorised copying of this file, via any medium is strictly prohibited.
Proprietary and confidential.
@date 2020-2022
*/

#ifndef CMS_V2X_RADIO_H_
#define CMS_V2X_RADIO_H_

/** @file
@brief Radio service (RIO) module API
*/

#include <stdint.h>
#include <cms_v2x/common_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
@defgroup rio Radio service (RIO)
@ingroup api
*/

/**
Radio channel setup.
@ingroup rio
@see cms_rio_set_channel
@see cms_rio_get_channel
*/
typedef struct cms_rio_channel_setup_t {
    cms_interface_id_t interface_id;        /**< Radio interface ID */
    cms_channel_id_t channel_id;            /**< Radio channel ID */
} CMS_PACKED cms_rio_channel_setup_t;

/**
Radio interface data.
@ingroup rio
@see cms_rio_get_channel
*/
typedef struct cms_rio_interface_ref_t {
    cms_interface_id_t interface_id;        /**< Radio interface ID */
} CMS_PACKED cms_rio_interface_ref_t;

/**
Maximum length of the name of an interface
@ingroup rio
@see cms_rio_interface_name_t
*/
#define CMS_RIO_IF_NAME_MAX_LENGTH 16U

/**
Radio interface name
@ingroup rio
@see cms_rio_resolve_interface_t
*/
typedef char cms_rio_interface_name_t[CMS_RIO_IF_NAME_MAX_LENGTH];

/**
Radio interface data
@ingroup radio
@see cms_rio_resolve_interface_name_to_id
*/
typedef struct cms_rio_resolve_interface_t {
    cms_rio_interface_name_t interface_name;    /**< Name of the interface */
} CMS_PACKED cms_rio_resolve_interface_t;

/**
Type of the radio packet
@ingroup rio
@see cms_rio_send
*/
typedef enum cms_rio_packet_t {
    CMS_RIO_GNP_PACKET = 1U,                /**< GeoNet packet */
    CMS_RIO_WSMP_PACKET,                    /**< WSMP packet */
    CMS_RIO_IPV6_PACKET,                    /**< IPv6 packet */
    CMS_RIO_DSMP_PACKET,                    /**< DSMP packet */
} cms_rio_packet_t;

/**
Send parameters for radio send
@ingroup rio
@see cms_rio_send
*/
typedef struct cms_rio_send_data_t {
    cms_rio_packet_t packet_type;           /**< Type of the radio packet */
    cms_radio_tx_params_t radio_params;     /**< Radio tx parameters of the packet */
} CMS_PACKED cms_rio_send_data_t;

/**
Channel Busy Ratio (CBR) of the radio
@ingroup rio
*/
typedef uint32_t cms_rio_cbr_t;

/** result of rio api call */
typedef enum cms_rio_result_t {
    CMS_RIO_RESULT_OK = 0,                  /**< Success */
    CMS_RIO_RESULT_GENERAL_ERROR,           /**< General error */
    CMS_RIO_RESULT_NO_IFACE_ERROR,          /**< Interface does not exist */
    CMS_RIO_RESULT_DISABLED_IFACE_ERROR,    /**< Interface is disabled */
} cms_rio_result_t;

/**
Function to set the radio channel of a radio interface. Interfaces for which
this operation is not implemented (e.g. C-V2X or UDP-emulated), an error is
returned.
@ingroup rio
@param  session             Client session
@param  set_channel         Radio channel setup data containing interface ID and channel ID
@param[out] unused_out      Currently unused output parameter
@return true in case of error
*/
cms_rio_result_t cms_rio_set_channel(const cms_session_t* session,
                                     const cms_rio_channel_setup_t* set_channel,
                                     void* unused_out);

/**
Function to get the radio channel of a radio interface. Interfaces for which
this operation is not implemented (e.g. C-V2X or UDP-emulated), an error is
returned.
@ingroup rio
@param  session                 Client session
@param  interface               Radio interface ID
@param[out] channel_setup       Result radio channel setup data containing interface ID and channel ID
@return CMS_RIO_RESULT_OK in case of success
*/
cms_rio_result_t cms_rio_get_channel(const cms_session_t* session,
                                     const cms_rio_interface_ref_t* interface,
                                     cms_rio_channel_setup_t* channel_setup);

/**
Function to get the radio interface id by the name of the interface. Returns error
if the interface doesn't exists.
@ingroup rio
@param  session                 Client session
@param  interface_name          Name of the radio interface
@param[out] interface_ref       Radio interface ID
@return true in case of an error
*/
bool cms_rio_resolve_interface_name_to_id(const cms_session_t* session,
                                          const cms_rio_resolve_interface_t* interface_name,
                                          cms_rio_interface_ref_t* interface_ref);

/** MAC address of a radio interface */
typedef struct cms_rio_mac_setup_t {
    cms_interface_id_t interface_id;        /**< Radio interface ID */
    cms_mac_addr_t mac_address;             /**< MAC address */
} CMS_PACKED cms_rio_mac_setup_t;

/**
Function to get the mac address of a radio interface. Interfaces for which
this operation is not implemented an error is returned.
@ingroup rio
@param  session                 Client session
@param  interface               Radio interface ID
@param[out] address_setup       currently used mac adress for the interface
@return CMS_RIO_RESULT_OK in case of success
*/
cms_rio_result_t cms_rio_get_mac_address(const cms_session_t* session,
                                         const cms_rio_interface_ref_t* interface,
                                         cms_rio_mac_setup_t* address_setup);

/**
Function to set the mac address of a radio interface. Interfaces for which
this operation is not implemented an error is returned.
@ingroup rio
@param  session                 Client session
@param[in] address_setup        Setup data containing interface ID and new mac address
@param[out] unused_out          Unused output parameter
@return CMS_RIO_RESULT_OK in case of success
*/
cms_rio_result_t cms_rio_set_mac_address(const cms_session_t* session,
                                         const cms_rio_mac_setup_t* address_setup,
                                         void* unused_out);

/**
Send a radio packet with the given parameters and information
@ingroup rio
@param session              Client session
@param send_params          Send parameters of the Radio packet
@param payload              Message to be sent
@return CMS_RIO_RESULT_OK in case of success
*/
cms_rio_result_t cms_rio_send(const cms_session_t* session,
                              const cms_rio_send_data_t* send_params,
                              cms_buffer_view_t payload,
                              void* unused_out);


/**
Radio receive notification data
@ingroup rio
@see cms_rio_send
*/
typedef struct cms_rio_rx_notif_data_t {
    cms_rio_packet_t packet_type;           /**< Type of the radio packet */
    cms_radio_rx_params_t radio_params;     /**< Radio rx parameters of the packet */
} CMS_PACKED cms_rio_rx_notif_data_t;

/**
Received radio notify callback function type.
@ingroup rio
When a notification is sent to a subscribed client the following
typed function callback is called. This callback function shall be registered
during cms_rio_rx_subscribe function call.
@param packet_type  Packet type of the received message
@param notif        Notification metadata
@param msg          Raw payload buffer
@param ctx          User context, registered during subscribe
@see cms_rio_rx_subscribe
*/
typedef void (*cms_rio_rx_notify_f)(cms_rio_packet_t packet_type,
                                    const cms_rio_rx_notif_data_t* notif,
                                    cms_buffer_view_t msg,
                                    void* ctx);


/**
Subscribed packet type for radio receive notification.
@ingroup rio
It holds either
- a value of cms_rio_packet_t, or
- the value CMS_FAC_SUBSCRIBE_ALL

@see cms_rio_rx_subscribe
*/
typedef uint32_t cms_rio_subscribed_packet_t;

/**
Wildcard value for the radio receive subscribe function's type parameter.
@ingroup rio
When this value is used as a key for the radio receive subscribe function,
all received packets will trigger a notification.
@see cms_rio_rx_subscribe
*/
static const cms_rio_subscribed_packet_t CMS_RIO_SUBSCRIBE_ALL = 0UL;


/**
Radio receive subscribe.
@ingroup rio
Upon successful subscription, when a radio message is received
with a matching packet type port, the ITS software stack sends a notification
to the subscribed client with some metadata and the raw message.
@param session              Client session
@param packet_type          Packet type to subscribe for
@param callback_f           Notification callback function
@param ctx                  User context
@param[out] subs_id         Subscription ID output
@return true in case of error
@see cms_rio_rx_notify_f
@see CMS_RIO_SUBSCRIBE_WILDCARD
@see cms_rio_rx_unsubscribe
*/
bool cms_rio_rx_subscribe(const cms_session_t* session,
                          cms_rio_subscribed_packet_t packet_type,
                          cms_rio_rx_notify_f callback_f,
                          void* ctx,
                          cms_subs_id_t* subs_id);

/**
Unsubscribe from received radio messages.
@ingroup rio
Upon successful unsubscription, when a radio message is received
with a matching packet type, the ITS software stack does not send a notification
to the subscribed client any more. Messages with different subscribed packet type
will still be sent as a notification.
@param session              Client session
@param subs_id              Subscription ID, retrieved by cms_rio_rx_subscribe
@return true in case of error
@see cms_rio_rx_subscribe
*/
bool cms_rio_rx_unsubscribe(const cms_session_t* session, cms_subs_id_t subs_id);

/**
@experimentalapi
Inject a radio packet with the given parameters.
Injected packets are handled the same way by
facility and application layer as packets received on radio.
This api can be useful for testing.
@ingroup rio
@param session              Client session
@param notif params         Inject parameters of the packet
@param payload              Message to be sent
@return true in case of error
*/
bool cms_rio_inject(const cms_session_t* session,
                    const cms_rio_rx_notif_data_t* inject_params,
                    cms_buffer_view_t payload,
                    void* unused_out);

/**
Function to get the radio Channel Busy Ratio (CBR) in percents. If the interface does not
support CBR an error is returned.
@ingroup rio
@param  session                 Client session
@param  interface               Radio interface
@param[out] cbr                 CBR for the radio interface
@return CMS_RIO_RESULT_OK in case of success
*/
cms_rio_result_t cms_rio_get_cbr(const cms_session_t* session,
                                 const cms_rio_interface_ref_t* interface,
                                 cms_rio_cbr_t* cbr);


#ifdef __cplusplus
}
#endif

#endif /* CMS_V2X_RADIO_H_ */

