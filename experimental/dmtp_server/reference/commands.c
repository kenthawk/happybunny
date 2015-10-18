/**
 * @file /brimstone/dmtp/server/commands.c
 *
 * @brief Functions involved with parsing and routing DMTP commands.
 *
 */

#include "stub.h"
#include "commands.h"


static
bool_t is_empty(char *s) {
    if (s == NULL || *s == 0) {
        return true;
    }
    return false;
}

int_t dmtp_compare(const void *compare, const void *command) {

	int_t result;
	command_t *cmd = (command_t *)command, *cmp = (command_t *)compare;

	if (!cmp->function)	result = st_cmp_ci_starts(PLACER(cmp->string, cmp->length), PLACER(cmd->string, cmd->length));
	else result = st_cmp_ci_eq(PLACER(cmp->string, cmp->length), PLACER(cmd->string, cmd->length));

	return result;
}

/**
 * @brief	Sort the DMTP command table to be ready for binary searches.
 * @return	This function returns no value.
 */
void dmtp_sort(void) {
	qsort(dmtp_commands, sizeof(dmtp_commands) / sizeof(dmtp_commands[0]), sizeof(command_t), &dmtp_compare);
	return;
}

#if 0
/**
 * @brief
 * dmtp_requeue checks if the connection has violated the interface.  If 
 * not, the command processor is requeued.
 */
void dmtp_requeue(connection_t *con) {

	if (!status() || con_status(con) < 0 || con->protocol.violations > con->server->violations.cutoff) {
		enqueue(&dmtp_quit, con);
	}
	else {
		enqueue(&dmtp_process, con);
	}

	return;
}
#endif

/**
 * @brief	The main entry point in the DMTP server for processing commands issued by clients.
 * @param	con		a pointer to the connection object of the client issuing the DMTP command.
 * @return	This function returns no value.
 */
int_t dmtp_process(connection_t *con) {

	command_t *command, client = { .function = NULL };
    con->command = NULL;

    // Violated?
    if (con->protocol.spins + con->protocol.violations > con->server->violations.cutoff) {
        goto error;
    }

    // Read error?
	if (con_read_line(con, false) < 0) {
        goto error;
	}

    // Keeping us busy with empty lines?
	if (is_empty(con->network.line)) {
        con->protocol.spins++;
        goto out;
	}

    // TODO: use SDS string here
	client.string = con->network.line;
	client.length = strlen(con->network.line);

	command = bsearch(&client, dmtp_commands, sizeof(dmtp_commands) / sizeof(dmtp_commands[0]),
                      sizeof(command_t), dmtp_compare);

	if (command != NULL) { 
		con->command = command;
		con->protocol.spins = 0;
	}
    // Command is invalid!  Note: this is the only place that
    // con->protocol.violations gets bumped
	else {
		con->command = NULL;
        dmtp_invalid(con);
        goto out;
	}

out:
    return 0;
error:
    return -1;
}
