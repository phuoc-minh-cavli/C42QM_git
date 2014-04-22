#===========================================================================
#  Copyright (c) 2016-2019 Qualcomm Technologies Incorporated.
#  All Rights Reserved.
#  Qualcomm Confidential and Proprietary
#===========================================================================

#initialize empty dict
app_params_dict = dict()

# Initialize parameters for aptcryptotestapp
def addParams(app_name, app_dict):
  app_params_dict[app_name] = dict()
  app_params_dict[app_name] = app_dict

# Load the Secure app builder in the environment
def callInit(env, app_dict):
  #this can be used by the library to identify the app name in cleanpack
  env.Replace(APP_NAME = app_dict['aliases'][0])
  #this allows the apps to use their own name in cleanpack
  env[app_dict['aliases'][0]] = app_dict
  image_name=app_dict['aliases'][0].upper()
  env.Replace(IMAGE_ALIASES=app_dict['aliases'] + ['all'])
  env.Replace(BUILDPATH=env.subst(image_name).lower() + "/${PROC}/${BUILD_ID}")
  for t in app_dict['tags']:
    env[t] = True


#======================================================
# aptcrytpotestapp
aptcryptotestapp_dict = {
  'aliases' : ['aptcryptotestapp','aptcryptotestapp64'],
  'arch' : 'scorpion',
  'tags' : ['APTCRYPTOTESTAPP_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/aptcryptotestapp/src/SConscript',
}
addParams('aptcryptotestapp', aptcryptotestapp_dict)
#======================================================
# aptcrytpotestapp64
aptcryptotestapp64_dict = {
  'aliases' : ['aptcryptotestapp64'],
  'arch' : 'A53_64',
  'tags' : ['APTCRYPTOTESTAPP64_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/aptcryptotestapp/src/SConscript',
}
addParams('aptcryptotestapp64', aptcryptotestapp64_dict)

#======================================================
# aptlksecapp
aptlksecapp_dict = {
  'aliases' : ['aptlksecapp', 'aptlksecapp64'],
  'arch' : 'scorpion',
  'tags' : ['APTLKSECAPP_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/aptlksecapp/src/SConscript',
}
addParams('aptlksecapp', aptlksecapp_dict)
#======================================================
# aptlksecapp64
aptlksecapp64_dict = {
  'aliases' : ['aptlksecapp64'],
  'arch' : 'A53_64',
  'tags' : ['APTLKSECAPP64_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/aptlksecapp/src/SConscript',
}
addParams('aptlksecapp64', aptlksecapp64_dict)


#======================================================
# apttestapp
apttestapp_dict = {
  'aliases' : ['apttestapp', 'apttestapp64'],
  'arch' : 'scorpion',
  'tags' : ['APTTESTAPP_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/apttestapp/src/SConscript',
}
addParams('apttestapp', apttestapp_dict)
#======================================================
# apttestapp64
apttestapp64_dict = {
  'aliases' : ['apttestapp64'],
  'arch' : 'A53_64',
  'tags' : ['APTTESTAPP64_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/apttestapp/src/SConscript',
}
addParams('apttestapp64', apttestapp64_dict)

#======================================================
# assurancetest
assurancetest_dict = {
  'aliases' : ['assurancetest', 'assurancetest64'],
  'arch' : 'scorpion',
  'tags' : ['ASSURANCETEST_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/assurancetest/src/SConscript',
}
addParams('assurancetest', assurancetest_dict)
#======================================================
# assurancetest64
assurancetest64_dict = {
  'aliases' : ['assurancetest64'],
  'arch' : 'A53_64',
  'tags' : ['ASSURANCETEST64_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/assurancetest/src/SConscript',
}
addParams('assurancetest64', assurancetest64_dict)

#======================================================
# celerity
celerity_dict = {
  'aliases' : ['celerity', 'celerity64'],
  'arch' : 'scorpion',
  'tags' : ['CELERITY_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/celerity/SConscript',
}
addParams('celerity', celerity_dict)

#======================================================
# celerity64
celerity64_dict = {
  'aliases' : ['celerity64'],
  'arch' : 'A53_64',
  'tags' : ['CELERITY64_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/celerity/SConscript',
}
addParams('celerity64', celerity64_dict)

#======================================================
# crikeymgmtapp
crikeymgmt_dict = {
  'aliases' : ['crikeymgmtapp'],
  'arch' : 'scorpion',
  'tags' : ['CRIKEYMGMTAPP_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/CRIKeyMgmtApp/src/SConscript',
}
addParams('crikeymgmtapp', crikeymgmt_dict)

#======================================================
# crikeymgmtapp64
crikeymgmt64_dict = {
  'aliases' : ['crikeymgmtapp64'],
  'arch' : 'A53_64',
  'tags' : ['CRIKEYMGMTAPP64_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/CRIKeyMgmtApp/src/SConscript',
}
addParams('crikeymgmtapp64', crikeymgmt64_dict)

#======================================================
# RTIC (former dhsecapp aka devicehealth)
rtic_dict = {
  'aliases' : ['rtic'],
  'arch' : 'A53_64',
  'tags' : ['RTIC_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/rtic/src/SConscript',
}
addParams('rtic', rtic_dict)

#======================================================
# dxhdcp2
dxhdcp2_dict = {
  'aliases' : ['dxhdcp2'],
  'arch' : 'A53_64',
  'tags' : ['DXHDCP2_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/dxhdcp2/src/SConscript',
}
addParams('dxhdcp2', dxhdcp2_dict)

#======================================================
# dxhdcp2dbg
dxhdcp2dbg_dict = {
  'aliases' : ['dxhdcp2dbg'],
  'arch' : 'A53_64',
  'tags' : ['DXHDCP2DBG_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/dxhdcp2dbg/src/SConscript',
}
addParams('dxhdcp2dbg', dxhdcp2dbg_dict)

#======================================================
# dxhdcp2wp
dxhdcp2wp_dict = {
  'aliases' : ['dxhdcp2wp'],
  'arch' : 'A53_64',
  'tags' : ['DXHDCP2WP_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/dxhdcp2wp/src/SConscript',
}
addParams('dxhdcp2wp', dxhdcp2wp_dict)

#======================================================
# face3d
face3d_dict = {
  'aliases' : ['face3d'],
  'arch' : 'scorpion',
  'tags' : ['FACE3D_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/biometrics/face3d/build/SConscript',
}
addParams('face3d', face3d_dict)

#======================================================
# face3d64
face3d64_dict = {
  'aliases' : ['face3d64'],
  'arch' : 'A53_64',
  'tags' : ['FACE3D64_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/biometrics/face3d/build/SConscript',
}
addParams('face3d64', face3d64_dict)

#======================================================
# fingerprint
fingerprint_dict = {
  'aliases' : ['fingerprint'],
  'arch' : 'scorpion',
  'tags' : ['FINGERPRINT_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/biometrics/fingerprint/build/SConscript',
}
addParams('fingerprint', fingerprint_dict)

#======================================================
# fingerprint64
fingerprint64_dict = {
  'aliases' : ['fingerprint64'],
  'arch' : 'A53_64',
  'tags' : ['FINGERPRINT64_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/biometrics/fingerprint/build/SConscript',
}
addParams('fingerprint64', fingerprint64_dict)

#======================================================
# fipstestapp
fipstestapp_dict = {
  'aliases' : ['fipstestapp','fipstestapp64'],
  'arch' : 'scorpion',
  'tags' : ['FIPSTESTAPP_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/fipstestapp/src/SConscript',
}
addParams('fipstestapp', fipstestapp_dict)
#======================================================
# fipstestapp64
fipstestapp64_dict = {
  'aliases' : ['fipstestapp64'],
  'arch' : 'A53_64',
  'tags' : ['FIPSTESTAPP64_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/fipstestapp/src/SConscript',
}
addParams('fipstestapp64', fipstestapp64_dict)
#======================================================
# fingerprintdbg
fingerprintdbg_dict = {
  'aliases' : ['fingerprintdbg'],
  'arch' : 'scorpion',
  'tags' : ['FINGERPRINTDBG_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/biometrics/fingerprint/build/SConscript',
}
addParams('fingerprintdbg', fingerprintdbg_dict)

#======================================================
# fingerprint64dbg
fingerprint64dbg_dict = {
  'aliases' : ['fingerprint64dbg'],
  'arch' : 'A53_64',
  'tags' : ['FINGERPRINT64DBG_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/biometrics/fingerprint/build/SConscript',
}
addParams('fingerprint64dbg', fingerprint64dbg_dict)

#======================================================
# scp11cry
scp11cry_dict = {
  'aliases' : ['scp11cry'],
  'arch' : 'scorpion',
  'tags' : ['SCP11CRY'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/scp11cry/src/SConscript',
}
addParams('scp11cry', scp11cry_dict)

#======================================================
# scp11prv
scp11prv_dict = {
  'aliases' : ['scp11prv'],
  'arch' : 'scorpion',
  'tags' : ['SCP11PRV'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/scp11prv/src/SConscript',
}
addParams('scp11prv', scp11prv_dict)

#======================================================
# scp11smp
scp11smp_dict = {
  'aliases' : ['scp11smp'],
  'arch' : 'scorpion',
  'tags' : ['SCP11SMP'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/scp11smp/src/SConscript',
}
addParams('scp11smp', scp11smp_dict)

#======================================================
# hdcp1
hdcp1_dict = {
  'aliases' : ['hdcp1'],
  'arch' : 'scorpion',
  'tags' : ['HDCP1_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/hdcp1/src/SConscript',
}
addParams('hdcp1', hdcp1_dict)

#======================================================
# hdcp2p2
hdcp2p2_dict = {
  'aliases' : ['hdcp2p2'],
  'arch' : 'scorpion',
  'tags' : ['HDCP2P2_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/hdcp2p2/SConscript',
}
addParams('hdcp2p2', hdcp2p2_dict)

#======================================================
# hdcptest
hdcptest_dict = {
  'aliases' : ['hdcptest'],
  'arch' : 'scorpion',
  'tags' : ['HDCPTEST_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/hdcp2p2/hdcptest/SConscript',
}
addParams('hdcptest', hdcptest_dict)

#======================================================
# hdcpsrm !! This app does not compile. Also not included in build_all.sh script
hdcpsrm_dict = {
  'aliases' : ['hdcpsrm'],
  'arch' : 'scorpion',
  'tags' : ['HDCPSRM_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/tzdrm/hdcpsrm/src/SConscript',
}
addParams('hdcpsrm', hdcpsrm_dict)
#======================================================
# secdspdemo64
secdspdemo64_dict = {
  'aliases' : ['secdspdemo64'],
  'arch' : 'A53_64',
  'tags' : ['SECDSPDEMO_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/secdspdemo/SConscript',
}
addParams('secdspdemo64', secdspdemo64_dict)
#======================================================
# secdspdemo
secdspdemo_dict = {
  'aliases' : ['secdspdemo', 'secdspdemo64'],
  'arch' : 'scorpion',
  'tags' : ['SECDSPDEMO_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/secdspdemo/SConscript',
}
addParams('secdspdemo', secdspdemo_dict)
#======================================================
# secdspdemo2
secdspdemo2_dict = {
  'aliases' : ['secdspdemo2'],
  'arch' : 'A53_64',
  'tags' : ['SECDSPDEMO2_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/secdspdemo2/SConscript',
}
addParams('secdspdemo2', secdspdemo2_dict)
#======================================================
# iris
iris_dict = {
  'aliases' : ['iris'],
  'arch' : 'scorpion',
  'tags' : ['IRIS_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/biometrics/iris/build/SConscript',
}
addParams('iris', iris_dict)
#======================================================
# isdbtmm
isdbtmm_dict = {
  'aliases' : ['isdbtmm'],
  'arch' : 'scorpion',
  'tags' : ['ISDBTMM_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/tzdrm/isdbtmm/core/shared/src/SConscript',
}
addParams('isdbtmm', isdbtmm_dict)
#======================================================
# keymasterapp -- 32 bit
keymasterapp32_dict = {
  'aliases' : ['keymasterapp32'],
  'arch' : 'scorpion',
  'tags' : ['KEYMASTERAPP_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/keymasterapp/km3/src/SConscript',
}
addParams('keymasterapp32', keymasterapp32_dict)
#======================================================
# keymasterapp -- 64 bit
keymasterapp_dict = {
  'aliases' : ['keymasterapp'],
  'arch' : 'A53_64',
  'tags' : ['KEYMASTERAPP_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/keymasterapp/km3/src/SConscript',
}
addParams('keymasterapp', keymasterapp_dict)
#======================================================
# keymaster
keymaster_dict = {
  'aliases' : ['keymaster', 'km4'],
  'arch' : 'scorpion',
  'tags' : ['KEYMASTER_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/keymaster/km1/src/SConscript',
}
addParams('keymaster', keymaster_dict)
#======================================================
# keymgrtest1
keymgrtest1_dict = {
  'aliases' : ['keymgrtest1', 'keymgrtest2'],
  'arch' : 'A53_64',
  'tags' : ['KEYMGRTEST1_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/keymanager/keymgrtest1/SConscript',
}
addParams('keymgrtest1', keymgrtest1_dict)
#======================================================
# keymgrtest2
keymgrtest2_dict = {
  'aliases' : ['keymgrtest1', 'keymgrtest2'],
  'arch' : 'A53_64',
  'tags' : ['KEYMGRTEST2_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/keymanager/keymgrtest2/SConscript',
}
addParams('keymgrtest2', keymgrtest2_dict)
#======================================================
# km4
km4_dict = {
  'aliases' : ['km4'],
  'arch' : 'A53_64',
  'tags' : ['KEYMASTER_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/keymaster/km4/src/SConscript',
}
addParams('km4', km4_dict)
#======================================================
# kmtest
kmtest_dict = {
  'aliases' : ['kmtest'],
  'arch' : 'A53_64',
  'tags' : ['KEYMASTER_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/keymaster/kmtest/src/SConscript',
}
addParams('kmtest', kmtest_dict)
#======================================================
# licmngr
licmngr_dict = {
  'aliases' : ['licmngr'],
  'arch' : 'A53_64',
  'tags' : ['LICMNGR_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/license_manager/SConscript',
}
addParams('licmngr', licmngr_dict)

#======================================================
# soter
soter_dict = {
  'aliases' : ['soter'],
  'arch' : 'A53_64',
  'tags' : ['SOTER_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/soter/src/SConscript',
}
addParams('soter', soter_dict)
#======================================================
# lksecapp
lksecapp_dict = {
  'aliases' : ['lksecapp'],
  'arch' : 'scorpion',
  'tags' : ['LKSECAPP_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/lksecapp/src/SConscript',
}
addParams('lksecapp', lksecapp_dict)
#======================================================
# lksecapp64
lksecapp64_dict = {
  'aliases' : ['lksecapp64'],
  'arch' : 'A53_64',
  'tags' : ['LKSECAPP64_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/lksecapp/src/SConscript',
}
addParams('lksecapp64', lksecapp64_dict)
#======================================================
# macchiato
macchiato_dict = {
  'aliases' : ['macchiato_sample'],
  'arch' : 'A53_64',
  'tags' : ['MACCHIATO_SAMPLE_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/macchiato_sample/src/SConscript',
}
addParams('macchiato_sample', macchiato_dict)

#======================================================
# petool
petool_dict = {
  'aliases' : ['petool'],
  'arch' : 'scorpion',
  'tags' : ['PETOOL_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/petool/SConscript',
}
addParams('petool', petool_dict)
#======================================================
# fmac
fmac_dict = {
  'aliases' : ['fmac'],
  'arch' : 'scorpion',
  'tags' : ['FMAC_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/flexsku/fmac/SConscript',
}
addParams('fmac', fmac_dict)

#======================================================
# gpqese
gpqese_dict = {
  'aliases' : ['gpqese'],
  'arch' : 'scorpion',
  'tags' : ['GPQESE_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/gpqese/src/SConscript',
}
addParams('gpqese', gpqese_dict)
#======================================================
# eseservice
eseservice_dict = {
  'aliases' : ['eseservice'],
  'arch' : 'A53_64',
  'tags' : ['ESESERVICE_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/eseservice/src/SConscript',
}
addParams('eseservice', eseservice_dict)
#======================================================
# smpcpyap
smpcpyap_dict = {
  'aliases' : ['smpcpyap', 'smpcpyap64'],
  'arch' : 'scorpion',
  'tags' : ['SAMPLECOPYAPP_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/samplecopyapp/src/SConscript',
}
addParams('smpcpyap', smpcpyap_dict)

smpcpyap64_dict = {
  'aliases' : ['smpcpyap64'],
  'arch' : 'A53_64',
  'tags' : ['SAMPLECOPYAPP64_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/samplecopyapp/src/SConscript',
}
addParams('smpcpyap64', smpcpyap64_dict)
#======================================================
# sbtest
sbtest_dict = {
  'aliases' : ['sbtest'],
  'arch' : 'scorpion',
  'tags' : ['SBTEST_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/sbtest/src/SConscript',
}
addParams('sbtest', sbtest_dict)
#======================================================
# haventoken
haventkn_dict = {
  'aliases' : ['haventkn'],
  'arch' : 'A53_64',
  'tags' : ['HAVENTOKEN_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/haventoken/src/SConscript',
}
addParams('haventkn', haventkn_dict)
#======================================================
# haventoken 32-bit TA for 32-bit targets.
haventkn32_dict = {
  'aliases' : ['haventkn32'],
  'arch' : 'scorpion',
  'tags' : ['HAVENTOKEN32_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/haventoken/src/SConscript',
}
addParams('haventkn32', haventkn32_dict)
#======================================================
# haventokentest
haventst_dict = {
  'aliases' : ['haventst'],
  'arch' : 'A53_64',
  'tags' : ['HAVENTOKENTEST_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/haventokentest/src/SConscript',
}
addParams('haventst', haventst_dict)

#======================================================
# rtic_tst - test app for rtic (fromer dhsecapp aka devicehealth)
# Should not be released externally
rtic_tst_dict = {
  'aliases' : ['rtic_tst'],
  'arch' : 'A53_64',
  'tags' : ['RTIC_TST_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/rtic_tst/src/SConscript',
}
addParams('rtic_tst', rtic_tst_dict)


#======================================================
# securemm
securemm_dict = {
  'aliases' : ['securemm'],
  'arch' : 'A53_64',
  'tags' : ['SECUREMM_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/securemm/src/SConscript',
}
addParams('securemm', securemm_dict)

#======================================================
# seccamdemo
seccamdemo_dict = {
  'aliases' : ['seccamdemo'],
  'arch' : 'scorpion',
  'tags' : ['SECCAMDEMO_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/seccamdemo64/src/SConscript',
}
addParams('seccamdemo', seccamdemo_dict)
#======================================================
# seccamdemo64
seccamdemo64_dict = {
  'aliases' : ['seccamdemo','seccamdemo64'],
  'arch' : 'A53_64',
  'tags' : ['SECCAMDEMO64_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/seccamdemo64/src/SConscript',
}
addParams('seccamdemo64', seccamdemo64_dict)
#======================================================
# secure_ui_sample
secure_ui_sample_dict = {
  'aliases' : ['secure_ui_sample','secure_ui_sample64'],
  'arch' : 'scorpion',
  'tags' : ['SECUREUISAMPLE_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/secure_ui_sample/src/SConscript',
}
addParams('secure_ui_sample', secure_ui_sample_dict)
#======================================================
# secure_ui_sample64
secure_ui_sample64_dict = {
  'aliases' : ['secure_ui_sample64'],
  'arch' : 'A53_64',
  'tags' : ['SECUREUISAMPLE64_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/secure_ui_sample/src/SConscript',
}
addParams('secure_ui_sample64', secure_ui_sample64_dict)



#======================================================
# ssmapp
ssmapp_dict = {
  'aliases' : ['ssmapp'],
  'arch' : 'scorpion',
  'tags' : ['SSMAPP_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/ssmapp/src/SConscript',
}
addParams('ssmapp', ssmapp_dict)
#======================================================
# skeleton
skeleton_dict = {
  'aliases' : ['skeleton'],
  'arch' : 'A53_64',
  'tags' : ['SKELETON_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/skeleton/src/SConscript',
}
addParams('skeleton', skeleton_dict)
#======================================================
# tloctst
tloctst_dict = {
  'aliases' : ['tloctst'],
  'arch' : 'A53_64',
  'tags' : ['TLOCTST_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/tloctst/src/SConscript',
}
addParams('tloctst', tloctst_dict)
#======================================================
# ttaari1
ttaari1_dict = {
  'aliases' : ['ttaari1'],
  'arch' : 'scorpion',
  'tags' : ['TTAARI1'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/gp-TTAs/TTA_Arithmetical/TTA_Arithmetical/SConscript',
}
addParams('ttaari1', ttaari1_dict)
#======================================================
# ttacapi1
ttacapi1_dict = {
  'aliases' : ['ttacapi1'],
  'arch' : 'scorpion',
  'tags' : ['TTACAPI1'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/gp-TTAs/TTA_ClientAPI/TTA_error_Invoke/SConscript',
}
addParams('ttacapi1', ttacapi1_dict)
#======================================================
# ttacapi2
ttacapi2_dict = {
  'aliases' : ['ttacapi2'],
  'arch' : 'scorpion',
  'tags' : ['TTACAPI2'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/gp-TTAs/TTA_ClientAPI/TTA_error_OpenSession/SConscript',
}
addParams('ttacapi2', ttacapi2_dict)
#======================================================
# ttacapi3
ttacapi3_dict = {
  'aliases' : ['ttacapi3'],
  'arch' : 'scorpion',
  'tags' : ['TTACAPI3'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/gp-TTAs/TTA_ClientAPI/TTA_success_OpenSession_Invoke/SConscript',
}
addParams('ttacapi3', ttacapi3_dict)
#======================================================
# ttacapi4
ttacapi4_dict = {
  'aliases' : ['ttacapi4'],
  'arch' : 'scorpion',
  'tags' : ['TTACAPI4'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/gp-TTAs/TTA_ClientAPI/TTA_OpenSession_with_4_params/SConscript',
}
addParams('ttacapi4', ttacapi4_dict)
#======================================================
# ttacapi5
ttacapi5_dict = {
  'aliases' : ['ttacapi5'],
  'arch' : 'scorpion',
  'tags' : ['TTACAPI5'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/gp-TTAs/TTA_ClientAPI/TTA_ClientAPI/SConscript',
}
addParams('ttacapi5', ttacapi5_dict)
#======================================================
# ttacapi6
ttacapi6_dict = {
  'aliases' : ['ttacapi6'],
  'arch' : 'scorpion',
  'tags' : ['TTACAPI6'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/gp-TTAs/TTA_ClientAPI/TTA_ClientAPI_Params/Invoke/SConscript',
}
addParams('ttacapi6', ttacapi6_dict)
#======================================================
# ttacapi7
ttacapi7_dict = {
  'aliases' : ['ttacapi7'],
  'arch' : 'scorpion',
  'tags' : ['TTACAPI7'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/gp-TTAs/TTA_ClientAPI/TTA_ClientAPI_Params/OpenSession/SConscript',
}
addParams('ttacapi7', ttacapi7_dict)
#======================================================
# ttacrp1
ttacrp1_dict = {
  'aliases' : ['ttacrp1'],
  'arch' : 'scorpion',
  'tags' : ['TTACRP1'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/gp-TTAs/TTA_Crypto/TTA_Crypto/SConscript',
}
addParams('ttacrp1', ttacrp1_dict)
#======================================================
# ttads1
ttads1_dict = {
  'aliases' : ['ttads1'],
  'arch' : 'scorpion',
  'tags' : ['TTADS1'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/gp-TTAs/TTA_DS/TTA_DS/SConscript',
}
addParams('ttads1', ttads1_dict)
#======================================================
# ttasecac
ttasecac_dict = {
  'aliases' : ['ttasecac'],
  'arch' : 'scorpion',
  'tags' : ['TTASECAC'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/gp-TTAs/TTA_TCF/Security_TTA_Abuse_Callee/SConscript',
}
addParams('ttasecac', ttasecac_dict)
#======================================================
# ttascapi
ttascapi_dict = {
  'aliases' : ['ttascapi'],
  'arch' : 'scorpion',
  'tags' : ['TTASCAPI'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/gp-TTAs/TTA_SE_API/TTA_SecureChannel/SConscript',
}
addParams('ttascapi', ttascapi_dict)
#======================================================
# ttatlapi
ttatlapi_dict = {
  'aliases' : ['ttatlapi'],
  'arch' : 'scorpion',
  'tags' : ['TTATLAPI'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/gp-TTAs/TTA_SE_API/TTA_TransportLayer/SConscript',
}
addParams('ttatlapi', ttatlapi_dict)
#======================================================
# ttaseccrp
ttaseccrp_dict = {
  'aliases' : ['ttaseccrp'],
  'arch' : 'scorpion',
  'tags' : ['TTASECCRP'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/gp-TTAs/TTA_Crypto/Security_TTA_Crypto/SConscript',
}
addParams('ttaseccrp', ttaseccrp_dict)
#======================================================
# ttasecds
ttasecds_dict = {
  'aliases' : ['ttasecds'],
  'arch' : 'scorpion',
  'tags' : ['TTASECDS'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/gp-TTAs/TTA_DS/Security_TTA_DS/SConscript',
}
addParams('ttasecds', ttasecds_dict)
#======================================================
# ttasecmem
ttasecmem_dict = {
  'aliases' : ['ttasecmem'],
  'arch' : 'scorpion',
  'tags' : ['TTASECMEM'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/gp-TTAs/TTA_Sec_Memory/TTA_Sec_Memory/SConscript',
}
addParams('ttasecmem', ttasecmem_dict)
#======================================================
# ttasectcf
ttasectcf_dict = {
  'aliases' : ['ttasectcf'],
  'arch' : 'scorpion',
  'tags' : ['TTASECTCF'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/gp-TTAs/TTA_TCF/Security_TTA_TCF/SConscript',
}
addParams('ttasectcf', ttasectcf_dict)
#======================================================
# ttatcf1
ttatcf1_dict = {
  'aliases' : ['ttatcf1'],
  'arch' : 'scorpion',
  'tags' : ['TTATCF1'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/gp-TTAs/TTA_TCF/TTA_TCF/SConscript',
}
addParams('ttatcf1', ttatcf1_dict)
#======================================================
# ttatcf2
ttatcf2_dict = {
  'aliases' : ['ttatcf2'],
  'arch' : 'scorpion',
  'tags' : ['TTATCF2'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/gp-TTAs/TTA_TCF/TTA_TCF_ICA/SConscript',
}
addParams('ttatcf2', ttatcf2_dict)
#======================================================
# ttatcf3
ttatcf3_dict = {
  'aliases' : ['ttatcf3'],
  'arch' : 'scorpion',
  'tags' : ['TTATCF3'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/gp-TTAs/TTA_TCF/TTA_TCF_ICA2/SConscript',
}
addParams('ttatcf3', ttatcf3_dict)
#======================================================
# ttatcf4
ttatcf4_dict = {
  'aliases' : ['ttatcf4'],
  'arch' : 'scorpion',
  'tags' : ['TTATCF4'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/gp-TTAs/TTA_TCF/TTA_TCF_MultipleInstanceTA/SConscript',
}
addParams('ttatcf4', ttatcf4_dict)
#======================================================
# ttatcf5
ttatcf5_dict = {
  'aliases' : ['ttatcf5'],
  'arch' : 'scorpion',
  'tags' : ['TTATCF5'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/gp-TTAs/TTA_TCF/TTA_TCF_SingleInstanceTA/SConscript',
}
addParams('ttatcf5', ttatcf5_dict)
#======================================================
# ttatcf6
ttatcf6_dict = {
  'aliases' : ['ttatcf6'],
  'arch' : 'scorpion',
  'tags' : ['TTATCF6'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/gp-TTAs/TTA_TCF/TTA_TCF_PanicAtCloseSession/SConscript',
}
addParams('ttatcf6', ttatcf6_dict)
#======================================================
# ttatcf7
ttatcf7_dict = {
  'aliases' : ['ttatcf7'],
  'arch' : 'scorpion',
  'tags' : ['TTATCF7'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/gp-TTAs/TTA_TCF/TTA_TCF_PanicAtCreation/SConscript',
}
addParams('ttatcf7', ttatcf7_dict)
#======================================================
# ttatime1
ttatime1_dict = {
  'aliases' : ['ttatime1'],
  'arch' : 'scorpion',
  'tags' : ['TTATIME1'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/gp-TTAs/TTA_Time/TTA_Time/SConscript',
}
addParams('ttatime1', ttatime1_dict)
#======================================================
# playready 3.0
pr_3_0_dict = {
  'aliases' : ['pr_3_0'],
  'arch' : 'A53_64',
  'tags' : ['PR_3_0_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/tzdrm/playready_3_0/build/SConscript',
}
addParams('pr_3_0', pr_3_0_dict)
#======================================================
# playready 3.0 for WP
pr_3_wp_dict = {
  'aliases' : ['pr_3_wp'],
  'arch' : 'A53_64',
  'tags' : ['PR_3_WP_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/tzdrm/playready_3_0/build/SConscript',
}
addParams('pr_3_wp', pr_3_wp_dict)
#======================================================
# uefisecapp
uefi_sec_dict = {
  'aliases' : ['uefi_sec'],
  'arch' : 'scorpion',
  'tags' : ['UEFISECAPP_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/uefisecapp/core/src/SConscript',
}
addParams('uefi_sec', uefi_sec_dict)
#======================================================
# storsecapp
storsecapp_dict = {
  'aliases' : ['storsec'],
  'arch' : 'scorpion',
  'tags' : ['STORSECAPP_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/storsecapp/src/SConscript',
}
addParams('storsec', storsecapp_dict)
#======================================================
# widewine
widevine_dict = {
  'aliases' : ['widevine'],
  'arch' : 'A53_64',
  'tags' : ['WIDEVINE_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/tzdrm/widevine_dash/build/SConscript',
}
addParams('widevine', widevine_dict)
#======================================================
# voiceprint
voiceprint_dict = {
  'aliases' : ['voiceprint'],
  'arch' : 'scorpion',
  'tags' : ['VOICEPRINT_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/biometrics/voiceprint/build/SConscript',
}
addParams('voiceprint', voiceprint_dict)
#======================================================
# mssecapp
mssecapp_dict = {
  'aliases' : ['mssecapp'],
  'arch' : 'A53_64',
  'tags' : ['MSSECAPP_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/mssecapp/build/SConscript',
}
addParams('mssecapp', mssecapp_dict)
#======================================================
# tzsptestapp1
tzsptestapp1_dict = {
  'aliases' : ['tzsptestapp1'],
  'arch' : 'A53_64',
  'tags' : ['TZSPTESTAPP1_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/tzsptestapp1/src/SConscript',
}
addParams('tzsptestapp1', tzsptestapp1_dict)
#======================================================
# tzsptestapp2
tzsptestapp2_dict = {
  'aliases' : ['tzsptestapp2'],
  'arch' : 'A53_64',
  'tags' : ['TZSPTESTAPP2_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/tzsptestapp2/src/SConscript',
}
addParams('tzsptestapp2', tzsptestapp2_dict)
#======================================================
# mldapta
mldapta_dict = {
  'aliases' : ['mldapta'],
  'arch' : 'A53_64',
  'tags' : ['MLDAPTA_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/mldapta/src/SConscript',
}
addParams('mldapta', mldapta_dict)
#======================================================
# featenabler
featenabler_dict = {
  'aliases' : ['featenabler'],
  'arch' : 'A53_64',
  'tags' : ['FEATURE_ENABLER_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/FeatureEnabler/src/SConscript',
}
addParams('featenabler', featenabler_dict)
#======================================================
# mink_call_forwarder
mink_call_forwarder_dict = {
  'aliases' : ['mink_call_forwarder'],
  'arch' : 'A53_64',
  'tags' : ['MINK_CALL_FORWARDER_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/mink_call_forwarder/SConscript',
}
addParams('mink_call_forwarder', mink_call_forwarder_dict)
#======================================================
# mink_call_forwarder
mink_call_forwarder32_dict = {
  'aliases' : ['mink_call_forwarder32'],
  'arch' : 'scorpion',
  'tags' : ['MINK_CALL_FORWARDER32_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/mink_call_forwarder/SConscript',
}
addParams('mink_call_forwarder32', mink_call_forwarder32_dict)
