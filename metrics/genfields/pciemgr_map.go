// Code generated by protoc-gen-grpc-pensando DO NOT EDIT.

package genfields

func init() {
	globalMetricsMap["pciemgr"] = make(map[string][]string)

	kindToFieldNameMap["PcieMgrMetrics"] = []string{
		"NotIntr",
		"NotSpurious",
		"NotCnt",
		"NotMax",
		"NotCfgrd",
		"NotCfgwr",
		"NotMemrd",
		"NotMemwr",
		"NotIord",
		"NotIowr",
		"NotUnknown",
		"NotRsrv0",
		"NotRsrv1",
		"NotMsg",
		"NotUnsupported",
		"NotPmv",
		"NotDbpmv",
		"NotAtomic",
		"NotPmtmiss",
		"NotPmrmiss",
		"NotPrtmiss",
		"NotBdf2Vfidmiss",
		"NotPrtoor",
		"NotVfidoor",
		"NotBdfoor",
		"NotPmrind",
		"NotPrtind",
		"NotPmrecc",
		"NotPrtecc",
		"IndIntr",
		"IndSpurious",
		"IndCfgrd",
		"IndCfgwr",
		"IndMemrd",
		"IndMemwr",
		"IndIord",
		"IndIowr",
		"IndUnknown",
		"Healthlog",
	}
	globalMetricsMap["pciemgr"]["PcieMgrMetrics"] = kindToFieldNameMap["PcieMgrMetrics"]

	kindToFieldNameMap["PciePortMetrics"] = []string{
		"IntrTotal",
		"IntrPolled",
		"IntrPerstn",
		"IntrLtssmstEarly",
		"IntrLtssmst",
		"IntrLinkup2Dn",
		"IntrLinkdn2Up",
		"IntrRstup2Dn",
		"IntrRstdn2Up",
		"IntrSecbus",
		"Linkup",
		"Hostup",
		"Phypolllast",
		"Phypollmax",
		"Phypollperstn",
		"Phypollfail",
		"Gatepolllast",
		"Gatepollmax",
		"Markerpolllast",
		"Markerpollmax",
		"Axipendpolllast",
		"Axipendpollmax",
		"Faults",
		"Powerdown",
		"LinkDn2UpInt",
		"LinkUp2DnInt",
		"SecBusRstInt",
		"RstUp2DnInt",
		"RstDn2UpInt",
		"PortgateOpen2CloseInt",
		"LtssmStChangedInt",
		"SecBusnumChangedInt",
		"RcPmeInt",
		"RcAerrInt",
		"RcSerrInt",
		"RcHpeInt",
		"RcEqReqInt",
		"RcDpcInt",
		"PmTurnoffInt",
		"TxbfrOverflowInt",
		"RxtlpErrInt",
		"TlFlrReqInt",
		"RcLegacyIntpinChangedInt",
	}
	globalMetricsMap["pciemgr"]["PciePortMetrics"] = kindToFieldNameMap["PciePortMetrics"]

}
