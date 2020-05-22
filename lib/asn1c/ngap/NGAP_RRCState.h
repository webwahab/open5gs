/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "NGAP-IEs"
 * 	found in "../support/ngap-r16.1.0/38413-g10.asn"
 * 	`asn1c -pdu=all -fcompound-names -findirect-choice -fno-include-deps`
 */

#ifndef	_NGAP_RRCState_H_
#define	_NGAP_RRCState_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeEnumerated.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum NGAP_RRCState {
	NGAP_RRCState_inactive	= 0,
	NGAP_RRCState_connected	= 1
	/*
	 * Enumeration is extensible
	 */
} e_NGAP_RRCState;

/* NGAP_RRCState */
typedef long	 NGAP_RRCState_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_NGAP_RRCState;
asn_struct_free_f NGAP_RRCState_free;
asn_struct_print_f NGAP_RRCState_print;
asn_constr_check_f NGAP_RRCState_constraint;
ber_type_decoder_f NGAP_RRCState_decode_ber;
der_type_encoder_f NGAP_RRCState_encode_der;
xer_type_decoder_f NGAP_RRCState_decode_xer;
xer_type_encoder_f NGAP_RRCState_encode_xer;
oer_type_decoder_f NGAP_RRCState_decode_oer;
oer_type_encoder_f NGAP_RRCState_encode_oer;
per_type_decoder_f NGAP_RRCState_decode_uper;
per_type_encoder_f NGAP_RRCState_encode_uper;
per_type_decoder_f NGAP_RRCState_decode_aper;
per_type_encoder_f NGAP_RRCState_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _NGAP_RRCState_H_ */
#include <asn_internal.h>
