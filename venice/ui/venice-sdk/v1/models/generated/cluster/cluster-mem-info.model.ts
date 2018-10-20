/**
 * This file is generated by the SwaggerTSGenerator.
 * Do not edit.
*/
/* tslint:disable */
import { Validators, FormControl, FormGroup, FormArray, ValidatorFn } from '@angular/forms';
import { minValueValidator, maxValueValidator, enumValidator } from './validators';
import { BaseModel, PropInfoItem } from './base-model';

import { ClusterMemInfo_type,  } from './enums';

export interface IClusterMemInfo {
    'type'?: ClusterMemInfo_type;
    'size'?: string;
}


export class ClusterMemInfo extends BaseModel implements IClusterMemInfo {
    'type': ClusterMemInfo_type = null;
    'size': string = null;
    public static propInfo: { [prop: string]: PropInfoItem } = {
        'type': {
            enum: ClusterMemInfo_type,
            default: 'UNKNOWN',
            type: 'string'
        },
        'size': {
            type: 'string'
        },
    }

    public getPropInfo(propName: string): PropInfoItem {
        return ClusterMemInfo.propInfo[propName];
    }

    /**
     * Returns whether or not there is an enum property with a default value
    */
    public static hasDefaultValue(prop) {
        return (ClusterMemInfo.propInfo[prop] != null &&
                        ClusterMemInfo.propInfo[prop].default != null &&
                        ClusterMemInfo.propInfo[prop].default != '');
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
        if (values && values['type'] != null) {
            this['type'] = values['type'];
        } else if (ClusterMemInfo.hasDefaultValue('type')) {
            this['type'] = <ClusterMemInfo_type>  ClusterMemInfo.propInfo['type'].default;
        }
        if (values && values['size'] != null) {
            this['size'] = values['size'];
        } else if (ClusterMemInfo.hasDefaultValue('size')) {
            this['size'] = ClusterMemInfo.propInfo['size'].default;
        }
    }




    protected getFormGroup(): FormGroup {
        if (!this._formGroup) {
            this._formGroup = new FormGroup({
                'type': new FormControl(this['type'], [enumValidator(ClusterMemInfo_type), ]),
                'size': new FormControl(this['size']),
            });
        }
        return this._formGroup;
    }

    setFormGroupValues() {
        if (this._formGroup) {
            this._formGroup.controls['type'].setValue(this['type']);
            this._formGroup.controls['size'].setValue(this['size']);
        }
    }
}

