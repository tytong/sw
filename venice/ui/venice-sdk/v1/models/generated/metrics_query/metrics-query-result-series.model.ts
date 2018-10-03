/**
 * This file is generated by the SwaggerTSGenerator.
 * Do not edit.
*/
/* tslint:disable */
import { Validators, FormControl, FormGroup, FormArray, ValidatorFn } from '@angular/forms';
import { minValueValidator, maxValueValidator, enumValidator } from './validators';
import { BaseModel, PropInfoItem } from './base-model';

import { ApiInterfaceSlice, IApiInterfaceSlice } from './api-interface-slice.model';

export interface IMetrics_queryResultSeries {
    'name'?: string;
    'columns'?: Array<string>;
    'values'?: Array<IApiInterfaceSlice>;
}


export class Metrics_queryResultSeries extends BaseModel implements IMetrics_queryResultSeries {
    'name': string = null;
    'columns': Array<string> = null;
    'values': Array<ApiInterfaceSlice> = null;
    public static propInfo: { [prop: string]: PropInfoItem } = {
        'name': {
            type: 'string'
        },
        'columns': {
            type: 'Array<string>'
        },
        'values': {
            type: 'object'
        },
    }

    public getPropInfo(propName: string): PropInfoItem {
        return Metrics_queryResultSeries.propInfo[propName];
    }

    /**
     * Returns whether or not there is an enum property with a default value
    */
    public static hasDefaultValue(prop) {
        return (Metrics_queryResultSeries.propInfo[prop] != null &&
                        Metrics_queryResultSeries.propInfo[prop].default != null &&
                        Metrics_queryResultSeries.propInfo[prop].default != '');
    }

    /**
     * constructor
     * @param values Can be used to set a webapi response to this newly constructed model
    */
    constructor(values?: any) {
        super();
        this['columns'] = new Array<string>();
        this['values'] = new Array<ApiInterfaceSlice>();
        this.setValues(values);
    }

    /**
     * set the values. If a value isn't provided and we have a default, we use that.
     * @param values Can be used to set a webapi response to this newly constructed model
    */
    setValues(values: any): void {
        if (values && values['name'] != null) {
            this['name'] = values['name'];
        } else if (Metrics_queryResultSeries.hasDefaultValue('name')) {
            this['name'] = Metrics_queryResultSeries.propInfo['name'].default;
        }
        if (values) {
            this.fillModelArray<string>(this, 'columns', values['columns']);
        }
        if (values) {
            this.fillModelArray<ApiInterfaceSlice>(this, 'values', values['values'], ApiInterfaceSlice);
        }
    }




    protected getFormGroup(): FormGroup {
        if (!this._formGroup) {
            this._formGroup = new FormGroup({
                'name': new FormControl(this['name']),
                'columns': new FormArray([]),
                'values': new FormArray([]),
            });
            // generate FormArray control elements
            this.fillFormArray<string>('columns', this['columns']);
            // generate FormArray control elements
            this.fillFormArray<ApiInterfaceSlice>('values', this['values'], ApiInterfaceSlice);
        }
        return this._formGroup;
    }

    setFormGroupValues() {
        if (this._formGroup) {
            this._formGroup.controls['name'].setValue(this['name']);
            this.fillModelArray<string>(this, 'columns', this['columns']);
            this.fillModelArray<ApiInterfaceSlice>(this, 'values', this['values'], ApiInterfaceSlice);
        }
    }
}

