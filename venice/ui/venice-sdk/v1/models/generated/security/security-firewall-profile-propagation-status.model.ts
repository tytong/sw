/**
 * This file is generated by the SwaggerTSGenerator.
 * Do not edit.
*/
/* tslint:disable */
import { Validators, FormControl, FormGroup, FormArray, ValidatorFn } from '@angular/forms';
import { minValueValidator, maxValueValidator, minLengthValidator, maxLengthValidator, required, enumValidator, patternValidator, CustomFormControl, CustomFormGroup } from '../../../utils/validators';
import { BaseModel, PropInfoItem } from './base-model';


export interface ISecurityFirewallProfilePropagationStatus {
    'generation-id'?: string;
    'updated'?: number;
    'pending'?: number;
    'min-version'?: string;
}


export class SecurityFirewallProfilePropagationStatus extends BaseModel implements ISecurityFirewallProfilePropagationStatus {
    'generation-id': string = null;
    'updated': number = null;
    /** Number of Naples pending. If this is 0 it can be assumed that everything is up to date. */
    'pending': number = null;
    'min-version': string = null;
    public static propInfo: { [prop: string]: PropInfoItem } = {
        'generation-id': {
            required: false,
            type: 'string'
        },
        'updated': {
            required: false,
            type: 'number'
        },
        'pending': {
            description:  'Number of Naples pending. If this is 0 it can be assumed that everything is up to date.',
            required: false,
            type: 'number'
        },
        'min-version': {
            required: false,
            type: 'string'
        },
    }

    public getPropInfo(propName: string): PropInfoItem {
        return SecurityFirewallProfilePropagationStatus.propInfo[propName];
    }

    public getPropInfoConfig(): { [key:string]:PropInfoItem } {
        return SecurityFirewallProfilePropagationStatus.propInfo;
    }

    /**
     * Returns whether or not there is an enum property with a default value
    */
    public static hasDefaultValue(prop) {
        return (SecurityFirewallProfilePropagationStatus.propInfo[prop] != null &&
                        SecurityFirewallProfilePropagationStatus.propInfo[prop].default != null);
    }

    /**
     * constructor
     * @param values Can be used to set a webapi response to this newly constructed model
    */
    constructor(values?: any, setDefaults:boolean = true) {
        super();
        this.setValues(values, setDefaults);
    }

    /**
     * set the values for both the Model and the Form Group. If a value isn't provided and we have a default, we use that.
     * @param values Can be used to set a webapi response to this newly constructed model
    */
    setValues(values: any, fillDefaults = true): void {
        if (values && values['generation-id'] != null) {
            this['generation-id'] = values['generation-id'];
        } else if (fillDefaults && SecurityFirewallProfilePropagationStatus.hasDefaultValue('generation-id')) {
            this['generation-id'] = SecurityFirewallProfilePropagationStatus.propInfo['generation-id'].default;
        } else {
            this['generation-id'] = null
        }
        if (values && values['updated'] != null) {
            this['updated'] = values['updated'];
        } else if (fillDefaults && SecurityFirewallProfilePropagationStatus.hasDefaultValue('updated')) {
            this['updated'] = SecurityFirewallProfilePropagationStatus.propInfo['updated'].default;
        } else {
            this['updated'] = null
        }
        if (values && values['pending'] != null) {
            this['pending'] = values['pending'];
        } else if (fillDefaults && SecurityFirewallProfilePropagationStatus.hasDefaultValue('pending')) {
            this['pending'] = SecurityFirewallProfilePropagationStatus.propInfo['pending'].default;
        } else {
            this['pending'] = null
        }
        if (values && values['min-version'] != null) {
            this['min-version'] = values['min-version'];
        } else if (fillDefaults && SecurityFirewallProfilePropagationStatus.hasDefaultValue('min-version')) {
            this['min-version'] = SecurityFirewallProfilePropagationStatus.propInfo['min-version'].default;
        } else {
            this['min-version'] = null
        }
        this.setFormGroupValuesToBeModelValues();
    }


    protected getFormGroup(): FormGroup {
        if (!this._formGroup) {
            this._formGroup = new FormGroup({
                'generation-id': CustomFormControl(new FormControl(this['generation-id']), SecurityFirewallProfilePropagationStatus.propInfo['generation-id']),
                'updated': CustomFormControl(new FormControl(this['updated']), SecurityFirewallProfilePropagationStatus.propInfo['updated']),
                'pending': CustomFormControl(new FormControl(this['pending']), SecurityFirewallProfilePropagationStatus.propInfo['pending']),
                'min-version': CustomFormControl(new FormControl(this['min-version']), SecurityFirewallProfilePropagationStatus.propInfo['min-version']),
            });
        }
        return this._formGroup;
    }

    setModelToBeFormGroupValues() {
        this.setValues(this.$formGroup.value, false);
    }

    setFormGroupValuesToBeModelValues() {
        if (this._formGroup) {
            this._formGroup.controls['generation-id'].setValue(this['generation-id']);
            this._formGroup.controls['updated'].setValue(this['updated']);
            this._formGroup.controls['pending'].setValue(this['pending']);
            this._formGroup.controls['min-version'].setValue(this['min-version']);
        }
    }
}

