/**
 * @file /brimstone/dmtp/server/stub.c
 *
 * @brief
 * Stub - find a home for these functions
 *
 * @Author
 *   Ripped off from magma, modified by Kent
 *
 */

//========================================

// from: src/network/connections.c
/**
 * @brief   Return the status of a specified connection.
 * @param   con     the input client connection.
 * @return  -1 on error, 0 for unknown status, 1 for connected, or 2 if the socket has been shutting down.
 */
int_t con_status(connection_t *con) {

    int_t result = -1;

    if (con && (con->network.sockd != -1)) {
        result = con->network.status;
    }

    return result;
}

/**
 * @brief
 * Destroy and free a generic connection object after executing its protocol-specific destructor; update any statistics accordingly.
 * @param con
 * a pointer to the connection to be destroyed.
 * @return
 * None
 * @Caller
 * called by dmtp_quit()
 */
void con_destroy(connection_t *con) {

    if (con) {
#ifdef TODO
        // TODO: SDS strings!
        free(con->dmtp.helo);
        free(con->dmtp.mailfrom);

        if (con->dmtp.message) {
            mail_destroy_message(con->smtp.message);
        }

        if (con->dmtp.in_prefs) {
            smtp_free_inbound(con->smtp.in_prefs);
        }

        if (con->dmtp.out_prefs) {
            smtp_free_outbound(con->smtp.out_prefs);
        }
#endif

        if (con->network.sockd != -1) {
            close(con->network.sockd);
        }

        // TODO: SDS strings!
        free(con->network.buffer);
        free(con->network.reverse.domain);
        free(con);
    }
    return;
}
