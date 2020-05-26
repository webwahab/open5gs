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

#include "test-common.h"

ogs_pkbuf_t *testgmm_build_registration_request(void)
{
    ogs_nas_5gs_message_t message;
    ogs_pkbuf_t *pkbuf = NULL;
    ogs_nas_5gs_registration_request_t *registration_request =
            &message.gmm.registration_request;
    ogs_nas_5gs_registration_type_t *registration_type =
            &registration_request->registration_type;
    ogs_nas_5gs_mobile_identity_t *mobile_identity =
            &registration_request->mobile_identity;
    ogs_nas_5gs_mobile_identity_imsi_t mobile_identity_imsi;
    ogs_nas_5gmm_capability_t *gmm_capability =
            &registration_request->gmm_capability;
    ogs_nas_ue_security_capability_t *ue_security_capability =
            &registration_request->ue_security_capability;

    memset(&message, 0, sizeof(message));
    message.gmm.h.extended_protocol_discriminator =
            OGS_NAS_EXTENDED_PROTOCOL_DISCRIMINATOR_5GMM;
    message.gmm.h.message_type = OGS_NAS_5GS_REGISTRATION_REQUEST;

    registration_type->type = 1;
    registration_type->follow_on_request = 1;
    registration_type->value = 1;

    memset(&mobile_identity_imsi, 0, sizeof(mobile_identity_imsi));
    mobile_identity_imsi.h.supi_format = OGS_NAS_5GS_SUPI_FORMAT_IMSI;
    mobile_identity_imsi.h.type = OGS_NAS_5GS_MOBILE_IDENTITY_SUCI;
    ogs_nas_from_plmn_id(&mobile_identity_imsi.nas_plmn_id,
            &test_self()->tai.plmn_id);
    mobile_identity_imsi.routing_indicator1 = 0;
    mobile_identity_imsi.routing_indicator2 = 0xf;
    mobile_identity_imsi.routing_indicator3 = 0xf;
    mobile_identity_imsi.routing_indicator4 = 0xf;
    mobile_identity_imsi.protection_scheme_id = OGS_NAS_5GS_NULL_SCHEME;
    mobile_identity_imsi.home_network_pki_value = 0;
    mobile_identity_imsi.scheme_output[0] = 0;
    mobile_identity_imsi.scheme_output[1] = 0;
    mobile_identity_imsi.scheme_output[2] = 0x47;
    mobile_identity_imsi.scheme_output[3] = 0x78;

    mobile_identity->length = 12;
    mobile_identity->buffer = &mobile_identity_imsi;

    registration_request->presencemask |=
            OGS_NAS_5GS_REGISTRATION_REQUEST_5GMM_CAPABILITY_PRESENT;
    gmm_capability->length = 1;
    gmm_capability->lte_positioning_protocol_capability = 1;
    gmm_capability->ho_attach = 1; 
    gmm_capability->s1_mode = 1; 

    registration_request->presencemask |=
            OGS_NAS_5GS_REGISTRATION_REQUEST_UE_SECURITY_CAPABILITY_PRESENT;
    ue_security_capability->length = 8;
    ue_security_capability->nea = 0xff;
    ue_security_capability->nia = 0xff;
    ue_security_capability->eps_ea = 0xff;
    ue_security_capability->eps_ia = 0xff;

    return ogs_nas_5gs_plain_encode(&message);
}