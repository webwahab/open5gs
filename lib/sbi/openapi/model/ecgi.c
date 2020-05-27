
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ecgi.h"

OpenAPI_ecgi_t *OpenAPI_ecgi_create(
    OpenAPI_plmn_id_t *plmn_id,
    char *eutra_cell_id,
    char *nid
    )
{
    OpenAPI_ecgi_t *ecgi_local_var = OpenAPI_malloc(sizeof(OpenAPI_ecgi_t));
    if (!ecgi_local_var) {
        return NULL;
    }
    ecgi_local_var->plmn_id = plmn_id;
    ecgi_local_var->eutra_cell_id = eutra_cell_id;
    ecgi_local_var->nid = nid;

    return ecgi_local_var;
}

void OpenAPI_ecgi_free(OpenAPI_ecgi_t *ecgi)
{
    if (NULL == ecgi) {
        return;
    }
    OpenAPI_lnode_t *node;
    OpenAPI_plmn_id_free(ecgi->plmn_id);
    ogs_free(ecgi->eutra_cell_id);
    ogs_free(ecgi->nid);
    ogs_free(ecgi);
}

cJSON *OpenAPI_ecgi_convertToJSON(OpenAPI_ecgi_t *ecgi)
{
    cJSON *item = NULL;

    if (ecgi == NULL) {
        ogs_error("OpenAPI_ecgi_convertToJSON() failed [Ecgi]");
        return NULL;
    }

    item = cJSON_CreateObject();
    if (!ecgi->plmn_id) {
        ogs_error("OpenAPI_ecgi_convertToJSON() failed [plmn_id]");
        goto end;
    }
    cJSON *plmn_id_local_JSON = OpenAPI_plmn_id_convertToJSON(ecgi->plmn_id);
    if (plmn_id_local_JSON == NULL) {
        ogs_error("OpenAPI_ecgi_convertToJSON() failed [plmn_id]");
        goto end;
    }
    cJSON_AddItemToObject(item, "plmnId", plmn_id_local_JSON);
    if (item->child == NULL) {
        ogs_error("OpenAPI_ecgi_convertToJSON() failed [plmn_id]");
        goto end;
    }

    if (!ecgi->eutra_cell_id) {
        ogs_error("OpenAPI_ecgi_convertToJSON() failed [eutra_cell_id]");
        goto end;
    }
    if (cJSON_AddStringToObject(item, "eutraCellId", ecgi->eutra_cell_id) == NULL) {
        ogs_error("OpenAPI_ecgi_convertToJSON() failed [eutra_cell_id]");
        goto end;
    }

    if (ecgi->nid) {
        if (cJSON_AddStringToObject(item, "nid", ecgi->nid) == NULL) {
            ogs_error("OpenAPI_ecgi_convertToJSON() failed [nid]");
            goto end;
        }
    }

end:
    return item;
}

OpenAPI_ecgi_t *OpenAPI_ecgi_parseFromJSON(cJSON *ecgiJSON)
{
    OpenAPI_ecgi_t *ecgi_local_var = NULL;
    cJSON *plmn_id = cJSON_GetObjectItemCaseSensitive(ecgiJSON, "plmnId");
    if (!plmn_id) {
        ogs_error("OpenAPI_ecgi_parseFromJSON() failed [plmn_id]");
        goto end;
    }

    OpenAPI_plmn_id_t *plmn_id_local_nonprim = NULL;

    plmn_id_local_nonprim = OpenAPI_plmn_id_parseFromJSON(plmn_id);

    cJSON *eutra_cell_id = cJSON_GetObjectItemCaseSensitive(ecgiJSON, "eutraCellId");
    if (!eutra_cell_id) {
        ogs_error("OpenAPI_ecgi_parseFromJSON() failed [eutra_cell_id]");
        goto end;
    }


    if (!cJSON_IsString(eutra_cell_id)) {
        ogs_error("OpenAPI_ecgi_parseFromJSON() failed [eutra_cell_id]");
        goto end;
    }

    cJSON *nid = cJSON_GetObjectItemCaseSensitive(ecgiJSON, "nid");

    if (nid) {
        if (!cJSON_IsString(nid)) {
            ogs_error("OpenAPI_ecgi_parseFromJSON() failed [nid]");
            goto end;
        }
    }

    ecgi_local_var = OpenAPI_ecgi_create (
        plmn_id_local_nonprim,
        ogs_strdup(eutra_cell_id->valuestring),
        nid ? ogs_strdup(nid->valuestring) : NULL
        );

    return ecgi_local_var;
end:
    return NULL;
}

