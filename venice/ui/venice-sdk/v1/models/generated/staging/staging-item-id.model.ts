/**
 * This file is generated by the SwaggerTSGenerator.
 * Do not edit.
*/
/* tslint:disable */
import { Validators, FormControl, FormGroup, FormArray, ValidatorFn } from '@angular/forms';
import { minValueValidator, maxValueValidator, enumValidator } from './validators';
import { BaseModel, PropInfoItem } from './base-model';


export interface IStagingItemId {
    'uri'?: string;
    'method'?: string;
}


export class StagingItemId extends BaseModel implements IStagingItemId {
    'uri': string = null;
    'method': string = null;
    public static propInfo: { [prop: string]: PropInfoItem } = {
        'uri': {
            type: 'string'
        },
        'method': {
            type: 'string'
        },
    }

    public getPropInfo(propName: string): PropInfoItem {
        return StagingItemId.propInfo[propName];
    }

    /**
     * Returns whether or not there is an enum property with a default value
    */
    public static hasDefaultValue(prop) {
        return (StagingItemId.propInfo[prop] != null &&
                        StagingItemId.propInfo[prop].default != null &&
                        StagingItemId.propInfo[prop].default != '');
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
        if (values && values['uri'] != null) {
            this['uri'] = values['uri'];
        } else if (StagingItemId.hasDefaultValue('uri')) {
            this['uri'] = StagingItemId.propInfo['uri'].default;
        }
        if (values && values['method'] != null) {
            this['method'] = values['method'];
        } else if (StagingItemId.hasDefaultValue('method')) {
            this['method'] = StagingItemId.propInfo['method'].default;
        }
    }




    protected getFormGroup(): FormGroup {
        if (!this._formGroup) {
            this._formGroup = new FormGroup({
                'uri': new FormControl(this['uri']),
                'method': new FormControl(this['method']),
            });
        }
        return this._formGroup;
    }

    setFormGroupValues() {
        if (this._formGroup) {
            this._formGroup.controls['uri'].setValue(this['uri']);
            this._formGroup.controls['method'].setValue(this['method']);
        }
    }
}

