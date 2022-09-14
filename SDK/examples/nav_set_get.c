/** @file
@copyright
(C) Commsignia Ltd. - All Rights Reserved.
Unauthorised copying of this file, via any medium is strictly prohibited.
Proprietary and confidential.
@date 2020-2021
*/

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#include <cms_v2x/api.h>
#include <cms_v2x/nav.h>

/** @file
@brief Set and get nav fix.
@ingroup ex
*/

const char* l_nav_drive_direction_to_str(cms_nav_drive_direction_t drive_direction)
{
    const char* str = "unavailable";

    switch(drive_direction) {
    case CMS_NAV_DRIVE_DIR_FORWARD:
        str = "forward";
        break;

    case CMS_NAV_DRIVE_DIR_BACKWARD:
        str = "backward";
        break;

    default:
        break;
    }

    return str;
}

void l_nav_print_fix(const cms_nav_fix_t* nav_fix)
{
    printf("NAV fix is valid: %u\n", (unsigned)nav_fix->is_valid);
    printf("Timestamp: %llu\n", (unsigned long long)nav_fix->timestamp);
    printf("Latitude angle: %ld\n", (long)nav_fix->latitude);
    printf("Longitudinal angle: %ld\n", (long)nav_fix->longitude);
    printf("Altitude: %ld\n", (long)nav_fix->altitude);
    printf("Confidence ellipse for latitude angle: %llu\n", (unsigned long long)nav_fix->pce_semi_major);
    printf("Confidence ellipse for longitude angle: %llu\n", (unsigned long long)nav_fix->pce_semi_minor);
    printf("Confidence ellipse orientation: %lu\n", (unsigned long)nav_fix->pce_orientation);
    printf("Heading: %lu\n", (unsigned long)nav_fix->heading);
    printf("Speed: %lu\n", (unsigned long)nav_fix->speed);
    printf("Direction: %s\n", l_nav_drive_direction_to_str(nav_fix->drive_direction));
}


int main(int argc, char* argv[])
{
    const char* host = (argc > 1) ? argv[1] : "127.0.0.1";

    /* Create a session */
    cms_session_t session = cms_get_session();

    /* Connect to the host */
    bool error = cms_api_connect_easy(&session, host);

    /* Get the latest position */
    cms_nav_fix_t nav_fix = {0};
    error = error || cms_nav_get_last_fix(&session, NULL, &nav_fix);
    if(error) {
        printf("Unable to get NAV fix\n");
    } else {
        printf("Nav fix before manual setting:\n");
        l_nav_print_fix(&nav_fix);
    }

    /*
    Create a nav data to set,
    e.g. set the position of the Commsignia office with 1.5m accuracy:
    - latitude: 47.4754593°
    - longitude: 19.0582323°
    */
    cms_nav_set_manual_t set_nav = {0};
    set_nav.auto_update = false; /* this functionality not yet supported */
    /* Set the time to current time, enable adjustSystemTime in config
    if you want to update system time of the Commsignia V2X software stack */
    set_nav.nav_fix.is_valid = true;
    set_nav.nav_fix.timestamp = (time(NULL) * 1000);
    set_nav.nav_fix.latitude = 474754590L;
    set_nav.nav_fix.longitude = 190582320L;
    set_nav.nav_fix.altitude = 3000;
    set_nav.nav_fix.pce_semi_major = 1500;
    set_nav.nav_fix.pce_semi_minor = 1500;
    set_nav.nav_fix.pce_orientation = 250;
    set_nav.nav_fix.heading = 300;
    set_nav.nav_fix.speed = 18000;
    set_nav.nav_fix.drive_direction = CMS_NAV_DRIVE_DIR_FORWARD;

    /* Switch to Manual navigation mode */
    cms_nav_source_t nav_source = CMS_NAV_SOURCE_MANUAL;
    error = error || cms_nav_set_source(&session, &nav_source, NULL);
    if(error) {
        printf("Unable to switch to manual mode\n");
    } else {
        printf("Switched to manual mode\n");
    }

    /* Set the position and time */
    for(int index = 0; index < 10; index++) {
        /* in every 0.1s setst time 0.1s ahead and latitude increase with 10 micro degree*/
        set_nav.nav_fix.timestamp += 100;
        set_nav.nav_fix.latitude += 100;
        error = error || cms_nav_set_manual(&session, &set_nav, NULL);
        if(error) {
            printf("Unable to set NAV fix\n");
        }
        /* sleep 0.1 s */
        const unsigned int sleep_us = 100000;
        usleep(sleep_us);
    }

    /* Get the latest position after set position, expected to be the same previously setted */
    error = error || cms_nav_get_last_fix(&session, NULL, &nav_fix);
    if(error) {
        printf("Unable to get NAV fix\n");
    } else {
        printf("Nav fix after manual setting:\n");
        l_nav_print_fix(&nav_fix);
    }

    /* timestamp will be overwritten by current system time in case of NA timestamp*/
    set_nav.nav_fix.timestamp = CMS_UTC_TIMESTAMP_NA;
    error = error || cms_nav_set_manual(&session, &set_nav, NULL);
    if(error) {
        printf("Unable to set NAV fix\n");
    }

    /* Close connection and cleanup */
    cms_api_disconnect(&session);
    cms_api_clean();

    return (int)error;
}
