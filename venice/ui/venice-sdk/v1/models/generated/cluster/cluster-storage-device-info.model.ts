/**
 * This file is generated by the SwaggerTSGenerator.
 * Do not edit.
*/
/* tslint:disable */
import { Validators, FormControl, FormGroup, FormArray, ValidatorFn } from '@angular/forms';
import { minValueValidator, maxValueValidator, enumValidator } from './validators';
import { BaseModel, PropInfoItem } from './base-model';


export interface IClusterStorageDeviceInfo {
    'serial-num'?: string;
    'type'?: string;
    'vendor'?: string;
    'capacity'?: string;
}


export class ClusterStorageDeviceInfo extends BaseModel implements IClusterStorageDeviceInfo {
    'serial-num': string = null;
    'type': string = null;
    'vendor': string = null;
    'capacity': string = null;
    public static propInfo: { [prop: string]: PropInfoItem } = {
        'serial-num': {
            type: 'string'
        },
        'type': {
            type: 'string'
        },
        'vendor': {
            type: 'string'
        },
        'capacity': {
            type: 'string'
        },
    }

    public getPropInfo(propName: string): PropInfoItem {
        return ClusterStorageDeviceInfo.propInfo[propName];
    }

    /**
     * Returns whether or not there is an enum property with a default value
    */
    public static hasDefaultValue(prop) {
        return (ClusterStorageDeviceInfo.propInfo[prop] != null &&
                        ClusterStorageDeviceInfo.propInfo[prop].default != null &&
                        ClusterStorageDeviceInfo.propInfo[prop].default != '');
    }

    /**
     * constructor
     * @param values Can be used to set a webapi response to this newly constructed model
    */
    constructor(values?: any) {
        super();
        this.setValues(values);
    }

    /**
     * set the values. If a value isn't provided and we have a default, we use that.
     * @param values Can be used to set a webapi response to this newly constructed model
    */
    setValues(values: any): void {
        if (values && values['serial-num'] != null) {
            this['serial-num'] = values['serial-num'];
        } else if (ClusterStorageDeviceInfo.hasDefaultValue('serial-num')) {
            this['serial-num'] = ClusterStorageDeviceInfo.propInfo['serial-num'].default;
        }
        if (values && values['type'] != null) {
            this['type'] = values['type'];
        } else if (ClusterStorageDeviceInfo.hasDefaultValue('type')) {
            this['type'] = ClusterStorageDeviceInfo.propInfo['type'].default;
        }
        if (values && values['vendor'] != null) {
            this['vendor'] = values['vendor'];
        } else if (ClusterStorageDeviceInfo.hasDefaultValue('vendor')) {
            this['vendor'] = ClusterStorageDeviceInfo.propInfo['vendor'].default;
        }
        if (values && values['capacity'] != null) {
            this['capacity'] = values['capacity'];
        } else if (ClusterStorageDeviceInfo.hasDefaultValue('capacity')) {
            this['capacity'] = ClusterStorageDeviceInfo.propInfo['capacity'].default;
        }
    }




    protected getFormGroup(): FormGroup {
        if (!this._formGroup) {
            this._formGroup = new FormGroup({
                'serial-num': new FormControl(this['serial-num']),
                'type': new FormControl(this['type']),
                'vendor': new FormControl(this['vendor']),
                'capacity': new FormControl(this['capacity']),
            });
        }
        return this._formGroup;
    }

    setFormGroupValues() {
        if (this._formGroup) {
            this._formGroup.controls['serial-num'].setValue(this['serial-num']);
            this._formGroup.controls['type'].setValue(this['type']);
            this._formGroup.controls['vendor'].setValue(this['vendor']);
            this._formGroup.controls['capacity'].setValue(this['capacity']);
        }
    }
}

