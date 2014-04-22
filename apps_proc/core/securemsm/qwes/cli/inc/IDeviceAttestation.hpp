#pragma once
/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/** @file  IDeviceAttestation.idl */

/**
 * IDAError provides a list of all the errors that might be returned
 * by the methods of the IAttestationReport, IAttestationBuilder, and
 * IDeviceAttestation interfaces.
 */

/** @cond */

// AUTOGENERATED FILE: DO NOT EDIT

#include <cstdint>
#include "object.h"
#include "proxy_base.hpp"

class IIDAError {
   public:
    static const int32_t NO_MEMORY = INT32_C(10);
    static const int32_t INVALID_BUFFER = INT32_C(11);
    static const int32_t INVALID_CERTIFICATE = INT32_C(12);
    static const int32_t MAX_APP_DATA_LIMIT_REACHED = INT32_C(13);
    static const int32_t INVALID_SECURITY_LEVEL = INT32_C(14);
    static const int32_t INVALID_ATTESTATION_CONTEXT = INT32_C(15);
    static const int32_t INVALID_SIGNING_KEY = INT32_C(16);
    static const int32_t INVALID_NONCE = INT32_C(17);
    static const int32_t INVALID_REPORT_OFFSET = INT32_C(18);
    static const int32_t ATTESTATION_REPORT_FAILURE = INT32_C(19);
    static const int32_t WARM_UP_FAILURE = INT32_C(20);

    virtual ~IIDAError() {}


   protected:
};

class IDAError : public IIDAError, public ProxyBase {
   public:
    IDAError() {}
    IDAError(Object impl) : ProxyBase(impl) {}
    virtual ~IDAError() {}

};
class IIAttestationReport {
   public:
    virtual ~IIAttestationReport() {}

    virtual int32_t getSize(uint64_t* attestationReportSize_ptr) = 0;
    virtual int32_t getBytes(uint64_t offset_val, void* attestation_ptr, size_t attestation_len, size_t* attestation_lenout) = 0;

   protected:
    static const ObjectOp OP_getSize = 0;
    static const ObjectOp OP_getBytes = 1;
};

class IAttestationReport : public IIAttestationReport, public ProxyBase {
   public:
    IAttestationReport() {}
    IAttestationReport(Object impl) : ProxyBase(impl) {}
    virtual ~IAttestationReport() {}

    virtual int32_t getSize(uint64_t* attestationReportSize_ptr) {
        ObjectArg a[1];
        a[0].b = (ObjectBuf) {attestationReportSize_ptr, sizeof(uint64_t)};

        return invoke(OP_getSize, a, ObjectCounts_pack(0, 1, 0, 0));
    }

    virtual int32_t getBytes(uint64_t offset_val, void* attestation_ptr, size_t attestation_len, size_t* attestation_lenout) {
        ObjectArg a[2];
        a[0].b = (ObjectBuf) {&offset_val, sizeof(uint64_t)};
        a[1].b = (ObjectBuf) {attestation_ptr, attestation_len * 1};

        int32_t result = invoke(OP_getBytes, a, ObjectCounts_pack(1, 1, 0, 0));
        if (Object_OK != result) { return result; }

        *attestation_lenout = a[1].b.size / 1;

        return result;
    }

};
class IIAttestationBuilder {
   public:
    static const uint32_t SECURITY_LEVEL_UNRESTRICTED = UINT32_C(1);
    static const uint32_t SECURITY_LEVEL_RESTRICTED = UINT32_C(2);
    static const uint32_t SECURITY_LEVEL_SECURERESTRICTED = UINT32_C(3);
    static const uint32_t SECURITY_LEVEL_HARDWARE = UINT32_C(4);
    static const uint32_t ATTESTATION_CONTEXT_GENERIC = UINT32_C(1);
    static const uint32_t ATTESTATION_CONTEXT_REGISTRATION = UINT32_C(2);
    static const uint32_t ATTESTATION_CONTEXT_PROVISIONING = UINT32_C(3);
    static const uint32_t ATTESTATION_CONTEXT_CERT_ISSUANCE = UINT32_C(4);
    static const uint32_t ATTESTATION_CONTEXT_PROOF_OF_POSSESSION = UINT32_C(5);
    static const uint32_t ATTESTATION_CONTEXT_LICENSING = UINT32_C(6);
    static const uint64_t OPT_ADDON_NONE = UINT64_C(0x0000000000000000);
    static const uint64_t OPT_ADDON_LOCATION = UINT64_C(0x0000000000000001);
    static const uint64_t OPT_ADDON_RTIC = UINT64_C(0x0000000000000002);
    static const uint64_t OPT_ADDON_TRUSTEDTIME = UINT64_C(0x0000000000000004);
    static const uint64_t OPT_ADDON_CONNSEC_CELLULAR = UINT64_C(0x0000000000000008);
    static const uint32_t FORMAT_EAT = UINT32_C(1);
    static const uint32_t KEY_DEMO = UINT32_C(1);
    static const uint32_t KEY_QDAK = UINT32_C(2);

    virtual ~IIAttestationBuilder() {}

    virtual int32_t clearBytes() = 0;
    virtual int32_t addBytes(uint32_t securityLevel_val, const int8_t* label_ptr, size_t label_len, const void* bytes_ptr, size_t bytes_len) = 0;
    virtual int32_t build(uint32_t attestationContext_val, uint64_t ADDONOptions_val, uint32_t formatType_val, uint32_t keyType_val, const void* nonce_ptr, size_t nonce_len, uint64_t* reportStatus_ptr, ProxyBase &attestationReport_ref) = 0;

   protected:
    static const ObjectOp OP_clearBytes = 0;
    static const ObjectOp OP_addBytes = 1;
    static const ObjectOp OP_build = 2;
};

class IAttestationBuilder : public IIAttestationBuilder, public ProxyBase {
   public:
    IAttestationBuilder() {}
    IAttestationBuilder(Object impl) : ProxyBase(impl) {}
    virtual ~IAttestationBuilder() {}

    virtual int32_t clearBytes() {
        return invoke(OP_clearBytes, 0, 0);
    }

    virtual int32_t addBytes(uint32_t securityLevel_val, const int8_t* label_ptr, size_t label_len, const void* bytes_ptr, size_t bytes_len) {
        ObjectArg a[3];
        a[0].b = (ObjectBuf) {&securityLevel_val, sizeof(uint32_t)};
        a[1].bi = (ObjectBufIn) {label_ptr, label_len * sizeof(int8_t)};
        a[2].bi = (ObjectBufIn) {bytes_ptr, bytes_len * 1};

        return invoke(OP_addBytes, a, ObjectCounts_pack(3, 0, 0, 0));
    }

    virtual int32_t build(uint32_t attestationContext_val, uint64_t ADDONOptions_val, uint32_t formatType_val, uint32_t keyType_val, const void* nonce_ptr, size_t nonce_len, uint64_t* reportStatus_ptr, ProxyBase &attestationReport_ref) {
        ObjectArg a[4];
        struct s1 {
            uint64_t m_ADDONOptions;
            uint32_t m_attestationContext;
            uint32_t m_formatType;
            uint32_t m_keyType;
        } i;
        a[0].b = (ObjectBuf) {&i, 20};
        i.m_attestationContext = attestationContext_val;
        i.m_ADDONOptions = ADDONOptions_val;
        i.m_formatType = formatType_val;
        i.m_keyType = keyType_val;
        a[1].bi = (ObjectBufIn) {nonce_ptr, nonce_len * 1};
        a[2].b = (ObjectBuf) {reportStatus_ptr, sizeof(uint64_t)};

        int32_t result = invoke(OP_build, a, ObjectCounts_pack(2, 1, 0, 1));
        if (Object_OK != result) { return result; }

        attestationReport_ref.consume(a[3].o);

        return result;
    }

};
class IIDeviceAttestation {
   public:
    virtual ~IIDeviceAttestation() {}

    virtual int32_t start(const void* licenseCert_ptr, size_t licenseCert_len, ProxyBase &attestationBuilder_ref) = 0;
    virtual int32_t getWarmUpStatus(void* warmUpStatus_ptr, size_t warmUpStatus_len, size_t* warmUpStatus_lenout) = 0;
    virtual int32_t warmUp(uint64_t options_val, uint64_t timeout_val, const ProxyBase &callback_ref) = 0;

   protected:
    static const ObjectOp OP_start = 0;
    static const ObjectOp OP_getWarmUpStatus = 1;
    static const ObjectOp OP_warmUp = 2;
};

class IDeviceAttestation : public IIDeviceAttestation, public ProxyBase {
   public:
    IDeviceAttestation() {}
    IDeviceAttestation(Object impl) : ProxyBase(impl) {}
    virtual ~IDeviceAttestation() {}

    virtual int32_t start(const void* licenseCert_ptr, size_t licenseCert_len, ProxyBase &attestationBuilder_ref) {
        ObjectArg a[2];
        a[0].bi = (ObjectBufIn) {licenseCert_ptr, licenseCert_len * 1};

        int32_t result = invoke(OP_start, a, ObjectCounts_pack(1, 0, 0, 1));
        if (Object_OK != result) { return result; }

        attestationBuilder_ref.consume(a[1].o);

        return result;
    }

    virtual int32_t getWarmUpStatus(void* warmUpStatus_ptr, size_t warmUpStatus_len, size_t* warmUpStatus_lenout) {
        ObjectArg a[1];
        a[0].b = (ObjectBuf) {warmUpStatus_ptr, warmUpStatus_len * 1};

        int32_t result = invoke(OP_getWarmUpStatus, a, ObjectCounts_pack(0, 1, 0, 0));
        if (Object_OK != result) { return result; }

        *warmUpStatus_lenout = a[0].b.size / 1;

        return result;
    }

    virtual int32_t warmUp(uint64_t options_val, uint64_t timeout_val, const ProxyBase &callback_ref) {
        ObjectArg a[2];
        struct s1 {
            uint64_t m_options;
            uint64_t m_timeout;
        } i;
        a[0].b = (ObjectBuf) {&i, 16};
        i.m_options = options_val;
        i.m_timeout = timeout_val;
        a[1].o = callback_ref.get();

        return invoke(OP_warmUp, a, ObjectCounts_pack(1, 0, 1, 0));
    }

};

