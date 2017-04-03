#define TRACE_MODULE _sgw_handler

#include "core_debug.h"

#include "event.h"
#include "sgw_path.h"
#include "s11_handler.h"

void sgw_s11_handle_create_session_request(
        gtp_xact_t *assoc_xact, c_uint8_t type, gtp_message_t *gtp_message)
{
    d_assert(sgw_s5c_send_to_pgw(assoc_xact, type, gtp_message) == CORE_OK, 
            return, "failed to send message");
}

void sgw_s11_handle_create_session_response(
        gtp_xact_t *xact, c_uint8_t type, gtp_message_t *gtp_message)
{
    d_info("handle create_session response");
    d_assert(sgw_s11_send_to_mme(xact, type, gtp_message) == CORE_OK, 
            return, "failed to send message");
}
