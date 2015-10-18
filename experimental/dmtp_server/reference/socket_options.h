/**
 * @file /brimstone/dmtp/network/socket_options.h
 *
 * @brief
 *
 * $Author$
 * $Date$
 * $Revision$
 *
 */


#ifndef SOCKET_OPTIONS_H
#define SOCKET_OPTIONS_H

bool_t   net_set_buffer_length(int sd, int buffer_recv, int buffer_send);
bool_t   net_set_keepalive(int sd, bool_t keepalive, int_t idle, int_t interval, int_t tolerance);
bool_t   net_set_linger(int sd, bool_t linger, int_t timeout);
bool_t   net_set_nodelay(int sd, bool_t nodelay);
bool_t   net_set_non_blocking(int sd, bool_t blocking);
bool_t   net_set_reuseable_address(int sd, bool_t reuse);
bool_t   net_set_timeout(int sd, uint32_t timeout_recv, uint32_t timeout_send);

#endif
