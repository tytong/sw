/**
 * This file is generated by the SwaggerTSGenerator.
 * Do not edit.
*/
/* tslint:disable */
import { Validators, FormControl, FormGroup, FormArray, ValidatorFn } from '@angular/forms';
import { minValueValidator, maxValueValidator, minLengthValidator, maxLengthValidator, required, enumValidator, patternValidator, CustomFormControl, CustomFormGroup } from '../../../utils/validators';
import { BaseModel, PropInfoItem } from '../basemodel/base-model';

import { MonitoringMirrorStartConditions, IMonitoringMirrorStartConditions } from './monitoring-mirror-start-conditions.model';
import { MonitoringMirrorCollector, IMonitoringMirrorCollector } from './monitoring-mirror-collector.model';
import { MonitoringMatchRule, IMonitoringMatchRule } from './monitoring-match-rule.model';
import { MonitoringMirrorSessionSpec_packet_filters,  MonitoringMirrorSessionSpec_packet_filters_uihint  } from './enums';
import { MonitoringInterfaceMirror, IMonitoringInterfaceMirror } from './monitoring-interface-mirror.model';

export interface IMonitoringMirrorSessionSpec {
    'packet-size'?: number;
    'start-condition'?: IMonitoringMirrorStartConditions;
    'collectors'?: Array<IMonitoringMirrorCollector>;
    'match-rules'?: Array<IMonitoringMatchRule>;
    'packet-filters': Array<MonitoringMirrorSessionSpec_packet_filters>;
    'interfaces'?: IMonitoringInterfaceMirror;
    '_ui'?: any;
}


export class MonitoringMirrorSessionSpec extends BaseModel implements IMonitoringMirrorSessionSpec {
    /** Field for holding arbitrary ui state */
    '_ui': any = {};
    /** PacketSize: Max size of a mirrored packet. PacketSize = 0 indicates complete packet is mirrored, except when mirrored packets are sent to Venice. For packets mirrored to Venice, max mirror packet size allowed is 256 B. */
    'packet-size': number = null;
    /** StartConditions. */
    'start-condition': MonitoringMirrorStartConditions = null;
    /** Mirrored packet collectors. */
    'collectors': Array<MonitoringMirrorCollector> = null;
    /** Traffic Selection Rules - Matching pakcets are mirrored, based on packet filters and start/stop conditions. */
    'match-rules': Array<MonitoringMatchRule> = null;
    'packet-filters': Array<MonitoringMirrorSessionSpec_packet_filters> = null;
    /** If specified, will pick up interface matching the selector. */
    'interfaces': MonitoringInterfaceMirror = null;
    public static propInfo: { [prop in keyof IMonitoringMirrorSessionSpec]: PropInfoItem } = {
        'packet-size': {
            description:  `PacketSize: Max size of a mirrored packet. PacketSize = 0 indicates complete packet is mirrored, except when mirrored packets are sent to Venice. For packets mirrored to Venice, max mirror packet size allowed is 256 B.`,
            required: false,
            type: 'number'
        },
        'start-condition': {
            description:  `StartConditions.`,
            required: false,
            type: 'object'
        },
        'collectors': {
            description:  `Mirrored packet collectors.`,
            required: false,
            type: 'object'
        },
        'match-rules': {
            description:  `Traffic Selection Rules - Matching pakcets are mirrored, based on packet filters and start/stop conditions.`,
            required: false,
            type: 'object'
        },
        'packet-filters': {
            enum: MonitoringMirrorSessionSpec_packet_filters_uihint,
            default: 'all-packets',
            required: true,
            type: 'Array<string>'
        },
        'interfaces': {
            description:  `If specified, will pick up interface matching the selector.`,
            required: false,
            type: 'object'
        },
    }

    public getPropInfo(propName: string): PropInfoItem {
        return MonitoringMirrorSessionSpec.propInfo[propName];
    }

    public getPropInfoConfig(): { [key:string]:PropInfoItem } {
        return MonitoringMirrorSessionSpec.propInfo;
    }

    /**
     * Returns whether or not there is an enum property with a default value
    */
    public static hasDefaultValue(prop) {
        return (MonitoringMirrorSessionSpec.propInfo[prop] != null &&
                        MonitoringMirrorSessionSpec.propInfo[prop].default != null);
    }

    /**
     * constructor
     * @param values Can be used to set a webapi response to this newly constructed model
    */
    constructor(values?: any, setDefaults:boolean = true) {
        super();
        this['start-condition'] = new MonitoringMirrorStartConditions();
        this['collectors'] = new Array<MonitoringMirrorCollector>();
        this['match-rules'] = new Array<MonitoringMatchRule>();
        this['packet-filters'] = new Array<MonitoringMirrorSessionSpec_packet_filters>();
        this['interfaces'] = new MonitoringInterfaceMirror();
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
        if (values && values['packet-size'] != null) {
            this['packet-size'] = values['packet-size'];
        } else if (fillDefaults && MonitoringMirrorSessionSpec.hasDefaultValue('packet-size')) {
            this['packet-size'] = MonitoringMirrorSessionSpec.propInfo['packet-size'].default;
        } else {
            this['packet-size'] = null
        }
        if (values) {
            this['start-condition'].setValues(values['start-condition'], fillDefaults);
        } else {
            this['start-condition'].setValues(null, fillDefaults);
        }
        if (values) {
            this.fillModelArray<MonitoringMirrorCollector>(this, 'collectors', values['collectors'], MonitoringMirrorCollector);
        } else {
            this['collectors'] = [];
        }
        if (values) {
            this.fillModelArray<MonitoringMatchRule>(this, 'match-rules', values['match-rules'], MonitoringMatchRule);
        } else {
            this['match-rules'] = [];
        }
        if (values && values['packet-filters'] != null) {
            this['packet-filters'] = values['packet-filters'];
        } else if (fillDefaults && MonitoringMirrorSessionSpec.hasDefaultValue('packet-filters')) {
            this['packet-filters'] = [ MonitoringMirrorSessionSpec.propInfo['packet-filters'].default];
        } else {
            this['packet-filters'] = [];
        }
        if (values) {
            this['interfaces'].setValues(values['interfaces'], fillDefaults);
        } else {
            this['interfaces'].setValues(null, fillDefaults);
        }
        this.setFormGroupValuesToBeModelValues();
    }


    protected getFormGroup(): FormGroup {
        if (!this._formGroup) {
            this._formGroup = new FormGroup({
                'packet-size': CustomFormControl(new FormControl(this['packet-size']), MonitoringMirrorSessionSpec.propInfo['packet-size']),
                'start-condition': CustomFormGroup(this['start-condition'].$formGroup, MonitoringMirrorSessionSpec.propInfo['start-condition'].required),
                'collectors': new FormArray([]),
                'match-rules': new FormArray([]),
                'packet-filters': CustomFormControl(new FormControl(this['packet-filters']), MonitoringMirrorSessionSpec.propInfo['packet-filters']),
                'interfaces': CustomFormGroup(this['interfaces'].$formGroup, MonitoringMirrorSessionSpec.propInfo['interfaces'].required),
            });
            // generate FormArray control elements
            this.fillFormArray<MonitoringMirrorCollector>('collectors', this['collectors'], MonitoringMirrorCollector);
            // generate FormArray control elements
            this.fillFormArray<MonitoringMatchRule>('match-rules', this['match-rules'], MonitoringMatchRule);
            // We force recalculation of controls under a form group
            Object.keys((this._formGroup.get('start-condition') as FormGroup).controls).forEach(field => {
                const control = this._formGroup.get('start-condition').get(field);
                control.updateValueAndValidity();
            });
            // We force recalculation of controls under a form group
            Object.keys((this._formGroup.get('collectors') as FormGroup).controls).forEach(field => {
                const control = this._formGroup.get('collectors').get(field);
                control.updateValueAndValidity();
            });
            // We force recalculation of controls under a form group
            Object.keys((this._formGroup.get('match-rules') as FormGroup).controls).forEach(field => {
                const control = this._formGroup.get('match-rules').get(field);
                control.updateValueAndValidity();
            });
            // We force recalculation of controls under a form group
            Object.keys((this._formGroup.get('interfaces') as FormGroup).controls).forEach(field => {
                const control = this._formGroup.get('interfaces').get(field);
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
            this._formGroup.controls['packet-size'].setValue(this['packet-size']);
            this['start-condition'].setFormGroupValuesToBeModelValues();
            this.fillModelArray<MonitoringMirrorCollector>(this, 'collectors', this['collectors'], MonitoringMirrorCollector);
            this.fillModelArray<MonitoringMatchRule>(this, 'match-rules', this['match-rules'], MonitoringMatchRule);
            this._formGroup.controls['packet-filters'].setValue(this['packet-filters']);
            this['interfaces'].setFormGroupValuesToBeModelValues();
        }
    }
}

