/**
 * This file is generated by the SwaggerTSGenerator.
 * Do not edit.
*/
/* tslint:disable */
import { Validators, FormControl, FormGroup, FormArray, ValidatorFn } from '@angular/forms';
import { minValueValidator, maxValueValidator, enumValidator } from './validators';
import { BaseModel, PropInfoItem } from './base-model';

import { RolloutRolloutPhase, IRolloutRolloutPhase } from './rollout-rollout-phase.model';
import { RolloutRolloutStatus_state,  } from './enums';

export interface IRolloutRolloutStatus {
    'controller-nodes-status'?: Array<IRolloutRolloutPhase>;
    'controller-services-status'?: Array<IRolloutRolloutPhase>;
    'smartnics-status'?: Array<IRolloutRolloutPhase>;
    'state'?: RolloutRolloutStatus_state;
    'completion-percent'?: number;
    'start-time'?: Date;
    'end-time'?: Date;
    'prev-version'?: string;
}


export class RolloutRolloutStatus extends BaseModel implements IRolloutRolloutStatus {
    'controller-nodes-status': Array<RolloutRolloutPhase> = null;
    'controller-services-status': Array<RolloutRolloutPhase> = null;
    /** Rollout status of SmartNICs in the cluster. Has entries for SmartNICs on Controller nodes as well as workload nodes
    The entries are group by parallelism based on the order-constraints and max-parallel specified by the user. */
    'smartnics-status': Array<RolloutRolloutPhase> = null;
    'state': RolloutRolloutStatus_state = null;
    'completion-percent': number = null;
    'start-time': Date = null;
    'end-time': Date = null;
    'prev-version': string = null;
    public static propInfo: { [prop: string]: PropInfoItem } = {
        'controller-nodes-status': {
            type: 'object'
        },
        'controller-services-status': {
            type: 'object'
        },
        'smartnics-status': {
            description:  'Rollout status of SmartNICs in the cluster. Has entries for SmartNICs on Controller nodes as well as workload nodes The entries are group by parallelism based on the order-constraints and max-parallel specified by the user.',
            type: 'object'
        },
        'state': {
            enum: RolloutRolloutStatus_state,
            default: 'PROGRESSING',
            type: 'string'
        },
        'completion-percent': {
            type: 'number'
        },
        'start-time': {
            type: 'Date'
        },
        'end-time': {
            type: 'Date'
        },
        'prev-version': {
            type: 'string'
        },
    }

    public getPropInfo(propName: string): PropInfoItem {
        return RolloutRolloutStatus.propInfo[propName];
    }

    /**
     * Returns whether or not there is an enum property with a default value
    */
    public static hasDefaultValue(prop) {
        return (RolloutRolloutStatus.propInfo[prop] != null &&
                        RolloutRolloutStatus.propInfo[prop].default != null &&
                        RolloutRolloutStatus.propInfo[prop].default != '');
    }

    /**
     * constructor
     * @param values Can be used to set a webapi response to this newly constructed model
    */
    constructor(values?: any) {
        super();
        this['controller-nodes-status'] = new Array<RolloutRolloutPhase>();
        this['controller-services-status'] = new Array<RolloutRolloutPhase>();
        this['smartnics-status'] = new Array<RolloutRolloutPhase>();
        this.setValues(values);
    }

    /**
     * set the values. If a value isn't provided and we have a default, we use that.
     * @param values Can be used to set a webapi response to this newly constructed model
    */
    setValues(values: any): void {
        if (values) {
            this.fillModelArray<RolloutRolloutPhase>(this, 'controller-nodes-status', values['controller-nodes-status'], RolloutRolloutPhase);
        }
        if (values) {
            this.fillModelArray<RolloutRolloutPhase>(this, 'controller-services-status', values['controller-services-status'], RolloutRolloutPhase);
        }
        if (values) {
            this.fillModelArray<RolloutRolloutPhase>(this, 'smartnics-status', values['smartnics-status'], RolloutRolloutPhase);
        }
        if (values && values['state'] != null) {
            this['state'] = values['state'];
        } else if (RolloutRolloutStatus.hasDefaultValue('state')) {
            this['state'] = <RolloutRolloutStatus_state>  RolloutRolloutStatus.propInfo['state'].default;
        }
        if (values && values['completion-percent'] != null) {
            this['completion-percent'] = values['completion-percent'];
        } else if (RolloutRolloutStatus.hasDefaultValue('completion-percent')) {
            this['completion-percent'] = RolloutRolloutStatus.propInfo['completion-percent'].default;
        }
        if (values && values['start-time'] != null) {
            this['start-time'] = values['start-time'];
        } else if (RolloutRolloutStatus.hasDefaultValue('start-time')) {
            this['start-time'] = RolloutRolloutStatus.propInfo['start-time'].default;
        }
        if (values && values['end-time'] != null) {
            this['end-time'] = values['end-time'];
        } else if (RolloutRolloutStatus.hasDefaultValue('end-time')) {
            this['end-time'] = RolloutRolloutStatus.propInfo['end-time'].default;
        }
        if (values && values['prev-version'] != null) {
            this['prev-version'] = values['prev-version'];
        } else if (RolloutRolloutStatus.hasDefaultValue('prev-version')) {
            this['prev-version'] = RolloutRolloutStatus.propInfo['prev-version'].default;
        }
    }




    protected getFormGroup(): FormGroup {
        if (!this._formGroup) {
            this._formGroup = new FormGroup({
                'controller-nodes-status': new FormArray([]),
                'controller-services-status': new FormArray([]),
                'smartnics-status': new FormArray([]),
                'state': new FormControl(this['state'], [enumValidator(RolloutRolloutStatus_state), ]),
                'completion-percent': new FormControl(this['completion-percent']),
                'start-time': new FormControl(this['start-time']),
                'end-time': new FormControl(this['end-time']),
                'prev-version': new FormControl(this['prev-version']),
            });
            // generate FormArray control elements
            this.fillFormArray<RolloutRolloutPhase>('controller-nodes-status', this['controller-nodes-status'], RolloutRolloutPhase);
            // generate FormArray control elements
            this.fillFormArray<RolloutRolloutPhase>('controller-services-status', this['controller-services-status'], RolloutRolloutPhase);
            // generate FormArray control elements
            this.fillFormArray<RolloutRolloutPhase>('smartnics-status', this['smartnics-status'], RolloutRolloutPhase);
        }
        return this._formGroup;
    }

    setFormGroupValues() {
        if (this._formGroup) {
            this.fillModelArray<RolloutRolloutPhase>(this, 'controller-nodes-status', this['controller-nodes-status'], RolloutRolloutPhase);
            this.fillModelArray<RolloutRolloutPhase>(this, 'controller-services-status', this['controller-services-status'], RolloutRolloutPhase);
            this.fillModelArray<RolloutRolloutPhase>(this, 'smartnics-status', this['smartnics-status'], RolloutRolloutPhase);
            this._formGroup.controls['state'].setValue(this['state']);
            this._formGroup.controls['completion-percent'].setValue(this['completion-percent']);
            this._formGroup.controls['start-time'].setValue(this['start-time']);
            this._formGroup.controls['end-time'].setValue(this['end-time']);
            this._formGroup.controls['prev-version'].setValue(this['prev-version']);
        }
    }
}

