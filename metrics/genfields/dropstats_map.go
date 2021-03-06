// Code generated by protoc-gen-grpc-pensando DO NOT EDIT.

package genfields

func init() {
	globalMetricsMap["dropstats"] = make(map[string][]string)

	kindToFieldNameMap["DropMetrics"] = []string{
		"DropMalformedPkt",
		"DropParserIcrcError",
		"DropParseLenError",
		"DropHardwareError",
		"DropInputMapping",
		"DropInputMappingDejavu",
		"DropMultiDestNotPinnedUplink",
		"DropFlowHit",
		"DropFlowMiss",
		"DropNacl",
		"DropIpsg",
		"DropIpNormalization",
		"DropTcpNormalization",
		"DropTcpRstWithInvalidAckNum",
		"DropTcpNonSynFirstPkt",
		"DropIcmpNormalization",
		"DropInputPropertiesMiss",
		"DropTcpOutOfWindow",
		"DropTcpSplitHandshake",
		"DropTcpWinZeroDrop",
		"DropTcpDataAfterFin",
		"DropTcpNonRstPktAfterRst",
		"DropTcpInvalidResponderFirstPkt",
		"DropTcpUnexpectedPkt",
		"DropSrcLifMismatch",
		"DropVfIpLabelMismatch",
		"DropVfBadRrDstIp",
		"DropIcmpFragPkt",
	}
	globalMetricsMap["dropstats"]["DropMetrics"] = kindToFieldNameMap["DropMetrics"]

	kindToFieldNameMap["EgressDropMetrics"] = []string{
		"DropOutputMapping",
		"DropPruneSrcPort",
		"DropMirror",
		"DropPolicer",
		"DropCopp",
		"DropChecksumErr",
	}
	globalMetricsMap["dropstats"]["EgressDropMetrics"] = kindToFieldNameMap["EgressDropMetrics"]

}
