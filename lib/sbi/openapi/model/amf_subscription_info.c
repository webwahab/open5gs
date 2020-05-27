
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "amf_subscription_info.h"

OpenAPI_amf_subscription_info_t *OpenAPI_amf_subscription_info_create(
    char *amf_instance_id,
    char *subscription_id,
    char *subs_change_notify_correlation_id
    )
{
    OpenAPI_amf_subscription_info_t *amf_subscription_info_local_var = OpenAPI_malloc(sizeof(OpenAPI_amf_subscription_info_t));
    if (!amf_subscription_info_local_var) {
        return NULL;
    }
    amf_subscription_info_local_var->amf_instance_id = amf_instance_id;
    amf_subscription_info_local_var->subscription_id = subscription_id;
    amf_subscription_info_local_var->subs_change_notify_correlation_id = subs_change_notify_correlation_id;

    return amf_subscription_info_local_var;
}

void OpenAPI_amf_subscription_info_free(OpenAPI_amf_subscription_info_t *amf_subscription_info)
{
    if (NULL == amf_subscription_info) {
        return;
    }
    OpenAPI_lnode_t *node;
    ogs_free(amf_subscription_info->amf_instance_id);
    ogs_free(amf_subscription_info->subscription_id);
    ogs_free(amf_subscription_info->subs_change_notify_correlation_id);
    ogs_free(amf_subscription_info);
}

cJSON *OpenAPI_amf_subscription_info_convertToJSON(OpenAPI_amf_subscription_info_t *amf_subscription_info)
{
    cJSON *item = NULL;

    if (amf_subscription_info == NULL) {
        ogs_error("OpenAPI_amf_subscription_info_convertToJSON() failed [AmfSubscriptionInfo]");
        return NULL;
    }

    item = cJSON_CreateObject();
    if (!amf_subscription_info->amf_instance_id) {
        ogs_error("OpenAPI_amf_subscription_info_convertToJSON() failed [amf_instance_id]");
        goto end;
    }
    if (cJSON_AddStringToObject(item, "amfInstanceId", amf_subscription_info->amf_instance_id) == NULL) {
        ogs_error("OpenAPI_amf_subscription_info_convertToJSON() failed [amf_instance_id]");
        goto end;
    }

    if (!amf_subscription_info->subscription_id) {
        ogs_error("OpenAPI_amf_subscription_info_convertToJSON() failed [subscription_id]");
        goto end;
    }
    if (cJSON_AddStringToObject(item, "subscriptionId", amf_subscription_info->subscription_id) == NULL) {
        ogs_error("OpenAPI_amf_subscription_info_convertToJSON() failed [subscription_id]");
        goto end;
    }

    if (amf_subscription_info->subs_change_notify_correlation_id) {
        if (cJSON_AddStringToObject(item, "subsChangeNotifyCorrelationId", amf_subscription_info->subs_change_notify_correlation_id) == NULL) {
            ogs_error("OpenAPI_amf_subscription_info_convertToJSON() failed [subs_change_notify_correlation_id]");
            goto end;
        }
    }

end:
    return item;
}

OpenAPI_amf_subscription_info_t *OpenAPI_amf_subscription_info_parseFromJSON(cJSON *amf_subscription_infoJSON)
{
    OpenAPI_amf_subscription_info_t *amf_subscription_info_local_var = NULL;
    cJSON *amf_instance_id = cJSON_GetObjectItemCaseSensitive(amf_subscription_infoJSON, "amfInstanceId");
    if (!amf_instance_id) {
        ogs_error("OpenAPI_amf_subscription_info_parseFromJSON() failed [amf_instance_id]");
        goto end;
    }


    if (!cJSON_IsString(amf_instance_id)) {
        ogs_error("OpenAPI_amf_subscription_info_parseFromJSON() failed [amf_instance_id]");
        goto end;
    }

    cJSON *subscription_id = cJSON_GetObjectItemCaseSensitive(amf_subscription_infoJSON, "subscriptionId");
    if (!subscription_id) {
        ogs_error("OpenAPI_amf_subscription_info_parseFromJSON() failed [subscription_id]");
        goto end;
    }


    if (!cJSON_IsString(subscription_id)) {
        ogs_error("OpenAPI_amf_subscription_info_parseFromJSON() failed [subscription_id]");
        goto end;
    }

    cJSON *subs_change_notify_correlation_id = cJSON_GetObjectItemCaseSensitive(amf_subscription_infoJSON, "subsChangeNotifyCorrelationId");

    if (subs_change_notify_correlation_id) {
        if (!cJSON_IsString(subs_change_notify_correlation_id)) {
            ogs_error("OpenAPI_amf_subscription_info_parseFromJSON() failed [subs_change_notify_correlation_id]");
            goto end;
        }
    }

    amf_subscription_info_local_var = OpenAPI_amf_subscription_info_create (
        ogs_strdup(amf_instance_id->valuestring),
        ogs_strdup(subscription_id->valuestring),
        subs_change_notify_correlation_id ? ogs_strdup(subs_change_notify_correlation_id->valuestring) : NULL
        );

    return amf_subscription_info_local_var;
end:
    return NULL;
}

