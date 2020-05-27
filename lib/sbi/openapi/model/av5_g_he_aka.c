
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "av5_g_he_aka.h"

OpenAPI_av5_g_he_aka_t *OpenAPI_av5_g_he_aka_create(
    OpenAPI_av_type_t *av_type,
    char *rand,
    char *xres_star,
    char *autn,
    char *kausf
    )
{
    OpenAPI_av5_g_he_aka_t *av5_g_he_aka_local_var = OpenAPI_malloc(sizeof(OpenAPI_av5_g_he_aka_t));
    if (!av5_g_he_aka_local_var) {
        return NULL;
    }
    av5_g_he_aka_local_var->av_type = av_type;
    av5_g_he_aka_local_var->rand = rand;
    av5_g_he_aka_local_var->xres_star = xres_star;
    av5_g_he_aka_local_var->autn = autn;
    av5_g_he_aka_local_var->kausf = kausf;

    return av5_g_he_aka_local_var;
}

void OpenAPI_av5_g_he_aka_free(OpenAPI_av5_g_he_aka_t *av5_g_he_aka)
{
    if (NULL == av5_g_he_aka) {
        return;
    }
    OpenAPI_lnode_t *node;
    OpenAPI_av_type_free(av5_g_he_aka->av_type);
    ogs_free(av5_g_he_aka->rand);
    ogs_free(av5_g_he_aka->xres_star);
    ogs_free(av5_g_he_aka->autn);
    ogs_free(av5_g_he_aka->kausf);
    ogs_free(av5_g_he_aka);
}

cJSON *OpenAPI_av5_g_he_aka_convertToJSON(OpenAPI_av5_g_he_aka_t *av5_g_he_aka)
{
    cJSON *item = NULL;

    if (av5_g_he_aka == NULL) {
        ogs_error("OpenAPI_av5_g_he_aka_convertToJSON() failed [Av5GHeAka]");
        return NULL;
    }

    item = cJSON_CreateObject();
    if (!av5_g_he_aka->av_type) {
        ogs_error("OpenAPI_av5_g_he_aka_convertToJSON() failed [av_type]");
        goto end;
    }
    cJSON *av_type_local_JSON = OpenAPI_av_type_convertToJSON(av5_g_he_aka->av_type);
    if (av_type_local_JSON == NULL) {
        ogs_error("OpenAPI_av5_g_he_aka_convertToJSON() failed [av_type]");
        goto end;
    }
    cJSON_AddItemToObject(item, "avType", av_type_local_JSON);
    if (item->child == NULL) {
        ogs_error("OpenAPI_av5_g_he_aka_convertToJSON() failed [av_type]");
        goto end;
    }

    if (!av5_g_he_aka->rand) {
        ogs_error("OpenAPI_av5_g_he_aka_convertToJSON() failed [rand]");
        goto end;
    }
    if (cJSON_AddStringToObject(item, "rand", av5_g_he_aka->rand) == NULL) {
        ogs_error("OpenAPI_av5_g_he_aka_convertToJSON() failed [rand]");
        goto end;
    }

    if (!av5_g_he_aka->xres_star) {
        ogs_error("OpenAPI_av5_g_he_aka_convertToJSON() failed [xres_star]");
        goto end;
    }
    if (cJSON_AddStringToObject(item, "xresStar", av5_g_he_aka->xres_star) == NULL) {
        ogs_error("OpenAPI_av5_g_he_aka_convertToJSON() failed [xres_star]");
        goto end;
    }

    if (!av5_g_he_aka->autn) {
        ogs_error("OpenAPI_av5_g_he_aka_convertToJSON() failed [autn]");
        goto end;
    }
    if (cJSON_AddStringToObject(item, "autn", av5_g_he_aka->autn) == NULL) {
        ogs_error("OpenAPI_av5_g_he_aka_convertToJSON() failed [autn]");
        goto end;
    }

    if (!av5_g_he_aka->kausf) {
        ogs_error("OpenAPI_av5_g_he_aka_convertToJSON() failed [kausf]");
        goto end;
    }
    if (cJSON_AddStringToObject(item, "kausf", av5_g_he_aka->kausf) == NULL) {
        ogs_error("OpenAPI_av5_g_he_aka_convertToJSON() failed [kausf]");
        goto end;
    }

end:
    return item;
}

OpenAPI_av5_g_he_aka_t *OpenAPI_av5_g_he_aka_parseFromJSON(cJSON *av5_g_he_akaJSON)
{
    OpenAPI_av5_g_he_aka_t *av5_g_he_aka_local_var = NULL;
    cJSON *av_type = cJSON_GetObjectItemCaseSensitive(av5_g_he_akaJSON, "avType");
    if (!av_type) {
        ogs_error("OpenAPI_av5_g_he_aka_parseFromJSON() failed [av_type]");
        goto end;
    }

    OpenAPI_av_type_t *av_type_local_nonprim = NULL;

    av_type_local_nonprim = OpenAPI_av_type_parseFromJSON(av_type);

    cJSON *rand = cJSON_GetObjectItemCaseSensitive(av5_g_he_akaJSON, "rand");
    if (!rand) {
        ogs_error("OpenAPI_av5_g_he_aka_parseFromJSON() failed [rand]");
        goto end;
    }


    if (!cJSON_IsString(rand)) {
        ogs_error("OpenAPI_av5_g_he_aka_parseFromJSON() failed [rand]");
        goto end;
    }

    cJSON *xres_star = cJSON_GetObjectItemCaseSensitive(av5_g_he_akaJSON, "xresStar");
    if (!xres_star) {
        ogs_error("OpenAPI_av5_g_he_aka_parseFromJSON() failed [xres_star]");
        goto end;
    }


    if (!cJSON_IsString(xres_star)) {
        ogs_error("OpenAPI_av5_g_he_aka_parseFromJSON() failed [xres_star]");
        goto end;
    }

    cJSON *autn = cJSON_GetObjectItemCaseSensitive(av5_g_he_akaJSON, "autn");
    if (!autn) {
        ogs_error("OpenAPI_av5_g_he_aka_parseFromJSON() failed [autn]");
        goto end;
    }


    if (!cJSON_IsString(autn)) {
        ogs_error("OpenAPI_av5_g_he_aka_parseFromJSON() failed [autn]");
        goto end;
    }

    cJSON *kausf = cJSON_GetObjectItemCaseSensitive(av5_g_he_akaJSON, "kausf");
    if (!kausf) {
        ogs_error("OpenAPI_av5_g_he_aka_parseFromJSON() failed [kausf]");
        goto end;
    }


    if (!cJSON_IsString(kausf)) {
        ogs_error("OpenAPI_av5_g_he_aka_parseFromJSON() failed [kausf]");
        goto end;
    }

    av5_g_he_aka_local_var = OpenAPI_av5_g_he_aka_create (
        av_type_local_nonprim,
        ogs_strdup(rand->valuestring),
        ogs_strdup(xres_star->valuestring),
        ogs_strdup(autn->valuestring),
        ogs_strdup(kausf->valuestring)
        );

    return av5_g_he_aka_local_var;
end:
    return NULL;
}
