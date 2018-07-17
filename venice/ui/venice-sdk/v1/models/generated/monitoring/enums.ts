/**
 * This file is generated by the SwaggerTSGenerator.
 * Do not edit.
*/
/* tslint:disable */

// generate enum based on strings instead of numbers
// (see https://blog.rsuter.com/how-to-implement-an-enum-with-string-values-in-typescript/)
export enum FieldsRequirement_operator {
    'equals' = "equals",
    'notEquals' = "notEquals",
    'in' = "in",
    'notIn' = "notIn",
    'gt' = "gt",
    'gte' = "gte",
    'lt' = "lt",
    'lte' = "lte",
}

export enum MonitoringAlertPolicySpec_severity {
    'INFO' = "INFO",
    'WARNING' = "WARNING",
    'CRITICAL' = "CRITICAL",
}

export enum MonitoringAlertSpec_state {
    'OPEN' = "OPEN",
    'RESOLVED' = "RESOLVED",
    'ACKNOWLEDGED' = "ACKNOWLEDGED",
}

export enum MonitoringAlertStatus_severity {
    'INFO' = "INFO",
    'WARNING' = "WARNING",
    'CRITICAL' = "CRITICAL",
}

export enum MonitoringAuthConfig_algo {
    'MD5' = "MD5",
    'SHA1' = "SHA1",
}

export enum MonitoringEventExport_format {
    'SYSLOG_BSD' = "SYSLOG_BSD",
    'SYSLOG_RFC5424' = "SYSLOG_RFC5424",
}

export enum MonitoringFlowExportTarget_format {
    'Ipfix' = "Ipfix",
}

export enum MonitoringFwlogExport_format {
    'SYSLOG_BSD' = "SYSLOG_BSD",
    'SYSLOG_RFC5424' = "SYSLOG_RFC5424",
}

export enum MonitoringFwlogExport_export_filter {
    'FWLOG_ALL' = "FWLOG_ALL",
    'FWLOG_ACCEPT' = "FWLOG_ACCEPT",
    'FWLOG_REJECT' = "FWLOG_REJECT",
    'FWLOG_DENY' = "FWLOG_DENY",
}

export enum MonitoringFwlogSpec_filter {
    'FWLOG_ALL' = "FWLOG_ALL",
    'FWLOG_ACCEPT' = "FWLOG_ACCEPT",
    'FWLOG_REJECT' = "FWLOG_REJECT",
    'FWLOG_DENY' = "FWLOG_DENY",
}

export enum MonitoringMatchedRequirement_operator {
    'Equals' = "Equals",
    'In' = "In",
    'NotEquals' = "NotEquals",
    'NotIn' = "NotIn",
    'Gt' = "Gt",
    'Lt' = "Lt",
}

export enum MonitoringMirrorCollector_type {
    'VENICE' = "VENICE",
    'ERSPAN' = "ERSPAN",
}

export enum MonitoringMirrorSessionSpec_packet_filters {
    'ALL_PKTS' = "ALL_PKTS",
    'ALL_DROPS' = "ALL_DROPS",
    'NETWORK_POLICY_DROP' = "NETWORK_POLICY_DROP",
    'FIREWALL_POLICY_DROP' = "FIREWALL_POLICY_DROP",
}

export enum MonitoringMirrorSessionStatus_oper_state {
    'RUNNING' = "RUNNING",
    'STOPPED' = "STOPPED",
    'SCHEDULED' = "SCHEDULED",
    'READY_TO_RUN' = "READY_TO_RUN",
}

export enum MonitoringPrivacyConfig_algo {
    'DES56' = "DES56",
    'AES128' = "AES128",
}

export enum MonitoringRequirement_operator {
    'Equals' = "Equals",
    'In' = "In",
    'NotEquals' = "NotEquals",
    'NotIn' = "NotIn",
    'Gt' = "Gt",
    'Lt' = "Lt",
}

export enum MonitoringSNMPTrapServer_version {
    'V2C' = "V2C",
    'V3' = "V3",
}


export enum FieldsRequirement_operator_uihint {
    'gt' = "greater than",
    'gte' = "greater than or equals",
    'lt' = "less than",
    'lte' = "less than or equals",
    'notEquals' = "not equals",
    'notIn' = "not in",
}

export enum MonitoringAlertPolicySpec_severity_uihint {
    'CRITICAL' = "Critical",
    'INFO' = "Informational",
    'WARNING' = "Warning",
}

export enum MonitoringAlertSpec_state_uihint {
    'ACKNOWLEDGED' = "Acknowledged",
    'OPEN' = "Open",
    'RESOLVED' = "Resolved",
}

export enum MonitoringAlertStatus_severity_uihint {
    'CRITICAL' = "Critical",
    'INFO' = "Informational",
    'WARNING' = "Warning",
}

export enum MonitoringFwlogExport_export_filter_uihint {
    'FWLOG_ACCEPT' = "Accept",
    'FWLOG_ALL' = "All",
    'FWLOG_DENY' = "Deny",
    'FWLOG_REJECT' = "Reject",
}

export enum MonitoringFwlogSpec_filter_uihint {
    'FWLOG_ACCEPT' = "Accept",
    'FWLOG_ALL' = "All",
    'FWLOG_DENY' = "Deny",
    'FWLOG_REJECT' = "Reject",
}

export enum MonitoringMatchedRequirement_operator_uihint {
    'Gt' = "Greater Than",
    'Lt' = "Less Than",
    'NotEquals' = "Not Equals",
    'NotIn' = "Not In",
}

export enum MonitoringMirrorCollector_type_uihint {
    'VENICE' = "Venice",
}

export enum MonitoringMirrorSessionSpec_packet_filters_uihint {
    'ALL_DROPS' = "All Drops",
    'ALL_PKTS' = "All Packets",
    'FIREWALL_POLICY_DROP' = "Firewall Policy Drops",
    'NETWORK_POLICY_DROP' = "Network Policy Drops",
}

export enum MonitoringMirrorSessionStatus_oper_state_uihint {
    'READY_TO_RUN' = "Ready To Run",
    'RUNNING' = "Running",
    'SCHEDULED' = "Scheduled",
    'STOPPED' = "Stopped",
}

export enum MonitoringRequirement_operator_uihint {
    'Gt' = "Greater Than",
    'Lt' = "Less Than",
    'NotEquals' = "Not Equals",
    'NotIn' = "Not In",
}




/**
 * bundle of all enums for databinding to options, radio-buttons etc.
 * usage in component:
 *   import { AllEnums, minValueValidator, maxValueValidator } from '../../models/webapi';
 *
 *   @Component({
 *       ...
 *   })
 *   export class xxxComponent implements OnInit {
 *       allEnums = AllEnums;
 *       ...
 *       ngOnInit() {
 *           this.allEnums = AllEnums.instance;
 *       }
 *   }
*/
export class AllEnums {
    private static _instance: AllEnums = new AllEnums();
    constructor() {
        if (AllEnums._instance) {
            throw new Error("Error: Instantiation failed: Use AllEnums.instance instead of new");
        }
        AllEnums._instance = this;
    }
    static get instance(): AllEnums {
        return AllEnums._instance;
    }

    FieldsRequirement_operator = FieldsRequirement_operator;
    MonitoringAlertPolicySpec_severity = MonitoringAlertPolicySpec_severity;
    MonitoringAlertSpec_state = MonitoringAlertSpec_state;
    MonitoringAlertStatus_severity = MonitoringAlertStatus_severity;
    MonitoringAuthConfig_algo = MonitoringAuthConfig_algo;
    MonitoringEventExport_format = MonitoringEventExport_format;
    MonitoringFlowExportTarget_format = MonitoringFlowExportTarget_format;
    MonitoringFwlogExport_format = MonitoringFwlogExport_format;
    MonitoringFwlogExport_export_filter = MonitoringFwlogExport_export_filter;
    MonitoringFwlogSpec_filter = MonitoringFwlogSpec_filter;
    MonitoringMatchedRequirement_operator = MonitoringMatchedRequirement_operator;
    MonitoringMirrorCollector_type = MonitoringMirrorCollector_type;
    MonitoringMirrorSessionSpec_packet_filters = MonitoringMirrorSessionSpec_packet_filters;
    MonitoringMirrorSessionStatus_oper_state = MonitoringMirrorSessionStatus_oper_state;
    MonitoringPrivacyConfig_algo = MonitoringPrivacyConfig_algo;
    MonitoringRequirement_operator = MonitoringRequirement_operator;
    MonitoringSNMPTrapServer_version = MonitoringSNMPTrapServer_version;

    FieldsRequirement_operator_uihint = FieldsRequirement_operator_uihint;
    MonitoringAlertPolicySpec_severity_uihint = MonitoringAlertPolicySpec_severity_uihint;
    MonitoringAlertSpec_state_uihint = MonitoringAlertSpec_state_uihint;
    MonitoringAlertStatus_severity_uihint = MonitoringAlertStatus_severity_uihint;
    MonitoringFwlogExport_export_filter_uihint = MonitoringFwlogExport_export_filter_uihint;
    MonitoringFwlogSpec_filter_uihint = MonitoringFwlogSpec_filter_uihint;
    MonitoringMatchedRequirement_operator_uihint = MonitoringMatchedRequirement_operator_uihint;
    MonitoringMirrorCollector_type_uihint = MonitoringMirrorCollector_type_uihint;
    MonitoringMirrorSessionSpec_packet_filters_uihint = MonitoringMirrorSessionSpec_packet_filters_uihint;
    MonitoringMirrorSessionStatus_oper_state_uihint = MonitoringMirrorSessionStatus_oper_state_uihint;
    MonitoringRequirement_operator_uihint = MonitoringRequirement_operator_uihint;
}
