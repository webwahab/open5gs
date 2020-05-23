#ifndef TEST_5GC_H
#define TEST_5GC_H

#include "test-common.h"

#include "ogs-ngap.h"
#include "ogs-sbi.h"
#include "ogs-nas-5gs.h"

#include "amf/context.h"

#ifdef __cplusplus
extern "C" {
#endif

void test_5gc_init(void);
#if 0
#define test_5gc_run test_app_run
#endif
#define test_5gc_final ogs_mongoc_final

#ifdef __cplusplus
}
#endif

#endif /* TEST_5GC_H */
