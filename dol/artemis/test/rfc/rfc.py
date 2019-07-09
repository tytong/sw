#! /usr/bin/python3
# RFC Module
import pdb
import artemis.test.callbacks.common.modcbs as modcbs
import apollo.config.objects.meter as meterstats

def Setup(infra, module):
    modcbs.Setup(infra, module)
    return True

def TestCaseSetup(tc):
    tc.AddIgnorePacketField('UDP', 'sport')
    tc.AddIgnorePacketField('UDP', 'chksum')
    tc.AddIgnorePacketField('TCP', 'chksum')
    tc.AddIgnorePacketField('IP', 'chksum') #Needed to pass NAT testcase
    iterelem = tc.module.iterator.Get()
    if iterelem:
        tc.pvtdata.verify_meter_stats = getattr(iterelem, "meter_stats", False)
    return True

def TestCaseTeardown(tc):
    return True

def TestCasePreTrigger(tc):
    if tc.pvtdata.verify_meter_stats:
        tc.pvtdata.meterstats = meterstats.MeterStatsHelper()
        tc.pvtdata.meterstats.ReadMeterStats(True)
    return True

def TestCaseStepSetup(tc, step):
    return True

def TestCaseStepTrigger(tc, step):
    return True

def TestCaseStepVerify(tc, step):
    return True

def TestCaseStepTeardown(tc, step):
    return True

def TestCaseVerify(tc):
    if tc.pvtdata.verify_meter_stats:
        tc.pvtdata.meterstats.ReadMeterStats(False)
        tc.pvtdata.meterstats.VerifyMeterStats()
    return True
