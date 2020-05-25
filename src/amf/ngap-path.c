/*
 * Copyright (C) 2019,2020 by Sukchan Lee <acetcom@gmail.com>
 *
 * This file is part of Open5GS.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "ogs-sctp.h"

#include "context.h"

#include "ngap-build.h"
#include "ngap-path.h"

int ngap_open(void)
{
    ogs_socknode_t *node = NULL;

    ogs_list_for_each(&amf_self()->ngap_list, node)
        ngap_server(node);

    ogs_list_for_each(&amf_self()->ngap_list6, node)
        ngap_server(node);

    return OGS_OK;
}

void ngap_close()
{
    ogs_socknode_remove_all(&amf_self()->ngap_list);
    ogs_socknode_remove_all(&amf_self()->ngap_list6);
}

int ngap_send(ogs_sock_t *sock, ogs_pkbuf_t *pkbuf,
        ogs_sockaddr_t *addr, uint16_t stream_no)
{
    int sent;

    ogs_assert(sock);
    ogs_assert(pkbuf);

    sent = ogs_sctp_sendmsg(sock, pkbuf->data, pkbuf->len,
            addr, OGS_SCTP_NGAP_PPID, stream_no);
    if (sent < 0 || sent != pkbuf->len) {
        ogs_error("ogs_sctp_sendmsg error (%d:%s)", errno, strerror(errno));
        return OGS_ERROR;
    }
    ogs_pkbuf_free(pkbuf);

    return OGS_OK;
}

int ngap_send_to_gnb(amf_gnb_t *gnb, ogs_pkbuf_t *pkbuf, uint16_t stream_no)
{
    char buf[OGS_ADDRSTRLEN];
    int rv;

    ogs_assert(gnb);
    ogs_assert(pkbuf);
    ogs_assert(gnb->sock);

    ogs_debug("    IP[%s] ENB_ID[%d]",
            OGS_ADDR(gnb->addr, buf), gnb->gnb_id);

    rv = ngap_send(gnb->sock, pkbuf,
            gnb->sock_type == SOCK_STREAM ? NULL : gnb->addr,
            stream_no);
    if (rv != OGS_OK) {
        ogs_error("ngap_send() failed");

        ogs_pkbuf_free(pkbuf);
        ngap_event_push(AMF_EVT_NGAP_LO_CONNREFUSED,
                gnb->sock, gnb->addr, NULL, 0, 0);
    }

    return rv;
}

#if 0
int ngap_send_to_gnb_ue(gnb_ue_t *gnb_ue, ogs_pkbuf_t *pkbuf)
{
    amf_gnb_t *gnb = NULL;

    ogs_assert(gnb_ue);
    gnb = gnb_ue->gnb;
    ogs_assert(gnb);

    return ngap_send_to_gnb(gnb, pkbuf, gnb_ue->gnb_ostream_id);
}

int ngap_delayed_send_to_gnb_ue(
        gnb_ue_t *gnb_ue, ogs_pkbuf_t *pkbuf, ogs_time_t duration)
{
    ogs_assert(gnb_ue);
    ogs_assert(pkbuf);
        
    if (duration) {
        amf_event_t *e = NULL;

        e = amf_event_new(AMF_EVT_NGAP_TIMER);
        ogs_assert(e);
        e->timer = ogs_timer_add(
                amf_self()->timer_mgr, amf_timer_s1_delayed_send, e);
        ogs_assert(e->timer);
        e->pkbuf = pkbuf;
        e->gnb_ue = gnb_ue;
        e->gnb = gnb_ue->gnb;

        ogs_timer_start(e->timer, duration);

        return OGS_OK;
    } else {
        amf_gnb_t *gnb = NULL;
        gnb = gnb_ue->gnb;
        ogs_assert(gnb);
        return ngap_send_to_gnb_ue(gnb_ue, pkbuf);
    }
}

int ngap_send_to_esm(amf_ue_t *amf_ue, ogs_pkbuf_t *esmbuf)
{
    int rv;
    amf_event_t *e = NULL;

    ogs_assert(amf_ue);
    ogs_assert(esmbuf);

    e = amf_event_new(AMF_EVT_ESM_MESSAGE);
    ogs_assert(e);
    e->amf_ue = amf_ue;
    e->pkbuf = esmbuf;
    rv = ogs_queue_push(amf_self()->queue, e);
    if (rv != OGS_OK) {
        ogs_warn("ogs_queue_push() failed:%d", (int)rv);
        ogs_pkbuf_free(e->pkbuf);
        amf_event_free(e);
    }

    return rv;
}
 
int ngap_send_to_nas(gnb_ue_t *gnb_ue,
        NGAP_ProcedureCode_t procedureCode, NGAP_NAS_PDU_t *nasPdu)
{
    ogs_nas_eps_security_header_t *sh = NULL;
    ogs_nas_security_header_type_t security_header_type;

    ogs_nas_emm_header_t *h = NULL;
    ogs_pkbuf_t *nasbuf = NULL;
    amf_event_t *e = NULL;

    ogs_assert(gnb_ue);
    ogs_assert(nasPdu);

    /* The Packet Buffer(pkbuf_t) for NAS message MUST make a HEADROOM. 
     * When calculating AES_CMAC, we need to use the headroom of the packet. */
    nasbuf = ogs_pkbuf_alloc(NULL, OGS_NAS_HEADROOM+nasPdu->size);
    ogs_pkbuf_reserve(nasbuf, OGS_NAS_HEADROOM);
    ogs_pkbuf_put_data(nasbuf, nasPdu->buf, nasPdu->size);

    sh = (ogs_nas_eps_security_header_t *)nasbuf->data;
    ogs_assert(sh);

    memset(&security_header_type, 0, sizeof(ogs_nas_security_header_type_t));
    switch(sh->security_header_type) {
    case OGS_NAS_SECURITY_HEADER_PLAIN_NAS_MESSAGE:
        break;
    case OGS_NAS_SECURITY_HEADER_FOR_SERVICE_REQUEST_MESSAGE:
        security_header_type.service_request = 1;
        break;
    case OGS_NAS_SECURITY_HEADER_INTEGRITY_PROTECTED:
        security_header_type.integrity_protected = 1;
        ogs_pkbuf_pull(nasbuf, 6);
        break;
    case OGS_NAS_SECURITY_HEADER_INTEGRITY_PROTECTED_AND_CIPHERED:
        security_header_type.integrity_protected = 1;
        security_header_type.ciphered = 1;
        ogs_pkbuf_pull(nasbuf, 6);
        break;
    case OGS_NAS_SECURITY_HEADER_INTEGRITY_PROTECTED_AND_NEW_SECURITY_CONTEXT:
        security_header_type.integrity_protected = 1;
        security_header_type.new_security_context = 1;
        ogs_pkbuf_pull(nasbuf, 6);
        break;
    case OGS_NAS_SECURITY_HEADER_INTEGRITY_PROTECTED_AND_CIPHTERD_WITH_NEW_INTEGRITY_CONTEXT:
        security_header_type.integrity_protected = 1;
        security_header_type.ciphered = 1;
        security_header_type.new_security_context = 1;
        ogs_pkbuf_pull(nasbuf, 6);
        break;
    default:
        ogs_error("Not implemented(security header type:0x%x)",
                sh->security_header_type);
        return OGS_ERROR;
    }

    if (gnb_ue->amf_ue) {
        if (nas_eps_security_decode(gnb_ue->amf_ue,
                security_header_type, nasbuf) != OGS_OK) {
            ogs_error("nas_eps_security_decode failed()");
	        return OGS_ERROR;
        }
    }

    h = (ogs_nas_emm_header_t *)nasbuf->data;
    ogs_assert(h);
    if (h->protocol_discriminator == OGS_NAS_PROTOCOL_DISCRIMINATOR_EMM) {
        int rv;
        e = amf_event_new(AMF_EVT_EMM_MESSAGE);
        ogs_assert(e);
        e->gnb_ue = gnb_ue;
        e->ngap_code = procedureCode;
        e->nas_type = security_header_type.type;
        e->pkbuf = nasbuf;
        rv = ogs_queue_push(amf_self()->queue, e);
        if (rv != OGS_OK) {
            ogs_warn("ogs_queue_push() failed:%d", (int)rv);
            ogs_pkbuf_free(e->pkbuf);
            amf_event_free(e);
        }
        return rv;
    } else if (h->protocol_discriminator == OGS_NAS_PROTOCOL_DISCRIMINATOR_ESM) {
        amf_ue_t *amf_ue = gnb_ue->amf_ue;
        ogs_assert(amf_ue);
        return ngap_send_to_esm(amf_ue, nasbuf);
    } else {
        ogs_error("Unknown/Unimplemented NAS Protocol discriminator 0x%02x",
                  h->protocol_discriminator);
        return OGS_ERROR;
    }
}

void ngap_send_s1_setup_response(amf_gnb_t *gnb)
{
    ogs_pkbuf_t *ngap_buffer;

    ogs_debug("[AMF] NG-Setup response");
    ngap_buffer = ngap_build_setup_rsp();
    ogs_expect_or_return(ngap_buffer);

    ogs_expect(OGS_OK ==
            ngap_send_to_gnb(gnb, ngap_buffer, NGAP_NON_UE_SIGNALLING));
}
#endif

void ngap_send_s1_setup_failure(
        amf_gnb_t *gnb, NGAP_Cause_PR group, long cause)
{
    ogs_pkbuf_t *ngap_buffer;

    ogs_debug("[AMF] NG-Setup failure");
    ngap_buffer = ngap_build_setup_failure(group, cause, NGAP_TimeToWait_v10s);
    ogs_expect_or_return(ngap_buffer);

    ogs_expect(OGS_OK ==
            ngap_send_to_gnb(gnb, ngap_buffer, NGAP_NON_UE_SIGNALLING));
}

#if 0
void ngap_send_initial_context_setup_request(amf_ue_t *amf_ue)
{
    int rv;
    ogs_pkbuf_t *ngapbuf = NULL;

    ogs_assert(amf_ue);

    ngapbuf = ngap_build_initial_context_setup_request(amf_ue, NULL);
    ogs_expect_or_return(ngapbuf);

    rv = nas_eps_send_to_gnb(amf_ue, ngapbuf);
    ogs_expect(rv == OGS_OK);
}

void ngap_send_ue_context_modification_request(amf_ue_t *amf_ue)
{
    int rv;
    ogs_pkbuf_t *ngapbuf = NULL;

    ogs_assert(amf_ue);

    ngapbuf = ngap_build_ue_context_modification_request(amf_ue);
    ogs_expect_or_return(ngapbuf);

    rv = nas_eps_send_to_gnb(amf_ue, ngapbuf);
    ogs_expect(rv == OGS_OK);
}

void ngap_send_ue_context_release_command(
    gnb_ue_t *gnb_ue, NGAP_Cause_PR group, long cause,
    uint8_t action, uint32_t delay)
{
    int rv;
    ogs_pkbuf_t *ngapbuf = NULL;

    ogs_assert(gnb_ue);

    ogs_debug("[AMF] UE Context release command");
    ogs_debug("    ENB_UE_NGAP_ID[%d] AMF_UE_NGAP_ID[%d]",
            gnb_ue->gnb_ue_ngap_id, gnb_ue->amf_ue_ngap_id);

    if (delay) {
        ogs_assert(action != NGAP_UE_CTX_REL_INVALID_ACTION);
        gnb_ue->ue_ctx_rel_action = action;

        ogs_debug("    Group[%d] Cause[%d] Action[%d] Delay[%d]",
                group, (int)cause, action, delay);

        ngapbuf = ngap_build_ue_context_release_command(gnb_ue, group, cause);
        ogs_expect_or_return(ngapbuf);

        rv = ngap_delayed_send_to_gnb_ue(gnb_ue, ngapbuf, delay);
        ogs_expect(rv == OGS_OK);
    } else {
        ogs_assert(action != NGAP_UE_CTX_REL_INVALID_ACTION);
        gnb_ue->ue_ctx_rel_action = action;

        ogs_debug("    Group[%d] Cause[%d] Action[%d] Delay[%d]",
                group, (int)cause, action, delay);

        ngapbuf = ngap_build_ue_context_release_command(gnb_ue, group, cause);
        ogs_expect_or_return(ngapbuf);

        rv = ngap_delayed_send_to_gnb_ue(gnb_ue, ngapbuf, 0);
        ogs_expect(rv == OGS_OK);
    }
}

void ngap_send_paging(amf_ue_t *amf_ue, NGAP_CNDomain_t cn_domain)
{
    ogs_pkbuf_t *ngapbuf = NULL;
    amf_gnb_t *gnb = NULL;
    int i;
    int rv;

    /* Find enB with matched TAI */
    ogs_list_for_each(&amf_self()->gnb_list, gnb) {
        for (i = 0; i < gnb->num_of_supported_ta_list; i++) {

            if (memcmp(&gnb->supported_ta_list[i], &amf_ue->tai,
                        sizeof(ogs_5gs_tai_t)) == 0) {

                if (amf_ue->t3413.pkbuf) {
                    ngapbuf = amf_ue->t3413.pkbuf;
                } else {
                    ngapbuf = ngap_build_paging(amf_ue, cn_domain);
                    ogs_expect_or_return(ngapbuf);
                }

                amf_ue->t3413.pkbuf = ogs_pkbuf_copy(ngapbuf);

                rv = ngap_send_to_gnb(gnb, ngapbuf, NGAP_NON_UE_SIGNALLING);
                ogs_expect(rv == OGS_OK);
            }
        }
    }

    /* Start T3413 */
    ogs_timer_start(amf_ue->t3413.timer, 
            amf_timer_cfg(AMF_TIMER_T3413)->duration);
}

void ngap_send_amf_configuration_transfer(
        amf_gnb_t *target_gnb,
        NGAP_SONConfigurationTransfer_t *SONConfigurationTransfer)
{
    int rv;
    ogs_pkbuf_t *ngapbuf = NULL;

    ogs_assert(target_gnb);
    ogs_assert(SONConfigurationTransfer);

    ngapbuf = ngap_build_amf_configuration_transfer(SONConfigurationTransfer);
    ogs_expect_or_return(ngapbuf);

    rv = ngap_send_to_gnb(target_gnb, ngapbuf, NGAP_NON_UE_SIGNALLING);
    ogs_expect(rv == OGS_OK);
}

void ngap_send_path_switch_ack(amf_ue_t *amf_ue)
{
    int rv;
    ogs_pkbuf_t *ngapbuf = NULL;

    ogs_assert(amf_ue);

    ngapbuf = ngap_build_path_switch_ack(amf_ue);
    ogs_expect_or_return(ngapbuf);

    rv = nas_eps_send_to_gnb(amf_ue, ngapbuf);
    ogs_expect(rv == OGS_OK);
}

void ngap_send_handover_command(gnb_ue_t *source_ue)
{
    int rv;
    ogs_pkbuf_t *ngapbuf = NULL;

    ogs_assert(source_ue);

    ngapbuf = ngap_build_handover_command(source_ue);
    ogs_expect_or_return(ngapbuf);

    rv = ngap_send_to_gnb_ue(source_ue, ngapbuf);
    ogs_expect(rv == OGS_OK);
}

void ngap_send_handover_preparation_failure(
        gnb_ue_t *source_ue, NGAP_Cause_t *cause)
{
    int rv;
    ogs_pkbuf_t *ngapbuf = NULL;

    ogs_assert(source_ue);
    ogs_assert(cause);

    ngapbuf = ngap_build_handover_preparation_failure(source_ue, cause);
    ogs_expect_or_return(ngapbuf);

    rv = ngap_send_to_gnb_ue(source_ue, ngapbuf);
    ogs_expect(rv == OGS_OK);
}

void ngap_send_handover_cancel_ack(gnb_ue_t *source_ue)
{
    int rv;
    ogs_pkbuf_t *ngapbuf = NULL;

    ogs_assert(source_ue);

    ngapbuf = ngap_build_handover_cancel_ack(source_ue);
    ogs_expect_or_return(ngapbuf);

    rv = ngap_send_to_gnb_ue(source_ue, ngapbuf);
    ogs_expect(rv == OGS_OK);
}


void ngap_send_handover_request(
        amf_ue_t *amf_ue,
        amf_gnb_t *target_gnb,
        NGAP_ENB_UE_NGAP_ID_t *gnb_ue_ngap_id,
        NGAP_AMF_UE_NGAP_ID_t *amf_ue_ngap_id,
        NGAP_HandoverType_t *handovertype,
        NGAP_Cause_t *cause,
        NGAP_Source_ToTarget_TransparentContainer_t
            *source_totarget_transparentContainer)
{
    int rv;
    ogs_pkbuf_t *ngapbuf = NULL;

    gnb_ue_t *source_ue = NULL, *target_ue = NULL;

    ogs_debug("[AMF] Handover request");
    
    ogs_assert(target_gnb);

    ogs_assert(amf_ue);
    source_ue = amf_ue->gnb_ue;
    ogs_assert(source_ue);
    ogs_assert(source_ue->target_ue == NULL);

    target_ue = gnb_ue_add(target_gnb, INVALID_UE_NGAP_ID);
    ogs_assert(target_ue);

    ogs_debug("    Source : ENB_UE_NGAP_ID[%d] AMF_UE_NGAP_ID[%d]",
            source_ue->gnb_ue_ngap_id, source_ue->amf_ue_ngap_id);
    ogs_debug("    Target : ENB_UE_NGAP_ID[Unknown] AMF_UE_NGAP_ID[%d]",
            target_ue->amf_ue_ngap_id);

    source_ue_associate_target_ue(source_ue, target_ue);

    ngapbuf = ngap_build_handover_request(amf_ue, target_ue,
            gnb_ue_ngap_id, amf_ue_ngap_id,
            handovertype, cause,
            source_totarget_transparentContainer);
    ogs_expect_or_return(ngapbuf);

    rv = ngap_send_to_gnb_ue(target_ue, ngapbuf);
    ogs_expect(rv == OGS_OK);
}

void ngap_send_amf_status_transfer(
        gnb_ue_t *target_ue,
        NGAP_ENB_StatusTransfer_TransparentContainer_t
            *gnb_statustransfer_transparentContainer)
{
    int rv;
    ogs_pkbuf_t *ngapbuf = NULL;

    ogs_assert(target_ue);

    ngapbuf = ngap_build_amf_status_transfer(target_ue,
            gnb_statustransfer_transparentContainer);
    ogs_expect_or_return(ngapbuf);

    rv = ngap_send_to_gnb_ue(target_ue, ngapbuf);
    ogs_expect(rv == OGS_OK);
}

void ngap_send_error_indication(
        amf_gnb_t *gnb,
        NGAP_AMF_UE_NGAP_ID_t *amf_ue_ngap_id,
        NGAP_ENB_UE_NGAP_ID_t *gnb_ue_ngap_id,
        NGAP_Cause_PR group, long cause)
{
    int rv;
    ogs_pkbuf_t *ngapbuf = NULL;

    ogs_assert(gnb);

    ngapbuf = ngap_build_error_indication(
            amf_ue_ngap_id, gnb_ue_ngap_id, group, cause);
    ogs_expect_or_return(ngapbuf);

    rv = ngap_send_to_gnb(gnb, ngapbuf, NGAP_NON_UE_SIGNALLING);
    ogs_expect(rv == OGS_OK);
}

void ngap_send_s1_reset_ack(
        amf_gnb_t *gnb,
        NGAP_UE_associatedLogicalNG_ConnectionListRes_t *partOfNG_Interface)
{
    int rv;
    ogs_pkbuf_t *ngapbuf = NULL;

    ogs_assert(gnb);

    ngapbuf = ngap_build_s1_reset_ack(partOfNG_Interface);
    ogs_expect_or_return(ngapbuf);

    rv = ngap_send_to_gnb(gnb, ngapbuf, NGAP_NON_UE_SIGNALLING);
    ogs_expect(rv == OGS_OK);
}
#endif
