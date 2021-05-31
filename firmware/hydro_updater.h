#include <cassert>

#include <hydrogen.h>
#include "stdextra.h"

#ifdef ESP8266
#include <Updater.h>
#else // ESP8266

// Abstract class to implement whatever signing hash desired
class UpdaterHashClass {
public:
    virtual ~UpdaterHashClass() = default;

    virtual void begin() = 0;
    virtual void add(const void *data, uint32_t len) = 0;
    virtual void end() = 0;
    virtual int len() = 0;
    virtual const void *hash() = 0;
    virtual const unsigned char *oid() = 0;
};

// Abstract class to implement a signature verifier
class UpdaterVerifyClass {
public:
    virtual ~UpdaterVerifyClass() = default;

    virtual uint32_t length() = 0; // How many bytes of signature are expected
    virtual bool verify(UpdaterHashClass *hash, const void *signature, uint32_t signatureLen) = 0; // Verify, return "true" on success
};

#endif // ESP8266

class HydroUpdaterHashAndVerify : public UpdaterHashClass, public UpdaterVerifyClass {
public:
    HydroUpdaterHashAndVerify(const Span<const uint8_t>& pubkey) {
        assert(pubkey.size() == hydro_sign_PUBLICKEYBYTES);
        memcpy(m_Pubkey, pubkey.data(), hydro_sign_PUBLICKEYBYTES);
    }

private:
    // UpdaterHashClass overrides
    void begin() override {
        hydro_sign_init(&m_State, "CmFwSkSg"); // 8 character abbreviation of "CheckMeet Firmware Sketch Signature"
    }

    void add(const void *data, uint32_t len) override {
        hydro_sign_update(&m_State, data, len);
    }

    void end() override {
    }

    int len() override {
        return 0;
    }

    const void *hash() override {
        return nullptr;
    }

    const unsigned char *oid() override {
        return nullptr;
    }

    // UpdaterVerifyClass overrides
    uint32_t length() override {
        return hydro_sign_BYTES;
    }

    bool verify(UpdaterHashClass *, const void *signature, uint32_t signatureLen) override {
        if (signatureLen != length()) {
            return false;
        }
        return hydro_sign_final_verify(&m_State, static_cast<const uint8_t *>(signature), m_Pubkey) == 0;
    }

    uint8_t m_Pubkey[hydro_sign_PUBLICKEYBYTES];
    hydro_sign_state m_State;
};
