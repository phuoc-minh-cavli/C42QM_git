/******************************************************************************
    Copyright (c) 2016 Qualcomm Technologies, Inc.
    All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *******************************************************************************/

#ifndef __XTRA_PLATFORM_H__
#define __XTRA_PLATFORM_H__

#include <stdint.h>
#include <string>

using std::string;

class XtraPlatform {
public:
    XtraPlatform();
    virtual ~XtraPlatform();

    // Populate the instance values
    void populateValues() const;

    // Methods to fetch platform values
    // Release
    virtual string getRelease(const string& def) const;

    // Manufacturer
    virtual string getManufacture(const string& def) const;

    // Model
    virtual string getModel(const string& def) const;

    // Carrier
    virtual string getCarrier(const string& def) const;

    // Board
    virtual string getBoard(const string& def) const;

    // Brand
    virtual string getBrand(const string& def) const;

    // TAC
    virtual string getTac(const string& def) const;

    // MCCMCN list
    virtual string getMccMnc(const string& def) const;

    // elapsed realtime
    virtual int64_t elapsedRealtimeMs() const;

private:
    // Have we fetched the values
    mutable bool valuesFetched;

    // Members holding platform values
    mutable string release;
    mutable string manufacturer;
    mutable string model;
    mutable string carrier;
    mutable string board;
    mutable string brand;
    mutable string tac;
    mutable string mccMnc;
};

#endif //__XTRA_PLATFORM_H__
