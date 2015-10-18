/**
 * @file /brimstone/dmtp/server/session.c
 *
 * @brief	Functions used to handle DMTP sessions.
 *
 */

#include "../borrowed/magma_required.h"

/**
 * @brief	Reset an DMTP session to its initialized state.
 * @param	con		the DMTP client connection to be reset.
 * @return	This function returns no value.
 */
void dmtp_session_reset(connection_t *con) {

	return;
}

/**
 * @brief	Destroy the data associated with an DMTP session.
 * @param	con		the DMTP client connection to be destroyed.
 * @return	This function returns no value.
 */
void dmtp_session_destroy(connection_t *con) {
    // TODO: SDS strings!
    free(con->dmtp.helo);
    free(con->dmtp.mailfrom);

#if TODO
    if (con->smtp.message) {
        mail_destroy_message(con->smtp.message);
    }

    if (con->smtp.in_prefs) {
        smtp_free_inbound(con->smtp.in_prefs);
    }

    if (con->smtp.out_prefs) {
        smtp_free_outbound(con->smtp.out_prefs);
    }
#endif

    return;
}


	return;
}

