/**
 * This file is generated by the SwaggerTSGenerator.
 * Do not edit.
*/
/* tslint:disable */
import { Validators, FormControl, FormGroup, FormArray, ValidatorFn } from '@angular/forms';
import { minValueValidator, maxValueValidator, minLengthValidator, maxLengthValidator, required, enumValidator, patternValidator, CustomFormControl, CustomFormGroup } from '../../../utils/validators';
import { BaseModel, PropInfoItem } from '../basemodel/base-model';

import { ClusterDistributedServiceCardStatus_admission_phase,  ClusterDistributedServiceCardStatus_admission_phase_uihint  } from './enums';
import { ClusterDSCCondition, IClusterDSCCondition } from './cluster-dsc-condition.model';
import { ClusterIPConfig, IClusterIPConfig } from './cluster-ip-config.model';
import { ClusterDSCInfo, IClusterDSCInfo } from './cluster-dsc-info.model';

export interface IClusterDistributedServiceCardStatus {
    'admission-phase': ClusterDistributedServiceCardStatus_admission_phase;
    'conditions'?: Array<IClusterDSCCondition>;
    'serial-num'?: string;
    'primary-mac'?: string;
    'ip-config'?: IClusterIPConfig;
    'system-info'?: IClusterDSCInfo;
    'interfaces'?: Array<string>;
    'DSCVersion'?: string;
    'DSCSku'?: string;
    'host'?: string;
    'adm-phase-reason'?: string;
    'version-mismatch'?: boolean;
    '_ui'?: any;
}


export class ClusterDistributedServiceCardStatus extends BaseModel implements IClusterDistributedServiceCardStatus {
    /** Field for holding arbitrary ui state */
    '_ui': any = {};
    /** Current admission phase of the DistributedServiceCard. When auto-admission is enabled, AdmissionPhase will be set to NIC_ADMITTED by CMD for validated NICs. When auto-admission is not enabled, AdmissionPhase will be set to NIC_PENDING by CMD for validated NICs since it requires manual approval. To admit the NIC as a part of manual admission, user is expected to set Spec.Admit to true for the NICs that are in NIC_PENDING state. Note : Whitelist mode is not supported yet. */
    'admission-phase': ClusterDistributedServiceCardStatus_admission_phase = null;
    /** List of current NIC conditions. */
    'conditions': Array<ClusterDSCCondition> = null;
    /** Serial number. */
    'serial-num': string = null;
    /** PrimaryMAC is the MAC address of the primary PF exposed by DistributedServiceCard. Should be a valid MAC address. */
    'primary-mac': string = null;
    /** IPConfig is the ip address related configuration obtained from DHCP. */
    'ip-config': ClusterIPConfig = null;
    /** Distributed service card system info. */
    'system-info': ClusterDSCInfo = null;
    /** Network Interfaces. */
    'interfaces': Array<string> = null;
    /** DSC Version. */
    'DSCVersion': string = null;
    /** DSC SKU. */
    'DSCSku': string = null;
    /** The name of the host this DistributedServiceCard is plugged into. */
    'host': string = null;
    /** The reason why the DistributedServiceCard is not in ADMITTED state. */
    'adm-phase-reason': string = null;
    /** Set to true if venice and dsc versions are incompatible. */
    'version-mismatch': boolean = null;
    public static propInfo: { [prop in keyof IClusterDistributedServiceCardStatus]: PropInfoItem } = {
        'admission-phase': {
            enum: ClusterDistributedServiceCardStatus_admission_phase_uihint,
            default: 'unknown',
            description:  `Current admission phase of the DistributedServiceCard. When auto-admission is enabled, AdmissionPhase will be set to NIC_ADMITTED by CMD for validated NICs. When auto-admission is not enabled, AdmissionPhase will be set to NIC_PENDING by CMD for validated NICs since it requires manual approval. To admit the NIC as a part of manual admission, user is expected to set Spec.Admit to true for the NICs that are in NIC_PENDING state. Note : Whitelist mode is not supported yet.`,
            required: true,
            type: 'string'
        },
        'conditions': {
            description:  `List of current NIC conditions.`,
            required: false,
            type: 'object'
        },
        'serial-num': {
            description:  `Serial number.`,
            required: false,
            type: 'string'
        },
        'primary-mac': {
            description:  `PrimaryMAC is the MAC address of the primary PF exposed by DistributedServiceCard. Should be a valid MAC address.`,
            hint:  'aabb.ccdd.0000, aabb.ccdd.0000, aabb.ccdd.0000',
            required: false,
            type: 'string'
        },
        'ip-config': {
            description:  `IPConfig is the ip address related configuration obtained from DHCP.`,
            required: false,
            type: 'object'
        },
        'system-info': {
            description:  `Distributed service card system info.`,
            required: false,
            type: 'object'
        },
        'interfaces': {
            description:  `Network Interfaces.`,
            required: false,
            type: 'Array<string>'
        },
        'DSCVersion': {
            description:  `DSC Version.`,
            required: false,
            type: 'string'
        },
        'DSCSku': {
            description:  `DSC SKU.`,
            required: false,
            type: 'string'
        },
        'host': {
            description:  `The name of the host this DistributedServiceCard is plugged into.`,
            required: false,
            type: 'string'
        },
        'adm-phase-reason': {
            description:  `The reason why the DistributedServiceCard is not in ADMITTED state.`,
            required: false,
            type: 'string'
        },
        'version-mismatch': {
            description:  `Set to true if venice and dsc versions are incompatible.`,
            required: false,
            type: 'boolean'
        },
    }

    public getPropInfo(propName: string): PropInfoItem {
        return ClusterDistributedServiceCardStatus.propInfo[propName];
    }

    public getPropInfoConfig(): { [key:string]:PropInfoItem } {
        return ClusterDistributedServiceCardStatus.propInfo;
    }

    /**
     * Returns whether or not there is an enum property with a default value
    */
    public static hasDefaultValue(prop) {
        return (ClusterDistributedServiceCardStatus.propInfo[prop] != null &&
                        ClusterDistributedServiceCardStatus.propInfo[prop].default != null);
    }

    /**
     * constructor
     * @param values Can be used to set a webapi response to this newly constructed model
    */
    constructor(values?: any, setDefaults:boolean = true) {
        super();
        this['conditions'] = new Array<ClusterDSCCondition>();
        this['ip-config'] = new ClusterIPConfig();
        this['system-info'] = new ClusterDSCInfo();
        this['interfaces'] = new Array<string>();
        this._inputValue = values;
        this.setValues(values, setDefaults);
    }

    /**
     * set the values for both the Model and the Form Group. If a value isn't provided and we have a default, we use that.
     * @param values Can be used to set a webapi response to this newly constructed model
    */
    setValues(values: any, fillDefaults = true): void {
        if (values && values['_ui']) {
            this['_ui'] = values['_ui']
        }
        if (values && values['admission-phase'] != null) {
            this['admission-phase'] = values['admission-phase'];
        } else if (fillDefaults && ClusterDistributedServiceCardStatus.hasDefaultValue('admission-phase')) {
            this['admission-phase'] = <ClusterDistributedServiceCardStatus_admission_phase>  ClusterDistributedServiceCardStatus.propInfo['admission-phase'].default;
        } else {
            this['admission-phase'] = null
        }
        if (values) {
            this.fillModelArray<ClusterDSCCondition>(this, 'conditions', values['conditions'], ClusterDSCCondition);
        } else {
            this['conditions'] = [];
        }
        if (values && values['serial-num'] != null) {
            this['serial-num'] = values['serial-num'];
        } else if (fillDefaults && ClusterDistributedServiceCardStatus.hasDefaultValue('serial-num')) {
            this['serial-num'] = ClusterDistributedServiceCardStatus.propInfo['serial-num'].default;
        } else {
            this['serial-num'] = null
        }
        if (values && values['primary-mac'] != null) {
            this['primary-mac'] = values['primary-mac'];
        } else if (fillDefaults && ClusterDistributedServiceCardStatus.hasDefaultValue('primary-mac')) {
            this['primary-mac'] = ClusterDistributedServiceCardStatus.propInfo['primary-mac'].default;
        } else {
            this['primary-mac'] = null
        }
        if (values) {
            this['ip-config'].setValues(values['ip-config'], fillDefaults);
        } else {
            this['ip-config'].setValues(null, fillDefaults);
        }
        if (values) {
            this['system-info'].setValues(values['system-info'], fillDefaults);
        } else {
            this['system-info'].setValues(null, fillDefaults);
        }
        if (values && values['interfaces'] != null) {
            this['interfaces'] = values['interfaces'];
        } else if (fillDefaults && ClusterDistributedServiceCardStatus.hasDefaultValue('interfaces')) {
            this['interfaces'] = [ ClusterDistributedServiceCardStatus.propInfo['interfaces'].default];
        } else {
            this['interfaces'] = [];
        }
        if (values && values['DSCVersion'] != null) {
            this['DSCVersion'] = values['DSCVersion'];
        } else if (fillDefaults && ClusterDistributedServiceCardStatus.hasDefaultValue('DSCVersion')) {
            this['DSCVersion'] = ClusterDistributedServiceCardStatus.propInfo['DSCVersion'].default;
        } else {
            this['DSCVersion'] = null
        }
        if (values && values['DSCSku'] != null) {
            this['DSCSku'] = values['DSCSku'];
        } else if (fillDefaults && ClusterDistributedServiceCardStatus.hasDefaultValue('DSCSku')) {
            this['DSCSku'] = ClusterDistributedServiceCardStatus.propInfo['DSCSku'].default;
        } else {
            this['DSCSku'] = null
        }
        if (values && values['host'] != null) {
            this['host'] = values['host'];
        } else if (fillDefaults && ClusterDistributedServiceCardStatus.hasDefaultValue('host')) {
            this['host'] = ClusterDistributedServiceCardStatus.propInfo['host'].default;
        } else {
            this['host'] = null
        }
        if (values && values['adm-phase-reason'] != null) {
            this['adm-phase-reason'] = values['adm-phase-reason'];
        } else if (fillDefaults && ClusterDistributedServiceCardStatus.hasDefaultValue('adm-phase-reason')) {
            this['adm-phase-reason'] = ClusterDistributedServiceCardStatus.propInfo['adm-phase-reason'].default;
        } else {
            this['adm-phase-reason'] = null
        }
        if (values && values['version-mismatch'] != null) {
            this['version-mismatch'] = values['version-mismatch'];
        } else if (fillDefaults && ClusterDistributedServiceCardStatus.hasDefaultValue('version-mismatch')) {
            this['version-mismatch'] = ClusterDistributedServiceCardStatus.propInfo['version-mismatch'].default;
        } else {
            this['version-mismatch'] = null
        }
        this.setFormGroupValuesToBeModelValues();
    }


    protected getFormGroup(): FormGroup {
        if (!this._formGroup) {
            this._formGroup = new FormGroup({
                'admission-phase': CustomFormControl(new FormControl(this['admission-phase'], [required, enumValidator(ClusterDistributedServiceCardStatus_admission_phase), ]), ClusterDistributedServiceCardStatus.propInfo['admission-phase']),
                'conditions': new FormArray([]),
                'serial-num': CustomFormControl(new FormControl(this['serial-num']), ClusterDistributedServiceCardStatus.propInfo['serial-num']),
                'primary-mac': CustomFormControl(new FormControl(this['primary-mac']), ClusterDistributedServiceCardStatus.propInfo['primary-mac']),
                'ip-config': CustomFormGroup(this['ip-config'].$formGroup, ClusterDistributedServiceCardStatus.propInfo['ip-config'].required),
                'system-info': CustomFormGroup(this['system-info'].$formGroup, ClusterDistributedServiceCardStatus.propInfo['system-info'].required),
                'interfaces': CustomFormControl(new FormControl(this['interfaces']), ClusterDistributedServiceCardStatus.propInfo['interfaces']),
                'DSCVersion': CustomFormControl(new FormControl(this['DSCVersion']), ClusterDistributedServiceCardStatus.propInfo['DSCVersion']),
                'DSCSku': CustomFormControl(new FormControl(this['DSCSku']), ClusterDistributedServiceCardStatus.propInfo['DSCSku']),
                'host': CustomFormControl(new FormControl(this['host']), ClusterDistributedServiceCardStatus.propInfo['host']),
                'adm-phase-reason': CustomFormControl(new FormControl(this['adm-phase-reason']), ClusterDistributedServiceCardStatus.propInfo['adm-phase-reason']),
                'version-mismatch': CustomFormControl(new FormControl(this['version-mismatch']), ClusterDistributedServiceCardStatus.propInfo['version-mismatch']),
            });
            // generate FormArray control elements
            this.fillFormArray<ClusterDSCCondition>('conditions', this['conditions'], ClusterDSCCondition);
            // We force recalculation of controls under a form group
            Object.keys((this._formGroup.get('conditions') as FormGroup).controls).forEach(field => {
                const control = this._formGroup.get('conditions').get(field);
                control.updateValueAndValidity();
            });
            // We force recalculation of controls under a form group
            Object.keys((this._formGroup.get('ip-config') as FormGroup).controls).forEach(field => {
                const control = this._formGroup.get('ip-config').get(field);
                control.updateValueAndValidity();
            });
            // We force recalculation of controls under a form group
            Object.keys((this._formGroup.get('system-info') as FormGroup).controls).forEach(field => {
                const control = this._formGroup.get('system-info').get(field);
                control.updateValueAndValidity();
            });
        }
        return this._formGroup;
    }

    setModelToBeFormGroupValues() {
        this.setValues(this.$formGroup.value, false);
    }

    setFormGroupValuesToBeModelValues() {
        if (this._formGroup) {
            this._formGroup.controls['admission-phase'].setValue(this['admission-phase']);
            this.fillModelArray<ClusterDSCCondition>(this, 'conditions', this['conditions'], ClusterDSCCondition);
            this._formGroup.controls['serial-num'].setValue(this['serial-num']);
            this._formGroup.controls['primary-mac'].setValue(this['primary-mac']);
            this['ip-config'].setFormGroupValuesToBeModelValues();
            this['system-info'].setFormGroupValuesToBeModelValues();
            this._formGroup.controls['interfaces'].setValue(this['interfaces']);
            this._formGroup.controls['DSCVersion'].setValue(this['DSCVersion']);
            this._formGroup.controls['DSCSku'].setValue(this['DSCSku']);
            this._formGroup.controls['host'].setValue(this['host']);
            this._formGroup.controls['adm-phase-reason'].setValue(this['adm-phase-reason']);
            this._formGroup.controls['version-mismatch'].setValue(this['version-mismatch']);
        }
    }
}

